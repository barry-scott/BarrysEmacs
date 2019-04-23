#
#   Copyright (c) 2010-2019 Barry A. Scott
#
import os
import sys

_debug = False

pycxx_version = (7,1,2)
pycxx_version_str = '%d.%d.%d' % pycxx_version

def debug( msg ):
    if _debug:
        sys.stderr.write( 'Debug: %s\n' % (msg,) )

class SetupError(Exception):
    pass


#--------------------------------------------------------------------------------
class Setup:
    def __init__( self, argv ):
        self.opt_enable_debug = False
        self.opt_lib_dir = '/usr/local/lib/bemacs'
        self.opt_bemacs_gui = False
        self.opt_bemacs_cli = False
        self.opt_utils = False
        self.opt_unit_tests = False
        self.opt_coverage = False
        self.opt_system_pycxx = False
        self.opt_system_ucd = False
        self.opt_warnings_as_errors = True


        args = argv[1:]
        if len(args) < 3:
            raise SetupError( 'Usage: setup.py win32|win64|macosx|netbsd|linux> <gui|cli|utils|unit-tests> <makefile> [<options>]' )

        self.platform = args[0]
        del args[0]

        target = args[0].split( ',' )
        del args[0]

        if 'all' in target:
            if self.platform in ('linux', 'macosx', 'netbsd'):
                target = ['gui', 'cli', 'utils', 'unit-tests']
            else:
                target = ['gui', 'utils', 'unit-tests']

        if 'gui' in target:
            self.opt_bemacs_gui = True

        if 'cli' in target:
            self.opt_bemacs_cli = True

        if 'utils' in target:
            self.opt_utils = True

        if 'unit-tests' in target:
            self.opt_unit_tests = True

        self.makefile_name = args[0]
        del args[0]

        print( 'Info: Creating makefile for platform %s in %s' % (self.platform, self.makefile_name) )
        print( 'Info: Working folder %s' % (os.getcwd(),) )
        sys.stdout.flush()

        while len(args) > 0:
            if args[0] == '--debug':
                global _debug
                _debug = True
                del args[0]

            elif args[0] == '--enable-debug':
                self.opt_enable_debug = True
                del args[0]

            elif args[0].startswith( '--lib-dir=' ):
                self.opt_lib_dir = args[0][len('--lib-dir='):]
                del args[0]

            elif args[0].startswith( '--system-pycxx' ):
                self.opt_system_pycxx = True
                del args[0]

            elif args[0].startswith( '--system-ucd' ):
                self.opt_system_ucd = True
                del args[0]

            elif args[0].startswith( '--no-warnings-as-errors' ):
                self.opt_warnings_as_errors = False
                del args[0]

            elif args[0].startswith( '--coverage' ):
                self.opt_coverage = True
                del args[0]

            else:
                raise SetupError( 'Unknown arg %r' % (args[0],) )

        for name in sorted( os.environ ):
            debug( 'Env %s:%r' % (name, os.environ[ name ]) )

        self.setupCompile()

    def makePrint( self, line ):
        debug( 'makePrint( %r )' % (line,) )
        self.__makefile.write( line )
        self.__makefile.write( '\n' )

    def setupCompile( self ):
        print( 'Info: Setup Compiler' )
        if self.opt_bemacs_gui:
            print( 'Info: Build bemacs GUI' )

        if self.opt_bemacs_cli:
            print( 'Info: Build bemacs CLI' )

        if self.opt_utils:
            print( 'Info: Build bemacs Utils' )

        if self.opt_unit_tests:
            print( 'Info: Build bemacs Unit tests' )

        self.c_python_tools = None
        self.c_pybemacs = None
        self.c_clibemacs = None
        self.c_utils = None
        self.c_unit_tests = None
        self.unicode_header = None

        if self.platform == 'win32':
            self.c_utils = Win32CompilerMSVC90( self )
            self.c_unit_tests = Win32CompilerMSVC90( self )
            self.c_python_tools = Win32CompilerMSVC90( self )
            self.c_pybemacs = Win32CompilerMSVC90( self )

            pybemacs_feature_defines = [('EXEC_BF', '1')]

        elif self.platform == 'win64':
            self.c_utils = Win64CompilerVC14( self )
            self.c_unit_tests = Win64CompilerVC14( self )
            self.c_python_tools = Win64CompilerVC14( self )
            self.c_pybemacs = Win64CompilerVC14( self )

            pybemacs_feature_defines = [('EXEC_BF', '1')]

        elif self.platform == 'macosx':
            if self.opt_utils:
                self.c_utils = MacOsxCompilerGCC( self )

            if self.opt_unit_tests:
                self.c_unit_tests = MacOsxCompilerGCC( self )

            if self.opt_bemacs_gui:
                self.c_python_tools = MacOsxCompilerGCC( self )
                self.c_pybemacs = MacOsxCompilerGCC( self )

            if self.opt_bemacs_cli:
                self.c_clibemacs = MacOsxCompilerGCC( self )

            pybemacs_feature_defines = [('EXEC_BF', '1')]
            cli_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]

        elif self.platform == 'linux':
            if self.opt_utils:
                self.c_utils = LinuxCompilerGCC( self )

            if self.opt_unit_tests:
                self.c_unit_tests = LinuxCompilerGCC( self )

            if self.opt_bemacs_gui:
                self.c_python_tools = LinuxCompilerGCC( self )
                self.c_pybemacs = LinuxCompilerGCC( self )

            if self.opt_bemacs_cli:
                self.c_clibemacs = LinuxCompilerGCC( self )

            pybemacs_feature_defines = [('EXEC_BF', '1')]
            cli_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]

        elif self.platform == 'netbsd':
            if self.opt_utils:
                self.c_utils = NetBSDCompilerGCC( self )

            if self.opt_unit_tests:
                self.c_unit_tests = NetBSDCompilerGCC( self )

            if self.opt_bemacs_gui:
                self.c_python_tools = NetBSDCompilerGCC( self )
                self.c_pybemacs = NetBSDCompilerGCC( self )

            if self.opt_bemacs_cli:
                self.c_clibemacs = NetBSDCompilerGCC( self )

            pybemacs_feature_defines = [('EXEC_BF', '1')]
            cli_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]

        else:
            raise SetupError( 'Unknown platform %r' % (self.platform,) )

        if self.opt_unit_tests:
            self.c_unit_tests.setupUnittests()

        if self.opt_utils:
            self.c_utils.setupUtilities()

        if self.opt_bemacs_gui:
            self.c_pybemacs.setupPythonEmacs( pybemacs_feature_defines )

        if self.opt_bemacs_gui:
            self.c_python_tools.setupPythonTools()
            self.unicode_header = UnicodeDataHeader( self.c_pybemacs )

        if self.opt_bemacs_cli:
            self.c_clibemacs.setupCliEmacs( cli_feature_defines )
            if self.unicode_header is None:
                self.unicode_header = UnicodeDataHeader( self.c_clibemacs )

        if self.opt_utils:
            self.db_files = [
                Source( self.c_utils, 'Utilities/db_rtl/db_rtl.cpp' ),
                Source( self.c_utils, 'Source/Common/doprint.cpp' ),
                Source( self.c_utils, 'Source/Common/em_stat.cpp' ),
                Source( self.c_utils, 'Source/Common/emstring.cpp' ),
                Source( self.c_utils, 'Source/Common/emunicode.cpp',
                                        ['Include/Common/em_unicode_data.h'] ),
                Source( self.c_utils, 'Source/Common/file_name_compare.cpp' ),
                Source( self.c_utils, 'Source/Common/ndbm.cpp' ),
                Source( self.c_utils, 'Utilities/db_rtl/stub_rtl.cpp' ),
                ]

            if self.unicode_header is None:
                self.unicode_header = UnicodeDataHeader( self.c_utils )

            if self.platform in ['linux', 'macosx', 'netbsd']:
                self.db_files.append( Source( self.c_utils, 'Source/Unix/unixfile.cpp' ) )

            elif self.platform in ('win32', 'win64'):
                self.db_files.append( Source( self.c_utils, 'Source/Windows/win_file.cpp' ) )

        if self.opt_bemacs_gui:
            if not os.path.exists( self.c_pybemacs.expand( '%(PYCXXSRC)s' ) ):
                raise SetupError( 'PyCXX version %s expect to be in %s' % (pycxx_version, self.c_pybemacs.expand( '%(PYCXX)s' )) )

            self.pycxx_obj_file = [
                Source( self.c_pybemacs, '%(PYCXXSRC)s/cxxsupport.cxx' ),
                Source( self.c_pybemacs, '%(PYCXXSRC)s/cxx_extensions.cxx' ),
                Source( self.c_pybemacs, '%(PYCXXSRC)s/cxx_exceptions.cxx' ),
                Source( self.c_pybemacs, '%(PYCXXSRC)s/cxxextensions.c' ),
                Source( self.c_pybemacs, '%(PYCXXSRC)s/IndirectPythonInterface.cxx' ),
                ]

            self.pybemacs_specific_obj_files = [
                Source( self.c_pybemacs, 'Source/pybemacs/python_commands.cpp' ),
                Source( self.c_pybemacs, 'Source/pybemacs/pybemacs.cpp' ),
                Source( self.c_pybemacs, 'Source/pybemacs/bemacs_python.cpp' ),
                Source( self.c_pybemacs, 'Source/pybemacs/python_thread_control.cpp' ),
                ] + self.pycxx_obj_file

            if self.platform in ('linux', 'macosx', 'netbsd'):
                self.pybemacs_specific_obj_files.extend( [
                    Source( self.c_pybemacs, 'Source/Unix/unix_rtl_pybemacs.cpp' ),
                    ] )
            elif self.platform in ('win32', 'win64'):
                self.pybemacs_specific_obj_files.extend( [
                    Source( self.c_pybemacs, 'Source/Windows/win_rtl_pybemacs.cpp' ),
                    ] )

        if self.opt_bemacs_cli:
            self.cli_specific_obj_files = [
                Source( self.c_clibemacs, 'Source/Unix/unix_main.cpp',
                                            ['Source/Unix/unix_rtl.cpp'] ),
                Source( self.c_clibemacs, 'Source/Unix/unix_trm.cpp' ),
                Source( self.c_clibemacs, 'Source/Unix/trm_ansi.cpp' ),
                ]

        def makeObjFiles( compiler ):
            obj_files = [
                Source( compiler, 'Source/Common/emunicode.cpp',
                                    ['Include/Common/em_unicode_data.h'] ),
                Source( compiler, 'Source/Common/abbrev.cpp' ),
                Source( compiler, 'Source/Common/abspath.cpp' ),
                Source( compiler, 'Source/Common/arith.cpp' ),
                Source( compiler, 'Source/Common/buf_man.cpp' ),
                Source( compiler, 'Source/Common/buffer.cpp' ),
                Source( compiler, 'Source/Common/caseconv.cpp' ),
                Source( compiler, 'Source/Common/columns.cpp' ),
                Source( compiler, 'Source/Common/CommandLine.cpp' ),
                Source( compiler, 'Source/Common/dbman.cpp' ),
                Source( compiler, 'Source/Common/display.cpp' ),
                Source( compiler, 'Source/Common/display_insert_delete.cpp' ),
                Source( compiler, 'Source/Common/display_line.cpp' ),
                Source( compiler, 'Source/Common/doprint.cpp' ),
                Source( compiler, 'Source/Common/em_stat.cpp' ),
                Source( compiler, 'Source/Common/em_time.cpp' ),
                Source( compiler, 'Source/Common/emacs.cpp' ),
                Source( compiler, 'Source/Common/emacs_init.cpp' ),
                Source( compiler, 'Source/Common/emacs_proc.cpp' ),
                Source( compiler, 'Source/Common/emacsrtl.cpp' ),
                Source( compiler, 'Source/Common/emarray.cpp' ),
                Source( compiler, 'Source/Common/emstring.cpp' ),
                Source( compiler, 'Source/Common/emstrtab.cpp' ),
                Source( compiler, 'Source/Common/errlog.cpp' ),
                Source( compiler, 'Source/Common/file_name_compare.cpp' ),
                Source( compiler, 'Source/Common/fileio.cpp' ),
                Source( compiler, 'Source/Common/fio.cpp' ),
                Source( compiler, 'Source/Common/function.cpp' ),
                Source( compiler, 'Source/Common/getdb.cpp' ),
                Source( compiler, 'Source/Common/getdirectory.cpp' ),
                Source( compiler, 'Source/Common/getfile.cpp' ),
                Source( compiler, 'Source/Common/glob_var.cpp' ),
                Source( compiler, 'Source/Common/gui_input_mode.cpp' ),
                Source( compiler, 'Source/Common/journal.cpp' ),
                Source( compiler, 'Source/Common/key_names.cpp' ),
                Source( compiler, 'Source/Common/keyboard.cpp' ),
                Source( compiler, 'Source/Common/lispfunc.cpp' ),
                Source( compiler, 'Source/Common/macros.cpp' ),
                Source( compiler, 'Source/Common/mem_man.cpp' ),
                Source( compiler, 'Source/Common/metacomm.cpp' ),
                Source( compiler, 'Source/Common/minibuf.cpp' ),
                Source( compiler, 'Source/Common/mlispars.cpp' ),
                Source( compiler, 'Source/Common/mlispexp.cpp' ),
                Source( compiler, 'Source/Common/mlisproc.cpp' ),
                Source( compiler, 'Source/Common/mlprintf.cpp' ),
                Source( compiler, 'Source/Common/ndbm.cpp' ),
                Source( compiler, 'Source/Common/options.cpp' ),
                Source( compiler, 'Source/Common/queue.cpp' ),
                Source( compiler, 'Source/Common/save_env.cpp' ),
                Source( compiler, 'Source/Common/search.cpp' ),
                Source( compiler, 'Source/Common/search_extended_algorithm.cpp' ),
                Source( compiler, 'Source/Common/search_extended_parser.cpp' ),
                Source( compiler, 'Source/Common/search_interface.cpp' ),
                Source( compiler, 'Source/Common/search_simple_algorithm.cpp' ),
                Source( compiler, 'Source/Common/search_simple_engine.cpp' ),
                Source( compiler, 'Source/Common/simpcomm.cpp' ),
                Source( compiler, 'Source/Common/string_map.cpp' ),
                Source( compiler, 'Source/Common/subproc.cpp' ),
                Source( compiler, 'Source/Common/syntax.cpp' ),
                Source( compiler, 'Source/Common/term.cpp' ),
                Source( compiler, 'Source/Common/timer.cpp' ),
                Source( compiler, 'Source/Common/undo.cpp' ),
                Source( compiler, 'Source/Common/variable.cpp' ),
                Source( compiler, 'Source/Common/varthunk.cpp' ),
                Source( compiler, 'Source/Common/windman.cpp' ),
                Source( compiler, 'Source/Common/window.cpp' ),
                ]
            if self.platform in ('linux'):
                obj_files.extend( [
                    Source( compiler, 'Source/Unix/unixfile.cpp' ),
                    Source( compiler, 'Source/Unix/emacs_signal.cpp' ),
                    Source( compiler, 'Source/Unix/unixcomm.cpp' ),
                    Source( compiler, 'Source/Unix/ptyopen_linux.cpp' ),
                    ] )
            if self.platform in ('macosx', 'netbsd'):
                obj_files.extend( [
                    # similar enough for the same set of source files
                    Source( compiler, 'Source/Unix/unixfile.cpp' ),
                    Source( compiler, 'Source/Unix/emacs_signal.cpp' ),
                    Source( compiler, 'Source/Unix/unixcomm.cpp' ),
                    Source( compiler, 'Source/Unix/ptyopen_bsd.cpp' ),
                    ] )
            if self.platform in ('win32', 'win64'):
                obj_files.extend( [
                    Source( compiler, 'Source/Windows/win_file.cpp' ),
                    Source( compiler, 'Source/Windows/win_subproc.cpp' ),
                    #Source( compiler, 'Source/Windows/win_ext_func.cpp' ),
                    ] )
            return obj_files

        if self.opt_bemacs_gui:
            self.py_obj_files = makeObjFiles( self.c_pybemacs )

        self.all_exe = []
        if self.opt_bemacs_gui:
            self.all_exe.append(
                PythonExtension( self.c_pybemacs, '_bemacs', self.pybemacs_specific_obj_files + self.py_obj_files ) )

        if self.opt_utils:
            self.all_exe.extend( [
                Program( self.c_utils, 'dbadd',      [Source( self.c_utils, 'Utilities/dbadd/dbadd.cpp' )]       +self.db_files ),
                Program( self.c_utils, 'dbcreate',   [Source( self.c_utils, 'Utilities/dbcreate/dbcreate.cpp' )] +self.db_files ),
                Program( self.c_utils, 'dbprint',    [Source( self.c_utils, 'Utilities/dbprint/dbprint.cpp' )]   +self.db_files ),
                Program( self.c_utils, 'dbdel',      [Source( self.c_utils, 'Utilities/dbdel/dbdel.cpp' )]       +self.db_files ),
                Program( self.c_utils, 'dblist',     [Source( self.c_utils, 'Utilities/dblist/dblist.cpp' )]     +self.db_files ),
                Program( self.c_utils, 'mll2db',     [Source( self.c_utils, 'Utilities/mll2db/mll2db.cpp' )]     +self.db_files ),
                # test tools
                Program( self.c_utils, 'dumpjnl',    [Source( self.c_utils, 'Source/Common/dumpjnl.cpp' )] ),
                ] )

        if self.opt_unit_tests:
            self.all_exe.extend( [
                # unit tests
                Program( self.c_unit_tests, 'emunicode',  [Source( self.c_unit_tests, 'Source/Common/emunicode.cpp' )] ),
                ] )

        if self.opt_bemacs_gui:
            self.all_exe.append(
                Program( self.c_python_tools, 'python-types',[Source( self.c_python_tools, 'Source/pybemacs/python-types.cpp' )] + self.pycxx_obj_file ) )

        if self.opt_bemacs_cli:
            self.cli_obj_files = makeObjFiles( self.c_clibemacs )
            self.all_exe.append( Program( self.c_clibemacs, 'bemacs-cli', self.cli_specific_obj_files + self.cli_obj_files ) )

    def generateMakefile( self ):
        try:
            with open( self.makefile_name, 'wt' ) as self.__makefile:
                if self.opt_bemacs_gui:
                    self.c_pybemacs.generateMakefileHeader()

                if self.opt_bemacs_cli:
                    self.c_clibemacs.generateMakefileHeader()

                assert len(self.all_exe) > 0

                self.makePrint( 'all: %s' % (' '.join( [exe.getTargetFilename() for exe in self.all_exe] )) )
                self.makePrint( '' )

                for exe in self.all_exe:
                    exe.generateMakefile()

                if self.unicode_header is not None:
                    self.unicode_header.generateMakefile()

            return 0

        except ValueError as e:
            sys.stderr.write( 'Error: generateMakefile %s\n' % (e,) )
            return 1

