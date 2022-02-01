# PvaPy - PvAccess for Python

The PvaPy package is a Python API for EPICS7. It supports both PVA and CA
providers, all standard EPICS7 types (structures, scalars, unions, etc), 
standard set of channel operations (put/get, monitor), RPC client/server, 
PVA server, etc. It uses Boost/Python to wrap EPICS7 C++ libraries and APIs, 
and is currently supported on Linux, OSX and Windows.

## Installation 

At the moment prebuilt PvaPy packages exist for the Conda and 
PyPI package management system. If none of the prebuilt packages
work for you, or if you need 32-bit Linux packages, you will have to build 
PvaPy from sources.

### Conda 

- Supported platforms: Linux (64-bit), OSX 
- Supported Python versions: Python2, Python3

Make sure `conda` command is in your path, and execute the following command:

```sh
  $ conda install -c epics pvapy
```

This should result in installation of pvapy, pvapy-boost and epics-base 
Conda packages.

### PyPI

- Supported platforms: Linux (64-bit), Windows (64-bit), OSX (64-bit), ARM (64-bit)
- Supported Python versions: Python2, Python3

Make sure `pip` command is in your path, and execute the following command:

```sh
  $ pip install pvapy
```

PvaPy pip packages contain all needed epics base and boost libraries.

## Build 

This section is intended for those users that would like to build PvaPy from 
the source code.

### Prerequisites

Building PvaPy from source requires recent versions of the following software:

1. Standard development tools (gcc, make, autoconf, etc.)
2. Python development header files/libraries (>=v2.6.6)
3. Sphinx/Sphinx RTD Theme (optional, for generating documentation)

There are two ways of building and installing PvaPy from sources: automated
and custom. The automated local install will download and build predefined
versions of EPICS Base, Boost and PvaPy. Custom local install relies on already existing EPICS Base and Boost libraries, and builds only PvaPy. 

### Automated Build

Automated build works under Linux and OSX.
Make sure the desired version of Python is in your path, and execute the 
following command from the top of the PvaPy source directory:

```sh
  $ PREFIX=/local/install/path make local
```

This should build and install predefined versions of EPICS Base, Boost Python 
and PvaPy libraries under the directory specified by the PREFIX variable. If
PREFIX is not specified, all software will be installed under the `tools/local`
subdirectory of the PvaPy distribution.

### Custom Build

In addition to the Python-related prerequsites, the custom build requires
the following software already built:

1. EPICS Base (>=v3.14.12.3)
2. EPICS4 CPP release (>=v4.4.0); note that EPICS7 release includes 
all needed v4 modules
3. Boost (>=v1.41.0); must have the boost_python library built
4. Boost.NumPy (optional, for retrieving numeric scalar arrays as numpy
arrays; this is not needed for Boost v1.63.0 and later)

The version numbers mentioned above are the earliest that PvaPy was tested with.
Any recent version of Python and the Boost libraries (such as those that come 
with current linux distributions) should work. Similarly, any recent version 
of EPICS Base that is supported by EPICS4 CPP should be fine. 

Nothing special needs to be done when building the EPICS4 CPP modules. Ensure
that the EPICS Base installation you use for this module is the same one that
was used to build the EPICS4 modules.

This module has not been adapted for use on Microsoft Windows. Only Unix-like
operating systems (e.g. Linux, MacOS, Solaris) are currently supported.

#### 1. Configure PvaPy

This can be done manually (all platforms), or using autoconf (on Linux
and OSX).

For manual configuration: Read the comments in both the `configure/RELEASE` and
`configure/CONFIG_SITE` files and follow the instructions given there.

For automatic configuration: In the top level directory run

```sh
  $ make configure EPICS_BASE=/epics/base/path [EPICS4_DIR=/epics/v4/path] [BOOST_NUMPY_DIR=/boost.numpy/path] [BOOST_ROOT=/boost/path] [PYTHON_VERSION=3]
```

Note that you can only use the automatic configuration if the v4 modules have
not been renamed. In the above command replace `/epics/base/path` with the full
path to your EPICS Base directory, and `/epics/v4/path` with the full path to 
your top level directory containing the v4 modules pvDataCPP, pvAccessCPP, etc. 
If you are using EPICS7 release, you can omit EPICS4_DIR argument, as 
configuration scripts will find the required v4 libraries and header files in 
the EPICS Base directory. 

The optional `BOOST_NUMPY_DIR` argument enables NumPy array support for older
Boost versions, as v1.63.0 and later releases already include NumPy libraries.
The optional `BOOST_ROOT` argument is typically used for custom Boost 
installation directory. In case you are using custom python installation that 
uses shared object libraries, you may need to set PATH and LD_LIBRARY_PATH 
environment variables before running the above `make configure` command.
Also, note that building with python 3.x requires PYTHON_VERSION=3 argument.

