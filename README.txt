Prerequisites
================

The python PV Access API package requires the following software:

1) EPICS base release (v3.14.12.3)
2) EPICS4 CPP release (v4.3.0 or v4.4.0)
3) Python development header files/libraries (v2.6.6)
4) Boost (v1.41.0); build must have boost_python library
5) Standard development tools (gcc, make, autoconf, etc.) 
6) Sphinx (used for generating documentation)

Software versions listed above were used for prototype development and testing.
It is likely that any recent version of python and boost libraries (such as
those that come with current linux distributions) would work. Similarly, any
recent version of EPICS base that is supported by EPICS4 CPP should be fine.
On the other hand, using any EPICS4 CPP version other than releases v4.3.0 
or v4.4.0 might not work.

Note that nothing special needs to be done to EPICS4 CPP build. 

Build
=======

1) Configure pvaPy. In the top level directory run

$ EPICS_BASE=<EPICS_BASE> EPICS4_DIR=<EPICS4_DIR> make configure
 
For the above command replace <EPICS_BASE> with the full path to your 
epics base directory, and <EPICS4_DIR> with the full path of your v4 top level
directory containing pvDataCPP, pvAccessCPP, etc.

The "make configure" command will check for your Boost/Python libraries, and
create RELEASE.local and CONFIG_SITE.local files. They should look
like in the example below:

$ cat configure/RELEASE.local 
EPICS4_DIR=/home/sveseli/Work/support/EPICS-CPP-4.3.0
EPICS_BASE=/home/sveseli/Work/support/epics/base

$ cat configure/CONFIG_SITE.local 
EPICS_HOST_ARCH=linux-x86_64
BOOST_PYTHON_LIB=boost_python-mt
PVA_PY_CPPFLAGS=-I/usr/include -I/usr/include/python2.6
PVA_PY_LDFLAGS=-L/usr/lib64 -L/usr/lib64 -lpython2.6

The above files were created automatically on a 64-bit CentOS 6.4 machine, with
the following boost/python packages installed:

$ rpm -qa | grep boost-python
boost-python-1.41.0-17.el6_4.x86_64
$ rpm -qa | grep python-devel
python-devel-2.6.6-37.el6_4.x86_64

Note that the "make configure" command also creates setup.(c)sh files
that configure PYTHONPATH for using pvaccess python module, e.g.:

$ cat setup.sh 
#!/bin/sh
#
# pvaPy sh setup script
#
# modifies PYTHONPATH environment variable
#
if test -z "$PYTHONPATH" ; then
    export PYTHONPATH=/home/sveseli/Work/pvaPy/lib/linux-x86_64
else
    export PYTHONPATH=/home/sveseli/Work/pvaPy/lib/linux-x86_64:$PYTHONPATH
fi

2) Compile pvaPy source. In the top level package directory run:

$ make

This will create pvaccess.so library in lib/$EPICS_HOST_ARCH directory.

3) Generate python html documentation (optional, requires sphinx):

$ make doc

If sphinx-build is present on the system, html pages will be generated
in doc/sphinx/_build directory.


Basic Usage: PV put/get
=========================

For simple testing, do the following:

1) In a separate terminal, start testDbPv IOC:

cd $EPICS4_DIR/pvaSrv/testApp/iocBoot/testDbPv
../../bin/$EPICS_HOST_ARCH/testDbPv st.cmd

2) Source setup file (or export PYTHONPATH=$PVAPY_DIR/lib/$EPICS_HOST_ARCH)
and start python (python PVA module is called pvaccess):

$ python
>>> import pvaccess
>>> dir (pvaccess)
['BOOLEAN', 'BYTE', 'Channel', 'DOUBLE', 'FLOAT', 'INT', 'LONG', 'PvBoolean', 'PvByte', 'PvDouble', 'PvFloat', 'PvInt', 'PvLong', 'PvObject', 'PvScalar', 'PvScalarArray', 'PvShort', 'PvString', 'PvType', 'PvUByte', 'PvUInt', 'PvULong', 'PvUShort', 'RpcClient', 'RpcServer', 'RpcServiceImpl', 'SHORT', 'STRING', 'UBYTE', 'UINT', 'ULONG', 'USHORT', '__doc__', '__file__', '__name__', '__package__']
>>> c = pvaccess.Channel('int01')
>>> print c.get()
epics:nt/NTScalar:1.0
    int value 0
>>> c.putInt(7)
>>> print c.get()
epics:nt/NTScalar:1.0
    int value 7
>>> c.put(pvaccess.PvInt(5))
>>> print c.get()
epics:nt/NTScalar:1.0 
    int value 5

In the above, note that in addition to PV object classes like PvInt, one
can also use standard python types as arguments for channel puts.


Basic Usage: PV monitor
=========================

1) In a separate terminal, start testDbPv IOC:

$ cd $EPICS4_DIR/pvaSrv/testApp/iocBoot/testDbPv
$ ../../bin/$EPICS_HOST_ARCH/testDbPv st.cmd

2) In the IOC shell change PV values using the dbpf command, e.g:

epics> dbpr 'float01'
ASG:                DESC:               DISA: 0             DISP: 0             
DISV: 1             NAME: float01       SEVR: MAJOR         STAT: LOLO          
TPRO: 0             VAL: 0              
epics> dbpf 'float01' 11.1
DBR_FLOAT: 

3) Monitor channel in python, by passing in subscriber object (function
that processes PvObject instance):

>>> c = pvaccess.Channel('float01')
>>> def echo(x):
...     print 'New PV value:', x
... 
>>> c.subscribe('echo', echo)
>>> c.startMonitor()
>>> New PV value: epics:nt/NTScalar:1.0 
    float value 11.1

New PV value: epics:nt/NTScalar:1.0
    float value 11.2

New PV value: epics:nt/NTScalar:1.0 
    float value 11.3

>>> c.stopMonitor()


Advanced Usage: RPC Client Class
==================================

1) In a separate terminal, start v4 test RPC service:

$ cd $EPICS4_DIR/pvAccessCPP/bin/$EPICS_HOST_ARCH
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