#--------------------------------------------------------------------------------
class Compiler:
    def __init__( self, setup ):
        debug( 'Compiler.__init__()' )
        self.setup = setup

        self.__variables = {}

        self._addVar( 'PYCXX_VER',       pycxx_version_str )
        if setup.opt_enable_debug:
            self._addVar( 'DEBUG',           '_DEBUG')
        else:
            self._addVar( 'DEBUG',           'NDEBUG')

        self._addFromEnv( 'BUILDER_TOP_DIR' )

        self._addVar( 'PYTHON_VERSION', '%d.%d' % (sys.version_info[0], sys.version_info[1]) )

    def platformFilename( self, filename ):
        return filename

    def makePrint( self, line ):
        self.setup.makePrint( line )

    def generateMakefileHeader( self ):
        raise NotImplementedError( 'generateMakefileHeader' )

    def _addFromEnv( self, name ):
        debug( 'Compiler._addFromEnv( %r )' % (name,) )

        value = os.environ[ name ]
        if value[0] == '"' and value[-1] == '"':
            value = value[1:-1]

        self._addVar( name, value )

    def _addVar( self, name, value ):
        debug( 'Compiler._addVar( %r, %r )' % (name,value) )

        try:
            if '%' in value:
                value = value % self.__variables

            self.__variables[ name ] = value

        except TypeError:
            raise SetupError( 'Cannot translate name %r value %r' % (name, value) )

        except KeyError as e:
            raise SetupError( 'Cannot translate name %r value %r - %s' % (name, value, e) )

    def addFeatureDefines( self, feature_defines=None ):
        if feature_defines is None:
            feature_defines = []

        print( 'feature_defines', feature_defines )
        self._addVar( 'FEATURE_DEFINES',' '.join( '-D%s=%s' % (name, value) for name, value in feature_defines ) )

    def expand( self, s ):
        try:
            return s % self.__variables

        except (ValueError, TypeError, KeyError) as e:
            print( 'Error: %s' % (e,) )
            print( 'String: %s' % (s,) )
            print( 'Vairables: %r' % (self.__variables,) )

            raise SetupError( 'Cannot translate string (%s)' % (e,) )


