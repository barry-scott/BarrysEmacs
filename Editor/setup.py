#
#   Copyright (c) 2010-2014 Barry A. Scott
#
import os
import sys

_debug = False

pycxx_version = '6.2.6'

def debug( msg ):
    if _debug:
        sys.stderr.write( 'Debug: %s\n' % (msg,) )

#--------------------------------------------------------------------------------
class Setup:
    def __init__( self, argv ):
        args = argv[1:]
        if len(args) < 2:
            raise ValueError( 'Usage: setup.py win32|win64|macosx|linux> <makefile>' )

        self.opt_debug = False

        self.platform = args[0]
        del args[0]

        self.__makefile = open( args[0], 'wt' )
        del args[0]

        while len(args) > 0:
            if args[0] == '--debug':
                self.opt_debug = True
                del args[0]

            else:
                raise ValueError( 'Unknown arg %r' % (args[0],) )

        self.setupCompile()

    def makePrint( self, line ):
        self.__makefile.write( line )
        self.__makefile.write( '\n' )

    def setupCompile( self ):
        if self.platform == 'win32':
            self.c_utils = Win32CompilerMSVC90( self )
            self.c_pybemacs = Win32CompilerMSVC90( self )

        elif self.platform == 'win64':
            self.c_utils = Win64CompilerVC14( self )
            self.c_pybemacs = Win64CompilerVC14( self )

        elif self.platform == 'macosx':
            self.c_utils = MacOsxCompilerGCC( self )
            self.c_pybemacs = MacOsxCompilerGCC( self )

        elif self.platform == 'linux':
            self.c_utils = LinuxCompilerGCC( self )
            self.c_pybemacs = LinuxCompilerGCC( self )

        else:
            raise ValueError( 'Unknown platform %r' % (self.platform,) )

        self.c_utils.setupUtilities()
        self.c_pybemacs.setupPythonEmacs()

        self.unicode_header = UnicodeDataHeader( self.c_utils )

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

        if self.platform in ['linux', 'macosx']:
            self.db_files.append( Source( self.c_utils, 'Source/Unix/unixfile.cpp' ) )

        elif self.platform in ('win32', 'win64'):
            self.db_files.append( Source( self.c_utils, 'Source/Windows/win_file.cpp' ) )

        self.pycxx_obj_file = [
            Source( self.c_pybemacs, '%(PYCXXSRC)s/cxxsupport.cxx' ),
            Source( self.c_pybemacs, '%(PYCXXSRC)s/cxx_extensions.cxx' ),
            Source( self.c_pybemacs, '%(PYCXXSRC)s/cxxextensions.c' ),
            Source( self.c_pybemacs, '%(PYCXXSRC)s/IndirectPythonInterface.cxx' ),
            ]

        self.pybemacs_specific_obj_files = [
            Source( self.c_pybemacs, 'Source/pybemacs/pybemacs.cpp' ),
            Source( self.c_pybemacs, 'Source/pybemacs/bemacs_python.cpp' ),
            Source( self.c_pybemacs, 'Source/pybemacs/python_thread_control.cpp' ),
            ] + self.pycxx_obj_file

        self.obj_files = [
            Source( self.c_pybemacs, 'Source/Common/abbrev.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/abspath.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/arith.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/buf_man.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/buffer.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/caseconv.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/columns.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/CommandLine.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/dbman.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/display.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/display_insert_delete.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/display_line.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/doprint.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/em_stat.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/em_time.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/emacs.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/emacs_init.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/emacs_proc.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/emacsrtl.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/emarray.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/emstring.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/emunicode.cpp',
                                    ['Include/Common/em_unicode_data.h'] ),
            Source( self.c_pybemacs, 'Source/Common/emstrtab.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/errlog.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/file_name_compare.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/fileio.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/fio.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/function.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/getdb.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/getdirectory.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/getfile.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/glob_var.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/gui_input_mode.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/journal.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/key_names.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/keyboard.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/lispfunc.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/macros.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/mem_man.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/metacomm.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/minibuf.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/mlispars.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/mlispexp.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/mlisproc.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/mlprintf.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/ndbm.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/options.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/queue.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/save_env.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/search.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/search_extended_algorithm.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/search_extended_parser.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/search_interface.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/search_simple_algorithm.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/search_simple_engine.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/simpcomm.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/string_map.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/subproc.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/syntax.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/term.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/timer.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/undo.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/variable.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/varthunk.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/windman.cpp' ),
            Source( self.c_pybemacs, 'Source/Common/window.cpp' ),
            ]

        if self.platform in ['linux', 'macosx']:
            self.obj_files.extend( [
                Source( self.c_pybemacs, 'Source/Unix/unix_rtl_pybemacs.cpp' ),
                #Source( self.c_pybemacs, 'Source/Unix/unix_ext_func.cpp' ),
                Source( self.c_pybemacs, 'Source/Unix/unixfile.cpp' ),
                ] )

        elif self.platform in ('win32', 'win64'):
            self.obj_files.extend( [
                Source( self.c_pybemacs, 'Source/Windows/win_rtl_pybemacs.cpp' ),
                Source( self.c_pybemacs, 'Source/Windows/win_file.cpp' ),
                #Source( self.c_pybemacs, 'Source/Windows/win_ext_func.cpp' ),
                ] )

        self.all_exe = [
            PythonExtension( self.c_pybemacs, '_bemacs', self.pybemacs_specific_obj_files + self.obj_files ),
            Program( self.c_utils, 'dbadd',      [Source( self.c_utils, 'Utilities/dbadd/dbadd.cpp' )]       +self.db_files ),
            Program( self.c_utils, 'dbcreate',   [Source( self.c_utils, 'Utilities/dbcreate/dbcreate.cpp' )] +self.db_files ),
            Program( self.c_utils, 'dbprint',    [Source( self.c_utils, 'Utilities/dbprint/dbprint.cpp' )]   +self.db_files ),
            Program( self.c_utils, 'dbdel',      [Source( self.c_utils, 'Utilities/dbdel/dbdel.cpp' )]       +self.db_files ),
            Program( self.c_utils, 'dblist',     [Source( self.c_utils, 'Utilities/dblist/dblist.cpp' )]     +self.db_files ),
            Program( self.c_utils, 'mll2db',     [Source( self.c_utils, 'Utilities/mll2db/mll2db.cpp' )]     +self.db_files ),
            ]

    def generateMakefile( self ):
        try:
            self.c_pybemacs.generateMakefileHeader()

            self.makePrint( 'all: %s' % (' '.join( [exe.getTargetFilename() for exe in self.all_exe] )) )
            self.makePrint( '' )

            for exe in self.all_exe:
                exe.generateMakefile()

            self.unicode_header.generateMakefile()

            return 0

        except ValueError as e:
            sys.stderr.write( 'Error: %s\n' % (e,) )
            return 1

