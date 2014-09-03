TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================


#=============================

LOADABLE_LIBRARY_HOST += pvaccess
LOADABLE_SHRLIB_PREFIX = 

#PROD_HOST_linux-x86_64 += testClient
PROD_HOST += $(PROD_HOST_$(T_A))

CXXFLAGS += -g -I$(BOOST_DIR)/include $(PVA_PY_CPPFLAGS)
LDFLAGS += $(PVA_PY_LDFLAGS)


pvaccess_SRCS += pvaccess.cpp
pvaccess_SRCS += CaClient.cpp
pvaccess_SRCS += Channel.cpp
pvaccess_SRCS += ChannelGetRequesterImpl.cpp
pvaccess_SRCS += ChannelMonitorRequesterImpl.cpp
pvaccess_SRCS += ChannelPutRequesterImpl.cpp
pvaccess_SRCS += ChannelRequesterImpl.cpp
#pvaccess_SRCS += ChannelRpcServiceImpl.cpp
pvaccess_SRCS += ChannelTimeout.cpp
pvaccess_SRCS += FieldNotFound.cpp
pvaccess_SRCS += GetFieldRequesterImpl.cpp
pvaccess_SRCS += InvalidArgument.cpp
pvaccess_SRCS += InvalidDataType.cpp
pvaccess_SRCS += InvalidRequest.cpp
pvaccess_SRCS += InvalidState.cpp
pvaccess_SRCS += NtTable.cpp
pvaccess_SRCS += NtType.cpp
pvaccess_SRCS += ObjectNotFound.cpp
pvaccess_SRCS += PvaClient.cpp
pvaccess_SRCS += PvaException.cpp
pvaccess_SRCS += PvaExceptionTranslator.cpp
pvaccess_SRCS += PvaPyLogger.cpp
pvaccess_SRCS += PvAlarm.cpp
pvaccess_SRCS += PvBoolean.cpp
pvaccess_SRCS += PvByte.cpp
pvaccess_SRCS += PvDouble.cpp
pvaccess_SRCS += PvFloat.cpp
pvaccess_SRCS += PvInt.cpp
pvaccess_SRCS += PvLong.cpp
pvaccess_SRCS += PvObject.cpp
pvaccess_SRCS += PvProvider.cpp
pvaccess_SRCS += PvScalar.cpp
pvaccess_SRCS += PvScalarArray.cpp
pvaccess_SRCS += PvShort.cpp
pvaccess_SRCS += PvString.cpp
pvaccess_SRCS += PvTimeStamp.cpp
pvaccess_SRCS += PvUByte.cpp
pvaccess_SRCS += PvUInt.cpp
pvaccess_SRCS += PvULong.cpp
pvaccess_SRCS += PvUShort.cpp
pvaccess_SRCS += PvUtility.cpp
pvaccess_SRCS += PyGilManager.cpp
pvaccess_SRCS += PyPvDataUtility.cpp
pvaccess_SRCS += PyUtility.cpp
pvaccess_SRCS += RequesterImpl.cpp
#pvaccess_SRCS += RpcChannelImpl.cpp
#pvaccess_SRCS += RpcChannelProviderFactory.cpp
#pvaccess_SRCS += RpcChannelProviderImpl.cpp
pvaccess_SRCS += RpcClient.cpp
#pvaccess_SRCS += RpcServerContextImpl.cpp
pvaccess_SRCS += RpcServiceImpl.cpp
pvaccess_SRCS += RpcServer.cpp
pvaccess_SRCS += RpcTimeout.cpp
pvaccess_SRCS += StringUtility.cpp

pvaccess_LIBS += pvData
pvaccess_LIBS += pvAccess
pvaccess_LIBS += Com
USR_SYS_LIBS += $(BOOST_PYTHON_LIB)

testClient_SRCS += testClient.cpp
testClient_LIBS += pvaccess
testClient_LIBS += python
testClient_LIBS += boost_python
testClient_LIBS += pvData pvAccess Com

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