class Win64CompilerVC14(Compiler):
    def __init__( self, setup ):
        Compiler.__init__( self, setup )

        self._addVar( 'PYCXX',          r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s' )
        self._addVar( 'PYCXXSRC',       r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s\Src' )
        self._addVar( 'UCDDIR',         r'%(BUILDER_TOP_DIR)s\Imports\ucd' )

        self._addVar( 'PYTHONDIR',      sys.exec_prefix )
        self._addVar( 'PYTHON_INCLUDE', r'%(PYTHONDIR)s\include' )
        self._addVar( 'PYTHON_LIB',     r'%(PYTHONDIR)s\libs' )
        self._addVar( 'PYTHON',         r'%(PYTHONDIR)s\python.exe' )

        self._addVar( 'LINK_LIBS',      '' )
        self._addVar( 'OBJ_SUFFIX',     '.obj' )


    def platformFilename( self, filename ):
        return filename.replace( '/', '\\' )

    def getPythonExtensionFileExt( self ):
        return '.pyd'

    def getProgramExt( self ):
        return '.exe'

    def generateMakefileHeader( self ):
        self.makePrint( '#' )
        self.makePrint( '# Bemacs Makefile generated by setup.py' )
        self.makePrint( '#' )
        self.makePrint( 'CCC=cl /nologo' )
        self.makePrint( 'CC=cl /nologo' )
        self.makePrint( '' )
        self.makePrint( 'LDSHARED=$(CCC) /LD /Zi /MT /EHsc' )
        self.makePrint( 'LDEXE=$(CCC) /Zi /MT /EHsc' )
        self.makePrint( '' )

    def ruleLinkProgram( self, target ):
        pyd_filename = target.getTargetFilename()
        pdb_filename = target.getTargetFilename( '.pdb' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link Program %s' % (pyd_filename,) )
        rules.append( '\t@if not exist %(EDIT_EXE)s mkdir %(EDIT_EXE)s' )
        rules.append( '\t@$(LDEXE)  %%(CCCFLAGS)s /Fe%s /Fd%s %s %%(LINK_LIBS)s' %
                            (pyd_filename, pdb_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleLinkShared( self, target ):
        pyd_filename = target.getTargetFilename()
        pdb_filename = target.getTargetFilename( '.pdb' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link Shared %s' % (pyd_filename,) )
        rules.append( '\t@if not exist %(EDIT_EXE)s mkdir %(EDIT_EXE)s' )
        rules.append( '\t@$(LDSHARED)  %%(CCCFLAGS)s /Fe%s /Fd%s %s %%(LINK_LIBS)s' %
                            (pyd_filename, pdb_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleCxx( self, target ):
        obj_filename = target.getTargetFilename()
        pdb_filename = target.dependent.getTargetFilename( '.pdb' )
        pdb_dir = os.path.dirname( pdb_filename )

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, target.getTargetFilename()) )
        rules.append( '\t@if not exist %(EDIT_OBJ)s mkdir %(EDIT_OBJ)s' )
        rules.append( '\t@if not exist %s mkdir %s' % (pdb_dir, pdb_dir) )   # For .pdb file
        rules.append( '\t@$(CCC) /c %%(CCCFLAGS)s /Fo%s /Fd%s %s' % (obj_filename, pdb_filename, target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleC( self, target ):
        # can reuse the C++ rule
        self.ruleCxx( target )

    def ruleClean( self, filename ):
        rules = []
        rules.append( 'clean::' )
        rules.append( '\tif exist %s del %s' % (filename, filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def setupUtilities( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-utils' )
        self._addVar( 'EDIT_EXE',       r'exe-utils' )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc /W4 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win64\"" '
                                        r'"-DCPU_TYPE=\"x86_64\"" "-DUI_TYPE=\"console\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-D_UNICODE -DUNICODE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

    def setupUnittests( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-unit-tests' )
        self._addVar( 'EDIT_EXE',       r'exe-unit-tests' )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc /W4 '
                                        r'/DUNIT_TEST=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win64\"" '
                                        r'"-DCPU_TYPE=\"x86_64\"" "-DUI_TYPE=\"console\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-D_UNICODE -DUNICODE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

    def setupPythonEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )
        self._addVar( 'EDIT_OBJ',       r'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       r'exe-pybemacs' )
        self._addVar( 'LINK_LIBS',      'advapi32.lib '
                                        '%%(PYTHON_LIB)s\python%d%d.lib' %
                                        (sys.version_info.major, sys.version_info.minor) )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win64\"" '
                                        r'"-DCPU_TYPE=\"x86_64\"" "-DUI_TYPE=\"python\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-D_UNICODE -DUNICODE '
                                        r'-U_DEBUG '
                                        r'%(FEATURE_DEFINES)s '
                                        r'-D%(DEBUG)s' )

    def setupCliEmacs( self, feature_defines=None ):
        raise SetupError( 'no support for CLI on Windows' )

    def setupPythonTools( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       r'exe-python-tools' )
        self._addVar( 'LINK_LIBS',      '%%(PYTHON_LIB)s\python%d%d.lib' %
                                        (sys.version_info.major, sys.version_info.minor) )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-D_UNICODE -DUNICODE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

class Win32CompilerMSVC90(Compiler):
    def __init__( self, setup ):
        Compiler.__init__( self, setup )

        self._addVar( 'PYCXX',          r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s' )
        self._addVar( 'PYCXXSRC',       r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s\Src' )
        self._addVar( 'UCDDIR',         r'%(BUILDER_TOP_DIR)s\Imports\ucd' )

        self._addVar( 'PYTHONDIR',      sys.exec_prefix )
        self._addVar( 'PYTHON_INCLUDE', r'%(PYTHONDIR)s\include' )
        self._addVar( 'PYTHON_LIB',     r'%(PYTHONDIR)s\libs' )
        self._addVar( 'PYTHON',         r'%(PYTHONDIR)s\python.exe' )
        self._addVar( 'OBJ_SUFFIX',     '.obj' )

    def platformFilename( self, filename ):
        return filename.replace( '/', '\\' )

    def getPythonExtensionFileExt( self ):
        return '.pyd'

    def getProgramExt( self ):
        return '.exe'

    def generateMakefileHeader( self ):
        self.makePrint( '#' )
        self.makePrint( '# Bemacs Makefile generated by setup.py' )
        self.makePrint( '#' )
        self.makePrint( 'CCC=cl /nologo' )
        self.makePrint( 'CC=cl /nologo' )
        self.makePrint( '' )
        self.makePrint( 'LDSHARED=$(CCC) /LD /Zi /MT /EHsc' )
        self.makePrint( 'LDEXE=$(CCC) /Zi /MT /EHsc' )
        self.makePrint( '' )

    def ruleLinkProgram( self, target ):
        pyd_filename = target.getTargetFilename()
        pdb_filename = target.getTargetFilename( '.pdb' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (pyd_filename,) )
        rules.append( '\t@$(LDEXE)  %%(CCCFLAGS)s /Fe%s /Fd%s %s Advapi32.lib' %
                            (pyd_filename, pdb_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleLinkShared( self, target ):
        pyd_filename = target.getTargetFilename()
        pdb_filename = target.getTargetFilename( '.pdb' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (pyd_filename,) )
        rules.append( '\t@mkdir %(EDIT_EXE)s' )
        rules.append( '\t@$(LDSHARED)  %%(CCCFLAGS)s /Fe%s /Fd%s %s %%(PYTHON_LIB)s\python%d%d.lib Advapi32.lib' %
                            (pyd_filename, pdb_filename, ' '.join( all_objects ), sys.version_info.major, sys.version_info.minor) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleCxx( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, target.getTargetFilename()) )
        rules.append( '\t@mkdir %(EDIT_OBJ)s' )
        rules.append( '\t@$(CCC) /c %%(CCCFLAGS)s /Fo%s /Fd%s %s' % (obj_filename, target.dependent.getTargetFilename( '.pdb' ), target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleC( self, target ):
        # can reuse the C++ rule
        self.ruleCxx( target )

    def ruleClean( self, filename ):
        rules = []
        rules.append( 'clean::' )
        rules.append( '\tif exist %s del %s' % (filename, filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def setupUtilities( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-utils' )
        self._addVar( 'EDIT_EXE',       r'exe-utils' )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win32\"" '
                                        r'"-DCPU_TYPE=\"i386\"" "-DUI_TYPE=\"console\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

    def setupUnittests( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-unit-tests' )
        self._addVar( 'EDIT_EXE',       r'exe-unit-tests' )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'/DUNIT_TEST=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win32\"" '
                                        r'"-DCPU_TYPE=\"i386\"" "-DUI_TYPE=\"console\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

    def setupPythonEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )
        self._addVar( 'EDIT_OBJ',       r'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       r'exe-pybemacs' )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win32\"" '
                                        r'"-DCPU_TYPE=\"i386\"" "-DUI_TYPE=\"python\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'%(FEATURE_DEFINES)s '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

    def setupCliEmacs( self, feature_defines=None ):
        raise SetupError( 'no support for CLI on Windows' )

    def setupPythonTools( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       r'exe-python-tools' )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

class CompilerGCC(Compiler):
    def __init__( self, setup ):
        Compiler.__init__( self, setup )

        if setup.opt_system_ucd:
            self._addVar( 'UCDDIR',         '/usr/share/unicode/ucd' )

        else:
            self._addVar( 'UCDDIR',         '%(BUILDER_TOP_DIR)s/Imports/ucd' )

        if setup.opt_system_pycxx:
            self._addVar( 'PYCXX',          '/usr/include' )
            self._addVar( 'PYCXXSRC',       '/usr/src/CXX' )

        else:
            self._addVar( 'PYCXX',          '%(BUILDER_TOP_DIR)s/Imports/pycxx-%(PYCXX_VER)s' )
            self._addVar( 'PYCXXSRC',       '%(BUILDER_TOP_DIR)s/Imports/pycxx-%(PYCXX_VER)s/Src' )

        if self.setup.platform == 'macosx':
            self._addVar( 'CCC',        'g++ -arch x86_64' )
            self._addVar( 'CC',         'gcc -arch x86_64' )

        else:
            self._addVar( 'CCC',        'g++' )
            self._addVar( 'CC',         'gcc' )

        self._addVar( 'LINK_LIBS',      '' )
        self._addVar( 'OBJ_SUFFIX',     '.o' )

        if self.setup.opt_enable_debug:
            self._addVar( 'CCC_OPT', '-Og' )

        else:
            self._addVar( 'CCC_OPT', '-O3' )

        if self.setup.opt_warnings_as_errors:
            self._addVar( 'CCC_WARNINGS', '-Werror ' )

        else:
            self._addVar( 'CCC_WARNINGS', '' )

    def getPythonExtensionFileExt( self ):
        return '.so'

    def getProgramExt( self ):
        return ''

    def generateMakefileHeader( self ):
        self.makePrint( '#' )
        self.makePrint( '# Bemacs Makefile generated by setup.py' )
        self.makePrint( '#' )
        self.makePrint( '' )

    def ruleLinkProgram( self, target ):
        target_filename = target.getTargetFilename()

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = []

        rules.append( '%s : %s' % (target_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (target_filename,) )
        rules.append( '\t@mkdir -p %(EDIT_EXE)s' )
        rules.append( '\t@%%(LDEXE)s -o %s %%(CCCFLAGS)s %%(LINK_LIBS)s %s' % (target_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleLinkShared( self, target ):
        target_filename = target.getTargetFilename()

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = []

        rules.append( '%s : %s' % (target_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (target_filename,) )
        rules.append( '\t@mkdir -p %(EDIT_EXE)s' )
        rules.append( '\t@%%(LDSHARED)s -o %s %%(CCCFLAGS)s %s' % (target_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleCxx( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, obj_filename) )
        rules.append( '\t@mkdir -p %(EDIT_OBJ)s' )
        rules.append( '\t%%(CCC)s -c %%(CCCFLAGS)s -o %s  %s' % (obj_filename, target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleC( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, obj_filename) )
        rules.append( '\t@mkdir -p %(EDIT_OBJ)s' )
        rules.append( '\t@%%(CC)s -c %%(CCFLAGS)s -o %s  %s' % (obj_filename, target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleClean( self, filename ):
        rules = []
        rules.append( 'clean::' )
        rules.append( '\trm -f %s' % (filename,) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )


class MacOsxCompilerGCC(CompilerGCC):
    def __init__( self, setup ):
        CompilerGCC.__init__( self, setup )

    def setupUtilities( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-utils' )
        self._addVar( 'EDIT_EXE',       'exe-utils' )

        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-DDARWIN '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g' )
        self._addVar( 'OBJ_SUFFIX',     '.o' )


    def setupUnittests( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-unit-tests' )
        self._addVar( 'EDIT_EXE',       'exe-unit-tests' )
        self._addVar( 'CCFLAGS',        '-g -O0 '
                                        '-DUNIT_TEST=1 '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-DDARWIN '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        self._addVar( 'PYTHONDIR',      '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s' )
        self._addVar( 'PYTHON_FRAMEWORK', '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s/Python' )

        self._addVar( 'PYTHON',         '%(PYTHONDIR)s/Resources/Python.app/Contents/MacOS/Python' )
        self._addVar( 'PYTHON_INCLUDE', '%(PYTHONDIR)s/include/python%(PYTHON_VERSION)sm' )

        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"python\\"" '
                                        '-DDARWIN '
                                        '%(FEATURE_DEFINES)s '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )


        self._addVar( 'LDSHARED',       '%(CCC)s -bundle -g '
                                        '-framework System '
                                        '%(PYTHON_FRAMEWORK)s '
                                        '-framework CoreFoundation '
                                        '-framework Kerberos '
                                        '-framework Security' )

    def setupCliEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-cli-bemacs' )
        self._addVar( 'EDIT_EXE',       'exe-cli-bemacs' )

        self._addVar( 'CCCFLAGS',       '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -fexceptions -frtti '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"ANSI\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '-D%(DEBUG)s' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonTools( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       'exe-python-tools' )

        self._addVar( 'PYTHONDIR',      '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s' )
        self._addVar( 'PYTHON_FRAMEWORK', '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s/Python' )

        self._addVar( 'PYTHON',         '%(PYTHONDIR)s/Resources/Python.app/Contents/MacOS/Python' )
        self._addVar( 'PYTHON_INCLUDE', '%(PYTHONDIR)s/include/python%(PYTHON_VERSION)sm' )

        self._addVar( 'CCCFLAGS',       '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -fexceptions -frtti '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '-D%(DEBUG)s' )

        self._addVar( 'LDEXE',          '%(CCC)s -g '
                                        '-framework System '
                                        '%(PYTHON_FRAMEWORK)s ' )

class LinuxCompilerGCC(CompilerGCC):
    def __init__( self, setup ):
        CompilerGCC.__init__( self, setup )

    def setupUtilities( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-utils' )
        self._addVar( 'EDIT_EXE',       'exe-utils' )
        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s' )
        self._addVar( 'OBJ_SUFFIX',     '.o' )

    def setupUnittests( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-unit-tests' )
        self._addVar( 'EDIT_EXE',       'exe-unit-tests' )
        self._addVar( 'CCFLAGS',        '-g -O0 '
                                        '-DUNIT_TEST=1 '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)sm' % (sys.prefix,) )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"python\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g ' )
        self._addVar( 'LDSHARED',       '%(CCC)s -shared -g ' )

    def setupCliEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-cli-bemacs' )
        self._addVar( 'EDIT_EXE',       'exe-cli-bemacs' )

        self._addVar( 'BEMACS_LIB_DIR', self.setup.opt_lib_dir )

        if self.setup.opt_coverage:
            self._addVar( 'CCC_OPT',    '-O0 '
                                        '-ftest-coverage '
                                        '-fprofile-arcs '
                                        '-fprofile-abs-path '
                                        '-fprofile-dir=%s ' % (os.getcwd(),) )

        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"ANSI\\"" '
                                        '-D%(DEBUG)s '
                                        '%(FEATURE_DEFINES)s '
                                        '-DBEMACS_LIB_DIR=\\"%(BEMACS_LIB_DIR)s\\"' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s ' )

    def setupPythonTools( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       'exe-python-tools' )

        self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)sm' % (sys.prefix,) )
        self._addVar( 'LINK_LIBS', '-L%s/lib64 -lpython%d.%dm' % (sys.prefix, sys.version_info.major, sys.version_info.minor) )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )

class NetBSDCompilerGCC(CompilerGCC):
    def __init__( self, setup ):
        CompilerGCC.__init__( self, setup )

    def setupUtilities( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-utils' )
        self._addVar( 'EDIT_EXE',       'exe-utils' )
        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -std=c++11 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s' )
        self._addVar( 'OBJ_SUFFIX',     '.o' )

    def setupUnittests( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-unit-tests' )
        self._addVar( 'EDIT_EXE',       'exe-unit-tests' )
        self._addVar( 'CCFLAGS',        '-g -O0 '
                                        '-DUNIT_TEST=1 '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -std=c++11 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)sm' % (sys.prefix,) )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -std=c++11 '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"python\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g ' )
        self._addVar( 'LDSHARED',       '%(CCC)s -shared -g ' )

    def setupCliEmacs( self, feature_defines=None ):
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-cli-bemacs' )
        self._addVar( 'EDIT_EXE',       'exe-cli-bemacs' )

        self._addVar( 'BEMACS_LIB_DIR', self.setup.opt_lib_dir )

        if self.setup.opt_coverage:
            self._addVar( 'CCC_OPT',    '-O0 '
                                        '-ftest-coverage '
                                        '-fprofile-arcs '
                                        '-fprofile-abs-path '
                                        '-fprofile-dir=%s ' % (os.getcwd(),) )

        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -std=c++11 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"ANSI\\"" '
                                        '-D%(DEBUG)s '
                                        '%(FEATURE_DEFINES)s '
                                        '-DBEMACS_LIB_DIR=\\"%(BEMACS_LIB_DIR)s\\"' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s ' )

    def setupPythonTools( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       'exe-python-tools' )

        self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)sm' % (sys.prefix,) )
        self._addVar( 'LINK_LIBS', '-L%s/lib64 -lpython%d.%dm' % (sys.prefix, sys.version_info.major, sys.version_info.minor) )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -std=c++11 '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '-D%(DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )

#--------------------------------------------------------------------------------
class Target:
    def __init__( self, compiler, all_sources ):
        self.compiler = compiler
        self.__generated = False
        self.dependent = None

        self.all_sources = all_sources
        for source in self.all_sources:
            source.setDependent( self )

    def getTargetFilename( self, ext=None ):
        raise NotImplementedError( '%s.getTargetFilename' % self.__class__.__name__ )

    def generateMakefile( self ):
        if self.__generated:
            return

        self.__generated = True
        return self._generateMakefile()

    def _generateMakefile( self ):
        raise NotImplementedError( '_generateMakefile' )

    def ruleClean( self, ext=None ):
        target_filename = self.getTargetFilename( ext )

        self.compiler.ruleClean( target_filename )

    def setDependent( self, dependent ):
        debug( '%r.setDependent( %r )' % (self, dependent) )
        self.dependent = dependent


class Program(Target):
    def __init__( self, compiler, output, all_sources ):
        self.output = output

        Target.__init__( self, compiler, all_sources )
        debug( '%r.__init__( %r )' % (self, self.all_sources) )

    def __repr__( self ):
        return '<Program:%s>' % (self.output,)

    def getTargetFilename( self, ext=None ):
        if ext is None:
            ext = self.compiler.getProgramExt()

        return self.compiler.platformFilename( self.compiler.expand( '%%(EDIT_EXE)s/%s%s' % (self.output, ext) ) )

    def _generateMakefile( self ):
        debug( '%r._generateMakefile() dependent %r' % (self, self.dependent) )

        self.compiler.ruleLinkProgram( self )

        if self.compiler.getProgramExt() != '':
            # assume windows compiler that creates more them just the .exe
            self.compiler.ruleClean( self.getTargetFilename( '.*' ) )

        else:
            self.compiler.ruleClean( self.getTargetFilename() )

        for source in self.all_sources:
            source.generateMakefile()

class PythonExtension(Target):
    def __init__( self, compiler, output, all_sources ):
        self.output = output

        Target.__init__( self, compiler, all_sources )
        debug( '%r.__init__( %r )' % (self, self.all_sources) )

        for source in self.all_sources:
            source.setDependent( self )

    def __repr__( self ):
        return '<PythonExtension:%s>' % (self.output,)

    def getTargetFilename( self, ext=None ):
        if ext is None:
            ext = self.compiler.getPythonExtensionFileExt()

        return self.compiler.platformFilename( self.compiler.expand( '%%(EDIT_EXE)s/%s%s' % (self.output, ext) ) )

    def _generateMakefile( self ):
        debug( '%s.generateMakefile() for %r' % (self, self.output,) )

        self.compiler.ruleLinkShared( self )
        self.compiler.ruleClean( self.getTargetFilename( '.*' ) )

        for source in self.all_sources:
            source.generateMakefile()

class Source(Target):
    def __init__( self, compiler, src_filename, all_dependencies=None ):
        self.src_filename = compiler.platformFilename( compiler.expand( src_filename ) )

        Target.__init__( self, compiler, [] )

        debug( '%r.__init__( %r )' % (self, self.all_sources) )

        self.all_dependencies = all_dependencies
        if self.all_dependencies is None:
            self.all_dependencies = []

        self.rule = None

    def __repr__( self ):
        return '<Source:%s>' % (self.src_filename)

    def getTargetFilename( self, ext=None ):
        #if not os.path.exists( self.src_filename ):
        #    raise ValueError( 'Cannot find source %s' % (self.src_filename,) )

        basename = os.path.basename( self.src_filename )
        if basename.endswith( '.cpp' ):
            self.rule = self.compiler.ruleCxx
            return self.compiler.platformFilename( self.compiler.expand( r'%%(EDIT_OBJ)s/%s%%(OBJ_SUFFIX)s' % (basename[:-len('.cpp')],) ) )

        if basename.endswith( '.cxx' ):
            self.rule = self.compiler.ruleCxx
            return self.compiler.platformFilename( self.compiler.expand( r'%%(EDIT_OBJ)s/%s%%(OBJ_SUFFIX)s' % (basename[:-len('.cxx')],) ) )

        if basename.endswith( '.c' ):
            self.rule = self.compiler.ruleC
            return self.compiler.platformFilename( self.compiler.expand( r'%%(EDIT_OBJ)s/%s%%(OBJ_SUFFIX)s' % (basename[:-len('.c')],) ) )

        raise SetupError( 'unknown source %r' % (self.src_filename,) )

    def _generateMakefile( self ):
        debug( '%r._generateMakefile()' % (self,) )

        self.rule( self )
        self.compiler.ruleClean( self.getTargetFilename() )

class UnicodeDataHeader(Target):
    def __init__( self, compiler ):
        Target.__init__( self, compiler, [] )

        debug( '%r.__init__( %r )' % (self, self.all_sources) )

    def __repr__( self ):
        return '<UnicodeDataHeader:%s>' % (self.getTargetFilename(),)

    def makePrint( self, line ):
        self.compiler.makePrint( line )

    def getTargetFilename( self, ext=None ):
        return self.compiler.platformFilename( 'Include/Common/em_unicode_data.h' )

    def generateMakefile( self ):
        debug( '%r.generateMakefile()' % (self,) )
        rules = ['']

        rules.append( '%s : make_unicode_data.py' % (self.getTargetFilename(),) )
        rules.append( '\t@ echo Info: Make %s' % (self.getTargetFilename(),) )
        rules.append( '\t%%(PYTHON)s make_unicode_data.py %%(UCDDIR)s/UnicodeData.txt %%(UCDDIR)s/CaseFolding.txt %s' %
                            (self.getTargetFilename(),) )

        self.makePrint( self.compiler.expand( '\n'.join( rules ) ) )
        self.ruleClean()

#--------------------------------------------------------------------------------
def main( argv ):
    try:
        print( "Info: setup.py for Barry's Emacs" )
        s = Setup( argv )
        rc =  s.generateMakefile()
        print( "Info: setup.py complete - %d" % (rc,) )
        return rc

    except SetupError as e:
        sys.stderr.write( 'Error: setup.py %s\n' % (e,) )
        return 1

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
