import os, sys
from distutils.core import setup, Extension

support_dir = os.path.normpath(
                   os.path.join(
			sys.prefix,
			'share',
			'python%d.%d' % (sys.version_info[0],sys.version_info[1]),
			'CXX') )

if os.name == 'posix':
	CXX_libraries = ['stdc++','m']
else:
	CXX_libraries = []

setup (name = "CXXDemo",
       version = "5.1",
       maintainer = "Barry Scott",
       maintainer_email = "barry-scott@users.sourceforge.net",
       description = "Demo of facility for extending Python with C++",
       url = "http://cxx.sourceforge.net",
       
       packages = ['CXX'],
       package_dir = {'CXX': '.'},
       ext_modules = [
         Extension('CXX.example',
                   sources = ['example.cxx',
                         'range.cxx',
                         'rangetest.cxx',
                         os.path.join(support_dir,'cxxsupport.cxx'),
                         os.path.join(support_dir,'cxx_extensions.cxx'),
                         os.path.join(support_dir,'IndirectPythonInterface.cxx'),
                         os.path.join(support_dir,'cxxextensions.c')
                         ],
            )
       ]
)
