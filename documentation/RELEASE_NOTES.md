## Release 1.3.0 (2019/01/24)

- fixed build issues related to EpicsHostArch script location
- updated code to reflect changes in CA provider 
- updated conda build scripts for EPICS 7.0.2

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
