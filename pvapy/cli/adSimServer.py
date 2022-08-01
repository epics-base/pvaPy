#!/usr/bin/env python

import curses
import time
import threading
import queue
import argparse
import numpy as np
import pvaccess as pva
import os
import os.path
from ..utility.adImageUtility import AdImageUtility
from ..utility.floatWithUnits import FloatWithUnits
from ..utility.intWithUnits import IntWithUnits

__version__ = pva.__version__

class AdSimServer:

    DELAY_CORRECTION = 0.0001
    NOTIFICATION_DELAY = 0.1
    BYTES_IN_MEGABYTE = 1000000

    def __init__(self, inputDirectory, inputFile, mmapMode, frameRate, nf, nx, ny, datatype, minimum, maximum, runtime, channelName, notifyPv, notifyPvValue, startDelay, reportPeriod):
        self.deltaT = 0
        if frameRate > 0:
            self.deltaT = 1.0/frameRate
        self.runtime = runtime
        self.reportPeriod = reportPeriod 

        inputFiles = []
        if inputDirectory is not None:
            inputFiles = [os.path.join(inputDirectory, f) for f in os.listdir(inputDirectory) if os.path.isfile(os.path.join(inputDirectory, f))]
        if inputFile is not None:
            inputFiles.append(inputFile)
        self.frames = None
        for f in inputFiles:
            try:
                if mmapMode:
                    newFrames = np.load(f, mmapMode='r')
                else:
                    newFrames = np.load(f)
                if self.frames is None:
                    self.frames = newFrames
                else:
                    self.frames = np.append(self.frames, newFrames, axis=0)
                print(f'Loaded input file {f}')
            except Exception as ex:
                print(f'Cannot load input file {f}, skipping it: {ex}')
        if self.frames is None:
            print('Generating random frames')
            # Example frame:
            # frame = np.array([[0,0,0,0,0,0,0,0,0,0],
            #                  [0,0,0,0,1,1,0,0,0,0],
            #                  [0,0,0,1,2,3,2,0,0,0],
            #                  [0,0,0,1,2,3,2,0,0,0],
            #                  [0,0,0,1,2,3,2,0,0,0],
            #                  [0,0,0,0,0,0,0,0,0,0]], dtype=np.uint16)
            dt = np.dtype(datatype)
            if datatype != 'float32' and datatype != 'float64':
                dtinfo = np.iinfo(dt)
                mn = dtinfo.min
                if minimum is not None:
                    mn = int(max(dtinfo.min, minimum))
                mx = dtinfo.max
                if maximum is not None:
                    mx = int(min(dtinfo.max, maximum))
                self.frames = np.random.randint(mn, mx, size=(nf, ny, nx), dtype=dt)
            else:
                # Use float32 for min/max, to prevent overflow errors
                dtinfo = np.finfo(np.float32)
                mn = dtinfo.min
                if minimum is not None:
                    mn = float(max(dtinfo.min, minimum))
                mx = dtinfo.max
                if maximum is not None:
                    mx = float(min(dtinfo.max, maximum))
                self.frames = np.random.uniform(mn, mx, size=(nf, ny, nx))
                if datatype == 'float32':
                    self.frames = np.float32(self.frames)

            print(f'Generated frame shape: {self.frames[0].shape}')
            print(f'Range of generated values: [{mn},{mx}]')
        self.frameRate = frameRate
        self.nInputFrames, self.rows, self.cols = self.frames.shape
        self.imageSize = IntWithUnits(self.rows*self.cols*self.frames[0].itemsize, 'B')
        self.expectedDataRate = FloatWithUnits(self.imageSize*self.frameRate/self.BYTES_IN_MEGABYTE, 'MBps')
        print(f'Number of input frames: {self.nInputFrames} (size: {self.cols}x{self.rows}, {self.imageSize}, type: {self.frames.dtype})')
        print(f'Expected data rate: {self.expectedDataRate}')

        self.channelName = channelName
        self.server = pva.PvaServer()
        self.server.addRecord(self.channelName, pva.NtNdArray())
        if notifyPv and notifyPvValue:
            try:
                time.sleep(self.NOTIFICATION_DELAY)
                notifyChannel = pva.Channel(notifyPv, pva.CA)
                notifyChannel.put(notifyPvValue)
                print(f'Set notification PV {notifyPv} to {notifyPvValue}')
            except Exception as ex:
                print(f'Could not set notification PV {notifyPv} to {notifyPvValue}: {ex}')

        self.frameMap = {}
        self.currentFrameId = 0
        self.nPublishedFrames = 0
        self.startTime = 0
        self.lastPublishedTime = 0
        self.startDelay = startDelay
        self.isDone = False
        self.screen = None

    def frameProducer(self, extraFieldsPvObject=None):
        for frameId in range(0, self.nInputFrames):
            if self.isDone:
               return

            frame = self.frames[frameId]
            self.frameMap[frameId] = AdImageUtility.generateNtNdArray2D(frameId, frame, extraFieldsPvObject)

    def prepareFrame(self):
        # Get cached frame
        cachedFrameId = self.currentFrameId % self.nInputFrames
        if cachedFrameId not in self.frameMap:
            # In case frames were not generated on time, use first frame
            cachedFrameId = 0
        frame = self.frameMap[cachedFrameId]

        # Correct image id and timestamps
        self.currentFrameId += 1
        frame['uniqueId'] = self.currentFrameId
        ts = pva.PvTimeStamp(time.time())
        frame['timeStamp'] = ts
        frame['dataTimeStamp'] = ts
        return frame

    def framePublisher(self):
        while True:
            if self.isDone:
                return

            frame = self.prepareFrame()
            self.server.update(self.channelName, frame)
            self.lastPublishedTime = time.time()
            self.nPublishedFrames += 1

            runtime = 0
            frameRate = 0
            if self.nPublishedFrames > 1:
                runtime = self.lastPublishedTime - self.startTime
                deltaT = runtime/(self.nPublishedFrames - 1)
                frameRate = 1.0/deltaT
            else:
                self.startTime = self.lastPublishedTime
            if self.reportPeriod > 0 and (self.nPublishedFrames % self.reportPeriod) == 0:
                report = 'Published frame id {:6d} @ {:.3f}s (frame rate: {:.4f}fps; runtime: {:.3f}s)'.format(self.currentFrameId, self.lastPublishedTime, frameRate, runtime)
                if not self.screen:
                    self.screen = curses.initscr()
                self.screen.erase()
                self.screen.addstr(f'{report}\n')
                self.screen.refresh()

            if runtime > self.runtime:
                if self.screen:
                    curses.endwin()
                print('Server exiting after reaching runtime of {:.3f} seconds'.format(self.runtime))
                return

            if self.deltaT > 0:
                nextPublishTime = self.startTime + self.nPublishedFrames*self.deltaT
                delay = nextPublishTime - time.time() - self.DELAY_CORRECTION
                if delay > 0:
                    threading.Timer(delay, self.framePublisher).start()
                    return

    def start(self):
        threading.Thread(target=self.frameProducer, daemon=True).start()
        self.server.start()
        threading.Timer(self.startDelay, self.framePublisher).start()

    def stop(self):
        self.isDone = True
        self.server.stop()
        runtime = self.lastPublishedTime - self.startTime
        deltaT = runtime/(self.nPublishedFrames - 1)
        frameRate = 1.0/deltaT
        dataRate = FloatWithUnits(self.imageSize*frameRate/self.BYTES_IN_MEGABYTE, 'MBps')
        if self.screen:
            curses.endwin()
        print('\nServer runtime: {:.4f} seconds'.format(runtime))
        print('Published frames: {:6d} @ {:.4f} fps'.format(self.nPublishedFrames, frameRate))
        print(f'Data rate: {dataRate}')

