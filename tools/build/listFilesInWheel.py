#!/usr/bin/env python
import pprint
import sys
from zipfile import ZipFile

if len(sys.argv) != 2:
    print('No path given')
    exit(1)

path = sys.argv[1]
names = ZipFile(path).namelist()
pprint.pprint(names)
