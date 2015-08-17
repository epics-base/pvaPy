# ===========================================================================
#       http://www.gnu.org/software/autoconf-archive/ax_epics4.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_EPICS4([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the EPICS4 libraries of a particular version (or newer)
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

DEFAULT_EPICS4_VERSION=4.4.0

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

    pvaccesscpp_dir="$ac_epics4_dir_path/pvAccessCPP"
    pvdatacpp_dir="$ac_epics4_dir_path/pvDataCPP"
    normativetypescpp_dir="$ac_epics4_dir_path/normativeTypesCPP"
    pvaclientcpp_dir="$ac_epics4_dir_path/pvaClientCPP"

    epics4_version_h="$pvaccesscpp_dir/include/pv/pvaVersion.h"
    if ! test -f "$epics4_version_h"; then
        pvaccesscpp_dir="$ac_epics4_dir_path"
        pvdatacpp_dir="$ac_epics4_dir_path"
        normativetypescpp_dir="$ac_epics4_dir_path"
        pvaclientcpp_dir="$ac_epics4_dir_path"
        epics4_version_h="$pvaccesscpp_dir/include/pv/pvaVersion.h"
    fi

    if ! test -f "$epics4_version_h"; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR(could not find valid EPICS4 installation in $ac_epics4_dir_path: no version header file $epics_version_h)
    fi
    AC_MSG_RESULT([yes])

    # define EPICS4_DIR
    EPICS4_DIR=$ac_epics4_dir_path

    # determine requested version
    epics4_version_req=ifelse([$1], , $DEFAULT_EPICS4_VERSION, $1)
    AC_MSG_CHECKING(for EPICS4 >= $epics4_version_req)
    epics4_version_req_major=`expr $epics4_version_req : '\([[0-9]]*\)'`
    epics4_version_req_minor=`expr $epics4_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
    epics4_version_req_sub_minor=`expr $epics4_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
    if test "x$epics4_version_req_sub_minor" = "x" ; then
        epics4_version_req_sub_minor="0"
        epics4_version_req_patch="0"
    else
        epics4_version_req_patch=`expr $epics4_version_req : '[[0-9]]*\.[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
        if test "x$epics4_version_req_patch" = "x" ; then
            epics4_version_req_patch="0"
        fi
    fi

    # get epics4 version info
    # assume release version is always 4
    epics4_version_release=4
    epics4_version_major=`grep EPICS_PVA_MAJOR_VERSION $epics4_version_h | head -1 | awk '{print $NF}' | sed $'s/\r//'`
    epics4_version_minor=`grep EPICS_PVA_MINOR_VERSION $epics4_version_h | head -1 | awk '{print $NF}' | sed $'s/\r//'`
    epics4_version_maintenance=`grep EPICS_PVA_MAINTENANCE_VERSION $epics4_version_h | head -1 | awk '{print $NF}' | sed $'s/\r//'`

    # test version
    want_epics4_version=`expr $epics4_version_req_major \* 1000000 \+  $epics4_version_req_minor \* 10000 \+ $epics4_version_req_sub_minor \* 100 \+ $epics4_version_req_patch`
    have_epics4_version=`expr $epics4_version_release \* 1000000 \+  $epics4_version_major \* 10000 \+ $epics4_version_minor \* 100 \+ $epics4_version_maintenance`
    if ! test $have_epics4_version -ge $want_epics4_version; then
        AC_MSG_RESULT([no])
        AC_MSG_ERROR("EPICS4 installation in $ac_epics4_dir_path is too old (required: $want_epics4_version, found: $have_epics4_version)")
    fi
    AC_MSG_RESULT([yes])

    # test basic libraries
    AC_MSG_CHECKING(for usable EPICS4 libraries for $EPICS_OS_CLASS OS ($EPICS_CMPLR_CLASS compiler) and host architecture $EPICS_HOST_ARCH)

    pva_api_version=430
    pva_rpc_api_version=430
    CPPFLAGS_SAVED="$CPPFLAGS"
    CPPFLAGS="$CPPFLAGS $EPICS_CPPFLAGS -I$EPICS_BASE/include -I$EPICS_BASE/include/os/$EPICS_OS_CLASS -I$EPICS_BASE/include/compiler/$EPICS_CMPLR_CLASS -I$pvdatacpp_dir/include -I$pvaccesscpp_dir/include"
    export CPPFLAGS

    LDFLAGS_SAVED="$LDFLAGS"
    LDFLAGS="$LDFLAGS $EPICS_LDFLAGS"
    LDFLAGS="$LDFLAGS -L$EPICS_BASE/lib/$EPICS_HOST_ARCH -L$pvdatacpp_dir/lib/$EPICS_HOST_ARCH -L$pvaccesscpp_dir/lib/$EPICS_HOST_ARCH"
    export LDFLAGS

    LIBS="-lpvAccess -lpvData -lCom"
    export LIBS

    succeeded=no
    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
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
    AC_LANG_POP([C++])

    # Check for pvaClient API
    CPPFLAGS_NO_PVACLIENTCPP=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS -I$normativetypescpp_dir/include -I$pvaclientcpp_dir/include"
    LDFLAGS_NO_PVACLIENTCPP=$LDFLAGS
    LDFLAGS="$LDFLAGS -L$pvaclientcpp_dir/lib/$EPICS_HOST_ARCH -L$normativetypescpp_dir/lib/$EPICS_HOST_ARCH"
    LIBS_NO_PVACLIENTCPP=$LIBS
    LIBS="-lpvaClient -lpvAccess -lnt -lpvData -lCom"
    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
        AC_LINK_IFELSE([AC_LANG_PROGRAM(
            [[
            #include "pv/pvaClient.h"
            ]],
            [[
            epics::pvaClient::PvaClientChannelPtr pvaClientChannel;
            ]])
        ],[pva_client_cpp=yes],[pva_client_cpp=no])
    AC_LANG_POP([C++])

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

    CPPFLAGS=$CPPFLAGS_NO_PVACLIENTCPP
    LDFLAGS=$LDFLAGS_NO_PVACLIENTCPP
    LIBS=$LIBS_NO_PVACLIENTCPP
    AC_MSG_CHECKING(EPICS4 PVA RPC API version)
    succeeded=no
    AC_REQUIRE([AC_PROG_CXX])
    AC_LANG_PUSH([C++])
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
            #include "pv/event.h"
            #include "pv/rpcClient.h"
            ]],
            [[
            epics::pvData::PVStructure::shared_pointer pvRequest;
            epics::pvAccess::RPCClient::shared_pointer rpcClient = epics::pvAccess::RPCClient::create("Channel", pvRequest);
            ]])
        ],[pva_rpc_api_version2=435],[pva_rpc_api_version2=undefined])
        AC_LINK_IFELSE([AC_LANG_PROGRAM(
            [[
            #include "pv/rpcClient.h"
            ]],
            [[
            epics::pvAccess::RPCClient::create("Channel");
            ]])
        ],[pva_rpc_api_version3=440],[pva_rpc_api_version3=undefined])
    AC_LANG_POP([C++])

    pva_rpc_api_version="undefined"
    if test "$pva_rpc_api_version1" != "undefined" ; then
        pva_rpc_api_version=$pva_rpc_api_version1
    elif test "$pva_rpc_api_version2" != "undefined" ; then
        pva_rpc_api_version=$pva_rpc_api_version2
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

    CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"

])
