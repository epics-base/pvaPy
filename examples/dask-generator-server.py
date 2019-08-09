#!/usr/bin/env python

# This server example produces a collection of waveforms and exposes it over 
# "server1_data" channel. It can be used for measuring PVA server performance.

# This example can be used together with daq-server2, which monitors
# "server1_data" channel, and exposes the same data on the "server2_data" 
# channel.

# Modified from the original example code by Tom Fors to allow
# measuring performance, array and data rates.

from math import pi, sin, asin
import time 
import numpy as np
import pvapy

from queue import Queue
from collections import OrderedDict
from distributed import Client
from threading import Event, Thread

MEGABYTE = 1024*1024
GIGABYTE = 1024*1024*1024
ARRAY_SIZE = 10*1024
N_FLAT_ARRAYS = 25

CLUSTER_PORT = 33333
#CLUSTER_MACHINES = ['127.0.0.1', '127.0.0.1', '127.0.0.1', '127.0.0.1', '127.0.0.1', '127.0.0.1'] # , 'bluegill1', 'bluegill1']
CLUSTER_MACHINES = ['127.0.0.1', '127.0.0.1', '127.0.0.1', '127.0.0.1', '127.0.0.1']
N_CLIENTS = len(CLUSTER_MACHINES)
N_INITIAL_OBJECTS = N_CLIENTS*10

class DaqDataStruct(dict):
    def __init__(self, nFlatArrays = 0):
        dict.__init__(self)
        self['ArrayId'] = pvapy.UINT
        self['Time']     = [pvapy.DOUBLE]
        self['Sinusoid'] = [pvapy.FLOAT]
        self['Triangle'] = [pvapy.FLOAT]
        self.nFlatArrays = nFlatArrays
        self.flatArrayNames = {}
        for i in range (0, nFlatArrays):
            arrayName = 'Flat%03d' % (i+1)
            self.flatArrayNames[i] = arrayName
            self[arrayName] = [pvapy.FLOAT]

    def calculateValueStructSizeInB(self, arraySize):
        dataSize = 4                                  # ArrayId
        dataSize += arraySize*8*1                     # Time
        dataSize += arraySize*4*(self.nFlatArrays+2)  # Sinusoid, Triangle, FlatXYZ
        return dataSize

    def getFlatArrayName(self, i):
        return self.flatArrayNames[i]

class DaqObjectGenerator:
    DEFAULT_ARRAY_SIZE = 10*1024
    DEFAULT_N_FLAT_ARRAYS = 25
    DEFAULT_DELTA_T = 0.001

    def __init__(self, arraySize = DEFAULT_ARRAY_SIZE, nFlatArrays = DEFAULT_N_FLAT_ARRAYS, deltaT = DEFAULT_DELTA_T):
        self.arraySize = arraySize
        self.nFlatArrays = nFlatArrays
        self.deltaT = deltaT
        self.daqDataStruct = DaqDataStruct(nFlatArrays)
        self.valueStructSizeInB = self.daqDataStruct.calculateValueStructSizeInB(arraySize)
        self.valueStructSizeInMB = float(self.valueStructSizeInB)/MEGABYTE

    def generate(self, objectId):
        #sTime = time.time()
        t0 = objectId*self.deltaT*self.arraySize
        timeArray = [t0+self.deltaT*i for i in range(0, self.arraySize)]
        sinusoid = [sin(2*pi*1.1*t + pi/2) for t in timeArray]
        triangle = [(2/pi)*asin(sin(2*pi*1.1*t)) for t in timeArray]
        #sinusoid = []
        #triangle = []
        valueStruct = {
            'ArrayId' : objectId, 
            'Time'    : np.array(timeArray, dtype=np.double), 
            'Sinusoid': np.array(sinusoid, dtype=np.float32),
            'Triangle': np.array(triangle, dtype=np.float32)
        }
        for i in range (0, self.nFlatArrays):
            arrayName = self.daqDataStruct.getFlatArrayName(i)
            f = float(i) + (objectId % self.arraySize)
            array = np.full((self.arraySize), f, dtype=np.float32)
            valueStruct[arrayName] = array
        pv = pvapy.PvObject(dict(self.daqDataStruct), valueStruct)
        #eTime = time.time()
        #dTime = eTime-sTime
        #print('ArrayId: %s, Time: %s' % (objectId, dTime))
        return pv