#--------------------------------------------------------------------------------
class Compiler:
    def __init__( self, setup ):
        debug( 'Compiler.__init__()' )
        self.setup = setup

        self.__variables = {}

        self._addVar( 'PYCXX_VER',       pycxx_version )
        self._addVar( 'DEBUG',           'NDEBUG')

        self._addFromEnv( 'BUILDER_TOP_DIR' )

    def platformFilename( self, filename ):
        return filename

    def makePrint( self, line ):
        self.setup.makePrint( line )

    def generateMakefileHeader( self ):
        raise NotImplementedError( 'generateMakefileHeader' )

    def _addFromEnv( self, name ):
        debug( 'Compiler._addFromEnv( %r )' % (name,) )

        self._addVar( name, os.environ[ name ] )

    def _addVar( self, name, value ):
        debug( 'Compiler._addVar( %r, %r )' % (name,value) )

        try:
            if '%' in value:
                value = value % self.__variables

            self.__variables[ name ] = value

        except TypeError:
            raise ValueError( 'Cannot translate name %r value %r' % (name, value) )

        except KeyError as e:
            raise ValueError( 'Cannot translate name %r value %r - %s' % (name, value, e) )

    def expand( self, s ):
        try:
            return s % self.__variables

        except (TypeError, KeyError) as e:
            print( 'Error: %s' % (e,) )
            print( 'String: %s' % (s,) )
            print( 'Vairables: %r' % (self.__variables,) )

            raise ValueError( 'Cannot translate string (%s)' % (e,) )


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

    def platformFilename( self, filename ):
        return filename.replace( '/', '\\' )

    def getPythonExtensionFileExt( self ):
        return '.pyd'

    def getProgramExt( self ):
        return '.exe'

    def generateMakefileHeader( self ):
        self.makePrint( '#' )
        self.makePrint( '#	Bemacs Makefile generated by setup.py' )
        self.makePrint( '#' )
        self.makePrint( 'CCC=cl /nologo' )
        self.makePrint( 'CC=cl /nologo' )
        self.makePrint( '' )
        self.makePrint( 'LDSHARED=$(CCC) /LD /Zi /MT /EHsc' )
        self.makePrint( 'LDEXE=$(CCC) /Zi /MT /EHsc' )
        self.makePrint( '' )

    def ruleLinkProgram( self, target ):
        pyd_filename = target.getTargetFilename()
        pdf_filename = target.getTargetFilename( '.pdf' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (pyd_filename,) )
        rules.append( '\t@$(LDEXE)  %%(CCCFLAGS)s /Fe%s /Fd%s %s' %
                            (pyd_filename, pdf_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleLinkShared( self, target ):
        pyd_filename = target.getTargetFilename()
        pdf_filename = target.getTargetFilename( '.pdf' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (pyd_filename,) )
        rules.append( '\t@$(LDSHARED)  %%(CCCFLAGS)s /Fe%s /Fd%s %s %%(PYTHON_LIB)s\python%d%d.lib' %
                            (pyd_filename, pdf_filename, ' '.join( all_objects ), sys.version_info.major, sys.version_info.minor) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleCxx( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, target.getTargetFilename()) )
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
        self._addVar( 'CCCFLAGS',
                                        r'/Zi /MT /EHsc '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win64\"" '
                                        r'"-DCPU_TYPE=\"x86_64\"" "-DUI_TYPE=\"console\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

    def setupPythonEmacs( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       r'exe-pybemacs' )
        self._addVar( 'CCCFLAGS',
                                        r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-DPYCXX_PYTHON_2TO3 -I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win64\"" '
                                        r'"-DCPU_TYPE=\"x86_64\"" "-DUI_TYPE=\"python\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
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

    def platformFilename( self, filename ):
        return filename.replace( '/', '\\' )

    def getPythonExtensionFileExt( self ):
        return '.pyd'

    def getProgramExt( self ):
        return '.exe'

    def generateMakefileHeader( self ):
        self.makePrint( '#' )
        self.makePrint( '#	Bemacs Makefile generated by setup.py' )
        self.makePrint( '#' )
        self.makePrint( 'CCC=cl /nologo' )
        self.makePrint( 'CC=cl /nologo' )
        self.makePrint( '' )
        self.makePrint( 'LDSHARED=$(CCC) /LD /Zi /MT /EHsc' )
        self.makePrint( 'LDEXE=$(CCC) /Zi /MT /EHsc' )
        self.makePrint( '' )

    def ruleLinkProgram( self, target ):
        pyd_filename = target.getTargetFilename()
        pdf_filename = target.getTargetFilename( '.pdf' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (pyd_filename,) )
        rules.append( '\t@$(LDEXE)  %%(CCCFLAGS)s /Fe%s /Fd%s %s Advapi32.lib' %
                            (pyd_filename, pdf_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleLinkShared( self, target ):
        pyd_filename = target.getTargetFilename()
        pdf_filename = target.getTargetFilename( '.pdf' )

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (pyd_filename,) )
        rules.append( '\t@$(LDSHARED)  %%(CCCFLAGS)s /Fe%s /Fd%s %s %%(PYTHON_LIB)s\python%d%d.lib Advapi32.lib' %
                            (pyd_filename, pdf_filename, ' '.join( all_objects ), sys.version_info.major, sys.version_info.minor) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleCxx( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, target.getTargetFilename()) )
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
        self._addVar( 'CCCFLAGS',
                                        r'/Zi /MT /EHsc '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win32\"" '
                                        r'"-DCPU_TYPE=\"i386\"" "-DUI_TYPE=\"console\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )

    def setupPythonEmacs( self ):
        self._addVar( 'EDIT_OBJ',       r'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       r'exe-pybemacs' )
        self._addVar( 'CCCFLAGS',
                                        r'/Zi /MT /EHsc '
                                        r'-DPYBEMACS=1 '
                                        r'-IInclude\Common -IInclude\Windows '
                                        r'-DPYCXX_PYTHON_2TO3 -I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        r'"-DOS_NAME=\"Windows\"" "-DOS_VERSION=\"win32\"" '
                                        r'"-DCPU_TYPE=\"i386\"" "-DUI_TYPE=\"python\"" '
                                        r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                        r'-U_DEBUG '
                                        r'-D%(DEBUG)s' )


class CompilerGCC(Compiler):
    def __init__( self, setup ):
        Compiler.__init__( self, setup )

        self._addVar( 'PYCXX',          '%(BUILDER_TOP_DIR)s/Imports/pycxx-%(PYCXX_VER)s' )
        self._addVar( 'PYCXXSRC',       '%(BUILDER_TOP_DIR)s/Imports/pycxx-%(PYCXX_VER)s/Src' )
        self._addVar( 'UCDDIR',         '%(BUILDER_TOP_DIR)s/Imports/ucd' )


        if self.setup.platform == 'macosx':
            self._addVar( 'CCC',            'g++ -arch i386 -arch x86_64' )
            self._addVar( 'CC',             'gcc -arch i386 -arch x86_64' )

        else:
            self._addVar( 'CCC',            'g++' )
            self._addVar( 'CC',             'gcc' )

    def getPythonExtensionFileExt( self ):
        return '.so'

    def getProgramExt( self ):
        return ''

    def generateMakefileHeader( self ):
        self.makePrint( '#' )
        self.makePrint( '#	Bemacs Makefile generated by setup.py' )
        self.makePrint( '#' )
        self.makePrint( '' )

    def ruleLinkProgram( self, target ):
        target_filename = target.getTargetFilename()

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = []

        rules.append( '%s : %s' % (target_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (target_filename,) )
        rules.append( '\t@%%(LDEXE)s -o %s %%(CCCFLAGS)s %s' % (target_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleLinkShared( self, target ):
        target_filename = target.getTargetFilename()

        all_objects = [source.getTargetFilename() for source in target.all_sources]

        rules = []

        rules.append( '%s : %s' % (target_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (target_filename,) )
        rules.append( '\t@%%(LDSHARED)s -o %s %%(CCCFLAGS)s %s' % (target_filename, ' '.join( all_objects )) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleCxx( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, obj_filename) )
        rules.append( '\t%%(CCC)s -c %%(CCCFLAGS)s -o %s  %s' % (obj_filename, target.src_filename) )

        self.makePrint( self.expand( '\n'.join( rules ) ) )

    def ruleC( self, target ):
        obj_filename = target.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, target.src_filename, ' '.join( target.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (target.src_filename, target) )
        rules.append( '\t@%%(CC)s -c %%(CCCFLAGS)s -o %s  %s' % (obj_filename, target.src_filename) )

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
        self._addVar( 'CCCFLAGS',
                                        '-g -O2 '
                                        '-Wall -fPIC -fexceptions -frtti '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-DDARWIN '
                                        '-D%(DEBUG)s' )
        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonEmacs( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        self._addFromEnv( 'PYTHON_VERSION' )
        self._addVar( 'PYTHONDIR',      '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s' )
        self._addVar( 'PYTHON_FRAMEWORK', '/Library/Frameworks/Python.framework/Versions/%(PYTHON_VERSION)s/Python' )

        self._addVar( 'PYTHON',         '%(PYTHONDIR)s/Resources/Python.app/Contents/MacOS/Python' )
        if self.expand( '%(PYTHON_VERSION)s' ).startswith( '3.' ):
            self._addVar( 'PYTHON_INCLUDE', '%(PYTHONDIR)s/include/python%(PYTHON_VERSION)sm' )
        else:
            self._addVar( 'PYTHON_INCLUDE', '%(PYTHONDIR)s/include/python%(PYTHON_VERSION)s' )

        self._addVar( 'CCCFLAGS',
                                        '-g '
                                        '-Wall -fPIC -fexceptions -frtti '
                                        '-DPYBEMACS=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-DPYCXX_PYTHON_2TO3 -I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '"-DOS_NAME=\\"MacOSX\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"python\\"" '
                                        '-DDARWIN '
                                        '-D%(DEBUG)s' )

        self._addVar( 'LDSHARED',       '%(CCC)s -bundle -g '
                                        '-framework System '
                                        '%(PYTHON_FRAMEWORK)s '
                                        '-framework CoreFoundation '
                                        '-framework Kerberos '
                                        '-framework Security' )


class LinuxCompilerGCC(CompilerGCC):
    def __init__( self, setup ):
        CompilerGCC.__init__( self, setup )


    def setupUtilities( self ):
        self._addVar( 'PYTHON',         sys.executable )

        self._addVar( 'EDIT_OBJ',       'obj-utils' )
        self._addVar( 'EDIT_EXE',       'exe-utils' )
        self._addVar( 'CCCFLAGS',
                                        '-g  '
                                        '-Wall -fPIC -fexceptions -frtti '
                                        '-IInclude/Common -IInclude/Unix '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"console\\"" '
                                        '-D%(DEBUG)s' )
        self._addVar( 'LDEXE',          '%(CCC)s -g' )

    def setupPythonEmacs( self ):
        self._addVar( 'PYTHON',         sys.executable )


        self._addVar( 'EDIT_OBJ',       'obj-pybemacs' )
        self._addVar( 'EDIT_EXE',       'exe-pybemacs' )

        self._addFromEnv( 'PYTHON_VERSION' )
        if self.expand( '%(PYTHON_VERSION)s' ).startswith( '3.' ):
            self._addVar( 'PYTHON_INCLUDE', '/usr/include/python%(PYTHON_VERSION)sm' )
        else:
            self._addVar( 'PYTHON_INCLUDE', '/usr/include/python%(PYTHON_VERSION)s' )

        self._addVar( 'CCCFLAGS',
                                        '-g '
                                        '-Wall -fPIC -fexceptions -frtti '
                                        '-DPYBEMACS=1 '
                                        '-IInclude/Common -IInclude/Unix '
                                        '-DPYCXX_PYTHON_2TO3 -I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                        '"-DOS_NAME=\\"Linux\\"" '
                                        '"-DCPU_TYPE=\\"i386\\"" "-DUI_TYPE=\\"python\\"" '
                                        '-D%(DEBUG)s' )

        self._addVar( 'LDEXE',          '%(CCC)s -g' )
        self._addVar( 'LDSHARED',       '%(CCC)s -shared -g ' )


#--------------------------------------------------------------------------------
class Target:
    def __init__( self, compiler, all_sources ):
        self.compiler = compiler
        self.__generated = False
        self.dependent = None


        self.all_sources = all_sources
        for source in self.all_sources:
            source.setDependent( self )

    def getTargetFilename( self ):
        raise NotImplementedError( '%s.getTargetFilename' % self.__class__.__name__ )

    def generateMakefile( self ):
        if self.__generated:
            return

        self.__generated = True
        return self._generateMakefile()

    def _generateMakefile( self ):
        raise NotImplementedError( '_generateMakefile' )

    def ruleClean( self, ext=None ):
        if ext is None:
            target_filename = self.getTargetFilename()
        else:
            target_filename = self.getTargetFilename( ext )

        self.compiler.ruleClean( target_filename )

    def setDependent( self, dependent ):
        debug( '%r.setDependent( %r )' % (self, dependent,) )
        self.dependent = dependent


class Program(Target):
    def __init__( self, compiler, output, all_sources ):
        self.output = output

        Target.__init__( self, compiler, all_sources )
        debug( 'Program:0x%8.8x.__init__( %r, ... )' % (id(self), output,) )

    def __repr__( self ):
        return '<Program:0x%8.8x %s>' % (id(self), self.output)

    def getTargetFilename( self, ext=None ):
        if ext is None:
            ext = self.compiler.getProgramExt()

        return self.compiler.platformFilename( self.compiler.expand( '%%(EDIT_OBJ)s/%s%s' % (self.output, ext) ) )


    def _generateMakefile( self ):
        debug( 'Program:0x%8.8x.generateMakefile() for %r dependent %r' % (id(self), self.output, self.dependent) )

        self.compiler.ruleLinkProgram( self )

        # QQQ only good for MSVC90 builds where program.* makes sense
        self.compiler.ruleClean( self.getTargetFilename( '.*' ) )

        for source in self.all_sources:
            source.generateMakefile()


class PythonExtension(Target):
    def __init__( self, compiler, output, all_sources ):
        self.output = output

        Target.__init__( self, compiler, all_sources )
        debug( 'PythonExtension:0x%8.8x.__init__( %r, ... )' % (id(self), output,) )


        for source in self.all_sources:
            source.setDependent( self )

    def __repr__( self ):
        return '<PythonExtension:0x%8.8x %s>' % (id(self), self.output)

    def getTargetFilename( self, ext=None ):
        if ext is None:
            ext = self.compiler.getPythonExtensionFileExt()
        return self.compiler.platformFilename( self.compiler.expand( '%%(EDIT_OBJ)s/%s%s' % (self.output, ext) ) )


    def _generateMakefile( self ):
        debug( 'PythonExtension:0x%8.8x.generateMakefile() for %r' % (id(self), self.output,) )

        self.compiler.ruleLinkShared( self )
        self.compiler.ruleClean( self.getTargetFilename( '.*' ) )

        for source in self.all_sources:
            source.generateMakefile()

class Source(Target):
    def __init__( self, compiler, src_filename, all_dependencies=None ):
        self.src_filename = compiler.platformFilename( compiler.expand( src_filename ) )

        Target.__init__( self, compiler, [] )

        debug( 'Source:0x%8.8x.__init__( %r, %r )' % (id(self), src_filename, all_dependencies) )

        self.all_dependencies = all_dependencies
        if self.all_dependencies is None:
            self.all_dependencies = []

    def __repr__( self ):
        return '<Source:0x%8.8x %s>' % (id(self), self.src_filename)

    def getTargetFilename( self ):
        #if not os.path.exists( self.src_filename ):
        #    raise ValueError( 'Cannot find source %s' % (self.src_filename,) )

        basename = os.path.basename( self.src_filename )
        if basename.endswith( '.cpp' ):
            return self.compiler.platformFilename( self.compiler.expand( r'%%(EDIT_OBJ)s/%s.obj' % (basename[:-len('.cpp')],) ) )

        if basename.endswith( '.cxx' ):
            return self.compiler.platformFilename( self.compiler.expand( r'%%(EDIT_OBJ)s/%s.obj' % (basename[:-len('.cxx')],) ) )

        if basename.endswith( '.c' ):
            return self.compiler.platformFilename( self.compiler.expand( r'%%(EDIT_OBJ)s/%s.obj' % (basename[:-len('.c')],) ) )

        raise ValueError( 'unknown source %r' % (self.src_filename,) )

    def _generateMakefile( self ):
        debug( 'Source:0x%8.8x.generateMakefile() for %r' % (id(self), self.src_filename,) )

        self.compiler.ruleCxx( self )
        self.compiler.ruleClean( self.getTargetFilename() )

class UnicodeDataHeader(Target):
    def __init__( self, compiler ):
        Target.__init__( self, compiler, [] )

        debug( 'UnicodeDataHeader:0x%8.8x.__init__()' % (id(self),) )

    def __repr__( self ):
        return '<UnicodeDataHeader:0x%8.8x>' % (id(self),)

    def makePrint( self, line ):
        self.compiler.makePrint( line )

    def getTargetFilename( self ):
        return self.compiler.platformFilename( 'Include/Common/em_unicode_data.h' )

    def generateMakefile( self ):
        rules = ['']

        rules.append( '%s : make_unicode_data.py' % self.getTargetFilename() )
        rules.append( '\t@ echo Info: Make %s' % self.getTargetFilename() )
        rules.append( '\t%%(PYTHON)s make_unicode_data.py %%(UCDDIR)s/UnicodeData.txt %%(UCDDIR)s/CaseFolding.txt %s' % self.getTargetFilename() )

        self.makePrint( self.compiler.expand( '\n'.join( rules ) ) )
        self.ruleClean()

#--------------------------------------------------------------------------------
def main( argv ):
    try:
        s = Setup( argv )
        s.generateMakefile()
        return 0

    except ValueError as e:
        sys.stderr.write( 'Error: %s\n' % (e,) )
        return 1

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