The `make configure` command will check for your Boost/Python libraries, and
create suitable `configure/RELEASE.local` and `configure/CONFIG_SITE.local` 
files that are used by the build process. They should look roughly like the 
examples below:

```sh
  $ cat RELEASE.local
  PVACLIENT = /home/epics/v4/pvaClientCPP
  PVDATABASE = /home/epics/v4/pvDatabaseCPP
  NORMATIVETYPES = /home/epics/v4/normativeTypesCPP
  PVACCESS = /home/epics/v4/pvAccessCPP
  PVDATA = /home/epics/v4/pvDataCPP
  EPICS_BASE = /home/epics/base-3.15.5

  $ cat CONFIG_SITE.local
  PVAPY_CPPFLAGS = -I/usr/include -I/usr/include/python2.7
  PVAPY_LDFLAGS = -L/usr/lib64 -lpython2.7
  PVAPY_SYS_LIBS = boost_python  
  PVA_API_VERSION = 480
  PVA_RPC_API_VERSION = 480
  HAVE_BOOST_NUMPY = 0
  HAVE_BOOST_PYTHON_NUMPY = 0
  PYTHON_VERSION = 2.7
  PVAPY_PYTHON = /bin/python
  PVAPY_PYTHONPATH = /home/epics/v4/pvaPy/lib/python/2.7/linux-x86_64
  PVAPY_LD_LIBRARY_PATH = /usr/lib64
  PVAPY_EPICS_BASE = /home/epics/base-3.15.5
  PVAPY_EPICS4_DIR = /home/epics/v4
  PVAPY_HOST_ARCH = linux-x86_64
  PVAPY_SETUP_SH = /home/epics/pvaPy/bin/linux-x86_64/pvapy_setup_full.2.7.sh
```

The above files were created automatically on a 64-bit RHEL 7.4 machine, with
the following boost/python packages installed:

```sh
  $ rpm -q boost-python python-devel
  boost-python-1.53.0-27.el7.x86_64
  python-devel-2.7.5-58.el7.x86_64
```

Note that the automatic configuration process also creates 
`pvapy_setup_full.$PYTHON_VERSION.(c)sh` and 
`pvapy_setup_pythonpath.$PYTHON_VERSION.(c)sh` files in the 
`bin/$EPICS_HOST_ARCH` directory. The full setup files modify PATH, 
LD_LIBRARY_PATH and PYTHONPATH environment variables, while the pythonpath
setup files modify only PYTHONPATH variable. For example,

```sh
  $ cat pvapy_setup_pythonpath.2.7.sh
  #!/bin/sh
  #
  # modifies PYTHONPATH environment variable
  #
  if test -z "$PYTHONPATH" ; then
      export PYTHONPATH=/home/epics/v4/pvaPy/lib/python/2.7/linux-x86_64
  else
      export PYTHONPATH=/home/epics/v4/pvaPy/lib/python/2.7/linux-x86_64:$PYTHONPATH
  fi
```

After building pvaPy, the environment setup files can be sourced to use
the built python module, e.g.:

```sh
  $ . /home/epics/v4/pvaPy/bin/linux-x86_64/pvapy_setup_pythonpath.2.7.sh
  $ echo $PYTHONPATH
  /home/epics/v4/pvaPy/lib/python/2.7/linux-x86_64
```
or for csh users:

```sh
  % source /home/epics/v4/pvaPy/bin/linux-x86_64/pvapy_setup_pythonpath.2.7.csh
  % echo $PYTHONPATH
  /home/epics/v4/pvaPy/lib/python/2.7/linux-x86_64
```
#### 2. Compile PvaPy Source

In the top level package directory run:

```sh
  $ make
```

The above command will create and install a loadable library `pvaccess.so`
under the `lib/python` directory which can be imported directly by Python.

#### 3. Generate Documentation

This step is optional and requires Sphinx to be installed:

```sh
  $ make doc
```

If a `sphinx-build` script is present on the system, html pages will be
generated in the `documentation/sphinx/_build/html` directory.

## Basic Usage: PV put/get

For simple testing, do the following:

1) In a separate terminal, start the testDbPv IOC:

```sh
  $ cd $EPICS4_DIR/pvaSrv/testTop/iocBoot/testDbPv
  $ ../../bin/$EPICS_HOST_ARCH/testDbPv st.cmd
```

2) Source the appropriate setup file from pvaPy's `bin/$EPICS_HOST_ARCH`
directory and start python (the Python PVA module is called pvaccess):

