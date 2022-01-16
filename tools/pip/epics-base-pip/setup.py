from distutils.core import setup, Extension
from distutils.command.build_ext import build_ext
import os
import platform
import fnmatch
##import tokenize

DEFAULT_EPICS_BASE_VERSION = '7.0.1.1'
MODULE_NAME = 'epics-base'
MODULE = Extension(MODULE_NAME, [])
PLATFORM = platform.uname()[0].lower()
BUILD_SCRIPT = './build.%s.sh' % PLATFORM
BUILD_CONF = os.environ.get('BUILD_CONF', 'non_existent_file')
EXECUTABLES=['caget', 'cainfo', 'camonitor', 'caput', 'caRepeater', 'eget', 'pvget', 'pvinfo', 'pvlist', 'pvput', 'softIoc', 'softIocPVA']

### Workaround for python3 scripts issue with binary files 
##try:
##    _detect_encoding = tokenize.detect_encoding
##except AttributeError:
##    pass
##else:
##    def detect_encoding(readline):
##        try:
##            return _detect_encoding(readline)
##        except SyntaxError:
##            return 'utf-8', []
##tokenize.detect_encoding = detect_encoding

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
    print('Building %s' % MODULE_NAME)
    os.system(BUILD_SCRIPT)

MODULE_VERSION = get_env_var('EPICS_BASE_VERSION', DEFAULT_EPICS_BASE_VERSION) 
MODULE_FILES = list(map(lambda f: f.replace('%s/' % MODULE_NAME, ''), find_files(MODULE_NAME)))
##MODULE_SCRIPTS = [f for f in find_files('%s/bin' % MODULE_NAME) if os.path.basename(f) in EXECUTABLES]

setup(
  name = MODULE_NAME,
  version = MODULE_VERSION,
  description = 'EPICS Base software',
  url = 'https://epics.anl.gov/index.php',
  license = 'EPICS Open License',
  packages = [MODULE_NAME],
  package_data = {
    MODULE_NAME : MODULE_FILES,
  },
  ##scripts = MODULE_SCRIPTS,
  ext_modules=[MODULE],
  cmdclass = {'build_ext': BuildExt}
)
