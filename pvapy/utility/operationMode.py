#!/usr/bin/env python

from enum import Enum
import pvaccess as pva

class OperationMode(Enum):
    '''
    This class enumerates possible operation modes.
    '''
    PVA = 0  # PVA client (monitor)
    PVAS = 1 # PVA server
    RPC = 2  # RPC client
    RPCS = 3 # RPC server
    CA = 4   # CA client (monitor)

    @classmethod
    def fromString(cls, input):
        try:
            if isinstance(input, str):
                try:
                    value = int(input)
                    return OperationMode(value)
                except ValueError:
                    value = input.upper()
                return OperationMode[value]
            elif isinstance(input, OperationMode):
                return input
            else:
                return OperationMode(input)
        except Exception as ex:
            raise pva.InvalidArgument(f'Invalid value for operation mode: {input}')
