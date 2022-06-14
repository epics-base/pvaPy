## Release 4.2.1 (2022/MM/DD)

- Added ability to generate frames with different data types using
  the area detector simulator

## Release 4.2.0 (2022/06/07)

- added support for NtEnum 
- updated structure IDs for NtScalar fields
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.6.1
  - BOOST      = 1.78.0
  - NUMPY      >= 1.22

## Release 4.1.1 (2022/03/22)

- added pip support for OSX and ARM64
- enabled assigning IDs for structure fields
- updated NtNdArray object with IDs for structure fields to
  make it compatible with standard normative type
- improved frame rate accuracy for area detector simulator 
- added ability to read image data from a given folder
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.6.1
  - BOOST      = 1.78.0
  - NUMPY      >= 1.22

## Release 4.1.0 (2022/01/26)

- introduced PVA server mirror class
- added distributor plugin for the PVA server (see the
  [plugin documentation](dataDistributorPlugin.md))
- restricted numpy version to >= 1.22, as these versions resolve
  issues with boost python segfaults
- introduced PVAPY_EPICS_LOG_LEVEL environment variable to control 
  logging in the epics PVA libraries; allowed log levels correspond to 
  pvAccess library logger: 0 (all messages) to 7 (no messages; default)
- added command line utilities:
  - pvapy-mirror-server
  - pvapy-ad-sim-detector
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.6.1
  - BOOST      = 1.78.0
  - NUMPY      >= 1.22

## Release 4.0.3 (2021/09/22)

- added isConnected() method to the Channel class
- introduced semi-permanent threads for asyncGet/asyncPut interfaces
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.6
  - BOOST      = 1.72.0
  - NUMPY      < 1.21

## Release 4.0.2 (2021/09/14)

- fixed decoding error for strings that cannot be handled with utf-8 codec
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.6
  - BOOST      = 1.72.0
  - NUMPY      < 1.21

## Release 4.0.1 (2021/09/12)