def main():
    parser = argparse.ArgumentParser(description='PvaPy Area Detector Simulator')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s {version}'.format(version=__version__))
    parser.add_argument('-id', '--input-directory', type=str, dest='input_directory', default=None, help='Directory containing input files to be streamed; if input directory or input file are not provided, random images will be generated')
    parser.add_argument('-if', '--input-file', type=str, dest='input_file', default=None, help='Input file to be streamed; if input directory or input file are not provided, random images will be generated')
    parser.add_argument('-mm', '--mmap-mode', action='store_true', dest='mmap_mode', default=False, help='Use NumPy memory map to load the specified input file. This flag typically results in faster startup and lower memory usage for large files.')
    parser.add_argument('-fps', '--frame-rate', type=float, dest='frame_rate', default=20, help='Frames per second (default: 20 fps)')
    parser.add_argument('-nx', '--n-x-pixels', type=int, dest='n_x_pixels', default=256, help='Number of pixels in x dimension (default: 256 pixels; does not apply if input file file is given)')
    parser.add_argument('-ny', '--n-y-pixels', type=int, dest='n_y_pixels', default=256, help='Number of pixels in x dimension (default: 256 pixels; does not apply if input file is given)')
    parser.add_argument('-dt', '--datatype', type=str, dest='datatype', default='uint8', help='Generated datatype. Possible options are int8, uint8, int16, uint16, int32, uint32, float32, float64 (default: uint8; does not apply if input file is given)')
    parser.add_argument('-mn', '--minimum', type=float, dest='minimum', default=None, help='Minimum generated value (does not apply if input file is given)')
    parser.add_argument('-mx', '--maximum', type=float, dest='maximum', default=None, help='Maximum generated value (does not apply if input file is given)')
    parser.add_argument('-nf', '--n-frames', type=int, dest='n_frames', default=1000, help='Number of different frames to generate and cache; those images will be published over and over again as long as the server is running')
    parser.add_argument('-rt', '--runtime', type=float, dest='runtime', default=300, help='Server runtime in seconds (default: 300 seconds)')
    parser.add_argument('-cn', '--channel-name', type=str, dest='channel_name', default='pvapy:image', help='Server PVA channel name (default: pvapy:image)')
    parser.add_argument('-npv', '--notify-pv', type=str, dest='notify_pv', default=None, help='CA channel that should be notified on start; for the default Area Detector PVA driver PV that controls image acquisition is 13PVA1:cam1:Acquire')
    parser.add_argument('-nvl', '--notify-pv-value', type=str, dest='notify_pv_value', default='1', help='Value for the notification channel; for the Area Detector PVA driver PV this should be set to "Acquire" (default: 1)')
    parser.add_argument('-sd', '--start-delay', type=float, dest='start_delay',  default=10.0, help='Server start delay in seconds (default: 10 seconds)')
    parser.add_argument('-rp', '--report-period', type=int, dest='report_period', default=1, help='Reporting period for publishing frames; if set to <=0 no frames will be reported as published (default: 1)')

    args, unparsed = parser.parse_known_args()
    if len(unparsed) > 0:
        print('Unrecognized argument(s): %s' % ' '.join(unparsed))
        exit(1)

    server = AdSimServer(inputDirectory=args.input_directory, inputFile=args.input_file, mmapMode=args.mmap_mode, frameRate=args.frame_rate, nf=args.n_frames, nx=args.n_x_pixels, ny=args.n_y_pixels, datatype=args.datatype, minimum=args.minimum, maximum=args.maximum, runtime=args.runtime, channelName=args.channel_name, notifyPv=args.notify_pv, notifyPvValue=args.notify_pv_value, startDelay=args.start_delay, reportPeriod=args.report_period)

    server.start()
    try:
        runtime = args.runtime + 2*args.start_delay
        time.sleep(runtime)
    except KeyboardInterrupt as ex:
        pass
    server.stop()

if __name__ == '__main__':
    main()
