try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup
from distutils.core import Extension
from distutils.command.build_ext import build_ext

import os
import platform
import fnmatch

DEFAULT_PVAPY_VERSION = 'master'
PVACCESS_MODULE = Extension('pvaccess', [])
PLATFORM = platform.uname()[0].lower()
MY_DIR = os.path.abspath(os.path.dirname(__file__))

BUILD_SCRIPT = './build.%s.sh' % PLATFORM
if PLATFORM == 'windows':
    BUILD_SCRIPT = 'build.%s.bat' % PLATFORM

README_FILE = 'pvaccess/doc/README.md'
if not os.path.exists(README_FILE):
    README_FILE = os.path.join(MY_DIR, '../../../README.md')
BUILD_CONF = os.environ.get('BUILD_CONF', 'non_existent_file')

def get_env_var(name, default):
    value = os.environ.get(name)
    if value is not None:
        return value

    if os.path.exists(BUILD_CONF):
        vars = open(BUILD_CONF).read().split()
        for v in vars:
            key = v.split('=')[0].strip()
            value = v.split('=')[1].strip()
            if key == name:
                return value
    return default

def find_files(rootDir='.', pattern='*'):
    result = []
    for root, dirs, files in os.walk(rootDir):
      for f in fnmatch.filter(files, pattern):
          result.append(os.path.join(root, f))
    return result

class BuildExt(build_ext):
    def build_extension(self, ext):
        print('Building pvaccess module')
        os.system(BUILD_SCRIPT)

PVAPY_VERSION = get_env_var('PVAPY_VERSION', DEFAULT_PVAPY_VERSION)
if PLATFORM == 'windows':
    PVACCESS_FILES = list(map(lambda f: f.replace('pvaccess\\', ''), find_files('pvaccess')))
    PVAPY_FILES = list(map(lambda f: f.replace('pvapy\\', ''), find_files('pvapy')))
else:
    PVACCESS_FILES = list(map(lambda f: f.replace('pvaccess/', ''), find_files('pvaccess')))
    PVAPY_FILES = list(map(lambda f: f.replace('pvapy/', ''), find_files('pvapy')))
LONG_DESCRIPTION = open(README_FILE).read()
    
setup(
    name = 'pvapy',
    version = PVAPY_VERSION,
    description = 'Python library for EPICS PV Access',
    long_description = LONG_DESCRIPTION,
    long_description_content_type='text/markdown',
    url = 'https://github.com/epics-base/pvaPy',
    license = 'EPICS Open License',
    packages = ['pvapy', 'pvaccess'],
    package_data = {
        'pvaccess' :  PVACCESS_FILES,
        'pvapy' :  PVAPY_FILES,
    },
    install_requires=[
        'numpy>=1.19,<1.21; python_version < "3.8"',
        'numpy>=1.22; python_version >= "3.8"',
        'pillow',
        'pycryptodome',
        'rsa' 
    ],
    entry_points = {
        'console_scripts': [
            'pvapy-ad-sim-server=pvapy.cli.adSimServer:main',
            'pvapy-mirror-server=pvapy.cli.mirrorServer:main',
            'pvapy-hpc-consumer=pvapy.cli.hpcConsumer:main',
            'pvapy-hpc-collector=pvapy.cli.hpcCollector:main'
        ],
    },
    ext_modules=[PVACCESS_MODULE],
    cmdclass = {'build_ext': BuildExt}
)