- updated asyncGet/asyncPut interfaces to allow exception callbacks, and fixed
  async connection issue (GitHub Issue #68, PR #69)
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.6
  - BOOST      = 1.72.0
  - NUMPY      < 1.21

## Release 4.0.0 (2021/08/12)

- disabled info printout for PvaServer unless the debug log level is
  set to INFO (8) or DEBUG (16) 
- fixed issue with channel puts involving objects like PvTimeStamp or
  PvAlarm (GitHub Issue #65, PR #66)
- made performance enhancements for channel gets and puts in
  multithreaded applications
- added asyncGet/asyncPut interfaces to the Channel class
- restricted numpy version to < 1.21 for conda and pip packages until 
  boost python segfaults are resolved
- added support for access security in the PvaServer class
- fixed issue with timestamp processing in the PyPvRecord class
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.6
  - BOOST      = 1.72.0
  - NUMPY      < 1.21

## Release 3.1.0 (2021/02/12)

- added support for MultiChannel put and monitor
- added PvValueAlarm, PvControl, and NtScalar wrapper classes
- fixed numpy initialization problem 
- added error handling to RpcServer class
- added copy method to PvObject class
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.4.1
  - BOOST      = 1.72.0
  - NUMPY      > 1.15

## Release 3.0.0 (2020/08/14)

- added support for windows
- added ability to retrieve PvObject field as a string
- added NtNdArray and its related wrapper classes
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.4
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 2.3.0 (2020/06/12)

- fixed issue with channel puts involving large integers (GitHub Issue #58)
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.4
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 2.2.0 (2020/04/21)

- added support for connection callback
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.3.1
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 2.1.0 (2020/01/20)

- added JSON support from pvaClientCPP (GitHub PR #52)
- fixed documentation build for older sphinx versions 
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.3.1
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 2.0.1 (2019/11/18)

- no source code changes
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.3.1
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 2.0.0 (2019/08/09)

- added MultiChannel class, which allows simultaneous retrieval of PV data 
  from multiple channels
- added getName() method to the Channel class python interface
- fixed RPC request timeout issue when both RpcClient and RpcServer instances 
  use the same interpreter (GitHub PR #50)
- fixed PvaServer issue with removal of connected records (GitHub Issue #49, 
  PR #51)
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.3
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 1.6.0 (2019/05/10)

- added ability to set structure fields using PvObjects
- fixed issue with setting variant unions using PvObjects
- added automated tests to conda packaging
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.2.2
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 1.5.0 (2019/04/25)

- added dictionary methods to PvObject class: items(), keys(), values()
- fixed build warning for multiple inclusion of build rules
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.2.2
  - BOOST      = 1.70.0
  - NUMPY      > 1.15

## Release 1.4.0 (2019/03/29)

- added method for removing all records from PvaServer
- added start/stop methods for PvaServer
- resolved issue with PvaServer record write callbacks hanging if both client
  and server are running in the same interpreter
- added support for building local installation of epics/boost/pvapy
- resolved several issues with pip packaging
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.2.1
  - BOOST      = 1.69.0
  - NUMPY      > 1.15

## Release 1.3.0 (2019/01/24)

- fixed build issues related to EpicsHostArch script location
- updated code to reflect changes in CA provider 
- conda/pip package dependencies: 
  - EPICS BASE = 7.0.2
  - BOOST      = 1.69.0
  - NUMPY      > 1.15

## Release 1.2.0 (2018/07/11)

- added ability to set numeric scalar arrays using NumPy arrays directly
- fixed issue with building documentation when multiple versions of python
  are present
- introduced support for building pvapy pip packages

## Release 1.1.0 (2018/04/30)

- added introspection interface to Channel class
- fixed configuration issue for OSX
- introduced support for building pvapy conda packages
- replaced old (c)sh setup files with new PYTHONPATH-only and and full (PATH,
  PYTHONPATH, LD_LIBRARY_PATH) file
- updated doc build so that it uses information in CONFIG_SITE.local; users
  are no longer required to configure python-related variables before
  building documentation 
- fixed issue with accessing NumPy arrays from temporary objects
- fixed issue with RPC service segfaulting while returning result
- fixed issue with RPC service non-interactive mode (listen() method call)
- added version string to the pvaccess module
- added ability to specify timeout for RPC client

## Release 1.0.0 (2018/01/04)

- added build support for python3 
- added build support for EPICS7 releases

## Release 0.9 (2017/09/17)

- improved support for channel monitors: no monitor startup thread results in
  faster initial connections; monitors connect automatically when channels 
  come online 
- fixed support for older EPICS v4 releases (4.4, 4.5 and 4.6)
- added build support for numpy included with boost releases 1.63.0 and later

## Release 0.8 (2017/07/17)

- added new Channel monitor() method that can be used instead of 
  subscribe()/startMonitor() sequence in case when there is only one 
  subscriber 
- enhanced PvaServer functionality: single server instance can serve multiple
  channels; channels can be added and removed dynamically; added (optional) 
  callback for channel writes

## Release 0.7 (2017/05/10)

- added initial version of PvaServer class: PvObject exposed via instance of
  this class can be retrieved and manipulated using standard PVA command line 
  tools and APIs
- fixed boolean array conversion to python list
- improved support for builds relying on custom boost installation

## Release 0.6 (2016/09/15)

- added support for channel putGet() and getPut() operations
- introduced PvObject support for field path notation (e.g, 'x.y.z')
- introduced PvObject support for __getitem__, __setitem__, __contains__ 
- new constructor for PvObject allows optional value dictionary
- added PvObject support for retrieving numeric scalar arrays as
  read-only numpy arrays (requires compiling with Boost.NumPy)
- added ability to specify PV request in RPC client code
- fixed PvObject type ID issues
- reworked monitor code to allow two processing modes: immediate processing
  and processing in a separate thread (requires copying/queueing monitor data);
  the processing thread mode now utilizes monitor requester callbacks, and
  unnecessary monitor polling when monitor queue is full has been eliminated

## Release 0.5 (2015/10/08)

- added support for unions (both variant and restricted)
- added getObject/setObject methods for retrieving/setting field values
  as/using native python types
- reorganized build configuration to better adhere to EPICS standards; moved
  setup scripts under `bin/$EPICS_HOST_ARCH` directory, and python .so
  library under `lib/python/$PYTHON_VERSION/$EPICS_HOST_ARCH` directory;
  introduced build configuration support for production deployment mode
- enhanced documentation with additional examples
- removed code for building against EPICS 4.3.0
- converted to PVA API from pvAccessCPP module
- README and Release notes converted to Markdown

## Release 0.4 (2014/12/03)

- added interactive mode for RPC server
- added ability to specify provider name (enables channel access)
- implemented number of convenience functions for channel put, which
  eliminates need to wrap standard python types with corresponding PvObject
  class instance (i.e., rather than having to do channel.put(PvDouble(3.3)),
  one can now invoke simply channel.put(3.3))
- updated code for most recent C++ API changes
- added documentation framework based on sphinx

## Release 0.3 (2014/05/09)

- added support for PvTimeStamp
- added support for PvAlarm
- added support for NtTable
- Channel class can now take arbitrary request descriptors for get(), put()
  and startMonitor()
- build configuration scripts check for correct RPC API version

## Release 0.2 (2013/12/13)

- automated build configuration (RELEASE.local, CONFIG_SITE.local and
  user setup scripts can be generated automatically using autoconf scripts,
  which also check for required libraries)
- added PV monitor functionality

## Release 0.1 (2013/11/13)

- initial prototype
- implemented wrappers for generic PV structure (PvObject), PV scalars and
  scalar arrays
- implemented PV get/put
- implemented RPC client/server
- initial exception handling
- uses epics build structure, with some manual configuration steps
