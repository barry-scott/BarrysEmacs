import os
import sys

_debug = False

def debug( msg ):
    if _debug:
        sys.stderr.write( 'Debug: %s\n' % (msg,) )

class Context:
    def __init__( self ):
        debug( 'Context.__init__()' )

        self.__variables = {}

        self.addVar( 'PYCXX_VER',       '6.2.0' )
        self.addVar( 'DEBUG',           'NDEBUG')

        self.addFromEnv( 'BUILDER_TOP_DIR' )

        self.addVar( 'PYTHONDIR',       r'c:\python26' )
        self.addVar( 'PYTHON_INCLUDE',  r'%(PYTHONDIR)s\include' )
        self.addVar( 'PYTHON_LIB',      r'%(PYTHONDIR)s\libs' )
        self.addVar( 'PYTHON',          r'%(PYTHONDIR)s\python.exe' )

        self.addVar( 'PYCXX',           r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s' )
        self.addVar( 'PYCXXSRC',        r'%(BUILDER_TOP_DIR)s\Imports\pycxx-%(PYCXX_VER)s\Src' )
        self.addVar( 'UCDDIR',          r'%(BUILDER_TOP_DIR)s\Imports\ucd' )

    def addFromEnv( self, name ):
        debug( 'Context.addFromEnv( %r )' % (name,) )

        self.addVar( name, os.environ[ name ] )

    def addVar( self, name, value ):
        debug( 'Context.addVar( %r, %r )' % (name,value) )

        try:
            if '%' in value:
                value = value % self.variables()

            self.__variables[ name ] = value

        except TypeError:
            raise ValueError( 'Cannot translate name %r value %r' % (name, value) )

    def variables( self ):
        return self.__variables

    def expand( self, s ):
        try:
            return s % self.variables()

        except TypeError, e:
            print 'Error: %s' % (e,)
            print 'String: %s' % (s,)
            print 'Vairables: %r' % (self.variables(),)

            raise ValueError( 'Cannot translate string (%s)' % (e,) )

class Target:
    def __init__( self, ctx, all_sources ):
        self.ctx = ctx
        self.__generated = False
        self.dependent = None


        self.all_sources = all_sources
        for source in self.all_sources:
            source.setDependent( self )

    def getTargetFilename( self ):
        raise NotImplementedError()

    def generateMakefile( self ):
        if self.__generated:
            return

        self.__generated = True
        return self._generateMakefile()

    def _generateMakefile( self ):
        raise NotImplementedError()

    def setDependent( self, dependent ):
        debug( '%r.setDependent( %r )' % (self, dependent,) )
        self.dependent = dependent


makefile_header_win32 = r'''
#
#	make-darwin-pybemacs.mak
#
CCC=cl /nologo
CC=cl /nologo

LDSHARED=$(CCC) /Zi /MT /EHsc
LDEXE=$(CCC) /Zi /MT /EHsc

'''

makefile_footer_win32 = r'''
Include\Common\em_unicode_data.h : make_unicode_data.py
	@ echo Info: Make Include\Common\em_unicode_data
	%(PYTHON)s make_unicode_data.py %(UCDDIR)s\UnicodeData.txt %(UCDDIR)s\CaseFolding.txt Include\Common\em_unicode_data.h

clean::
    if exist Include\Common\em_unicode_data.h del Include\Common\em_unicode_data.h
'''


class Program(Target):
    def __init__( self, ctx, output, all_sources ):
        self.output = output

        Target.__init__( self, ctx, all_sources )
        debug( 'Program:0x%8.8x.__init__( %r, ... )' % (id(self), output,) )


    def __repr__( self ):
        return '<Program:0x%8.8x %s>' % (id(self), self.output)

    def getTargetFilename( self, ext='.exe' ):
        return self.ctx.expand( '%%(EDIT_OBJ)s\%s%s' % (self.output,ext) )


    def _generateMakefile( self ):
        debug( 'Program:0x%8.8x.generateMakefile() for %r dependent %r' % (id(self), self.output, self.dependent) )

        exe_filename = self.getTargetFilename()
        pdf_filename = self.getTargetFilename( '.pdf' )

        all_objects = [source.getTargetFilename() for source in self.all_sources]

        rules = ['']

        rules.append( '%s : %s' % (exe_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s.exe' % (self.output,) )
        rules.append( '\t@$(LDEXE) %%(CCCFLAGS)s /Fe%s /Fd%s %s' %
                        (exe_filename, pdf_filename, ' '.join( all_objects )) )
        rules.append( 'clean::' )
        rules.append( '\tif exist %s del %s' % (self.getTargetFilename( '.*' ), self.getTargetFilename( '.*' )) )

        print self.ctx.expand( '\n'.join( rules ) )

        for source in self.all_sources:
            source.generateMakefile()


class PythonExtension(Target):
    def __init__( self, ctx, output, all_sources ):
        self.output = output

        Target.__init__( self, ctx, all_sources )
        debug( 'PythonExtension:0x%8.8x.__init__( %r, ... )' % (id(self), output,) )


        for source in self.all_sources:
            source.setDependent( self )

    def __repr__( self ):
        return '<PythonExtension:0x%8.8x %s>' % (id(self), self.output)

    def getTargetFilename( self, ext='.pyd' ):
        return self.ctx.expand( '%%(EDIT_OBJ)s\%s%s' % (self.output,ext) )


    def _generateMakefile( self ):
        debug( 'PythonExtension:0x%8.8x.generateMakefile() for %r' % (id(self), self.output,) )

        pyd_filename = self.getTargetFilename()
        pdf_filename = self.getTargetFilename( '.pdf' )

        all_objects = [source.getTargetFilename() for source in self.all_sources]

        rules = ['']

        rules.append( '' )
        rules.append( '%s : %s' % (pyd_filename, ' '.join( all_objects )) )
        rules.append( '\t@echo Link %s' % (pyd_filename,) )
        rules.append( '\t@$(LDEXE)  %%(CCCFLAGS)s /LD /Fe%s /Fd%s %s %%(PYTHON_LIB)s\python26.lib Advapi32.lib' %
                            (pyd_filename, pdf_filename, ' '.join( all_objects )) )
        rules.append( 'clean::' )
        rules.append( '\tif exist %s del %s' % (self.getTargetFilename( '.*' ), self.getTargetFilename( '.*' )) )

        print self.ctx.expand( '\n'.join( rules ) )

        for source in self.all_sources:
            source.generateMakefile()


class Source(Target):
    def __init__( self, ctx, src_filename, all_dependencies=None ):
        self.src_filename = ctx.expand( src_filename )

        Target.__init__( self, ctx, [] )

        debug( 'Source:0x%8.8x.__init__( %r, %r )' % (id(self), src_filename, all_dependencies) )

        self.all_dependencies = all_dependencies
        if self.all_dependencies is None:
            self.all_dependencies = []

    def __repr__( self ):
        return '<Source:0x%8.8x %s>' % (id(self), self.src_filename)

    def getTargetFilename( self ):
        if not os.path.exists( self.src_filename ):
            raise ValueError( 'Cannot find source %s' % (self.src_filename,) )

        basename = os.path.basename( self.src_filename )
        if basename.endswith( '.cpp' ):
            return self.ctx.expand( r'%%(EDIT_OBJ)s\%s.obj' % (basename[:-len('.cpp')],) )

        if basename.endswith( '.cxx' ):
            return self.ctx.expand( r'%%(EDIT_OBJ)s\%s.obj' % (basename[:-len('.cxx')],) )

        if basename.endswith( '.c' ):
            return self.ctx.expand( r'%%(EDIT_OBJ)s\%s.obj' % (basename[:-len('.c')],) )

        raise ValueError( 'unknown source %r' % (self.src_filename,) )

    def _generateMakefile( self ):
        debug( 'Source:0x%8.8x.generateMakefile() for %r' % (id(self), self.src_filename,) )
        obj_filename = self.getTargetFilename()

        rules = []

        rules.append( '%s: %s %s' % (obj_filename, self.src_filename, ' '.join( self.all_dependencies )) )
        rules.append( '\t@echo Compile: %s into %s' % (self.src_filename,self.getTargetFilename()) )
        rules.append( '\t@$(CCC) /c %%(CCCFLAGS)s /Fo%s /Fd%s %s' % (obj_filename, self.dependent.getTargetFilename( '.pdb' ), self.src_filename) )
        rules.append( 'clean::' )
        rules.append( '\tif exist %s del %s' % (obj_filename, obj_filename) )
        rules.append( '' )

        print self.ctx.expand( '\n'.join( rules ) )

ctx_utils = Context()
ctx_utils.addVar( 'EDIT_OBJ',       r'win32\obj-utils' )
ctx_utils.addVar( 'EDIT_EXE',       r'win32\exe-utils' )
ctx_utils.addVar( 'CCCFLAGS',
                                    r'/Zi /MT /EHsc '
                                    r'-IInclude\Common -IInclude\Windows '
                                    r'"-DOS_NAME=\"windows\"" "-DOS_VERSION=\"win32\"" "-DCPU_TYPE=\"i386\"" "-DUI_TYPE=\"console\"" '
                                    r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                    r'-U_DEBUG '
                                    r'-D%(DEBUG)s' )

ctx_pybemacs = Context()
ctx_pybemacs.addVar( 'EDIT_OBJ',    r'win32\obj-pybemacs' )
ctx_pybemacs.addVar( 'EDIT_EXE',    r'win32\exe-pybemacs' )
ctx_pybemacs.addVar( 'CCCFLAGS',
                                    r'/Zi /MT /EHsc '
                                    r'-DPYBEMACS=1 '
                                    r'-IInclude\Common -IInclude\Windows '
                                    r'-DPYCXX_PYTHON_2TO3 -I%(PYCXX)s -I%(PYCXXSRC)s -I%(PYTHON_INCLUDE)s '
                                    r'"-DOS_NAME=\"windows\"" "-DOS_VERSION=\"win32\"" "-DCPU_TYPE=\"i386\"" "-DUI_TYPE=\"python\"" '
                                    r'-DWIN32=1 -D_CRT_NONSTDC_NO_DEPRECATE '
                                    r'-U_DEBUG '
                                    r'-D%(DEBUG)s' )


db_common_files = [
    Source( ctx_utils, r'Utilities\db_rtl\db_rtl.cpp' ),
    Source( ctx_utils, r'Source\Common\doprint.cpp' ),
    Source( ctx_utils, r'Source\Common\em_stat.cpp' ),
    Source( ctx_utils, r'Source\Common\emstring.cpp' ),
    Source( ctx_utils, r'Source\Common\emunicode.cpp',
            [r'Include\Common\em_unicode_data.h'] ),
    Source( ctx_utils, r'Source\Common\file_name_compare.cpp' ),
    Source( ctx_utils, r'Source\Common\ndbm.cpp' ),
    Source( ctx_utils, r'Utilities\db_rtl\stub_rtl.cpp' ),
    Source( ctx_utils, r'Source\Windows\win_file.cpp' ),
    ]

pycxx_obj_file= [
    Source( ctx_pybemacs, r'%(PYCXXSRC)s\cxxsupport.cxx' ),
    Source( ctx_pybemacs, r'%(PYCXXSRC)s\cxx_extensions.cxx' ),
    Source( ctx_pybemacs, r'%(PYCXXSRC)s\cxxextensions.c' ),
    Source( ctx_pybemacs, r'%(PYCXXSRC)s\IndirectPythonInterface.cxx' ),
    ]

pybemacs_specific_obj_files = [
    Source( ctx_pybemacs, r'Source\pybemacs\pybemacs.cpp' ),
    Source( ctx_pybemacs, r'Source\pybemacs\bemacs_python.cpp' ),
    Source( ctx_pybemacs, r'Source\pybemacs\python_thread_control.cpp' ),
    ] + pycxx_obj_file

obj_files = [
    Source( ctx_pybemacs, r'Source\Common\abbrev.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\abspath.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\arith.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\buf_man.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\buffer.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\caseconv.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\columns.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\CommandLine.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\dbman.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\display.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\display_insert_delete.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\display_line.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\doprint.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\em_stat.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\em_time.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\emacs.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\emacs_init.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\emacs_proc.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\emacsrtl.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\emarray.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\emstring.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\emunicode.cpp',
            [r'Include\Common\em_unicode_data.h'] ),
    Source( ctx_pybemacs, r'Source\Common\emstrtab.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\errlog.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\file_name_compare.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\fileio.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\fio.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\function.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\getdb.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\getdirectory.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\getfile.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\glob_var.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\gui_input_mode.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\journal.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\key_names.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\keyboard.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\lispfunc.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\macros.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\mem_man.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\metacomm.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\minibuf.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\mlispars.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\mlispexp.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\mlisproc.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\mlprintf.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\ndbm.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\options.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\queue.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\save_env.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\search.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\search_extended_algorithm.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\search_extended_parser.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\search_interface.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\search_simple_algorithm.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\search_simple_engine.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\simpcomm.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\string_map.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\subproc.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\syntax.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\term.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\timer.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\undo.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\variable.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\varthunk.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\windman.cpp' ),
    Source( ctx_pybemacs, r'Source\Common\window.cpp' ),
    ]

unix_specific_files = [
    Source( ctx_pybemacs, r'Source\Unix\unix_ext_func.cpp' ),
    Source( ctx_pybemacs, r'Source\Unix\unixfile.cpp' ),
    Source( ctx_pybemacs, r'Source\Unix\unix_rtl.cpp' ),
    ]

win_specific_files = [
    Source( ctx_pybemacs, r'Source\Windows\win_rtl_pybemacs.cpp' ),
    Source( ctx_pybemacs, r'Source\Windows\win_file.cpp' ),
    #Source( ctx_pybemacs, r'Source\Windows\win_ext_func.cpp' ),
    ]


all_exe = [
    PythonExtension( ctx_pybemacs, '_bemacs', pybemacs_specific_obj_files + obj_files + win_specific_files ),
    Program( ctx_utils, 'dbadd',      [Source( ctx_utils, r'Utilities\dbadd\dbadd.cpp' )]       +db_common_files ),
    Program( ctx_utils, 'dbcreate',   [Source( ctx_utils, r'Utilities\dbcreate\dbcreate.cpp' )] +db_common_files ),
    Program( ctx_utils, 'dbprint',    [Source( ctx_utils, r'Utilities\dbprint\dbprint.cpp' )]   +db_common_files ),
    Program( ctx_utils, 'dbdel',      [Source( ctx_utils, r'Utilities\dbdel\dbdel.cpp' )]       +db_common_files ),
    Program( ctx_utils, 'dblist',     [Source( ctx_utils, r'Utilities\dblist\dblist.cpp' )]     +db_common_files ),
    Program( ctx_utils, 'mll2db',     [Source( ctx_utils, r'Utilities\mll2db\mll2db.cpp' )]     +db_common_files ),
    ]

def main( argv ):
    try:
        print ctx_pybemacs.expand( makefile_header_win32 )


        print 'all: %s' % (' '.join( [exe.getTargetFilename() for exe in all_exe] ))
        print

        for exe in all_exe:
            exe.generateMakefile()

        print ctx_pybemacs.expand( makefile_footer_win32 )
        return 0

    except ValueError, e:
        sys.stderr.write( 'Error: %s\n' % (e,) )
        return 1

#    except TypeError, e:
#        sys.stderr.write( 'Error: %s\n' % (e,) )
#        return 1

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
