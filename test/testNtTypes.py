#!/usr/bin/env python

import numpy as np
import random
import math
import pickle
from pvaccess import NtTable
from pvaccess import NtNdArray
from pvaccess import NtAttribute
from pvaccess import NtEnum
from pvaccess import NtScalar
from pvaccess import PvDimension
from pvaccess import PvCodec
from pvaccess import PvInt
from pvaccess import INT
from pvaccess import UBYTE
from testUtility import TestUtility


class TestNtTypes:

    #
    # NtTable
    #

    def test_NtTable(self):
        print()
        nColumns = random.randint(3,10)
        dim = random.randint(10,100)
        print('Using {} columns of size: {}'.format(nColumns,dim))
        maxInt = int(math.fabs(TestUtility.getRandomInt()))
        print('Using Max Int for array element: {}'.format(maxInt))
        ntTable = NtTable(nColumns, INT)
        labels = []
        columns = []
        for i in range(0, nColumns):
             labels.append('Col%d' % (i+1))
             columns.append(list(np.random.randint(-maxInt,maxInt, size=dim, dtype=np.int32)))
             ntTable.setColumn(i, columns[i])
        ntTable.setLabels(labels)
        TestUtility.assertListEquality(ntTable['labels'], labels)
        for i in range(0, nColumns):
            a1 = ntTable.getColumn(i)
            a2 = columns[i]
            TestUtility.assertListEquality(a1,a2)
        

    #
    # NtNdArray
    #

    def test_NtNdArray(self):
        print()
        timeStamp = TestUtility.getTimeStamp()
        id = random.randint(0,100000)
        nx = 1024
        ny = 1024
        nda = NtNdArray()
        nda['uniqueId'] = id
        dims = [PvDimension(nx, 0, nx, 1, False), PvDimension(ny, 0, ny, 1, False)]
        nda['codec'] = PvCodec('pvapyc', PvInt(14))
        nda['dimension'] = dims
        nda['descriptor'] = 'PvaPy Simulated Image'
        nda['compressedSize'] = nx*ny
        nda['uncompressedSize'] = nx*ny
        nda['timeStamp'] = timeStamp
        nda['dataTimeStamp'] = timeStamp
        attrs = [NtAttribute('ColorMode', PvInt(0))]
        nda['attribute'] = attrs
        value = np.random.randint(0,256, size=nx*ny, dtype=np.uint8)
        nda['value'] = {'ubyteValue' : value}
        value2 = nda['value'][0]['ubyteValue']
        print('Comparing image arrays {} to {}'.format(value2, value))
        assert(np.array_equiv(value, value2))

        s = pickle.dumps(nda)
        nda2 = pickle.loads(s)
        value2 = nda2['value'][0]['ubyteValue']
        print('After pickling, comparing image arrays {} to {}'.format(value2, value))
        assert(np.array_equiv(value, value2))

    #
    # NtScalar
    #
    def test_NtScalar(self):
        print()
        value = random.randint(0,100000)
        s = NtScalar(INT, value)
        value2 = s['value']
        print('Comparing scalar value {} to {}'.format(value2, value))
        assert(value==value2)

    #
    # NtEnum
    #
    def test_NtEnum(self):
        print()
        maxInt = random.randint(2,100)
        indices = list(range(0,maxInt))
        choices = list(map(lambda i: 'Ch%d' % i, indices))
        current = random.randint(0,maxInt-1)
        e = NtEnum(choices, current) 
        current2 = e['value.index']
        print('Comparing enum choice {} to {}'.format(current2, current))
        assert(current==current2)
        current = random.randint(0,maxInt)
        e['value.index'] = current
        current2 = e['value.index']
        print('Comparing enum choice {} to {}'.format(current2, current))
