import os, sys
from glob import glob
from distutils.command.install import install
from distutils.core import setup
from distutils.sysconfig import get_python_inc

headers = glob (os.path.join ("CXX","*.hxx"))
sources = glob (os.path.join ("Src", "*.cxx")) + \
          glob (os.path.join ("Src", "*.c"))

support_dir = os.path.normpath(
                   os.path.join(
			sys.prefix,
			'share',
			'python%d.%d' % (sys.version_info[0],sys.version_info[1]),
			'CXX') )

class my_install (install):

    user_options = install.user_options + \
              [('install-header=', None,
                "CXX header file installation directory"),
               ('install-source=', None,
                "CXX support source directory"),
              ]

    def initialize_options (self):
        install.initialize_options (self)
        self.install_header = None
        self.install_source = None

    def finalize_options (self):
        install.finalize_options (self)
        if self.install_header is None:
            inc_base = get_python_inc (prefix=self.install_base)
            self.install_header = os.path.join (inc_base, "CXX")
        if self.install_source is None:
            self.install_source = support_dir
    def run (self):
        global headers

        install.run (self)
        self.mkpath (self.install_header)
        print "Installation directory for headers is", self.install_header
        print "Installation directory for support files is", self.install_source
        for header in headers:
            self.copy_file (header, self.install_header)
        self.mkpath (self.install_source)
        for s in sources:
            self.copy_file (s, self.install_source)

setup (name = "CXX",
       version = "5.1",
       maintainer = "BArry Scott",
       maintainer_email = "barry-scott@users.sourceforge.net",
       description = "Facility for extending Python with C++",
       url = "http://cxx.sourceforge.net",
       
       cmdclass = {'install': my_install},
       packages=['CXX'],
       package_dir={'CXX':'Lib'},
       )