```
  $ python
  >>> import pvaccess
  >>> dir (pvaccess)
  ['BOOLEAN', 'BYTE', 'CA', 'Channel', 'DOUBLE', 'FLOAT', 'FieldNotFound',
  'INT', 'InvalidArgument', 'InvalidDataType', 'InvalidRequest', 'LONG',
  'NtTable', 'NtType', 'PVA', 'ProviderType', 'PvAlarm', 'PvBoolean', 'PvByte',
  'PvDouble', 'PvFloat', 'PvInt', 'PvLong', 'PvObject', 'PvScalar',
  'PvScalarArray', 'PvShort', 'PvString', 'PvTimeStamp', 'PvType', 'PvUByte',
  'PvUInt', 'PvULong', 'PvUShort', 'PvUnion', 'PvaException', 'RpcClient',
  'RpcServer', 'SHORT', 'STRING', 'UBYTE', 'UINT', 'ULONG', 'USHORT', '__doc__',
  '__file__', '__name__', '__package__']
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
```

In the above, note that in addition to PV object classes like PvInt, one
can also use standard Python types as arguments for channel puts.


## Basic Usage: PV monitor

1) In a separate terminal, start the testDbPv IOC:

```sh
  $ cd $EPICS4_DIR/pvaSrv/testTop/iocBoot/testDbPv
  $ ../../bin/$EPICS_HOST_ARCH/testDbPv st.cmd
```

2) PV values can be changed using the IOC shell command `dbpf`, e.g:

```
  epics> dbpr 'float01'
  ASG:                DESC:               DISA: 0             DISP: 0
  DISV: 1             NAME: float01       SEVR: MAJOR         STAT: LOLO
  TPRO: 0             VAL: 0
  epics> dbpf 'float01' 11.1
  DBR_FLOAT:          11.1
```

3) Monitor a channel in Python, passing in a subscriber object (function
that processes PvObject instance):

```python
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
```

## Advanced Usage: PVA Server Class

1) In terminal 1, create a simple 'pair' channel:

```
  $ python 
  >>> pv = PvObject({'x': INT, 'y' : INT})
  >>> pvaServer = PvaServer('pair', pv)
```

2) In terminal 2, start monitoring this channel:

```
  $ pvget -m pair
```

3) In terminal 1, update one of the object's fields:

```
  >>> pv['x'] = 1
```

This change should appear in terminal 2.

## Advanced Usage: RPC Client Class

1) In a separate terminal, start the v4 test RPC service:

```sh
  $ cd $EPICS4_DIR/pvAccessCPP/bin/$EPICS_HOST_ARCH
  $ ./rpcServiceExample # in terminal 2
```

2) RPC test channel is 'sum':

```python
  >>> rpc = pvaccess.RpcClient('sum')
  >>> request = pvaccess.PvObject({'a': pvaccess.STRING, 'b': pvaccess.STRING})
  >>> request.set({'a': '11', 'b': '22' })
  >>> print request
  structure
      string a 11
      string b 22
  >>> response = rpc.invoke(request)
  >>> print response
  structure
      double c 33
```

## Advanced Usage: RPC Server Class

### Example 1

1) In a separate terminal, source the environment file and start python:

```
  $ python # in terminal 2
  >>> import pvaccess
  >>> srv = pvaccess.RpcServer()
  >>> def echo(x):    # x is an instance of PvObject
  ...     print 'Got object: ', x
  ...     return x    # service must return an instance of PvObject
  >>> srv.registerService('echo', echo)
  >>> srv.listen()
```

2) In terminal 1, reuse previous request object

```python
  >>> rpc = pvaccess.RpcClient('echo')
  >>> response = rpc.invoke(request)
  >>> print response
  structure
      string a 11
      string b 22
```

### Example 2

1) In terminal 2:

```
  $ python
  >>> import pvaccess
  >>> srv = pvaccess.RpcServer()
  >>> def sum(x):
  ...     a = x.getInt('a')
  ...     b = x.getInt('b')
  ...     return pvaccess.PvInt(a+b)
  >>> srv.registerService('sum', sum)
  >>> srv.listen()
```

2) In terminal 1:

```python
  >>> rpc = pvaccess.RpcClient('sum')
  >>> request = pvaccess.PvObject({'a': pvaccess.INT, 'b': pvaccess.INT})
  >>> request.set({'a': 11, 'b': 22})
  >>> print request
  structure
      int a 11
      int b 22
  >>> response = rpc.invoke(request)
  >>> print response
  structure
      int value 33
```

### Example 3

1) In terminal 2:

```python
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
```

2) In terminal 1:

```python
  >>> rpc = pvaccess.RpcClient('hash')
  >>> request = pvaccess.PvString('abcd')
  >>> print rpc.invoke(request)
  structure
      string hash 0a380e7375d8c3f68d1bbe068141d6ce
      string value
```

