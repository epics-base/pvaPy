# jsonREADME

Provides a set of examples that use JASON syntax.

jsonPutExample.py and jsonPutGetExample.py require **exampleCPP/database/exampleDatabase**.

For example:

    mrk> pwd
    /home/epicsv4/master/exampleCPP/database/iocBoot/exampleDatabase
    mrk> ../../bin/$EPICS_HOST_ARCH/exampleDatabase st.cmd



## jsonHelp.py

This calls python help for the JSON methods implemented by pvaPy.

This produces:

    toJSON(...) unbound pvaccess.PvObject method
        toJSON( (PvObject)arg1, (bool)multiLine) -> str :
            displays PvObject as a JSON string.
            :argument multiLine (True or False) - display via multiple lines
            :return JSON string
    parsePut(...) unbound pvaccess.Channel method
        parsePut( (Channel)arg1, (list)argList, (str)requestDescriptor, (bool)zeroArrayLength) -> None :
            Assigns json args to the channel PV according to the specified request descriptor.
            :arguments
                 argList (list) - json args that will be assigned to the channel PV
                 requestDescriptor (str) - request to pass to createRequest
                 zeroArrayLength (True or False) - call zeroArrayLength before pars
    parsePutGet(...) unbound pvaccess.Channel method
        parsePutGet( (Channel)arg1, (list)valueList, (str)requestDescriptor, (bool)zeroArrayLength) -> PvObject :
            :arguments
                 argList (list) - json args that will be assigned to the channel PV
                 requestDescriptor (str) - request to pass to createRequest
                 zeroArrayLength (True or False) - call zeroArrayLength before parse
            :returns: channel PV data corresponding to the specified request descriptor


## jsonPutExample.py 

This provides a number of parsePut examples

## jsonPutGetExample.py

This provides a number of parsePutGet examples

## jsonPut.py

This allow a client to try calling parsePut

## jsonMonitor.py

This allows a client to create a monitor that produces JSON formatted output.


