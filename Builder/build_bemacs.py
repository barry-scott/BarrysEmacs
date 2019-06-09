#!/usr/bin/env python
#
#   build_bemacs.py
#
#   Needs to be able to be run under python2 or python3
#   to allow older systems to use this script.
#
from __future__ import print_function

import sys
import os
import shutil
import subprocess
import platform
import glob
import build_log
import build_utils

sys.path.insert( 0, '../Editor' )
sys.path.insert( 0, '../MLisp' )
sys.path.insert( 0, '../Describe' )

log = build_log.BuildLog()

# setup build_utils
build_utils.log = log
# alias run()
run = build_utils.run
BuildError = build_utils.BuildError

class BuildBEmacs(object):
    valid_targets = ('gui', 'cli')

    def __init__( self ):
        self.target = 'gui'

        self.opt_colour = False
        self.opt_verbose = False
        self.opt_sqlite = True
        self.opt_vcredist = None
        self.opt_editor_setup_opt = []

        self.bemacs_version_info = None

    def main( self, argv ):
        try:
            self.parseArgs( argv )

            log.info( 'Building target %s' % (self.target,) )
            self.setupVars()

            if self.target == 'gui':
                self.checkGuiDeps()

            self.ruleClean()
            self.ruleBuild()
            log.info( 'Build complete' )

        except BuildError as e:
            log.error( str(e) )
            return 1

        return 0

    def parseArgs( self, argv ):
        positional = []
        try:
            args = iter( argv )
            next(args)

            while True:
                arg = next(args)
                if arg.startswith( '--' ):
                    if arg == '--verbose':
                        self.opt_verbose = True

                    elif arg == '--colour':
                        self.opt_colour = True

                    elif arg.startswith( '--vcredist=' ):
                        self.opt_vcredist = arg[len('--vcredist='):]

                    elif arg == '--no-sqlite':
                        self.opt_sqlite = False

                    elif arg in ('--enable-debug'
                                ,'--system-pycxx'
                                ,'--system-ucd'
                                ,'--system-sqlite'
                                ,'--no-warnings-as-errors'):
                        self.opt_editor_setup_opt.append( arg )

                    else:
                        raise BuildError( 'Unknown option %r' % (arg,) )

                else:
                    positional.append( arg )

        except StopIteration:
            pass

        if len(positional) > 1:
            raise BuildError( 'Extra arguments %r' % (' '.join( positional[1:] ),) )

        if len(positional) == 1:
            self.target = positional[0]

        if self.target not in self.valid_targets:
            raise BuildError( 'Unknown target %r pick on of %s' % (self.target, ', '.join( self.valid_targets )) )

        if self.opt_sqlite:
            self.opt_editor_setup_opt.append( '--sqlite' )

        log.setColour( self.opt_colour )
        if self.opt_colour:
            self.opt_editor_setup_opt.append( '--colour' )

    def setupVars( self ):
        self.platform = platform.system()
        if self.platform == 'Darwin':
            if platform.mac_ver()[0] != '':
                self.platform = 'MacOSX'

        elif self.platform == 'Windows':
            if platform.architecture()[0] == '64bit':
                self.platform = 'win64'
                self.VC_VER = '14.0'
            else:
                raise BuildError( 'Windows 32 bit is not supported' )

        self.BUILDER_TOP_DIR = os.environ[ 'BUILDER_TOP_DIR' ]

        if self.platform in ('Linux', 'NetBSD'):
            if 'DESTDIR' in os.environ:
                self.BEMACS_ROOT_DIR = os.environ[ 'DESTDIR' ]

                self.INSTALL_BEMACS_DOC_DIR = '/usr/share/bemacs/doc'
                self.INSTALL_BEMACS_LIB_DIR = '/usr/share/bemacs/lib'
                self.INSTALL_BEMACS_BIN_DIR = '/usr/bin'

            else:
                self.BEMACS_ROOT_DIR = '%s/Kits/%s/ROOT' % (self.BUILDER_TOP_DIR, self.platform)

                self.INSTALL_BEMACS_DOC_DIR = '/usr/local/share/bemacs/doc'
                self.INSTALL_BEMACS_LIB_DIR = '/usr/local/lib/bemacs'
                self.INSTALL_BEMACS_BIN_DIR = '/usr/local/bin'

            self.BUILD_BEMACS_DOC_DIR = '%s%s' % (self.BEMACS_ROOT_DIR, self.INSTALL_BEMACS_DOC_DIR)
            self.BUILD_BEMACS_LIB_DIR = '%s%s' % (self.BEMACS_ROOT_DIR, self.INSTALL_BEMACS_LIB_DIR)
            self.BUILD_BEMACS_BIN_DIR = '%s%s' % (self.BEMACS_ROOT_DIR, self.INSTALL_BEMACS_BIN_DIR)

            if self.platform == 'NetBSD':
                self.cmd_make = 'gmake'
                self.cmd_make_args = ['-j', '%d' % (build_utils.numCpus(),)]

            else:
                self.cmd_make = 'make'
                self.cmd_make_args = ['-j', '%d' % (build_utils.numCpus(),)]

        elif self.platform == 'MacOSX':
            self.BUILD_BEMACS_BIN_DIR = "%s/Builder/tmp/pkg/Barry's Emacs-Devel.app/Contents/Resources/bin" % (self.BUILDER_TOP_DIR,)
            self.BUILD_BEMACS_LIB_DIR = "%s/Builder/tmp/pkg/Barry's Emacs-Devel.app/Contents/Resources/emacs_library" % (self.BUILDER_TOP_DIR,)
            self.BUILD_BEMACS_DOC_DIR = "%s/Builder/tmp/pkg/Barry's Emacs-Devel.app/Contents/Resources/documentation" % (self.BUILDER_TOP_DIR,)

            self.INSTALL_BEMACS_LIB_DIR = self.BUILD_BEMACS_LIB_DIR

            self.cmd_make = 'make'
            self.cmd_make_args = ['-j', '%d' % (build_utils.numCpus(),)]

        elif self.platform == 'win64':
            self.KITSRC = r'%s\Kits\Windows' % (self.BUILDER_TOP_DIR,)
            self.KITROOT = r'%s\Builder\tmp' % (self.BUILDER_TOP_DIR,)
            self.KITFILES = r'%s\kitfiles' % (self.KITROOT,)

            self.BUILD_BEMACS_BIN_DIR = self.KITFILES
            self.BUILD_BEMACS_LIB_DIR = r'%s\emacs_library' % (self.KITFILES,)
            self.BUILD_BEMACS_DOC_DIR = r'%s\Documentation' % (self.KITFILES,)

            self.INSTALL_BEMACS_BIN_DIR = self.BUILD_BEMACS_BIN_DIR
            self.INSTALL_BEMACS_LIB_DIR = self.BUILD_BEMACS_LIB_DIR
            self.INSTALL_BEMACS_DOC_DIR = self.BUILD_BEMACS_DOC_DIR

            self.cmd_make = 'nmake'
            self.cmd_make_args = ['/nologo']

        else:
            raise BuildError( 'Unsupported platform: %s' % (self.platform,) )

    def checkGuiDeps( self ):
        log.info( 'Checking GUI dependencies...' )
        if sys.version_info.major != 3:
            raise BuildError( 'bemacs GUI needs python version 3' )

        if self.platform in ('Linux',):
            try:
                from PyQt5 import QtWidgets, QtGui, QtCore
            except ImportError:
                raise BuildError( 'PyQt5 is not installed for %s. Hint: dnf install PyQt5' % (sys.executable,) )
            try:
                import xml_preferences
            except ImportError:
                raise BuildError( 'xml-preferences is not installed for %s. Hint: dnf install python3-xml-preferences' % (sys.executable,) )

        if self.platform in ('MacOSX', 'win64', 'NetBSD'):
            try:
                from PyQt5 import QtWidgets, QtGui, QtCore
            except ImportError:
                raise BuildError( 'PyQt5 is not installed for %s. Hint: pip3 install --user PyQt5' % (sys.executable,) )
            try:
                import xml_preferences
            except ImportError:
                raise BuildError( 'xml-preferences is not installed for %s. Hint: pip3 install --user xml-preferences' % (sys.executable,) )

        if self.platform in ('win64',):
            try:
                import win_app_packager
            except ImportError:
                raise BuildError( 'win_app_packager is not installed for %s. Hint: pip3 install --user win_app_packager' % (sys.executable,) )

    def ruleClean( self ):
        log.info( 'Running ruleClean' )

        self.setupEditorMakefile()
        self.make( 'clean' )

        if self.platform in ('Linux', 'NetBSD'):
            build_utils.rmdirAndContents( self.BEMACS_ROOT_DIR )

        elif self.platform == 'win64':
            if os.path.exists( self.KITROOT ):
                run( 'cmd /c "rmdir /s /q %s"' % self.KITROOT )

        elif self.platform == 'MacOSX':
            build_utils.rmdirAndContents( 'tmp' )

    def ruleBuild( self ):
        self.ruleBrand()
        for folder in (self.BUILD_BEMACS_DOC_DIR, self.BUILD_BEMACS_LIB_DIR, self.BUILD_BEMACS_BIN_DIR):
            build_utils.mkdirAndParents( folder )

        if self.target == 'gui':
            self.ruleBemacsGui()
        else:
            self.ruleBemacsCli()
        if not self.opt_sqlite:
            self.ruleUtils()
        self.ruleMlisp()
        self.ruleDescribe()
        self.ruleQuickInfo()
        self.ruleDocs()
        if self.platform == 'MacOSX':
            self.ruleMacosPackage()
        if self.platform == 'win64':
            self.ruleInnoInstaller()

    def ruleBrand( self ):
        log.info( 'Running ruleBrand' )
        import brand_version


        try:
            if self.opt_verbose:
                log_info = log.info
            else:
                log_info = logNothing

            self.bemacs_version_info = brand_version.VersionInfo( self.BUILDER_TOP_DIR, log_info )
            self.bemacs_version_info.parseVersionInfo( 'version_info.txt' )
            finder = brand_version.FileFinder( self.bemacs_version_info )
            finder.findAndBrandFiles( self.BUILDER_TOP_DIR )

        except brand_version.Error as e:
            raise BuildError( str(e) )

    def ruleBemacsGui( self ):
        log.info( 'Running ruleBemacsGui' )

        self.make( 'all' )

        if self.platform in ('Linux', 'NetBSD', 'MacOSX'):
            build_utils.copyFile( '../Editor/exe-pybemacs/_bemacs.so', self.BUILD_BEMACS_LIB_DIR, 0o555 )
            build_utils.copyFile( '../Editor/exe-cli-bemacs/bemacs-cli',  self.BUILD_BEMACS_BIN_DIR, 0o555 )

        if self.platform == 'MacOSX':
            run( ('./build-macosx.sh'
                 ,'--package'),
                    cwd='../Editor/PyQtBEmacs' )

            build_utils.mkdirAndParents( self.BUILD_BEMACS_BIN_DIR )
            build_utils.copyFile( '../Editor/exe-cli-bemacs/bemacs-cli',  self.BUILD_BEMACS_BIN_DIR, 0o555 )

        elif self.platform == 'win64':
            run( ('build-windows.cmd', self.KITFILES, 'all'), cwd=r'..\Editor\PyQtBEmacs' )

        else:
            run( ('./build-linux.sh'
                 ,self.BEMACS_ROOT_DIR
                 ,self.INSTALL_BEMACS_BIN_DIR
                 ,self.INSTALL_BEMACS_LIB_DIR
                 ,self.INSTALL_BEMACS_DOC_DIR),
                    cwd='../Editor/PyQtBEmacs' )

    def ruleBemacsCli( self ):
        log.info( 'Running ruleBemacsCli' )
        self.make( 'all' )

        build_utils.mkdirAndParents( self.BUILD_BEMACS_BIN_DIR )
        build_utils.copyFile( '../Editor/exe-cli-bemacs/bemacs-cli',  self.BUILD_BEMACS_BIN_DIR, 0o555 )

    def ruleUtils( self ):
        log.info( 'Running ruleUtils' )
        build_utils.copyFile( '../Editor/exe-utils/dbadd',    '%s/bemacs-dbadd' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        build_utils.copyFile( '../Editor/exe-utils/dbcreate', '%s/bemacs-dbcreate' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        build_utils.copyFile( '../Editor/exe-utils/dbdel',    '%s/bemacs-dbdel' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        build_utils.copyFile( '../Editor/exe-utils/dbprint',  '%s/bemacs-dbprint' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        build_utils.copyFile( '../Editor/exe-utils/dblist',   '%s/bemacs-dblist' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        build_utils.copyFile( '../Editor/exe-utils/mll2db',   '%s/bemacs-mll2db' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )

    def ruleMlisp( self ):
        log.info( 'Running ruleMlisp' )
        build_utils.copyFile( '../MLisp/emacsinit.ml', self.BUILD_BEMACS_LIB_DIR, 0o444 )
        build_utils.copyFile( '../MLisp/emacs_profile.ml', self.BUILD_BEMACS_LIB_DIR, 0o444 )

        import create_library

        if self.opt_sqlite:
            dbtools = create_library.BemacsSqliteTools()
            create_library.createLibrary( ('common', 'unix'), '%s/emacslib' % (self.BUILD_BEMACS_LIB_DIR,), dbtools )
            os.chmod( '%s/emacslib.db' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )

        else:
            dbtools = create_library.BemacsDatabaseTools( self.BUILD_BEMACS_BIN_DIR )
            create_library.createLibrary( ('common', 'unix'), '%s/emacslib' % (self.BUILD_BEMACS_LIB_DIR,), dbtools )
            os.chmod( '%s/emacslib.dat' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )
            os.chmod( '%s/emacslib.pag' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )
            os.chmod( '%s/emacslib.dir' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )

    def ruleDescribe( self ):
        log.info( 'Running ruleDescribe' )
        self.mllToDb( '../Describe/em_desc.mll', '%s/emacsdesc' % (self.BUILD_BEMACS_LIB_DIR,) )

    def ruleMacosPackage( self ):
        log.info( 'Make macOS package' )

        pkg_name = 'BarrysEmacs-%s' % (self.bemacs_version_info.get('version'),)
        dmg_folder = '%s/Builder/tmp/dmg' % (self.BUILDER_TOP_DIR,)
        pkg_folder = '%s/Builder/tmp/pkg' % (self.BUILDER_TOP_DIR,)

        build_utils.mkdirAndParents( pkg_folder )
        build_utils.mkdirAndParents( dmg_folder )

        run( ('cp', '-r',
                "%s/Barry's Emacs-Devel.app" % (pkg_folder,),
                "%s/Barry's Emacs.app" % (dmg_folder,)) )

        os.rename(
            "%s/Barry's Emacs.app/Contents/MacOS/Barry's Emacs-Devel" % (dmg_folder,),
            "%s/Barry's Emacs.app/Contents/MacOS/Barry's Emacs" % (dmg_folder,) )

        log.info( 'Create DMG' )
        # use 2.7 version as 3.5 version does not work yet (confuses bytes and str)
        run( ('/Library/Frameworks/Python.framework/Versions/2.7/bin/dmgbuild',
                '--settings', 'package_macos_dmg_settings.py',
                "Barry's Emacs",
                '%s/%s.dmg' % (dmg_folder, pkg_name)) )

    def ruleInnoInstaller( self ):
        import package_windows_inno_setup_files
        inno_setup = package_windows_inno_setup_files.InnoSetup( self.platform, self.VC_VER, self.opt_vcredist )
        inno_setup.createInnoInstall()

        run( (r'c:\Program Files (x86)\Inno Setup 5\ISCC.exe', r'tmp\bemacs.iss') )
        build_utils.copyFile(
            r'tmp\Output\mysetup.exe',
            r'tmp\bemacs-%s-setup.exe' % (self.bemacs_version_info.get('version'),),
            0o600 )
        log.info( r'Created kit tmp\bemacs-%s-setup.exe' % (self.bemacs_version_info.get('version'),) )

    def mllToDb( self, mll_file, db_file ):
        if self.opt_sqlite:
            db_file = '%s.db' % (db_file,)
            import create_describe_database
            create_describe_database.createDatabaseFromMll(
                mll_file,
                db_file )
            os.chmod( db_file, 0o444 )
        else:
            run( ('%s/bemacs-dbcreate' % (self.BUILD_BEMACS_BIN_DIR,), db_file, '-c') )
            run( ('%s/bemacs-mll2db' % (self.BUILD_BEMACS_BIN_DIR,), mll_file, db_file) )
            os.chmod( '%s.dat' % (db_file,), 0o444 )
            os.chmod( '%s.pag' % (db_file,), 0o444 )
            os.chmod( '%s.dir' % (db_file,), 0o444 )

    def ruleQuickInfo( self ):
        log.info( 'Running ruleQuickInfo' )
        self.mllToDb( '../Describe/qi_cc.mll', '%s/emacs_qinfo_c' % (self.BUILD_BEMACS_LIB_DIR,) )

    def ruleDocs( self ):
        log.info( 'Running ruleDocs' )
        build_utils.copyFile( '../Kits/readme.txt', self.BUILD_BEMACS_DOC_DIR, 0o444 )
        build_utils.copyFile( '../Editor/PyQtBEmacs/bemacs.png', self.BUILD_BEMACS_DOC_DIR, 0o444 )
        for pattern in ('../HTML/*.html',
                        '../HTML/*.gif',
                        '../HTML/*.css',
                        '../HTML/*.js'):
            for src in glob.glob( pattern ):
                build_utils.copyFile( src, self.BUILD_BEMACS_DOC_DIR, 0o444 )

    def setupEditorMakefile( self ):
        import setup

        if self.platform in ('Linux', 'MacOSX', 'NetBSD'):
            setup_targets = set( [self.target, 'cli', 'unit-tests'] )

        else:
            setup_targets = set( ['gui', 'unit-tests'] )

        if not self.opt_sqlite:
            setup_targets.add( 'utils' )

        log.info( 'Creating ../Editor/Makefile-all for %s' % ', '.join( sorted( setup_targets ) ) )

        cwd = os.getcwd()
        try:
            os.chdir( '../Editor' )
            setup_argv = [sys.argv[0]
                         ,self.platform
                         ,','.join( setup_targets )
                         ,'Makefile-all'
                         ,'--lib-dir=%s' % (self.INSTALL_BEMACS_LIB_DIR,)
                         ]
            setup_argv += self.opt_editor_setup_opt
            if setup.main( setup_argv ) != 0:
                raise BuildError( 'Editor/setup.py failed' )

        finally:
            os.chdir( cwd )

    def make( self, make_target ):
        make_cmd = [self.cmd_make, '-f', 'Makefile-all']
        make_cmd.extend( self.cmd_make_args )
        make_cmd.append( make_target )
        run( make_cmd, cwd='../Editor' )

def logNothing( msg ):
    pass


if __name__ == '__main__':
    sys.exit( BuildBEmacs().main( sys.argv ) )