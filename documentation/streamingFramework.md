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
interface class located in the 'pvapy.hpc.userDataProcessor' module:

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

A working example of a simple processor that rotates area detector images
can be found [here](../examples/hpcAdImageProcessorExample.py). A python class
that derives from UserDataProcessor class and implements the above interface
is passed into one of the two main command line utilities:
- pvapy-hpc-consumer: 
- pvapy-hpc-collector:
