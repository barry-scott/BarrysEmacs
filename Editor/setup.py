#
#   Copyright (c) 2010-2020 Barry A. Scott
#
import os
import sys
import subprocess
import glob

pycxx_version = (7,1,5)
pycxx_version_str = '%d.%d.%d' % pycxx_version

sys.path.insert( 0, '../Builder' )
import build_log
log = build_log.BuildLog()

class SetupError(Exception):
    pass

#--------------------------------------------------------------------------------
class Setup:
    def __init__( self, argv ):
        self.opt_enable_debug = False
        self.opt_colour = False
        self.opt_lib_dir = '/usr/local/lib/bemacs'
        self.opt_doc_dir = '/usr/local/share/bemacs/doc'
        self.opt_bemacs_gui = False
        self.opt_bemacs_cli = False
        self.opt_utils = False
        self.opt_unit_tests = False
        self.opt_coverage = False
        self.opt_system_pycxx = False
        self.opt_system_ucd = False
        self.opt_sftp = True
        self.opt_system_libssh = False
        self.opt_warnings_as_errors = True
        self.opt_sqlite = True
        self.opt_system_sqlite = False
        self.opt_hunspell = True
        self.opt_system_hunspell = False

        self.parseArgs( argv )

        log.setColour( self.opt_colour )

        log.info( 'Creating makefile for platform %s in %s' % (self.platform, self.makefile_name) )
        log.info( 'Working folder %s' % (os.getcwd(),) )
        if self.opt_bemacs_cli:
            log.info( 'Building bemacs-cli' )
        if self.opt_bemacs_gui:
            log.info( 'Building bemacs-gui' )

        for name in sorted( os.environ ):
            log.debug( 'Env %s:%r' % (name, os.environ[ name ]) )

        self.setupCompile()

    def parseArgs( self, argv ):
        try:
            args = iter(argv)
            next(args) # skip script name

            self.platform = next(args).lower()

            target = next(args).split( ',' )

            if 'gui' in target:
                self.opt_bemacs_gui = True

            if 'cli' in target:
                self.opt_bemacs_cli = True

            if 'utils' in target:
                self.opt_utils = True

            if 'unit-tests' in target:
                self.opt_unit_tests = True

            self.makefile_name = next(args)

        except StopIteration:
            raise SetupError( 'Usage: setup.py win32|win64|macosx|netbsd|linux> <gui|cli|utils|unit-tests> <makefile> [<options>]' )

        try:
            while True:
                arg = next(args)
                if arg == '--debug':
                    log.setDebug( True )

                elif arg == '--colour':
                    self.opt_colour = True

                elif arg == '--enable-debug':
                    self.opt_enable_debug = True

                elif arg.startswith( '--lib-dir=' ):
                    self.opt_lib_dir = arg[len('--lib-dir='):]

                elif arg.startswith( '--doc-dir=' ):
                    self.opt_doc_dir = arg[len('--doc-dir='):]

                elif arg == '--system-hunspell':
                    self.opt_system_hunspell = True

                elif arg == '--system-libssh':
                    self.opt_system_libssh = True

                elif arg == '--system-sqlite':
                    self.opt_system_sqlite = True

                elif arg == '--system-pycxx':
                    self.opt_system_pycxx = True

                elif arg == '--system-ucd':
                    self.opt_system_ucd = True

                elif arg == '--no-warnings-as-errors':
                    self.opt_warnings_as_errors = False

                elif arg == '--coverage':
                    self.opt_coverage = True

                elif arg == '--no-sftp':
                    self.opt_sftp = False

                elif arg == '--no-sqlite':
                    self.opt_sqlite = False

                elif arg == '--no-hunspell':
                    self.opt_hunspell = False

                else:
                    raise SetupError( 'Unknown option in Editor/setup.py %r' % (arg,) )

        except StopIteration:
            pass

    def makePrint( self, line ):
        log.debug( 'makePrint( %r )' % (line,) )
        self.__makefile.write( line )
        self.__makefile.write( '\n' )

    def setupCompile( self ):
        log.info( 'Setup Compiler' )
        if self.opt_bemacs_gui:
            log.info( 'Build bemacs GUI' )

        if self.opt_bemacs_cli:
            log.info( 'Build bemacs CLI' )

        if self.opt_utils:
            log.info( 'Build bemacs Utils' )

        if self.opt_unit_tests:
            log.info( 'Build bemacs Unit tests' )

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

            pybemacs_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]
            utils_feature_defines = []

            if self.opt_sqlite:
                pybemacs_feature_defines.append( ('DB_SQLITE', '1') )
                utils_feature_defines.append( ('DB_SQLITE', '1') )

            if self.opt_hunspell:
                pybemacs_feature_defines.append( ('SPELL_CHECKER', '1') )
                pybemacs_feature_defines.append( ('SPELL_DICTIONARY_DIR', '\\"emacs_library:\\"') )

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

            pybemacs_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]
            cli_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]
            utils_feature_defines = []

            if self.opt_sqlite:
                pybemacs_feature_defines.append( ('DB_SQLITE', '1') )
                cli_feature_defines.append( ('DB_SQLITE', '1') )
                utils_feature_defines.append( ('DB_SQLITE', '1') )

            if self.opt_hunspell:
                pybemacs_feature_defines.append( ('SPELL_CHECKER', '1') )
                pybemacs_feature_defines.append( ('SPELL_DICTIONARY_DIR', '\\"emacs_library:\\"') )
                cli_feature_defines.append( ('SPELL_CHECKER', '1') )
                cli_feature_defines.append( ('SPELL_DICTIONARY_DIR', '\\"emacs_library:\\"') )

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

            pybemacs_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]
            cli_feature_defines = [('EXEC_BF', '1'), ('SUBPROCESSES', '1')]
            utils_feature_defines = []

            if self.opt_sftp:
                pybemacs_feature_defines.append( ('SFTP', '1') )
                cli_feature_defines.append( ('SFTP', '1') )

            if self.opt_sqlite:
                pybemacs_feature_defines.append( ('DB_SQLITE', '1') )
                cli_feature_defines.append( ('DB_SQLITE', '1') )
                utils_feature_defines.append( ('DB_SQLITE', '1') )

            if self.opt_hunspell:
                spell_dictionary_dir = self.findSpellDictionaryDir()
                log.info( 'Using SPELL_DICTIONARY_DIR %s' % (spell_dictionary_dir,) )

                pybemacs_feature_defines.append( ('SPELL_CHECKER', '1') )
                pybemacs_feature_defines.append( ('SPELL_DICTIONARY_DIR',
                                                  '\\"%s/\\"' % (spell_dictionary_dir,) ) )
                cli_feature_defines.append( ('SPELL_CHECKER', '1') )
                cli_feature_defines.append( ('SPELL_DICTIONARY_DIR',
                                             '\\"%s/\\"' % (spell_dictionary_dir,) ) )

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
            utils_feature_defines = []

            if self.opt_sqlite:
                pybemacs_feature_defines.append( ('DB_SQLITE', '1') )
                cli_feature_defines.append( ('DB_SQLITE', '1') )
                utils_feature_defines.append( ('DB_SQLITE', '1') )

            if self.opt_hunspell:
                spell_dictionary_dir = self.findSpellDictionaryDir()
                log.info( 'Using SPELL_DICTIONARY_DIR %s' % (spell_dictionary_dir,) )

                pybemacs_feature_defines.append( ('SPELL_CHECKER', '1') )
                pybemacs_feature_defines.append( ('SPELL_DICTIONARY_DIR',
                                                  '\\"%s/\\"' % (spell_dictionary_dir,) ) )
                cli_feature_defines.append( ('SPELL_CHECKER', '1') )
                cli_feature_defines.append( ('SPELL_DICTIONARY_DIR',
                                             '\\"%s/\\"' % (spell_dictionary_dir,) ) )
        else:
            raise SetupError( 'Unknown platform %r' % (self.platform,) )

        if self.opt_unit_tests:
            self.c_unit_tests.setupUnittests()

        if self.opt_utils:
            self.c_utils.setupUtilities( utils_feature_defines )

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
                Source( self.c_utils, 'Utilities/db_rtl/stub_rtl.cpp' ),
                ]

            if self.opt_sqlite:
                self.db_files.append( Source( self.c_utils, 'Source/Common/db_sqlite3.cpp' ) )
            else:
                self.db_files.append( Source( self.c_utils, 'Source/Common/ndbm.cpp' ) )

            if self.unicode_header is None:
                self.unicode_header = UnicodeDataHeader( self.c_utils )

            if self.platform in ['linux', 'macosx', 'netbsd']:
                self.db_files.append( Source( self.c_utils, 'Source/Unix/unix_file_local.cpp' ) )
                if self.opt_sftp:
                    self.db_files.append( Source( self.c_utils, 'Source/Unix/unix_file_remote.cpp' ) )

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
                    Source( self.c_pybemacs, 'Source/Windows/nt_async.cpp' ),
                    Source( self.c_pybemacs, 'Source/Windows/nt_comm.cpp' ),
                    Source( self.c_pybemacs, 'Source/Windows/nt_pipe.cpp' ),
                    Source( self.c_pybemacs, 'Source/Windows/nt_proc.cpp' ),
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
                Source( compiler, 'Source/Common/spell_checker.cpp' ),
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
            if self.opt_sqlite:
                obj_files.append( Source( compiler, 'Source/Common/db_sqlite3.cpp' ) )
                if not self.opt_system_sqlite:
                    obj_files.append( Source( compiler, '%(SQLITESRC)s/sqlite3.c' ) )
            else:
                obj_files.append( Source( compiler, 'Source/Common/ndbm.cpp' ) )

            if self.opt_hunspell and not self.opt_system_hunspell:
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/affentry.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/affixmgr.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/csutil.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/filemgr.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/hashmgr.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/hunspell.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/hunzip.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/phonet.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/replist.cxx' ) )
                obj_files.append( Source( compiler, '%(HUNSPELL_SRC)s/suggestmgr.cxx' ) )

            if self.platform in ('linux',):
                obj_files.extend( [
                    Source( compiler, 'Source/Unix/unix_file_local.cpp' ),
                    Source( compiler, 'Source/Unix/emacs_signal.cpp' ),
                    Source( compiler, 'Source/Unix/unixcomm.cpp' ),
                    ] )
                if self.opt_sftp:
                     obj_files.append( Source( compiler, 'Source/Unix/unix_file_remote.cpp' ) )

            if self.platform in ('macosx',):
                obj_files.extend( [
                    # similar enough for the same set of source files
                    Source( compiler, 'Source/Unix/unix_file_local.cpp' ),
                    Source( compiler, 'Source/Unix/emacs_signal.cpp' ),
                    Source( compiler, 'Source/Unix/unixcomm.cpp' ),
                    ] )
                if self.opt_sftp:
                     obj_files.append( Source( compiler, 'Source/Unix/unix_file_remote.cpp' ) )
            if self.platform in ('netbsd',):
                obj_files.extend( [
                    # similar enough for the same set of source files
                    Source( compiler, 'Source/Unix/unix_file_local.cpp' ),
                    Source( compiler, 'Source/Unix/emacs_signal.cpp' ),
                    Source( compiler, 'Source/Unix/unixcomm.cpp' ),
                    Source( compiler, 'Source/Unix/ptyopen_bsd.cpp' ),
                    ] )
                if self.opt_sftp:
                     obj_files.append( Source( compiler, 'Source/Unix/unix_file_remote.cpp' ) )
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
                Program( self.c_unit_tests, 'emunicode',  [Source( self.c_unit_tests, 'Source/Common/emunicode.cpp',
                                                                                ['Include/Common/em_unicode_data.h'] )] ),
                ] )

        if self.opt_bemacs_gui:
            self.all_exe.append(
                Program( self.c_python_tools, 'python-types',[Source( self.c_python_tools, 'Source/pybemacs/python-types.cpp' )] + self.pycxx_obj_file ) )

        if self.opt_bemacs_cli:
            self.cli_obj_files = makeObjFiles( self.c_clibemacs )
            self.all_exe.append( Program( self.c_clibemacs, 'bemacs-cli', self.cli_specific_obj_files + self.cli_obj_files ) )

    def findSpellDictionaryDir( self ):
        if not self.opt_system_hunspell:
            # hunspell is not part of the system
            # in which case emacs packages the dictionaries
            # in emacs_library:.
            return 'emacs_library:'

        p = subprocess.run(
                ['hunspell', '-D'],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                stdin=subprocess.DEVNULL,
                encoding='utf-8', check=False )
        if p.returncode != 0:
            raise SetupError( 'hunspell error %d' % (p.returncode,) )

        all_lines = p.stdout.split('\n')
        lines_iter = iter( all_lines )
        for line in lines_iter:
            # scan for the list of available dictionaries
            if line.startswith( 'AVAILABLE DICTIONARIES' ):
                break

        all_spell_dic = {}

        for line in lines_iter:
            # older hunspell print their loaded dic
            if line.startswith( 'LOADED DICTIONARIES' ):
                break

            dic_path = line.strip()
            all_spell_dic[ os.path.basename( dic_path ) ] = os.path.dirname( dic_path )

        # hunspell seems to always install en_US
        # use its dir as the default
        if 'en_US' in all_spell_dic:
            return all_spell_dic['en_US']

        # pick the first as the default
        for key in all_spell_dic:
            return all_spell_dic[ key ]

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

                self.unicode_header.generateMakefile()

            return 0

        except ValueError as e:
            log.error( 'generateMakefile %s\n' % (e,) )
            return 1

