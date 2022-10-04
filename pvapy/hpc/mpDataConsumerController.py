#!/usr/bin/env python

import threading
import time
import queue
import pvaccess as pva
import multiprocessing as mp
from ..utility.loggingManager import LoggingManager
from .dataConsumer import DataConsumer
from .hpcController import HpcController
from .dataConsumerController import DataConsumerController

class MpDataConsumerController(HpcController):

    def __init__(self, args):
        HpcController.__init__(self, args)
        self.mpProcessMap = {}
        self.requestQueueMap = {}
        self.responseQueueMap = {}
        self.lastStatsObjectIdMap = {}

    def start(self):
        # Replace interrupt handler for worker processes
        # so we can exit cleanly
        import signal
        originalSigintHandler = signal.signal(signal.SIGINT, signal.SIG_IGN)
        for consumerId in range(self.args.consumer_id, self.args.consumer_id+self.args.n_consumers):
            requestQueue = mp.Queue()
            self.requestQueueMap[consumerId] = requestQueue
            responseQueue = mp.Queue()
            self.responseQueueMap[consumerId] = responseQueue
            mpProcess = mp.Process(target=mpdcController, args=(consumerId, requestQueue, responseQueue, self.args,))
            self.mpProcessMap[consumerId] = mpProcess
            self.logger.debug(f'Starting consumer {consumerId}')
            mpProcess.start()
        signal.signal(signal.SIGINT, originalSigintHandler)

    def reportStats(self, statsDict=None):
        if not statsDict:
            statsDict = self.getStats()
        report = ''
        for consumerId,statsDict2 in statsDict.items():
            statsDict2['consumerId'] = consumerId
            report += self.prettyPrinter.pformat(statsDict2)
            report += '\n'
        if self.screen:
            try:
                self.screen.erase()
                self.screen.addstr(report)
                self.screen.refresh()
                return
            except Exception as ex:
                # Turn screen off on errors
                self.stopScreen()
        # Remove extra newline character
        print(report[0:-1])

    def getStats(self):
        for consumerId in range(self.args.consumer_id, self.args.consumer_id+self.args.n_consumers):
            requestQueue = self.requestQueueMap[consumerId]
            try:
                requestQueue.put(self.GET_STATS_COMMAND, block=True, timeout=self.WAIT_TIME)
            except Exception as ex:
                self.stopScreen()
                self.logger.error(f'Cannot request stats from consumer {consumerId}: {ex}')
        statsDict = {}
        for consumerId in range(self.args.consumer_id, self.args.consumer_id+self.args.n_consumers):
            statsDict[consumerId] = {}
            lastStatsObjectId = self.lastStatsObjectIdMap.get(consumerId, 0)
            try:
                while True:
                    responseQueue = self.responseQueueMap[consumerId]
                    statsDict[consumerId] = responseQueue.get(block=True, timeout=self.WAIT_TIME)
                    statsObjectId = statsDict[consumerId].get('objectId', 0)
                    if statsObjectId != lastStatsObjectId:
                        self.lastStatsObjectIdMap[consumerId] = statsObjectId 
                        break
                    else:
                        self.logger.warning(f'Discarding stale stats received from consumer {consumerId}')
            except queue.Empty:
                self.stopScreen()
                self.logger.error(f'No stats received from consumer {consumerId}')
        return statsDict

    def processPvUpdate(self, updateWaitTime):
        return False

    def stop(self):
        for consumerId in range(self.args.consumer_id, self.args.consumer_id+self.args.n_consumers):
            requestQueue = self.requestQueueMap[consumerId]
            try:
                requestQueue.put(self.STOP_COMMAND, block=True, timeout=self.WAIT_TIME)
            except Exception as ex:
                self.stopScreen()
                self.logger.error(f'Cannot stop consumer {consumerId}: {ex}')
        statsDict = {}
        for consumerId in range(self.args.consumer_id, self.args.consumer_id+self.args.n_consumers):
            statsDict[consumerId] = {}
            try:
                responseQueue = self.responseQueueMap[consumerId]
                statsDict[consumerId] = responseQueue.get(block=True, timeout=self.WAIT_TIME)
            except queue.Empty:
                self.stopScreen()
                self.logger.error(f'No stats received from consumer {consumerId}')
        for consumerId in range(self.args.consumer_id, self.args.consumer_id+self.args.n_consumers):
            mpProcess = self.mpProcessMap[consumerId]
            mpProcess.join(self.WAIT_TIME)
            self.logger.info(f'Stopped consumer {consumerId}')
        if self.screen:
            self.curses.endwin()
            self.screen = None
        self.logger.debug('Controller exiting')
        return statsDict

class MpdcControllerRequestProcessingThread(threading.Thread):

    def __init__(self, controller, consumerId, requestQueue, responseQueue):
        threading.Thread.__init__(self)
        self.controller = controller
        self.consumerId = consumerId
        self.requestQueue = requestQueue
        self.responseQueue = responseQueue
        self.logger = LoggingManager.getLogger(f'rpThread-{self.consumerId}')

    def run(self):
        self.logger.debug(f'Request processing thread for consumer {self.consumerId} starting')
        while True:
            try:
                if self.controller.isDone:
                    self.logger.debug(f'Consumer {self.consumerId} is done, request processing thread is exiting')
                    break

                # Check for new request
                try:
                    request = self.requestQueue.get(block=True, timeout=self.controller.WAIT_TIME)
                    self.logger.debug(f'Received request: {request}')
                    if request == self.controller.STOP_COMMAND:
                        self.controller.isDone = True
                        self.logger.debug(f'Consumer {self.consumerId} received stop command, request processing thread is exiting')
                        break
                    elif request == self.controller.GET_STATS_COMMAND:
                        statsDict = self.controller.getStats()
                        try:
                            self.responseQueue.put(statsDict, block=False)
                        except Exception as ex:
                            self.logger.error(f'Consumer {consumerId} cannot report stats: {ex}')
                except queue.Empty:
                    pass

            except Exception as ex:
                self.logger.error(f'Request processing error: {ex}')

        self.logger.debug(f'Request processing thread for consumer {self.consumerId} exited')
   
def mpdcController(consumerId, requestQueue, responseQueue, args):
    args.consumer_id = consumerId
    logger = LoggingManager.getLogger(f'mpdcController-{consumerId}')
    controller = DataConsumerController(args)
    controller.start()

    # Process controller requests in a separate thread
    rpThread = MpdcControllerRequestProcessingThread(controller, consumerId, requestQueue, responseQueue)
    rpThread.start()

    waitTime = controller.WAIT_TIME
    while True:
        try:
            if controller.isDone:
                logger.debug(f'Consumer {consumerId} is done')
                break

            now = time.time()
            wakeTime = now+waitTime

            # Try to process object
            delay = 0
            hasProcessedObject = controller.processPvUpdate(waitTime)
            if not hasProcessedObject:
                # Determine if we can wait
                delay = wakeTime-time.time()
                if delay > 0:
                    time.sleep(delay)

        except Exception as ex:
            controller.stopScreen()
            logger.error(f'Processing error: {ex}')

    try: 
        controller.stop()
    except Exception as ex:
        self.logger.warn(f'Could not stop consumer {dataConsumer.getConsumerId()}')
    statsDict = controller.getStats()
    try:
        responseQueue.put(statsDict, block=True, timeout=controller.WAIT_TIME)
    except Exception as ex:
        logger.error(f'Consumer {consumerId} cannot report stats on exit: {ex}')
    time.sleep(controller.WAIT_TIME)

