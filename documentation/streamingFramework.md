# Streaming Framework

Processing data generated at high rates in real time typically faces many
challenges, from computing and networking infrastructure, to availability of
fast storage, and to user applications themselves. Streaming data directly
form the source into processing applications does not solve all issues,
but it does eliminate potential delays related to file input and output.

PvaPy Streaming Framework allows users to setup distributed streaming 
workflows with a very little effort. This documents describes various
framework components, user interfaces and available command line utilities.

## User Interfaces

Users interact with the framework by implementing the data processor
interface class located in the [pvapy.hpc.userDataProcessor](../pvapy/hpc/userDataProcessor.py) module:

```python
class UserDataProcessor:

    def __init__(self, configDict={}):
        ...

    # Method called at start
    def start(self):
        pass

    # Configure user processor
    def configure(self, kwargs):
        pass

    # Process monitor update
    def process(self, pvObject):
        ...
        self.updateOutputChannel(pvObject)
        return pvObject

    # Method called at shutdown
    def stop(self):
        pass
    
    # Reset statistics for user processor
    def resetStats(self):
        pass

    # Retrieve statistics for user processor
    def getStats(self):
        return {}

    # Define PVA types for different stats variables
    def getStatsPvaTypes(self):
        return {}

    # Define PVA types for output object
    # This method does not need to be implemented if output
    # object has the same structure as the input object
    def getOutputPvaTypes(self):
        return {}
```

A working example of a simple processor that rotates Area Detector images
can be found [here](../examples/hpcAdImageProcessorExample.py). In addition,


A python class
that derives from UserDataProcessor class and implements the above interface
is passed into one of the two main command line utilities:
- pvapy-hpc-consumer: used for splitting streams and processing stream objects
- pvapy-hpc-collector: used for gathering streams, sorting and processing
stream objects

In addition to these two commands, the framework also relies on the following:
- pvapy-mirror-server: can be used for data distribution in those cases
when the original data source does not have distributor plugin, for stream 
isolation and IOC protection from high client loads, as bridge between 
network interfaces, etc.
- pvapy-ad-sim-server: testing utility that can generate and serve 
NtNdArray objects (Area Detector images) at frame rates exceeding 10k fps



