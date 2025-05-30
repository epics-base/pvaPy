#!/usr/bin/env python

from enum import Enum
import pvaccess as pva

class OperationMode(Enum):
    '''
    This class enumerates possible operation modes.
    '''
    PVA = 0    # PVA client/monitor
    PVAS = 1   # PVA server
    RPC = 2    # RPC client/server
    CA = 3     # CA monitor
    EJFAT = 4  # EJFAT client/server (or sender/receiver)

    @classmethod
    def fromString(cls, s):
        try:
            if isinstance(s, str):
                try:
                    value = int(s)
                    return OperationMode(value)
                except ValueError:
                    value = s.upper()
                return OperationMode[value]
            elif isinstance(s, OperationMode):
                return s
            else:
                return OperationMode(s)
        except Exception as ex:
            raise pva.InvalidArgument(f'Invalid value for operation mode: {s}')