#--------------------------------------------------------------------------------
class Compiler:
    def __init__( self, setup ):
        log.debug( 'Compiler.__init__()' )
        self.setup = setup

        self.__variables = {}

        self._addVar( 'PYCXX_VER',       pycxx_version_str )
        if setup.opt_enable_debug:
            self._addVar( 'LOG.DEBUG',           '_DEBUG')
        else:
            self._addVar( 'LOG.DEBUG',           'NDEBUG')

        self._addFromEnv( 'BUILDER_TOP_DIR' )

        self._addVar( 'PYTHON_VERSION', '%d.%d' % (sys.version_info[0], sys.version_info[1]) )

    def platformFilename( self, filename ):
        return filename

    def makePrint( self, line ):
        self.setup.makePrint( line )

    def generateMakefileHeader( self ):
        raise NotImplementedError( 'generateMakefileHeader' )

    def _addFromEnv( self, name ):
        log.debug( 'Compiler._addFromEnv( %r )' % (name,) )

        value = os.environ[ name ]
        if value[0] == '"' and value[-1] == '"':
            value = value[1:-1]

        self._addVar( name, value )

    def _addVar( self, name, value ):
        log.debug( 'Compiler._addVar( %r, %r )' % (name,value) )

        try:
            if '%' in value:
                value = value % self.__variables

            self.__variables[ name ] = value

        except TypeError:
            raise SetupError( '%s: Cannot translate name %r value %r' % (self.__class__.__name__, name, value) )

        except KeyError as e:
            raise SetupError( '%s: Cannot translate name %r value %r - %s' % (self.__class__.__name__, name, value, e) )

    def addFeatureDefines( self, feature_defines=None ):
        if feature_defines is None:
            feature_defines = []

        defines = ' '.join( '-D%s=%s' % (name, value) for name, value in feature_defines )
        log.info( 'Feature defines %s' % (defines,) )
        self._addVar( 'FEATURE_DEFINES', defines )

    def expand( self, s ):
        try:
            return s % self.__variables

        except (ValueError, TypeError, KeyError) as e:
            log.error( 'Exception: %r' % (e,) )
            log.error( 'String: %s' % (s,) )
            log.error( 'Vairables: %r' % (self.__variables,) )

            raise SetupError( '%s: Cannot translate string (%s)' % (self.__class__.__name__, e) )


