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

#serial 1

DEFAULT_PVA_VERSION=4.0.4

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
            ac_epics4_dir_path="/usr/local/epics4"
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

    pva_version_h="$pvaccesscpp_dir/include/pv/pvaVersion.h"
    if ! test -f "$pva_version_h"; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR("could not find pvAccess installation: no header file $epics_version_h")
    fi
    AC_MSG_RESULT([yes])

    # define EPICS4_DIR
    EPICS4_DIR=$ac_epics4_dir_path

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

    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(could not compile and link EPICS4 test code: check your EPICS4 installation)
    else
        AC_SUBST(NORMATIVETYPESCPP_DIR, "")
        AC_SUBST(PVACLIENTCPP_DIR, "")
        if test "$pva_client_cpp" == "yes" ; then
            pva_api_version=450
            AC_SUBST(NORMATIVETYPESCPP_DIR, $normativetypescpp_dir)
            AC_SUBST(PVACLIENTCPP_DIR, $pvaclientcpp_dir)
        fi
        AC_MSG_RESULT([yes (pva api version: $pva_api_version)])
        AC_DEFINE(HAVE_EPICS4,,[define if the EPICS4 libraries are available])
        AC_DEFINE(PVA_API_VERSION,$pva_api_version,[define PVA API version])
        AC_SUBST(PVA_API_VERSION, $pva_api_version)
        AC_SUBST(EPICS4_DIR)
        AC_SUBST(PVDATACPP_DIR, $pvdatacpp_dir)
        AC_SUBST(PVACCESSCPP_DIR, $pvaccesscpp_dir)
    fi

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

    pva_rpc_api_version="undefined"
    if test "$pva_rpc_api_version1" != "undefined" ; then
        pva_rpc_api_version=$pva_rpc_api_version1
    elif test "$pva_rpc_api_version3" != "undefined" ; then
        pva_rpc_api_version=$pva_rpc_api_version3
    fi

    if test "$succeeded" != "yes" ; then
        AC_MSG_RESULT([unknown])
        AC_MSG_ERROR(could not compile and link EPICS4 RPC test code: check your EPICS4 installation)
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
