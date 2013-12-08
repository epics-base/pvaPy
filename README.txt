Prerequisites
================

The python PV Access API package requires the following software:

1) EPICS base release (v3.14.12.3)
2) EPICS4 CPP release (v4.3.0)
3) Python development header files/libraries (v2.7.3)
4) Boost (v1.54.0); build must have boost_python library

Software versions listed above were used for prototype development and testing.
It is likely that any recent version of python and boost libraries (such as
those that come with current linux distributions) would work. Similarly, any
recent version of EPICS base that is supported by EPICS4 CPP should be fine.
On the other hand, using any EPICS4 CPP version other than release v4.3.0 
might not work.

Note that nothing special needs to be done to EPICS4 CPP build. 

Build
=======

1) Edit configure/RELEASE.local and correct directories pointing to the
above packages.

2) make (in the top level package directory)

3) Create soft link pvaccess.so => libpvaccess.so in lib/$EPICS_HOST_ARCH
directory (needs to be done only after the first build)

4) (Optional) Prepare setup file (PYTHONPATH needs to have entry to 
$PVAPY_DIR/lib/$EPICS_HOST_ARCH). 

Basic Usage: PV put/get
=========================

For simple testing, do the following:

1) In a separate terminal, start testDbPv IOC:

cd $EPICS4_DIR/pvaSrv/testApp/iocBoot/testDbPv
../../bin/$EPICS_HOST_ARCH/testDbPv st.cmd

2) Source setup file (or export PYTHONPATH=$PVAPY_DIR/lib/$EPICS_HOST_ARCH)
and start oython (python PVA module is called pvaccess):

$ python
>>> import pvaccess
>>> dir (pvaccess)
['BOOLEAN', 'BYTE', 'Channel', 'DOUBLE', 'FLOAT', 'INT', 'LONG', 'PvBoolean', 'PvByte', 'PvDouble', 'PvFloat', 'PvInt', 'PvLong', 'PvObject', 'PvScalar', 'PvScalarArray', 'PvShort', 'PvString', 'PvType', 'PvUByte', 'PvUInt', 'PvULong', 'PvUShort', 'RpcClient', 'RpcServer', 'RpcServiceImpl', 'SHORT', 'STRING', 'UBYTE', 'UINT', 'ULONG', 'USHORT', '__doc__', '__file__', '__name__', '__package__']
>>> c = pvaccess.Channel('int01')
>>> print c.get()
uri:ev4:nt/2012/pwd:NTScalar 
    int value 0
>>> c.put(pvaccess.PvInt(5))
>>> print c.get()
uri:ev4:nt/2012/pwd:NTScalar 
    int value 5


Advanced Usage: RPC Client Class
==================================

1) In a separate terminal, start v4 test RPC service:

cd $EPICS4_DIR/pvAccessCPP/bin/$EPICS_HOST_ARCH
$ ./rpcServiceExample # in terminal 2

2) RPC test channel is “sum”: 

>>> rpc = pvaccess.RpcClient('sum')
>>> request = pvaccess.PvObject({'a' : pvaccess.STRING, 'b' : pvaccess.STRING})
>>> request.set({'a' : '11', 'b' : '22' })
>>> print request
structure 
    string a 11
    string b 22
>>> response = rpc.invoke(request)
>>> print response
structure 
    double c 33


Advanced Usage: RPC Server Class
==================================

Example 1:
----------

1) In a separate terminal, source environment file and start python:
$ python # in terminal 2
>>> import pvaccess 
>>> srv = pvaccess.RpcServer()
>>> def echo(x):    # x is an instance of PvObject
...     print 'Got object: ', x
...     return x    # service must return an instance of PvObject 
>>> srv.registerService('echo', echo)
>>> srv.listen()

2) In terminal 1, reuse previous request object

>>> rpc = pvaccess.RpcClient('echo')
>>> response = rpc.invoke(request)
>>> print response
structure 
    string a 11
    string b 22

Example 2:
----------

1) In terminal 2: 

$ python
>>> import pvaccess 
>>> srv = pvaccess.RpcServer()
>>> def sum(x):
...     a = x.getInt('a')
...     b = x.getInt('b')
...     return pvaccess.PvInt(a+b)
>>> srv.registerService('sum', sum)
>>> srv.listen()

2) In terminal 1:

>>> rpc = pvaccess.RpcClient('sum')
>>> request = pvaccess.PvObject({'a' : pvaccess.INT, 'b' : pvaccess.INT})
>>> request.set({'a' : 11, 'b' : 22})
>>> print request
structure 
    int a 11
    int b 22
>>> response = rpc.invoke(request)
>>> print response
structure 
    int value 33


Example 3:
----------

1) In terminal 2: 

>>> import pvaccess 
>>> srv = pvaccess.RpcServer()
>>> def hash(x):
...     import hashlib
...     md5 = hashlib.md5()
...     md5.update(str(x))
...     h = md5.hexdigest()
...     dict = x.getStructureDict()
...     dict['hash'] = pvaccess.STRING
...     response = pvaccess.PvObject(dict)
...     response.setString('hash', h)
...     return response
>>> srv.registerService('hash', hash)
>>> srv.listen()

2) In terminal 1:

>>> rpc = pvaccess.RpcClient('hash')
>>> request = pvaccess.PvString('abcd')
>>> print rpc.invoke(request)
structure 
    string hash 0a380e7375d8c3f68d1bbe068141d6ce
    string value 