class Win64CompilerVC14(Compiler):
    def __init__( self, setup ):
        Compiler.__init__( self, setup )

        self._addVar( 'PYCXX',          r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s' )
        self._addVar( 'PYCXXSRC',       r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s\Src' )
        self._addVar( 'SQLITESRC',      r'%(BUILDER_TOP_DIR)s\Imports\sqlite' )
        self._addVar( 'UCDDIR',         r'%(BUILDER_TOP_DIR)s\Imports\ucd' )

        if not setup.opt_sqlite:
            self._addVar( 'SQLITE_FLAGS',   '' )

        else:
            self._addVar( 'SQLITESRC',     r'%(BUILDER_TOP_DIR)s\Imports\sqlite' )
            self._addVar( 'SQLITE_FLAGS',  r'-I%(BUILDER_TOP_DIR)s\Imports\sqlite' )

        if not self.setup.opt_hunspell:
            self._addVar( 'HUNSPELL_CFLAGS', '' )

        else:
            self._addVar( 'HUNSPELL_SRC',   r'%(BUILDER_TOP_DIR)s\Imports\hunspell\src\hunspell' )
            self._addVar( 'HUNSPELL_CFLAGS', r'-I%(BUILDER_TOP_DIR)s\Imports\hunspell\src\hunspell -DHUNSPELL_STATIC=1' )

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
        rules.append( '\t@echo %s' % (log.colourFormat('<>info Link Program:<> %s') % (pyd_filename,),) )
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
        rules.append( '\t@echo %s' % (log.colourFormat('<>info Link Shared:<> %s') % (pyd_filename,),) )
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
        rules.append( '\t@echo %s' % (log.colourFormat('<>info Compile:<> %s <>em into<> %s') % (target.src_filename, target.getTargetFilename()),) )
        rules.append( '\t@if not exist %(EDIT_OBJ)s mkdir %(EDIT_OBJ)s' )
        rules.append( '\t@if not exist %s mkdir %s' % (pdb_dir, pdb_dir) )   # For .pdb file
        rules.append( '\t@@$(CCC) /c %%(CCCFLAGS)s /Fo%s /Fd%s %s' % (obj_filename, pdb_filename, target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleC( self, target ):
        # can reuse the C++ rule
        self.ruleCxx( target )

    def ruleClean( self, filename ):
        rules = []
        rules.append( 'clean::' )
        rules.append( '\tif exist %s del %s' % (filename, filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def setupUtilities( self, feature_defines ):
        log.info( 'setupUtilities' )
        self.addFeatureDefines( feature_defines )
        self._addVar( 'EDIT_OBJ',       r'obj-utils' )
        self._addVar( 'EDIT_EXE',       r'exe-utils' )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc /W4 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win64\"" '
                                        r'"-DCPU_TYPE=\"x86_64\"" "-DUI_TYPE=\"console\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-D_UNICODE -DUNICODE '
                                        r'-U_DEBUG '
                                        r'%(FEATURE_DEFINES)s '
                                        r'-D%(LOG.DEBUG)s' )

    def setupUnittests( self ):
        log.info( 'setupUnittests' )
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
                                        r'-D%(LOG.DEBUG)s' )

    def setupPythonEmacs( self, feature_defines=None ):
        log.info( 'setupPythonEmacs' )
        self.addFeatureDefines( feature_defines )
        self._addVar( 'EDIT_OBJ',       r'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       r'exe-pybemacs' )
        self._addVar( 'LINK_LIBS',      'advapi32.lib '
                                        'user32.lib '
                                        '"%%(PYTHON_LIB)s\python%d%d.lib"' %
                                        (sys.version_info.major, sys.version_info.minor) )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-I%(PYCXX)s -I%(PYCXXSRC)s "-I%(PYTHON_INCLUDE)s" '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win64\"" '
                                        r'"-DCPU_TYPE=\"x86_64\"" "-DUI_TYPE=\"python\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-D_UNICODE -DUNICODE '
                                        r'-U_DEBUG '
                                        r'%(FEATURE_DEFINES)s '
                                        r'%(SQLITE_FLAGS)s '
                                        r'%(HUNSPELL_CFLAGS)s '
                                        r'-D%(LOG.DEBUG)s' )

    def setupCliEmacs( self, feature_defines=None ):
        log.info( 'setupCliEmacs' )
        raise SetupError( 'no support for CLI on Windows' )

    def setupPythonTools( self ):
        log.info( 'setupPythonTools' )
        self._addVar( 'EDIT_OBJ',       r'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       r'exe-python-tools' )
        self._addVar( 'LINK_LIBS',      '"%%(PYTHON_LIB)s\python%d%d.lib"' %
                                        (sys.version_info.major, sys.version_info.minor) )
        self._addVar( 'CCCFLAGS',       r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-I%(PYCXX)s -I%(PYCXXSRC)s "-I%(PYTHON_INCLUDE)s" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-D_UNICODE -DUNICODE '
                                        r'-U_DEBUG '
                                        r'-D%(LOG.DEBUG)s' )


class CompilerGCC(Compiler):
    def __init__( self, setup ):
        Compiler.__init__( self, setup )

        if setup.opt_system_ucd:
            for ucd_dir in ('/usr/share/unicode/ucd', '/usr/share/unicode'):
                if os.path.exists( os.path.join( ucd_dir, 'UnicodeData.txt' ) ):
                    self._addVar( 'UCDDIR',         ucd_dir )
                    break

            else:
                raise SetupError( 'Cannot find UnicodeData.txt' )

        else:
            self._addVar( 'UCDDIR',         '%(BUILDER_TOP_DIR)s/Imports/ucd' )

        if setup.opt_system_pycxx:
            self._addVar( 'PYCXX',          '/usr/include' )
            self._addVar( 'PYCXXSRC',       '/usr/src/CXX' )

        else:
            self._addVar( 'PYCXX',          '%(BUILDER_TOP_DIR)s/Imports/pycxx-%(PYCXX_VER)s' )
            self._addVar( 'PYCXXSRC',       '%(BUILDER_TOP_DIR)s/Imports/pycxx-%(PYCXX_VER)s/Src' )

        if self.setup.platform == 'macosx':
            self._addVar( 'CCC',        'g++ -arch x86_64 -arch arm64' )
            self._addVar( 'CC',         'gcc -arch x86_64 -arch arm64' )

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
        rules.append( '\t@echo %s' % (log.colourFormat('<>info Link Program:<> %s') % (target_filename,),) )
        rules.append( '\t@mkdir -p %(EDIT_EXE)s' )
        rules.append( '\t@%%(LDEXE)s -o %s %%(CCCFLAGS)s %s %%(LINK_LIBS)s' % (target_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleLinkShared( self, target ):
        target_filename = target.getTargetFilename()

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = []

        rules.append( '%s : %s' % (target_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo "%s"' % (log.colourFormat('<>info Link Shared:<> %s') % (target_filename,)) )
        rules.append( '\t@mkdir -p %(EDIT_EXE)s' )
        rules.append( '\t@%%(LDSHARED)s -o %s %%(CCCFLAGS)s %s %%(LINK_LIBS)s' % (target_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleCxx( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo %s' % (log.colourFormat('<>info Compile:<> %s <>em into<> %s') % (target.src_filename, obj_filename),) )
        rules.append( '\t@mkdir -p %(EDIT_OBJ)s' )
        rules.append( '\t@%%(CCC)s -c %%(CCCFLAGS)s -o %s  %s' % (obj_filename, target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleC( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo %s' % (log.colourFormat('<>info Compile:<> %s <>em into<> %s') % (target.src_filename, obj_filename),) )
        rules.append( '\t@mkdir -p %(EDIT_OBJ)s' )
        rules.append( '\t@%%(CC)s -c %%(CCFLAGS)s -o %s  %s' % (obj_filename, target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleClean( self, filename ):
        rules = []
        rules.append( 'clean::' )
        rules.append( '\t@ rm -f %s' % (filename,) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )


class MacOsxCompilerGCC(CompilerGCC):
    def __init__( self, setup ):
        CompilerGCC.__init__( self, setup )
        if setup.opt_sqlite:
            self._addVar( 'SQLITESRC',      '%(BUILDER_TOP_DIR)s/Imports/sqlite' )
            self._addVar( 'SQLITE_FLAGS',   '-I%(BUILDER_TOP_DIR)s/Imports/sqlite' )

        else:
            self._addVar( 'SQLITE_FLAGS',   '' )

        if not self.setup.opt_hunspell:
            self._addVar( 'HUNSPELL_CFLAGS', '' )

        else:
            self._addVar( 'HUNSPELL_SRC',   r'%(BUILDER_TOP_DIR)s/Imports/hunspell/src/hunspell' )
            self._addVar( 'HUNSPELL_CFLAGS', '-I%(BUILDER_TOP_DIR)s/Imports/hunspell/src/hunspell -DHUNSPELL_STATIC=1' )

    def setupUtilities( self, feature_defines ):
        log.info( 'setupUtilities' )
        self.addFeatureDefines( feature_defines )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-utils' )
        self._addVar( 'EDIT_EXE',       'exe-utils' )

        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-DDARWIN '
                                        '%(FEATURE_DEFINES)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g' )
        self._addVar( 'OBJ_SUFFIX',     '.o' )


    def setupUnittests( self ):
        log.info( 'setupUnittests' )
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
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonEmacs( self, feature_defines=None ):
        log.info( 'setupPythonEmacs' )
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        self._addVar( 'PYTHONDIR',      '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s' )
        self._addVar( 'PYTHON_FRAMEWORK', '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s/Python' )

        self._addVar( 'PYTHON',         '%(PYTHONDIR)s/Resources/Python.app/Contents/MacOS/Python' )

        if sys.version_info >= (3, 8):
            self._addVar( 'PYTHON_INCLUDE', '%(PYTHONDIR)s/include/python%(PYTHON_VERSION)s' )
        else:
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
                                        '%(SQLITE_FLAGS)s '
                                        '%(HUNSPELL_CFLAGS)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDSHARED',       '%(CCC)s -bundle -g '
                                        '-framework System '
                                        '%(PYTHON_FRAMEWORK)s '
                                        '-framework CoreFoundation '
                                        '-framework Kerberos '
                                        '-framework Security' )

    def setupCliEmacs( self, feature_defines=None ):
        log.info( 'setupCliEmacs' )
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-cli-bemacs' )
        self._addVar( 'EDIT_EXE',       'exe-cli-bemacs' )

        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"ANSI\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '%(SQLITE_FLAGS)s '
                                        '%(HUNSPELL_CFLAGS)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonTools( self ):
        log.info( 'setupPythonTools' )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       'exe-python-tools' )

        self._addVar( 'PYTHONDIR',      '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s' )
        self._addVar( 'PYTHON_FRAMEWORK', '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s/Python' )

        self._addVar( 'PYTHON',         '%(PYTHONDIR)s/Resources/Python.app/Contents/MacOS/Python' )
        if sys.version_info >= (3, 8):
            self._addVar( 'PYTHON_INCLUDE', '%(PYTHONDIR)s/include/python%(PYTHON_VERSION)s' )
        else:
            self._addVar( 'PYTHON_INCLUDE', '%(PYTHONDIR)s/include/python%(PYTHON_VERSION)sm' )

        self._addVar( 'CCCFLAGS',       '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC -fexceptions -frtti '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '-D%(LOG.DEBUG)s' )

        self._addVar( 'LDEXE',          '%(CCC)s -g '
                                        '-framework System '
                                        '%(PYTHON_FRAMEWORK)s ' )

class LinuxCompilerGCC(CompilerGCC):
    def __init__( self, setup ):
        CompilerGCC.__init__( self, setup )

        # look for debian lib dir
        if os.path.exists( '/usr/bin/dpkg-architecture' ):
            p = subprocess.Popen(['/usr/bin/dpkg-architecture', '-qDEB_HOST_MULTIARCH'], stdout=subprocess.PIPE)
            stdout, stderr = p.communicate()

            if type( stdout ) == type( str() ):
                arch = stdout.replace("\n", "")

            elif type( stdout ) == type( bytes() ):
                arch = stdout.decode("utf-8").replace("\n", "")

            self.lib_dir = '/usr/lib/%s' % (arch,)
            self.doc_dir = '/usr/share/bemacs/doc'

        else:
            # other systems use this
            self.lib_dir = '/usr/lib64'
            self.doc_dir = '/usr/share/bemacs/doc'

        #
        #   sqlite
        #
        if setup.opt_sqlite and not setup.opt_system_sqlite:
            self._addVar( 'SQLITESRC',      '%(BUILDER_TOP_DIR)s/Imports/sqlite' )
            self._addVar( 'SQLITE_FLAGS',   '-I%(BUILDER_TOP_DIR)s/Imports/sqlite' )

        else:
            self._addVar( 'SQLITE_FLAGS',   '' )

        #
        #   hunspell
        #
        if not self.setup.opt_hunspell:
            self._addVar( 'HUNSPELL_CFLAGS', '' )
            self._addVar( 'HUNSPELL_LFLAGS', '' )

        else:
            if self.setup.opt_system_hunspell:
                p = subprocess.run( ['pkg-config', 'hunspell', '--cflags'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
                self._addVar( 'HUNSPELL_CFLAGS', p.stdout.strip() )

                p = subprocess.run( ['pkg-config', 'hunspell', '--libs'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
                self._addVar( 'HUNSPELL_LFLAGS', p.stdout.strip() )

            else:
                # hunspell is in Imports
                self._addVar( 'HUNSPELL_CFLAGS', '-I%(BUILDER_TOP_DIR)s/Imports/hunspell/src' )
                self._addVar( 'HUNSPELL_LFLAGS', '' )

    def setupUtilities( self, feature_defines ):
        log.info( 'setupUtilities' )
        self.addFeatureDefines( feature_defines )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-utils' )
        self._addVar( 'EDIT_EXE',       'exe-utils' )
        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '%(SQLITE_FLAGS)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s' )

        self._addVar( 'OBJ_SUFFIX',     '.o' )

    def setupUnittests( self ):
        log.info( 'setupUnittests' )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-unit-tests' )
        self._addVar( 'EDIT_EXE',       'exe-unit-tests' )
        self._addVar( 'CCFLAGS',        '-g -O0 '
                                        '-DUNIT_TEST=1 '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '%(SQLITE_FLAGS)s '
                                        '%(HUNSPELL_CFLAGS)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g '
                                        '%(HUNSPELL_LFLAGS)s ' )

    def setupPythonEmacs( self, feature_defines=None ):
        log.info( 'setupPythonEmacs' )
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        if sys.version_info >= (3, 8):
            self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)s' % (sys.prefix,) )
        else:
            self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)sm' % (sys.prefix,) )

        link_libs = ['-lutil']
        if self.setup.opt_sftp:
            link_libs.append( '-lssh' )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"python\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '%(SQLITE_FLAGS)s '
                                        '%(HUNSPELL_CFLAGS)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        #link_libs = '-L%s -lpython%d.%dm' % (self.lib_dir, sys.version_info.major, sys.version_info.minor)
        link_libs.append( '-L%s' % (self.lib_dir,) )
        if self.setup.opt_system_sqlite:
            link_libs.append( '-lsqlite3' )

        if self.setup.opt_system_hunspell:
            link_libs.append( self.expand( '%(HUNSPELL_LFLAGS)s' ) )

        self._addVar( 'LINK_LIBS', ' '.join( link_libs ) )

        self._addVar( 'LDEXE',          '%(CCC)s -g ' )
        self._addVar( 'LDSHARED',       '%(CCC)s -shared -g ' )

    def setupCliEmacs( self, feature_defines=None ):
        log.info( 'setupCliEmacs' )
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-cli-bemacs' )
        self._addVar( 'EDIT_EXE',       'exe-cli-bemacs' )

        self._addVar( 'BEMACS_LIB_DIR', self.setup.opt_lib_dir )
        self._addVar( 'BEMACS_DOC_DIR', self.setup.opt_doc_dir )

        link_libs = ['-lutil']
        if self.setup.opt_sftp:
            link_libs.append( '-lssh' )

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
                                        '-D%(LOG.DEBUG)s '
                                        '%(FEATURE_DEFINES)s '
                                        '%(SQLITE_FLAGS)s '
                                        '%(HUNSPELL_CFLAGS)s '
                                        '-DBEMACS_DOC_DIR=\\"%(BEMACS_DOC_DIR)s\\" '
                                        '-DBEMACS_LIB_DIR=\\"%(BEMACS_LIB_DIR)s\\"' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        if self.setup.opt_system_sqlite:
            link_libs.append( '-lsqlite3' )

        if self.setup.opt_hunspell:
            p = subprocess.run( ['pkg-config', 'hunspell', '--libs'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
            link_libs.append( p.stdout.strip() )

        link_libs.append( '-pthread -ldl' )


        self._addVar( 'LINK_LIBS',      ' '.join( link_libs ) )
        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s ' )

    def setupPythonTools( self ):
        log.info( 'setupPythonTools' )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       'exe-python-tools' )

        if sys.version_info >= (3, 8):
            self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)s' % (sys.prefix,) )
            self._addVar( 'LINK_LIBS',      '-L%s -lpython%d.%d' % (self.lib_dir, sys.version_info.major, sys.version_info.minor) )
        else:
            self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)sm' % (sys.prefix,) )
            self._addVar( 'LINK_LIBS',      '-L%s -lpython%d.%dm' % (self.lib_dir, sys.version_info.major, sys.version_info.minor) )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '%(SQLITE_FLAGS)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )

class NetBSDCompilerGCC(CompilerGCC):
    def __init__( self, setup ):
        CompilerGCC.__init__( self, setup )
        if setup.opt_sqlite and not setup.opt_system_sqlite:
            self._addVar( 'SQLITESRC',      '%(BUILDER_TOP_DIR)s/Imports/sqlite' )
            self._addVar( 'SQLITE_FLAGS',   '-I%(BUILDER_TOP_DIR)s/Imports/sqlite' )

        elif setup.opt_system_sqlite:
            p = subprocess.run( ['pkg-config', 'sqlite3', '--cflags'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
            self._addVar( 'SQLITE_FLAGS', p.stdout.strip() )

        else:
            self._addVar( 'SQLITE_FLAGS',   '' )

        if self.setup.opt_hunspell:
            p = subprocess.run( ['pkg-config', 'hunspell', '--cflags'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
            self._addVar( 'HUNSPELL_CFLAGS', p.stdout.strip() )

        else:
            self._addVar( 'HUNSPELL_CFLAGS', '' )

    def setupUtilities( self, feature_defines ):
        log.info( 'setupUtilities' )
        self.addFeatureDefines( feature_defines )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-utils' )
        self._addVar( 'EDIT_EXE',       'exe-utils' )
        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '%(SQLITE_FLAGS)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s -std=c++11 '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s' )
        self._addVar( 'OBJ_SUFFIX',     '.o' )

    def setupUnittests( self ):
        log.info( 'setupUnittests' )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-unit-tests' )
        self._addVar( 'EDIT_EXE',       'exe-unit-tests' )
        self._addVar( 'CCFLAGS',        '-g -O0 '
                                        '-DUNIT_TEST=1 '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I/usr/pkg/include '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s -std=c++11 '
                                        '-fexceptions -frtti ' )
        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonEmacs( self, feature_defines=None ):
        log.info( 'setupPythonEmacs' )
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)s' % (sys.prefix,) )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '-I/usr/pkg/include '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"python\\"" '
                                        '%(FEATURE_DEFINES)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s -std=c++11 '
                                        '-fexceptions -frtti ' )

        if self.setup.opt_system_sqlite:
            self._addVar( 'LINK_LIBS',  '-L/usr/pkg/lib -lsqlite3')

        self._addVar( 'LDEXE',          '%(CCC)s -g ' )
        self._addVar( 'LDSHARED',       '%(CCC)s -shared -g ' )

    def setupCliEmacs( self, feature_defines=None ):
        log.info( 'setupCliEmacs' )
        self.addFeatureDefines( feature_defines )

        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-cli-bemacs' )
        self._addVar( 'EDIT_EXE',       'exe-cli-bemacs' )

        self._addVar( 'BEMACS_LIB_DIR', self.setup.opt_lib_dir )
        self._addVar( 'BEMACS_DOC_DIR', self.setup.opt_doc_dir )

        if self.setup.opt_hunspell:
            p = subprocess.run( ['pkg-config', 'hunspell', '--cflags'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
            self._addVar( 'HUNSPELL_CFLAGS', p.stdout.strip() )

        else:
            self._addVar( 'HUNSPELL_CFLAGS', '' )

        if self.setup.opt_coverage:
            self._addVar( 'CCC_OPT',    '-O0 '
                                        '-ftest-coverage '
                                        '-fprofile-arcs '
                                        '-fprofile-abs-path '
                                        '-fprofile-dir=%s ' % (os.getcwd(),) )

        self._addVar( 'CCFLAGS',        '-g %(CCC_OPT)s '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"NetBSD\\"" '
                                        '-DUSE_UTIL_H '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"ANSI\\"" '
                                        '-D%(LOG.DEBUG)s '
                                        '%(FEATURE_DEFINES)s '
                                        '%(SQLITE_FLAGS)s '
                                        '%(HUNSPELL_CFLAGS)s '
                                        '-DBEMACS_DOC_DIR=\\"%(BEMACS_DOC_DIR)s\\" '
                                        '-DBEMACS_LIB_DIR=\\"%(BEMACS_LIB_DIR)s\\"' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s '
                                        '-fexceptions -frtti ' )

        link_libs = []

        if self.setup.opt_system_sqlite:
            p = subprocess.run( ['pkg-config', 'sqlite3', '--libs'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
            link_libs.append( p.stdout.strip() )

        if self.setup.opt_hunspell:
            p = subprocess.run( ['pkg-config', 'hunspell', '--libs'], stdout=subprocess.PIPE, encoding='utf-8', check=True )
            link_libs.append( p.stdout.strip() )

        link_libs += ['-lutil', '-pthread']

        self._addVar( 'LINK_LIBS',      ' '.join( link_libs ) )
        self._addVar( 'LDEXE',          '%(CCC)s -g %(CCC_OPT)s ' )

    def setupPythonTools( self ):
        log.info( 'setupPythonTools' )
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-python-tools' )
        self._addVar( 'EDIT_EXE',       'exe-python-tools' )

        self._addVar( 'PYTHON_INCLUDE', '%s/include/python%%(PYTHON_VERSION)s' % (sys.prefix,) )
        self._addVar( 'LINK_LIBS', '-L%s/lib -lpython%d.%d' % (sys.prefix, sys.version_info.major, sys.version_info.minor) )

        self._addVar( 'CCFLAGS',        '-g '
                                        '%(CCC_WARNINGS)s -Wall -fPIC '
                                        '-DPYBEMACS=1 '
                                        '-DEMACS_PYTHON_EXTENSION=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '-D%(LOG.DEBUG)s' )
        self._addVar( 'CCCFLAGS',       '%(CCFLAGS)s -std=c++11 '
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
        log.debug( '%r.setDependent( %r )' % (self, dependent) )
        self.dependent = dependent


class Program(Target):
    def __init__( self, compiler, output, all_sources ):
        self.output = output

        Target.__init__( self, compiler, all_sources )
        log.debug( '%r.__init__( %r )' % (self, self.all_sources) )

    def __repr__( self ):
        return '<Program:%s>' % (self.output,)

    def getTargetFilename( self, ext=None ):
        if ext is None:
            ext = self.compiler.getProgramExt()

        return self.compiler.platformFilename( self.compiler.expand( '%%(EDIT_EXE)s/%s%s' % (self.output, ext) ) )

    def _generateMakefile( self ):
        log.debug( '%r._generateMakefile() dependent %r' % (self, self.dependent) )

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
        log.debug( '%r.__init__( %r )' % (self, self.all_sources) )

        for source in self.all_sources:
            source.setDependent( self )

    def __repr__( self ):
        return '<PythonExtension:%s>' % (self.output,)

    def getTargetFilename( self, ext=None ):
        if ext is None:
            ext = self.compiler.getPythonExtensionFileExt()

        return self.compiler.platformFilename( self.compiler.expand( '%%(EDIT_EXE)s/%s%s' % (self.output, ext) ) )

    def _generateMakefile( self ):
        log.debug( '%s.generateMakefile() for %r' % (self, self.output,) )

        self.compiler.ruleLinkShared( self )
        self.compiler.ruleClean( self.getTargetFilename( '.*' ) )

        for source in self.all_sources:
            source.generateMakefile()

class Source(Target):
    def __init__( self, compiler, src_filename, all_dependencies=None ):
        self.src_filename = compiler.platformFilename( compiler.expand( src_filename ) )

        Target.__init__( self, compiler, [] )

        log.debug( '%r.__init__( %r )' % (self, self.all_sources) )

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
        log.debug( '%r._generateMakefile()' % (self,) )

        self.rule( self )
        self.compiler.ruleClean( self.getTargetFilename() )

class UnicodeDataHeader(Target):
    def __init__( self, compiler ):
        Target.__init__( self, compiler, [] )

        log.debug( '%r.__init__( %r )' % (self, self.all_sources) )

    def __repr__( self ):
        return '<UnicodeDataHeader:%s>' % (self.getTargetFilename(),)

    def makePrint( self, line ):
        self.compiler.makePrint( line )

    def getTargetFilename( self, ext=None ):
        return self.compiler.platformFilename( 'Include/Common/em_unicode_data.h' )

    def generateMakefile( self ):
        log.debug( '%r.generateMakefile()' % (self,) )
        rules = ['']

        rules.append( '%s : make_unicode_data.py' % (self.getTargetFilename(),) )
        rules.append( '\t@echo %s' % (log.colourFormat('<>info Make<> %s') % (self.getTargetFilename(),),) )

        rules.append( '\t@"%%(PYTHON)s" make_unicode_data.py %s %%(UCDDIR)s/UnicodeData.txt %%(UCDDIR)s/CaseFolding.txt %s' %
                            ('--colour' if self.compiler.setup.opt_colour else '', self.getTargetFilename()) )

        self.makePrint( self.compiler.expand( '\n'.join( rules ) ) )
        self.ruleClean()

#--------------------------------------------------------------------------------
def main( argv ):
    try:
        s = Setup( argv )
        rc = s.generateMakefile()
        log.info( 'setup.py complete - %d' % (rc,) )
        return rc

    except SetupError as e:
        log.error( 'setup.py %s\n' % (e,) )
        return 1

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
