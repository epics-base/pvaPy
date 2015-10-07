# pvaPy - pvAccess for Python

## Prerequisites

The pvAccess for Python package requires recent versions of the following
software:

1. EPICS Base (v3.14.12.3 or 3.15.2)
2. EPICS4 CPP release (v4.4.0 or v4.5.0)
3. Python development header files/libraries (v2.6.6)
4. Boost (v1.41.0); must have the boost_python library built.
5. Standard development tools (gcc, make, autoconf, etc.)
6. Sphinx (optional, for generating documentation)

The Software versions listed above were used for prototype development and
testing. It is likely that any recent version of Python and the Boost libraries
(such as those that come with current linux distributions) should work.
Similarly, any recent version of EPICS Base that is supported by EPICS4 CPP
should be fine. EPICS4 CPP versions newer than v4.5.0 may not work.

Nothing special needs to be done when building the EPICS4 CPP modules. Ensure
that the EPICS Base installation you use for this module is the same one that
was used to build the EPICS4 modules.

This module has not been adapted for use on Microsoft Windows. Only Unix-like
operating systems (e.g. Linux, MacOS, Solaris) are currently supported.


## Build

### 1. Configure pvaPy.

This can be done manually, or using autoconf.

For manual configuration: Read the comments in both the `configure/RELEASE` and
`configure/CONFIG_SITE` files and follow the instructions given there.

For automatic configuration: In the top level directory run

```sh
  $ make configure EPICS_BASE=/epics/base/path EPICS4_DIR=/epics/v4/path
```

In the above command replace `/epics/base/path` with the full path to your
EPICS Base directory, and `/epics/v4/path` with the full path to your top level
directory containing the v4 modules pvDataCPP, pvAccessCPP, etc.
Note that you can only use the automatic configuration if the v4 modules have
not been renamed.

The `make configure` command will check for your Boost/Python libraries, and
create suitable `configure/RELEASE.local` and `configure/CONFIG_SITE.local` files.
They should look roughly like the examples below:

```sh
  $ cat RELEASE.local
  PVACLIENT = /home/epics/v4/pvaClientCPP
  PVACCESS = /home/epics/v4/pvAccessCPP
  NORMATIVETYPES = /home/epics/v4/normativeTypesCPP
  PVDATA = /home/epics/v4/pvDataCPP
  EPICS_BASE = /home/epics/base-3.14.12.5

  $ cat CONFIG_SITE.local
  PVA_PY_CPPFLAGS = -I/usr/include -I/usr/include/python2.6
  PVA_PY_LDFLAGS = -L/usr/lib64 -L/usr/lib64 -lpython2.6
  PVA_PY_SYS_LIBS = boost_python-mt
  PVA_API_VERSION = 450
  PVA_RPC_API_VERSION = 440
```

The above files were created automatically on a 64-bit RHEL 6.6 machine, with
the following boost/python packages installed:

```sh
  $ rpm -q boost-python python-devel
  boost-python-1.41.0-25.el6.x86_64
  python-devel-2.6.6-52.el6.x86_64
```

### 2. Compile the pvaPy source.

In the top level package directory run:

```sh
  $ make
```

This will create and install a loadable library named `pvaccess.so` under the
`lib/python` directory which can be imported directly by Python.

This also creates `setup.sh` and `setup.csh` files in the `bin/$EPICS_HOST_ARCH`
directory that configure PYTHONPATH for using the pvaccess Python module, e.g.:

```sh
  $ cat setup.sh
  #!/bin/sh
  #
  # pvaPy sh setup script
  #
  # modifies PYTHONPATH environment variable
  #
  if test -z "$PYTHONPATH" ; then
      export PYTHONPATH=/home/epics/v4/pvaPy/lib/python/2.6/linux-x86_64
  else
      export PYTHONPATH=/home/epics/v4/pvaPy/lib/python/2.6/linux-x86_64:$PYTHONPATH
  fi
```

These files must be sourced to use, e.g.:

```sh
  $ . /home/epics/v4/pvaPy/bin/linux-x86_64/setup.sh
  $ echo $PYTHONPATH
  /home/epics/v4/pvaPy/lib/python/2.6/linux-x86_64
```
or for csh users:

```sh
  % source /home/epics/v4/pvaPy/bin/linux-x86_64/setup.csh
  % echo $PYTHONPATH
  /home/epics/v4/pvaPy/lib/python/2.6/linux-x86_64
```

### 3. Generate documentation.

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
