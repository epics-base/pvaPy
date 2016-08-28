## Release 0.6 ()

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