class DaskClient(Thread):
    def __init__(self, clientUrl, clientId, daqObjectGenerator, resultQ):
        Thread.__init__(self, name='DaskClient-%s' % clientId)
        self.client = Client(clientUrl)
        self.clientId = clientId
        self.daqObjectGenerator = daqObjectGenerator
        self.resultQ = resultQ
        self.idQ = Queue()
        self.remoteIdQ = self.client.scatter(self.idQ)
        self.generatorQ = self.client.map(self.daqObjectGenerator.generate, self.remoteIdQ)
        self.pvQ = self.client.gather(self.generatorQ)
        self.nGenerated = 0
        self.event = Event() 

    def putTask(self, objectId):
        #t0 = time.time()
        self.idQ.put(objectId)
        #t1 = time.time()
        #dt = t1-t0
        #print('PUSH TASK: %s' % dt)
        #self.event.set()

    def getPv(self, timeout=None):
        #t0 = time.time()
        pv = self.pvQ.get(timeout=timeout)
        #t1 = time.time()
        #dt = t1-t0
        #print('GET PV: %s' % dt)
        return pv
       
    def run(self):
        print('STARTING THREAD, CLIENT ID: %s' % self.clientId)
        while True:
            pv = self.pvQ.get(timeout=None)
            self.nGenerated += 1
            #print('GOT PV , CLIENT ID %s: %s' % (self.clientId, pv['ArrayId']))
            #print('CLIENT ID %s: N GENERATED=%s' % (self.clientId, self.nGenerated))
            self.resultQ.put((pv, self.clientId))
             
if __name__ == '__main__':
    daqObjectGenerator = DaqObjectGenerator(arraySize=ARRAY_SIZE, nFlatArrays=N_FLAT_ARRAYS)
    #client = Client('tcp://127.0.0.1:33333')
    #print(client)

    #idQ = Queue()
    #remoteIdQ = client.scatter(idQ)
    #generatorQ = client.map(daqObjectGenerator.generate, remoteIdQ)
    #pvQ = client.gather(generatorQ)

    daskClientList = []
    clientId = 0
    resultQ = Queue()
    for host in CLUSTER_MACHINES:
        clientUrl = 'tcp://%s:%s' % (host, CLUSTER_PORT)
        daskClient = DaskClient(clientUrl, clientId, daqObjectGenerator, resultQ)
        daskClient.start()
        daskClientList.append(daskClient)
        clientId += 1

    nGenerated = 0
    nPublished = 0
    print('BEGIN PREP')
    t0 = time.time()
    for objectId in range(0, N_INITIAL_OBJECTS):
        #idQ.put(objectId)
        clientId = objectId % N_CLIENTS
        daskClient = daskClientList[clientId]
        daskClient.putTask(objectId)
        time.sleep(0.01)
    t1 = time.time()
    dt = t1-t0
    print('PREP DONE in %.3f seconds' % dt)
    print('BEGIN SERVING OBJECTS')
    server = pvapy.PvaServer('server1_data', pvapy.PvObject(dict(daqObjectGenerator.daqDataStruct)))
    publishedObjectId = 0
    generatedObjectMap = {}
    startTime = time.time()
    while True:
        objectId += 1
        #clientId = objectId % N_CLIENTS
        #idQ.put(objectId)
        ##daskClient.putTask(objectId)
        #pv = pvQ.get(timeout=None)
        t0 = time.time()
        #pv = daskClient.getPv(timeout=None)
        ##pv = resultQ.get(timeout=None)
        (pv,clientId) = resultQ.get(timeout=None)
        daskClient = daskClientList[clientId]
        daskClient.putTask(objectId)
        pvId = pv['ArrayId']
        generatedObjectMap[pvId] = pv
        while True:
            updatePv = generatedObjectMap.get(publishedObjectId)
            if updatePv:
                server.update(updatePv)
                del generatedObjectMap[publishedObjectId]
                publishedObjectId += 1
                nPublished += 1
            else:
                break

        nGenerated += 1
        if nGenerated % 1000 == 0:
            deltaT = time.time() - startTime
            arrayRate = nGenerated / deltaT
            dataRateInBps = daqObjectGenerator.valueStructSizeInB*arrayRate
            dataRateInMbps = daqObjectGenerator.valueStructSizeInMB*arrayRate
            print('*****************')
            print('Runtime: %.3f [s], Generated Arrays: %s, Published Arrays: %s, Array Size: %s [B], %.3f [MB], Array Rate: %.3f [Hz], Data Rate: %.3f [B/s], %.3f [MB/s]' % (deltaT, nGenerated, nPublished, daqObjectGenerator.valueStructSizeInB, daqObjectGenerator.valueStructSizeInMB, arrayRate, dataRateInBps, dataRateInMbps))
            print('Generated object map size: %s' % len(generatedObjectMap))
            print('Client Stats:')
            for c in daskClientList:
                print('Client id: %s, Generated Arrays: %s' % (c.clientId, c.nGenerated))
            print('')



