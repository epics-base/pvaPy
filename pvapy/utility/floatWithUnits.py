#!/usr/bin/env python

class FloatWithUnits(float):

    DEFAULT_PRECISION = 4

    # Float is immutable, we have to override __new__
    def __new__(self, value, units='', precision=DEFAULT_PRECISION):
        return float.__new__(self, value)

    def __init__(self, value, units='', precision=DEFAULT_PRECISION):
        float.__init__(value)
        self.units = units
        self.precision = precision

    def __repr__(self):
        if self == 0:
            return f'0.0{self.units}'
        return f'{float(self):.{self.precision}f}{self.units}'

    def __str__(self):
        return self.__repr__()
