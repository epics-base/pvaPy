#!/usr/bin/env python

import pprint

class PvaPyPrettyPrinter(pprint.PrettyPrinter):

    DEFAULT_PPRINT_INDENT = 2
    DEFAULT_PPRINT_DEPTH = 4
    DEFAULT_PPRINT_WIDTH = 512
    DEFAULT_PPRINT_COMPACT = True

    def __init__(self, indent=DEFAULT_PPRINT_INDENT, width=DEFAULT_PPRINT_WIDTH, depth=DEFAULT_PPRINT_DEPTH, compact=DEFAULT_PPRINT_COMPACT):
        pprint.PrettyPrinter.__init__(self, indent=indent, width=width, depth=depth, compact=compact)

