# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics4.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_EPICS4([MINIMUM-PVA-VERSION])
#
# DESCRIPTION
#
#   Test for EPICS V4 libraries with a minimum pvAccess version
#
#   If no path to the installed epics4 directory is given, the macro 
#   evaluates $EPICS4_DIR environment variable and searches under
#   /usr/local/epics4 directory
#
#   This macro sets:
#
#     HAVE_EPICS4
#     PVA_API_VERSION
#     EPICS4_DIR
#
# LICENSE
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 6

DEFAULT_PVA_VERSION=4.0.3

AC_DEFUN([AX_EPICS4],

[AC_ARG_WITH([epics4],
    [AS_HELP_STRING([--with-epics4=EPICS4_DIR],
        [Use EPICS4 from the specified location])
    ],
    [
        if test -z "$withval"; then 
            AC_MSG_ERROR(--with-epics4 requires directory name)
        else
            ac_epics4_dir_path="$withval"
        fi
    ],
    [ac_epics4_dir_path=""])

    if test -z "$ac_epics4_dir_path"; then
        if test -z "$EPICS4_DIR"; then
            if test -z "$EPICS_BASE"; then
                ac_epics4_dir_path="/usr/local/epics4"
            else
                ac_epics4_dir_path=$EPICS_BASE
            fi
        else
            ac_epics4_dir_path=$EPICS4_DIR
        fi
    fi
    AC_MSG_CHECKING(for EPICS4 directory $ac_epics4_dir_path)
    if ! test -d "$ac_epics4_dir_path"; then
        AC_MSG_ERROR($ac_epics4_dir_path is not a valid directory path)
    fi

    # save compile/link flags, set lang to C++
    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
    SAVED_CPPFLAGS="$CPPFLAGS"
    SAVED_LDFLAGS="$LDFLAGS"

    # look for required v4 modules, assume common epics4 install dir if not found
    pvdatacpp_dir="$ac_epics4_dir_path/pvDataCPP"
    if ! test -d "$pvdatacpp_dir"; then
        pvdatacpp_dir="$ac_epics4_dir_path"
    fi
    pvaccesscpp_dir="$ac_epics4_dir_path/pvAccessCPP"
    if ! test -d "$pvaccesscpp_dir"; then
        pvaccesscpp_dir="$ac_epics4_dir_path"
    fi
    normativetypescpp_dir="$ac_epics4_dir_path/normativeTypesCPP"
    if ! test -d "$normativetypescpp_dir"; then
        normativetypescpp_dir="$ac_epics4_dir_path"
    fi
    pvaclientcpp_dir="$ac_epics4_dir_path/pvaClientCPP"
    if ! test -d "$pvaclientcpp_dir"; then
        pvaclientcpp_dir="$ac_epics4_dir_path"
    fi
    pvdatabasecpp_dir="$ac_epics4_dir_path/pvDatabaseCPP"
    if ! test -d "$pvdatabasecpp_dir"; then
        pvdatabasecpp_dir="$ac_epics4_dir_path"
    fi

    pva_version_h="$pvaccesscpp_dir/include/pv/pvaVersion.h"
    if ! test -f "$pva_version_h"; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR("could not find pvAccess installation: no header file $pva_version_h")
    fi
    AC_MSG_RESULT([yes])

    # define EPICS4_DIR
    EPICS4_DIR=$ac_epics4_dir_path
    AC_MSG_NOTICE([EPICS4_DIR is defined as: $EPICS4_DIR])

    # determine required version
    pva_version_req=ifelse([$1], , $DEFAULT_PVA_VERSION, $1)
    AC_MSG_CHECKING(for pvAccess version >= $pva_version_req)
    pva_version_req_major=`expr $pva_version_req : '\([[0-9]]*\)'`
    pva_version_req_minor=`expr $pva_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    pva_version_req_patch=`expr $pva_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$pva_version_req_patch" == "x" ; then
        pva_version_req_patch="0"
    fi

    # options for building with pvData & pvAccess
    PVA_CPPFLAGS="-I$pvdatacpp_dir/include -I$pvaccesscpp_dir/include"
    PVA_LDFLAGS="-L$pvdatacpp_dir/lib/$EPICS_HOST_ARCH -L$pvaccesscpp_dir/lib/$EPICS_HOST_ARCH"
    PVA_LIBS="-lpvAccess -lpvData -lCom"

    # check pvAccess version number
    export CPPFLAGS="$PVA_CPPFLAGS $EPICS_CPPFLAGS"
    AC_PREPROC_IFELSE([AC_LANG_SOURCE([[
        #include "pv/pvaVersion.h"
        #undef VERSION_INT
        #undef PVA_VERSION_INT
        #define VERSION_INT(V,M,P) (((V)<<16) | ((M)<<8) | (P))
        #define PVA_VERSION_INT VERSION_INT(EPICS_PVA_MAJOR_VERSION,EPICS_PVA_MINOR_VERSION,EPICS_PVA_MAINTENANCE_VERSION)
        #if PVA_VERSION_INT < VERSION_INT($pva_version_req_major,$pva_version_req_minor,$pva_version_req_patch)
        #  error too old
        #endif
    ]])],[AC_MSG_RESULT([yes])],[
        AC_MSG_RESULT([no])
        AC_MSG_ERROR("EPICS4 installation in $ac_epics4_dir_path is too old (required: pvAccess $pva_version_req)")
    ])

    # check pvaClient version number
    AC_MSG_CHECKING(for EPICS_PVACLIENT_VERSION)
    EPICS_PVACLIENT_VERSION=0
    pvaClientVersionConfigFile=$ac_epics4_dir_path/cfg/CONFIG_PVACLIENT_VERSION
    if test -f $pvaClientVersionConfigFile ; then 
        pvaClientMajorVersion=`cat $pvaClientVersionConfigFile | grep MAJOR | grep "=" | awk '{print $NF}'`
        pvaClientMinorVersion=`cat $pvaClientVersionConfigFile | grep MINOR | grep "=" | awk '{print $NF}'`
        pvaClientMaintVersion=`cat $pvaClientVersionConfigFile | grep MAINT | grep "=" | awk '{print $NF}'`
        EPICS_PVACLIENT_VERSION=${pvaClientMajorVersion}${pvaClientMinorVersion}${pvaClientMaintVersion}
    fi
    AC_MSG_RESULT([$EPICS_PVACLIENT_VERSION])

    # check pvDatabase version number
    AC_MSG_CHECKING(for EPICS_PVDATABASE_VERSION)
    EPICS_PVDATABASE_VERSION=0
    pvDatabaseVersionConfigFile=$ac_epics4_dir_path/cfg/CONFIG_PVDATABASE_VERSION
    if test -f $pvDatabaseVersionConfigFile ; then 
        pvDatabaseMajorVersion=`cat $pvDatabaseVersionConfigFile | grep MAJOR | grep "=" | awk '{print $NF}'`
        pvDatabaseMinorVersion=`cat $pvDatabaseVersionConfigFile | grep MINOR | grep "=" | awk '{print $NF}'`
        pvDatabaseMaintVersion=`cat $pvDatabaseVersionConfigFile | grep MAINT | grep "=" | awk '{print $NF}'`
        EPICS_PVDATABASE_VERSION=${pvDatabaseMajorVersion}${pvDatabaseMinorVersion}${pvDatabaseMaintVersion}
    fi
    AC_MSG_RESULT([$EPICS_PVDATABASE_VERSION])

    # test basic libraries
    AC_MSG_CHECKING(for EPICS4 libraries for $EPICS_HOST_ARCH)
    pva_api_version=430
    pva_rpc_api_version=430

    export LDFLAGS="$PVA_LDFLAGS $EPICS_LDFLAGS"
    export LIBS="$PVA_LIBS"
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "pv/pvAccess.h"
        #include "pv/pvData.h"
        ]],
        [[
        epics::pvData::FieldConstPtrArray fields;
        ]])
    ],[succeeded=yes],[succeeded=no])
    AC_MSG_RESULT([$succeeded])
    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([no])
        AC_MSG_FAILURE(could not compile and link EPICS4 test code: check your EPICS4 installation)
    fi

    # test for pvaClientCPP and pva api version
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "pv/pvAccess.h"
        #include "pv/pvData.h"
        class RequesterImpl : public epics::pvData::Requester
        {
        public:
            RequesterImpl(const epics::pvData::String& requesterName) {}
            virtual epics::pvData::String getRequesterName() { return "requester"; }
            virtual void message(const epics::pvData::String& message, epics::pvData::MessageType messageType) {}
        };
        ]],
        [[
        epics::pvData::Requester::shared_pointer requester(new RequesterImpl("Channel"));
        epics::pvData::PVStructure::shared_pointer pvRequest = epics::pvAccess::getCreateRequest()->createRequest("field(value)", requester);
        ]])
    ],[pva_api_version=430],[pva_api_version=440])

    # options for building with normativeTypes & pvaClient
    PVAC_CPPFLAGS="-I$normativetypescpp_dir/include -I$pvaclientcpp_dir/include"
    PVAC_LDFLAGS="-L$pvaclientcpp_dir/lib/$EPICS_HOST_ARCH -L$normativetypescpp_dir/lib/$EPICS_HOST_ARCH"
    PVAC_LIBS="-lpvaClient -lnt"
    for libFile in pvAccessCA pvAccessIOC ca dbRecStd dbCore; do
        soLib=""
        if test -d "$EPICS4_DIR/lib"; then 
            soLib=`find $EPICS4_DIR/lib -name "lib$libFile.so*" | head -1`
        fi
        if ! test -z "$soLib"; then 
            PVAC_LIBS="$PVAC_LIBS -l$libFile"
        fi
    done
    AC_MSG_NOTICE([Using following libraries for pvaClient tests: $PVAC_LIBS])
    AC_MSG_CHECKING(for EPICS4 pvaClient)

    export CPPFLAGS="$PVAC_CPPFLAGS $PVA_CPPFLAGS $EPICS_CPPFLAGS"
    export LDFLAGS="$PVAC_LDFLAGS $PVA_LDFLAGS $EPICS_LDFLAGS"
    export LIBS="$PVAC_LIBS $PVA_LIBS"

    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "pv/pvaClient.h"
        ]],
        [[
        epics::pvaClient::PvaClientChannelPtr pvaClientChannel;
        ]])
    ],[pva_client_cpp=yes],[pva_client_cpp=no])
    AC_MSG_RESULT([$pva_client_cpp])

    # We got pvaClientCPP, must determine version
    AC_MSG_CHECKING(EPICS4 pva api version)
    if test "$pva_client_cpp" == "yes" ; then
        pva_api_version=450
        AC_SUBST(NORMATIVETYPESCPP_DIR, $normativetypescpp_dir)
        AC_SUBST(PVACLIENTCPP_DIR, $pvaclientcpp_dir)
        AC_SUBST(PVDATABASECPP_DIR, $pvdatabasecpp_dir)
        AC_LINK_IFELSE([AC_LANG_PROGRAM(
            [[
            #include "pv/pvaClient.h"
            ]],
            [[
            epics::pvaClient::PvaClientPtr pvaClientPtr(epics::pvaClient::PvaClient::get());
            ]])
        ],[pva_api_version=460],[pva_api_version=$pva_api_version])
        AC_LINK_IFELSE([AC_LANG_PROGRAM(
            [[
            #include "pv/pvaClient.h"
            ]],
            [[
            epics::pvaClient::PvaClientGetRequester::shared_pointer getRequesterPtr;
            ]])
        ],[pva_api_version=470],[pva_api_version=$pva_api_version])

        if test "$pva_api_version" == "470" ; then
            # Detect deprecated monitor method
            export CPPFLAGS_SAVE=$CPPFLAGS
            export CPPFLAGS="$CPPFLAGS -Werror"
            AC_LINK_IFELSE([AC_LANG_PROGRAM(
                [[
                #include "pv/pvaClient.h"
                ]],
                [[
                epics::pvaClient::PvaClientPtr pvaClientPtr(epics::pvaClient::PvaClient::get());
                epics::pvaClient::PvaClientMonitorPtr pvaClientMonitorPtr = epics::pvaClient::PvaClientMonitor::create(pvaClientPtr, "myChannel", "pva", "field(value)");
                ]])
            ],[pva_api_version=$pva_api_version],[pva_api_version=480])
            export CPPFLAGS=$CPPFLAGS_SAVE
        fi
    fi

    # Use EPICS_PVACLIENT_VERSION if it is defined
    if test "$EPICS_PVACLIENT_VERSION" != "0" ; then
        if test "$EPICS_PVACLIENT_VERSION" == "460" ; then
            pva_api_version=481
        else 
            if test $EPICS_PVDATABASE_VERSION -lt 460 ; then
                pva_api_version=482
            else
                pva_api_version=483
            fi
        fi
    fi


    AC_MSG_RESULT([$pva_api_version])
    AC_DEFINE(HAVE_EPICS4,,[define if the EPICS4 libraries are available])
    AC_DEFINE(PVA_API_VERSION,$pva_api_version,[define PVA API version])
    AC_SUBST(PVA_API_VERSION, $pva_api_version)
    AC_SUBST(EPICS4_DIR)
    AC_SUBST(PVDATACPP_DIR, $pvdatacpp_dir)
    AC_SUBST(PVACCESSCPP_DIR, $pvaccesscpp_dir)

    export CPPFLAGS="$PVA_CPPFLAGS $EPICS_CPPFLAGS"
    export LDFLAGS="$PVA_LDFLAGS $EPICS_LDFLAGS"
    export LIBS="$PVA_LIBS"
    AC_MSG_CHECKING(EPICS4 PVA RPC API version)
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "pv/pvAccess.h"
        #include "pv/pvData.h"
        #include "pv/event.h"
        #include "pv/rpcClient.h"
        ]],
        [[
        epics::pvAccess::RPCClient::shared_pointer rpcClient;
        ]])
    ],[succeeded=yes],[succeeded=no])
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "pv/rpcClient.h"
        ]],
        [[
        epics::pvAccess::RPCClientFactory::create("Channel");
        ]])
    ],[pva_rpc_api_version1=430],[pva_rpc_api_version1=undefined])
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "pv/rpcClient.h"
        ]],
        [[
        epics::pvAccess::RPCClient::create("Channel");
        ]])
    ],[pva_rpc_api_version3=440],[pva_rpc_api_version3=undefined])
    AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[
        #include "pv/rpcClient.h"
        #include "pv/createRequest.h"
        ]],
        [[
        epics::pvAccess::RPCClient::create("Channel");
        epics::pvAccess::RPCClient::create("Channel",
            epics::pvData::CreateRequest::create()->createRequest(""));
        ]])
    ],[pva_rpc_api_version4=$pva_api_version],[pva_rpc_api_version4=undefined])

    pva_rpc_api_version="undefined"
    if test "$pva_rpc_api_version1" != "undefined" ; then
        pva_rpc_api_version=$pva_rpc_api_version1
    elif test "$pva_rpc_api_version4" != "undefined" ; then
        pva_rpc_api_version=$pva_rpc_api_version4
    elif test "$pva_rpc_api_version3" != "undefined" ; then
        pva_rpc_api_version=$pva_rpc_api_version3
    fi

    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([unknown])
        AC_MSG_FAILURE(could not compile and link EPICS4 RPC test code: check your EPICS4 installation)
    elif test "$pva_rpc_api_version" == "undefined" ; then
        AC_MSG_RESULT([unknown])
        AC_MSG_ERROR(could not determine EPICS4 RPC API version: check your EPICS4 installation)
    else
        AC_MSG_RESULT([$pva_rpc_api_version])
        AC_DEFINE(PVA_RPC_API_VERSION,$pva_rpc_api_version,[define PVA RPC API version])
        AC_SUBST(PVA_RPC_API_VERSION, $pva_rpc_api_version)
    fi

    # restore compile/link flags
    CPPFLAGS="$SAVED_CPPFLAGS"
    LDFLAGS="$SAVED_LDFLAGS"
    AC_LANG_POP([C++])
])
