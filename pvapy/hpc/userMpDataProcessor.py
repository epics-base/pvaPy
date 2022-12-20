#!/usr/bin/env python

from ..utility.loggingManager import LoggingManager

class UserMpDataProcessor:
    ''' 
    Class that serves as a base for any user class that will be processing
    data from a multiprocessing queue. 
    The following variables will be set after processor instance is created and before processing starts:\n
    \t\- *logger* (logging.Logger) : logger object\n
  
    **UserMpDataProcessor(processorId=1)**

    :Parameter: *processorId* (int) - processor id
    '''

    def __init__(self, processorId=1):
        '''
        Constructor.
        '''
        self.logger = LoggingManager.getLogger(self.__class__.__name__)
        self.processorId = processorId

    def start(self):
        '''
        Method invoked at processing startup.
        '''
        pass

    def configure(self, configDict):
        ''' 
        Method invoked at user initiated runtime configuration changes.

        :Parameter: *configDict* (dict) - dictionary containing configuration parameters
        '''
        pass

    def process(self, mpqObject):
        ''' 
        Data processing method.

        :Parameter: *mpqObject* (object) - object received from multiprocessing queue
        '''
        self.logger.debug(f'Processor {self.processorId} processing object {mpqObject}')

    def stop(self):
        '''
        Method invoked at processing shutdown.
        '''
        pass
    
    def resetStats(self):
        ''' 
        Method invoked at user initiated application statistics reset. 
        '''
        pass

    def getStats(self):
        '''
        Method invoked periodically for generating application statistics.
        
        :Returns: Dictionary containing application statistics parameters
        '''
        return {}

