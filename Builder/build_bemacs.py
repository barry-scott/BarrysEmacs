#!/usr/bin/env python
#
#   build_bemacs.py
#
#   Needs to run under python2 or python3
#
from __future__ import print_function

import sys
import os
import shutil
import subprocess
import platform
import glob

sys.path.insert( 0, '../Editor' )
sys.path.insert( 0, '../MLisp' )
sys.path.insert( 0, '../Describe' )

class BuildBEmacs(object):
    valid_targets = ('gui', 'cli')

    def __init__( self ):
        self.target = 'gui'

        self.opt_verbose = False
        self.opt_sqlite3 = False
        self.opt_editor_setup_opt = []

    def main( self, argv ):
        try:
            self.parseArgs( argv )

            info( 'Building target %s' % (self.target,) )
            self.setupVars()

            if self.target == 'gui':
                self.checkGuiDeps()

            self.ruleClean()
            self.ruleBuild()
            info( 'Build complete' )

        except BuildError as e:
            error( str(e) )
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

                    elif arg == '--sqlite3':
                        self.opt_sqlite3 = True
                        self.opt_editor_setup_opt.append( arg )

                    elif arg in ('--enable-debug'
                                ,'--system-pycxx'
                                ,'--system-ucd'
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

    def setupVars( self ):
        if platform.system() in ('Linux', 'NetBSD'):
            if 'DESTDIR' in os.environ:
                self.BEMACS_ROOT_DIR = os.environ[ 'DESTDIR' ]

                self.INSTALL_BEMACS_DOC_DIR = '/usr/share/bemacs/doc'
                self.INSTALL_BEMACS_LIB_DIR = '/usr/share/bemacs/lib'
                self.INSTALL_BEMACS_BIN_DIR = '/usr/bin'

            else:
                self.BUILDER_TOP_DIR = os.environ[ 'BUILDER_TOP_DIR' ]
                self.BEMACS_ROOT_DIR = '%s/Kits/%s/ROOT' % (self.BUILDER_TOP_DIR, platform.system())

                self.INSTALL_BEMACS_DOC_DIR = '/usr/local/share/bemacs/doc'
                self.INSTALL_BEMACS_LIB_DIR = '/usr/local/lib/bemacs'
                self.INSTALL_BEMACS_BIN_DIR = '/usr/local/bin'

            self.BUILD_BEMACS_DOC_DIR = '%s%s' % (self.BEMACS_ROOT_DIR, self.INSTALL_BEMACS_DOC_DIR)
            self.BUILD_BEMACS_LIB_DIR = '%s%s' % (self.BEMACS_ROOT_DIR, self.INSTALL_BEMACS_LIB_DIR)
            self.BUILD_BEMACS_BIN_DIR = '%s%s' % (self.BEMACS_ROOT_DIR, self.INSTALL_BEMACS_BIN_DIR)

            if platform.system() == 'NetBSD':
                self.cmd_make = 'gmake'
                self.cmd_make_args = ['-j', '%d' % (numCpus(),)]
            else:
                self.cmd_make = 'make'
                self.cmd_make_args = ['-j', '%d' % (numCpus(),)]

        else:
            raise BuildError( 'Unsupported platform: %s' % (platform.system(),) )

    def checkGuiDeps( self ):
        info( 'Checking GUI dependencies...' )
        if sys.version_info.major != 3:
            raise BuildError( 'bemacs GUI needs python version 3' )

        try:
            from PyQt5 import QtWidgets, QtGui, QtCore

        except ImportError:
            raise BuildError( 'PyQt5 is not installed for %s. Hint: dnf install PyQt5' % (sys.executable,) )

    def ruleClean( self ):
        info( 'Running ruleClean' )

        self.setupEditorMakefile()
        self.make( 'clean' )
        run( ('rm', '-rf', self.BEMACS_ROOT_DIR) )

    def ruleBuild( self ):
        self.ruleBrand()
        for folder in (self.BUILD_BEMACS_DOC_DIR, self.BUILD_BEMACS_LIB_DIR, self.BUILD_BEMACS_BIN_DIR):
            if not os.path.exists( folder ):
                os.makedirs( folder, 0o750 )

        if self.target == 'gui':
            self.ruleBemacsGui()
        else:
            self.ruleBemacsCli()
        if not self.opt_sqlite3:
            self.ruleUtils()
        self.ruleMlisp()
        self.ruleDescribe()
        self.ruleQuickInfo()
        self.ruleDocs()

    def ruleBrand( self ):
        info( 'Running ruleBrand' )
        import brand_version
        try:
            if self.opt_verbose:
                log_info = info
            else:
                log_info = logNothing

            brand_version.brandVersion( 'version_info.txt', self.BUILDER_TOP_DIR, log_info )

        except brand_version.Error as e:
            raise BuildError( str(e) )

    def ruleBemacsGui( self ):
        info( 'Running ruleBemacsGui' )

        self.make( 'all' )
        copyFile( '../Editor/exe-pybemacs/_bemacs.so', self.BUILD_BEMACS_LIB_DIR, 0o555 )

        run( ('./build-linux.sh'
             ,self.BEMACS_ROOT_DIR
             ,self.INSTALL_BEMACS_BIN_DIR
             ,self.INSTALL_BEMACS_LIB_DIR
             ,self.INSTALL_BEMACS_DOC_DIR),
                cwd='../Editor/PyQtBEmacs' )

    def ruleBemacsCli( self ):
        info( 'Running ruleBemacsCli' )
        self.make( 'all' )

    def ruleUtils( self ):
        info( 'Running ruleUtils' )
        copyFile( '../Editor/exe-utils/dbadd',    '%s/bemacs-dbadd' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        copyFile( '../Editor/exe-utils/dbcreate', '%s/bemacs-dbcreate' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        copyFile( '../Editor/exe-utils/dbdel',    '%s/bemacs-dbdel' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        copyFile( '../Editor/exe-utils/dbprint',  '%s/bemacs-dbprint' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        copyFile( '../Editor/exe-utils/dblist',   '%s/bemacs-dblist' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )
        copyFile( '../Editor/exe-utils/mll2db',   '%s/bemacs-mll2db' % (self.BUILD_BEMACS_BIN_DIR,), 0o555 )

    def ruleMlisp( self ):
        info( 'Running ruleMlisp' )
        copyFile( '../MLisp/emacsinit.ml', self.BUILD_BEMACS_LIB_DIR, 0o444 )
        copyFile( '../MLisp/emacs_profile.ml', self.BUILD_BEMACS_LIB_DIR, 0o444 )

        import create_library

        if self.opt_sqlite3:
            dbtools = create_library.BemacsSqlite3Tools()
            create_library.createLibrary( ('common', 'unix'), '%s/emacslib' % (self.BUILD_BEMACS_LIB_DIR,), dbtools )
            os.chmod( '%s/emacslib.db' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )

        else:
            dbtools = create_library.BemacsDatabaseTools( self.BUILD_BEMACS_BIN_DIR )
            create_library.createLibrary( ('common', 'unix'), '%s/emacslib' % (self.BUILD_BEMACS_LIB_DIR,), dbtools )
            os.chmod( '%s/emacslib.dat' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )
            os.chmod( '%s/emacslib.pag' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )
            os.chmod( '%s/emacslib.dir' % (self.BUILD_BEMACS_LIB_DIR,), 0o444 )

    def ruleDescribe( self ):
        info( 'Running ruleDescribe' )
        self.mllToDb( '../Describe/em_desc.mll', '%s/emacsdesc' % (self.BUILD_BEMACS_LIB_DIR,) )

    def mllToDb( self, mll_file, db_file ):
        if self.opt_sqlite3:
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
        info( 'Running ruleQuickInfo' )
        self.mllToDb( '../Describe/qi_cc.mll', '%s/emacs_qinfo_c' % (self.BUILD_BEMACS_LIB_DIR,) )

    def ruleDocs( self ):
        info( 'Running ruleDocs' )
        copyFile( '../Kits/readme.txt', self.BUILD_BEMACS_DOC_DIR, 0o444 )
        copyFile( '../Editor/PyQtBEmacs/bemacs.png', self.BUILD_BEMACS_DOC_DIR, 0o444 )
        for pattern in ('../HTML/*.html',
                        '../HTML/*.gif',
                        '../HTML/*.css',
                        '../HTML/*.js'):
            for src in glob.glob( pattern ):
                copyFile( src, self.BUILD_BEMACS_DOC_DIR, 0o444 )

    def setupEditorMakefile( self ):
        import setup

        system = platform.system().lower()
        if system in ('linux', 'macosx', 'netbsd'):
            setup_targets = set( [self.target, 'cli', 'unit-tests'] )

        else:
            setup_targets = set( ['gui', 'unit-tests'] )

        if not self.opt_sqlite3:
            setup_targets.add( 'utils' )

        info( 'Creating ../Editor/Makefile-all for %s' % ', '.join( sorted( setup_targets ) ) )

        cwd = os.getcwd()
        try:
            os.chdir( '../Editor' )
            setup_argv = [sys.argv[0]
                         ,platform.system().lower()
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

class BuildError(Exception):
    def __init__( self, msg ):
        super(BuildError, self).__init__( msg )

def info( msg ):
    print( '\033[32mInfo:\033[m %s' % (msg,) )
    sys.stdout.flush()

def error( msg ):
    print( '\033[31;1mError: %s\033[m' % (msg,) )
    sys.stdout.flush()

def logNothing( msg ):
    pass

def copyFile( src, dst, mode ):
    if os.path.isdir( dst ):
        dst = os.path.join( dst, os.path.basename( src ) )

    if os.path.exists( dst ):
        os.chmod( dst, 0o600 )
        os.remove( dst )

    shutil.copyfile( src, dst )
    os.chmod( dst, mode )

# use a python3 compatible subprocess.run() function
class CompletedProcess(object):
    def __init__(self, returncode, stdout=None, stderr=None):
        self.returncode = returncode

        if stdout is not None:
            self.stdout = stdout.decode( 'utf-8' )
        else:
            self.stdout = stdout

        if stderr is not None:
            self.stderr = stderr.decode( 'utf-8' )
        else:
            self.stderr = stderr

class Popen(subprocess.Popen):
    def __init__( self, *args, **kwargs ):
        super(Popen, self).__init__( *args, **kwargs )

    def __enter__(self):
        return self

    def __exit__(self, exc_type, value, traceback):
        if self.stdout:
            self.stdout.close()
        if self.stderr:
            self.stderr.close()

        # Wait for the process to terminate, to avoid zombies.
        self.wait()

def run( cmd, check=True, output=False, cwd=None ):
    kwargs = {}
    if type(cmd) is str:
        info( 'Running %s' % (cmd,) )
        kwargs['shell'] = True
    else:
        info( 'Running %s' % (' '.join( cmd ),) )
    if cwd:
        kwargs['cwd'] = cwd
    else:
        cwd = os.getcwd()

    if output:
        kwargs['stdout'] = subprocess.PIPE
        kwargs['stderr'] = subprocess.PIPE

    with Popen(cmd, **kwargs) as process:
        try:
            stdout, stderr = process.communicate( input=None )

        except:  # Including KeyboardInterrupt, communicate handled that.
            process.kill()
            # We don't call process.wait() as .__exit__ does that for us.
            raise

        retcode = process.poll()
        r = CompletedProcess( retcode, stdout, stderr )
        if check and retcode != 0:
            raise BuildError( 'Cmd failed %s - %r' % (retcode, cmd) )

    return r

def numCpus():
    return os.sysconf( os.sysconf_names['SC_NPROCESSORS_ONLN'] )

if __name__ == '__main__':
    sys.exit( BuildBEmacs().main( sys.argv ) )
