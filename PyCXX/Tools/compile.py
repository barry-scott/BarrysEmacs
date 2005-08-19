#!/usr/bin/env python -O 
"""
This program takes a Setup file and creates the Makefile-like files
(.dsw and .dsp) needed for simple compilation of Python extension
modules on Windows using DevStudio97.  

Author: David Ascher  [compile_unix modified from Konrad Hinsen's compile.py]
=======

Features:
=========
  - On Unix, does the usual make based on Makefile.pre.in
  - On Windows, creates a .DSW file and as many .DSP files as there
    are modules in the Setup file, and invokes MSVC to build the projects.
    (note: the auto-build facility requires the Python COM extensions)
  - Should work ok for most Setup files without modifications
  - Auto-identifies C and C++ files using wide range of extensions
  - Converts -D, -L, -l tags to approprate Windows versions.
  - Tries to find the location of python15.lib and the Include
    directory (or directories if you have Python before 1.5.1).  Asks
    the user for their locations if can't find it in a few standard places.
  - Builds only one configuration (I *hate* the Debug/Release
    user interface.
  - Builds a project dependency chain based on the sequential order of
    the modules in the Setup file.
  - If a .c file ends in _wrap.c, looks for the corresponding .i file in the
    same directory.  If it finds it, adds the .i file to the project, with
    the commands needed to run SWIG (assuming it can find SWIG).
  - VC-specific options in a Setup file are indicate with the syntax e.g.

      #[VC50]cpp_options.append('/U DOUBLE')
      #[VC50]cfiles.append('_numpy.def')
      #[VC50]swig_options.append('-shadow')

    which can be useful to override defaults set in the Setup file, and 
    to add files to the compilation which are Windows-only (e.g. DEF files
    when building DLLs you want other DLL's to link against).

    Default flags are:
       Defines:  WIN32, NDEBUG, _WINDOWS
       Link: wherever the python15.lib is.
       Include: wherever the Python includes are.

    Useful flags to document in eventual documentation:
       /GR : Enable RTTI --- should it be made default if C++?
    (NOTE: [VC50] tags are used for all versions of VC)

Usage:
======
  On Unix:
    csh> compile.py
    csh> install.py

  On Windows:
    c:\wherever> python compile.py workspacename
    
    If the COM extensions are installed, this should also
    invoke MSDev and build the projects.  To build manually:

    c:\wherever> msdev workspacename.dsw
    [Go to "Build", pull down "Batch Build", do "Build All", exit DevStudio]

    # install by hand -- bins are in pyds/, .py's are in lib/

Todo:
=====
  * write install.py for Windows
  * write test.py for Windows & Unix
  * Figure out how to avoid reSWIGGing every time.

Release History:
================
  0.21 [Mar 10, 1998] Put back Konrad's new compile stuff.  
                      Should work on Unix as well now [da]
  0.2  [Feb 13, 1998] Got rid of unix part -- NumPy specific
                      Released on newsgroup.
  0.1h [Feb 8, 1998]  Made options be lists instead of strings (cleaner Setup
                      files) - backwards compatible thanks to RAddList
  0.1g [Feb 8, 1998]  fixed bug with -L flag
  0.1f [Feb 7, 1998]  Added auto-detect and custom build stuff for shadow
                      files (swig_options, and final 'c' on init routine)
  0.1e [Feb 7, 1998]  Added auto-detect and custom build stuff for SWIG .i files
  0.1d [Feb 6, 1998]  Fixed bug re: detection of -l tags.
  0.1c [Feb 6, 1998]  Limited edition release to P, G, K, H, dek
                      - 'cfiles' moved into self.__dict__
                      - fixed Setup file for NumPy to include .def file
  0.1b [Feb 6, 1998]  Limited edition release to P, G, K, H.
                      - now putting .lib and .exp with .pyd's
  0.1a [Feb 5, 1998]  Limited edition release to P
  0.0  [Feb 4, 1998]  Started work, prompted by discussion with Paul,
                      Guido, Konrad, Harri.
 
Known bugs:
===========
  * It doesn't work on macs.
  * I'd like the .plg files to go in ./tmp (maybe even the .dsp files)

Comments
========
  - There is no way, apparently, to specify C++ compilation on a
    single file, only on all the files in a module (project) at once.
    This is probably not a problem in reality, as most C files go
    through C++ compilers. 

Bug reports:
============
  email me at da@skivs.ski.org

URL: [eventually]
====
  http://starship.skyport.net/~da/compile.py/

"""

_version = '0.1h'

import sys, string, os, copy

from UserList import UserList

###
#
# Constants which I guess you can change.  Let me know why, and I may
# change the master copy.
# 

# These are the extensions of C++ files *other than .cpp and .CPP*
# which should force the use of the C++ compiler.  (VC5 recognizes
# .cpp and .CPP automatically).

CPP_EXTENSIONS = ('c++', 'C++', 'C', 'cxx', 'CXX', 'cc')

# These are lowercased extensions which mark C or C++ files
SOURCE_EXTENSIONS = ('c', 'c++', 'cpp', 'cxx', 'cc')

# Could maybe have a "default configuration" we use to grab base settings?
DEFAULT_DEFINES = ('WIN32', '_WINDOWS')

# All the configurations we know how to build.
class Config:
    def __init__(self, **kw):
        self.config_cpp_options = ()
        self.config_link_options = ()
        self.config_based_on = "Win32 (x86) Dynamic-Link Library"
        self.config_target_type = 0x0102
        self.config_is_debug = 0
        self.__dict__.update(kw)

ConfigWin32Release = Config(
    config_name = "Win32 Release",
    config_outdir = ".",
    config_cpp_options = ('/O2', '/Ob2', '/MD'),
    config_defines = ('NDEBUG',),
)

ConfigWin32Debug = Config(
    config_name = "Win32 Debug",
    config_outdir = ".",
    config_defines = ('DEBUG', '_DEBUG'),
    config_cpp_options = ('/Zi','/Od', '/MDd', '/FR'),
    config_link_options = ('/pdb:sept', '/DEBUG'),
    config_is_debug = 1,
)

all_configurations = {
    "release" : ConfigWin32Release,
    "debug"   : ConfigWin32Debug,
}
# Python's main library [what's the algorithm which determines it from
# sys.version?  Will 1.5.1 have a python151.lib?  Probably not.]
# NOTE: Currently this is used only to locate the path - the .lib does
# not need to be written to the makefile as the Python headers magically
# imply.  MHammond prefers to add the Python include path to the
# MSVC global paths, thereby making all this searching unnecessary
# (although I do admit it may be too much to ask everyone to do this?)
LIBNAME = "python15.lib"

# Possible places where LIBNAME might be found, if it's not in
# os.path.join(sys.prefix, 'lib') as Guido promised.
LIB_POSSIBLES = (os.path.join(sys.prefix, 'Python-1.5\PCbuild'),
                 "C:\Python-1.5\PCbuild",
                 "C:\local\src\Python-1.5\PCbuild",
                 "D:\Python-1.5\PCbuild",
                 "D:\local\src\Python-1.5\PCbuild",
                 )
# Possible places where the Include directory might be, if it's not in
# os.path.join(sys.prefix, 'include') as Guido promised.
# The ../PC directory is inferred from it in this case.
INCLUDE_POSSIBLES = (os.path.join(sys.prefix, 'Python-1.5\Include'),
                     "C:\Python-1.5\Include",
                     "C:\local\src\Python-1.5\Include",
                     "D:\Python-1.5\Include",
                     "D:\local\src\Python-1.5\Include",
                     )

# places where we should look for swig.exe
SWIG_POSSIBLES = ('C:\SWIG1.1\SWIG.EXE',
                  'C:\SWIG1.0\SWIG.EXE',
                  'D:\SWIG1.1\SWIG.EXE',
                  'D:\SWIG1.0\SWIG.EXE')

########## end of reasonably configurable things ###############

#
# Note: don't play too much with these -- DevStudio really cares about
# some of the apparently useless things in here (e.g. the BASE
# definitions), and will refuse to deal with "corrupt" project and
# workspace files (and sometimes crash).
#
# If you have suggestions for different compilation options, by all
# means let me know.
#
project_header_template = """\
# Microsoft Developer Studio Project File - Name="%(projectname)s" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version %(vc_version)s
# ** DO NOT EDIT **

"""

# Then come the #TARGTYPE lines
project_messages_template = """\

CFG=%(projectname)s - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "%(projectname)s.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "%(projectname)s.mak" CFG="%(projectname)s - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE
"""

# Then come the !MESSAGE sections
config_message_template="""\
!MESSAGE "%(projectname)s - %(config_name)s" (based on%(vc_longlinesep)s "%(config_based_on)s")"""

project_preconfigs_template = """
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

"""
# Then come the
# !IF "$(CFG)... / #ELIF ... / #ENDIF
# lines for each configuration.
config_build_template = """\
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries %(config_is_debug)d
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries %(config_is_debug)d
# PROP Output_Dir "%(outdir)s\\%(config_outdir)s"
# PROP Intermediate_Dir "%(tmpdir)s\\%(config_outdir)s"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX %(includeline)s /YX /FD /c %(cpp_options)s %(config_cpp_options)s
# ADD BASE MTL /nologo /mktyplib203 /o NUL /win32
# ADD MTL /nologo /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 
# ADD RSC /l 0x409 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib %(python_lib)s /nologo /subsystem:windows /dll /machine:I386 %(linkline)s %(outline)s %(exportline)s %(config_link_options)s

"""
project_target_header_template = """\
!ENDIF 

# Begin Target
"""
# Then the "#Name ..." lines - one per config
config_name_template = '''\
# Name "%(projectname)s - %(config_name)s"'''

# Then the source section.

project_target_footer_template = """\
# End Target
# End Project
"""
## Templates used to build the %(sourcesection) string
# template for C files
cfiletemplate = r"""
# Begin Source File

SOURCE=%(sourcepath)s
# End Source File
"""

# Template for SWIG files
ifiletemplate = r"""# Begin Source File

SOURCE=./%(sourcepath)s
# PROP Ignore_Default_Tool 1
# Begin Custom Build - SWIG'ing
InputPath=./%(sourcepath)s
InputName=%(prefix)s

"$(InputName)%(wrapsuffix)s" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
        %(swig_exe)s -python -Iinclude -I. %(swig_options)s -o "$(InputName)%(wrapsuffix)s" $(InputPath)

# End Custom Build
# End Source File
"""

## And all the Workspace template stuff.
projectInWorkspaceTemplate = r"""Project: "%(projectname)s"="%(projectfilepath)s" - Package Owner=<4>

Package=<5>
{{{
    begin source code control
    %(projectname)s
    %(thedir)s
    end source code control
}}}

Package=<4>
{{{
%(dependency_part)s
}}}
"""

workspacetemplate = r"""Microsoft Developer Studio Workspace File, Format Version %(vc_version)s
# WARNING: DO NOT EDIT OR DELETE THIS WORKSPACE FILE!

###############################################################################

%(projectinfo)s

###############################################################################

Global:

Package=<5>
{{{
}}}

Package=<3>
{{{
}}}

###############################################################################
"""


# These parse the 'words' in the Setup lines

# Get source files
def get_cfiles(candidates):
    cfiles = []
    rest = []
    for candidate in candidates:
        if string.find(candidate, '.') != -1:
            if string.lower(string.split(candidate, '.')[-1]) in SOURCE_EXTENSIONS:
                if string.find(candidate, '/'):
                    candidate = string.replace(candidate, '/', '\\')
                cfiles.append(candidate)
            else:
                rest.append(candidate)
        else:
            rest.append(candidate)
    return cfiles, rest

# helper function
def get_prefixed(candidates, *prefixlist):
    matched = []
    rest = []
    for candidate in candidates:
        for prefix in prefixlist:
            if string.find(candidate, prefix) == 0:
                matched.append(os.path.normpath(candidate[len(prefix):]))
            else:
                rest.append(candidate)
    return matched, rest

def get_includedirs(candidates):
    return get_prefixed(candidates, '-I')

def get_libfiles(candidates):
    return get_prefixed(candidates, '-l')

def get_libdirs(candidates):
    return get_prefixed(candidates, '-L')

def get_defines(candidates):
    return get_prefixed(candidates, '-D')

# look in the someday standard place for e.g. lib and include directories.
def get_pydir(what):
    attempt = os.path.join(sys.prefix, what)
    if os.path.exists(attempt):
        return attempt

# find the Python15.lib library
def get_pylibsdir(): 
    attempt = get_pydir('libs')
    if attempt is None and sys.platform=="win32":
        # User may be running from sources - check
        attempt = get_pydir('pcbuild')
    if attempt:
        if os.path.exists(os.path.join(attempt, LIBNAME)):
            if __debug__: print "  Found " +LIBNAME+" in", attempt
            return [attempt]
        attempt = None
    if not attempt:
        for attempt in LIB_POSSIBLES:
            if os.path.exists(os.path.join(attempt, LIBNAME)):
                if __debug__: print "  Found " + LIBNAME + " in", attempt
                return [attempt]
    while 1:
        attempt = raw_input("What directory is python15.lib in? ")
        if os.path.exists(os.path.join(attempt, LIBNAME)):
            if __debug__: print "  Found " + LIBNAME + " in", attempt
            return [attempt]

# checks the potentially singleton list of Python include directories
def valid_includedir(attempt):
    if os.path.exists(attempt):
        attempt = [os.path.join(os.path.split(attempt)[0], 'PC'), attempt]
        if not os.path.exists(attempt[0]):
            attempt = [attempt[1]]  # try with just main one.
        if __debug__: print "  Found Python include directories in", string.join(attempt, ', ')
        return attempt
    return None

# find the list of Python include directories
def get_pyincludedir(): 
    attempt = get_pydir('include')
    if attempt:
        if __debug__: print "  Found Python include directory in", attempt
        return [attempt]
    if not attempt:
        for attempt in INCLUDE_POSSIBLES:
            gooddirs = valid_includedir(attempt)
            if gooddirs:
                return gooddirs
    while 1:
        attempt = raw_input("Where is the Python include directory? ")
        gooddirs = valid_includedir(attempt)
        if gooddirs:
            return gooddirs

# The following few functions return things which can be understood by
# the VC and LINK executables

# helper function
def make_mssyntax(libdirs, sep):
    def quote(x):
        return '"'+x+'"'
    return sep + string.join(map(quote,libdirs), sep)

# equivalent of -L 
def make_libdirline(libdirs):
    return make_mssyntax(libdirs, ' /libpath:')

#equivalent of -l
def make_libfileline(libdirs):
    return string.join(map(lambda x: x + '.lib ', libdirs))

#equivalent of -D
def make_defline(defines):
    return make_mssyntax(defines, ' /D ')

#equivalent of -I
def make_includeline(includepaths):
    return make_mssyntax(includepaths, ' /I ')

def search_for_ifiles(cfiles):
    # returns [(ifile, "_wrap.?"), ...] for each cfile with a matching ifile.
    wrapfiles = filter(lambda x: os.path.splitext(x)[0][-5:] == '_wrap', cfiles)
    ifiles = map(lambda x: (os.path.splitext(x)[0][:-5]+'.i', '_wrap'+os.path.splitext(x)[1]), wrapfiles)
    ifiles = filter(lambda x: os.path.exists(x[0]), ifiles)
    return ifiles 

def find_swig():
    # The Win32 extension makefiles used %SWIG_EXE% to locate
    # SWIG - so we also try there
    explicit = os.environ.get("SWIG_EXE")
    if explicit and os.path.exists(explicit): return explicit
    for candidate in SWIG_POSSIBLES:
        if os.path.exists(candidate):
            return candidate
    # not found
    return None

class RAddList(UserList):
    """ a list which allows addition with strings -- for backwards
    compatibility with all 2 users =)"""
    def __init__(self, data):
        UserList.__init__(self, data)
    def __add__(self, what):
        self.append(what)
        return self

# A class that knows a little about MSDev.
# If the COM extensions are installed, they will be used
# to query the current MSDEV version, and generate the appropriate
# makefile and project file (this is because MSDEV will not upgrade
# a VC5 makefile to VC6 when attempting an automated build.
class Msdev:
    def __init__(self):
        try:
            import win32com.client
            import pythoncom
            if __debug__: print "Attempting to start MSDev..."
            try:
                app = win32com.client.Dispatch("Msdev.Application")
            except pythoncom.error:
                app = None
        except ImportError:
            app = None
        self.app = app
        # default to VC5 if unknown
        version = "5.0"
        if app is not None:
            import time; time.sleep(1) # Appears to like some time to startup.
            version = app.Version
            try:
              app.Visible = 1
            except AttributeError:
              print "MSDev appears to be confused.  You may need to build the project manually"
            # Close everything now incase MsDev has our workspace already open.
            self.app.Documents.CloseAll
            self.app.ExecuteCommand("WorkspaceClose")
        else:
          print r"""I wasn't able to start MSDev via COM.  This could be because:

MSDev is not installed on the computer:  This tool will only work with
  Microsoft Visual Studio version 5 or later. It must be installed to
  allow compilation of this package. 

MSDev is installed but unknown to COM: you need to start DevStudio
  by hand once, then try running this tool again.

The Python COM extensions were not installed: These are shipped as
  part of "win32all", Mark Hammond's Python extensions.  These are not
  needed for compilation, only for 'automatic' compilation.  

A Visual C++ 5.0 workspace will be generated, and must be built manually."""

        if version == "6.0":
            self.vc_version = "6.00"
            self.vc_longlinesep = ""
            self.vc_extraprops = "\n# PROP AllowPerConfigDependencies 0"
        else:
            if version <> "5.0":
                print "Warning: Unknown Msdev version (%s) - using Version 5.0 format" % (version)
            self.vc_version = "5.00"
            self.vc_longlinesep = "\\"
            self.vc_extraprops = ""

    def Quit(self):
        if self.app is not None:
            self.app.Quit()
            self.app = None

    def build(self, workspace):
        if self.app is None:
            return # User has already been told we can't build.
        print "Building project"
        import pythoncom # This can't fail if self.app is not None!
        try:
            # Everything is already closed - just open the new workspace.
            workspacename = os.path.abspath(workspace.workspacename+".dsw")
            self.app.Documents.Open(workspacename, 'Auto', 1)
            # Because of the way dependencies are setup, we only need to build
            # the last project in the list.  Get the configuration
            projectname=workspace.projects[-1].projectname
            for config in all_configurations.values():
                configname = config.config_name
                print configname
                config = self.app.Projects(projectname).Configurations(projectname + " - " + configname)
                self.app.Build(config)
            #configname = "Release"
        except pythoncom.com_error, (hr, desc, exc, arg):
            if exc and exc[2]: desc=exc[2]
            print "Error building project", desc

class Project:
    """
    A Project corresponds to a single (logical) line in a Setup file
    """
    def __init__(self, ws, msdev, line, extra_stuff):
        try:
            words = string.split(line)
            self.projectname, rest = words[0], words[1:]
        except TypeError:
            raise "Couldn't parse the Setup file!"
        print "Creating project (.dsp) file for", self.projectname

        # parse the words in the line
        self.cfiles, rest = get_cfiles(rest)
        incstmts, rest = get_includedirs(rest)
        libfilestmts, rest = get_libfiles(rest)
        libdirstmts, rest = get_libdirs(rest)
        definestmts, rest = get_defines(rest)

        # search for SWIG output file, and return the corresponding .i files which exist
        if swig_found:
            self.swig_exe = swig_found
            self.ifiles = search_for_ifiles(self.cfiles)
        else:
            self.ifiles = []
            
        libdiritems = ws.libsdir
        libfileitems = []
        includepaths = ws.incsdir
        for libdir in libdirstmts:
            if libdir not in libdiritems:
                libdiritems.append(libdir)
        for libfile in libfilestmts:
            if libfile not in libfileitems:
                libfileitems.append(libfile)
        for incstmt in incstmts:
            if incstmt not in includepaths:
                includepaths.append(incstmt)
        self.defines = list(DEFAULT_DEFINES)

        for define in definestmts:
            if define not in self.defines:
                self.defines.append(define)

        self.linkline = string.join(string.split(make_libdirline(libdiritems) + ' ' + make_libfileline(libfileitems)))
        
        self.includeline = make_includeline(includepaths)
        self.defline = make_defline(self.defines)

        configs = all_configurations.keys()
        # the following detects C++ files which VC5 won't recognize as C++
        # files, and forces VC to compile them as C++ files
        def iscppfile(fname):
            extensions = CPP_EXTENSIONS  # extend as needed
            ext = string.split(fname, '.')[-1]
            return ext in extensions
        
        # if any of the files for this module are C++ files..
        if filter(iscppfile, self.cfiles):  
            self.cpp_options = RAddList(["/Tp"])  # force C++ compilation of all of them
            self.swig_options = RAddList(['-c++'])
        else:
            self.cpp_options = RAddList([])
            self.swig_options = RAddList([])

        building_shadow_module = '-shadow' in self.swig_options
        self.swig_options = string.join(self.swig_options, ' ')
        self.cpp_options = self.defline + ' ' + string.join(self.cpp_options, ' ')

        # Build the list of source files for the project.
        sourcesection = ""
        for self.sourcepath in self.cfiles:
            # The variables in the template are all instance
            # attributes of our self.
            sourcesection = sourcesection + cfiletemplate % self.__dict__

        for self.sourcepath, self.wrapsuffix in self.ifiles:
            # The variables in the template are all instance
            # attributes of our self.
            self.prefix = os.path.split(self.sourcepath)[-1][:-2]
            sourcesection = sourcesection + ifiletemplate % self.__dict__

        # if there was any Setup-specified code, execute it now
        if extra_stuff:
            exec extra_stuff+'\n\n\n' in self.__dict__

        # Now begin the complicated process of building the project
        # file, inserting each configuration in the correct spots.
        self.outdir = "pyds"
        self.tmpdir = "tmp"
        # Create a namespace, then loop over the configs updating the
        # namespace and building each chunk of the project.
        template_dict = self.__dict__.copy()
        template_dict.update(msdev.__dict__)
        configs_message_section = []
        configs_build_section = []
        configs_name_section = []
        for configname in configs:
            config = copy.copy(all_configurations[configname])

            config.config_cpp_options = make_defline(config.config_defines) + " " + string.join(config.config_cpp_options, ' ')
            config.config_link_options = string.join(config.config_link_options, ' ')
            # The init function of the module needs to be exported
            # (replaces use of .DEF file)
            debug_suffix = ""
            if config.config_is_debug: debug_suffix = "_d"
            config.python_lib = "python15" + debug_suffix + '.lib'
            if building_shadow_module:
                config.exportline = "/export:init%sc" % (self.projectname,)
                config.outline = '/out:"%s\\%s\\%sc%s.pyd"' % (self.outdir, config.config_outdir, self.projectname, debug_suffix)
            else:
                config.exportline = "/export:init%s" % (self.projectname,)
                config.outline = '/out:"%s\\%s\\%s%s.pyd"' % (self.outdir, config.config_outdir, self.projectname, debug_suffix)

            # make target and temp directories if needed
            p = os.path.join(self.outdir, config.config_outdir)
            if not os.path.exists(p): os.makedirs(p)
            p = os.path.join(self.tmpdir, config.config_outdir)
            if not os.path.exists(p): os.makedirs(p)

            template_dict.update(config.__dict__)

            configs_message_section.append(config_message_template % template_dict)
            configs_build_section.append(config_build_template % template_dict)
            configs_name_section.append(config_name_template % template_dict)

        # Combine everything into the project file.
        project_file = open(self.projectname+'.dsp', 'w')
        project_file.write(project_header_template % template_dict)
        # Write the TARGTYPE strings.
        targets_done = []
        for config_name in configs:
            config = all_configurations[config_name]
            if config.config_target_type not in targets_done:
                project_file.write("""# TARGTYPE "%s" 0x%04x\n""" % (config.config_based_on, config.config_target_type))
                targets_done.append(config.config_target_type)

        project_file.write(project_messages_template % template_dict)
        project_file.write(string.join(configs_message_section, "\n"))
        project_file.write(project_preconfigs_template % template_dict)
        # Each config build info.
        project_file.write('!IF  "$(CFG)" == "%s - %s"\n\n' % (self.projectname, all_configurations[configs[0]].config_name))
        project_file.write(configs_build_section[0])
        for i in range(1, len(configs)):
            config = all_configurations[configs[i]]
            project_file.write('!ELSEIF  "$(CFG)" == "%s - %s"\n\n' % (self.projectname, config.config_name))
            project_file.write(configs_build_section[i])

        project_file.write(project_target_header_template % template_dict)
        project_file.write(string.join(configs_name_section, "\n"))

        project_file.write(sourcesection)
        project_file.write(project_target_footer_template % template_dict)

class Workspace:
    """
    A Workspace is needed for the entire workspace.
    """
    def __init__(self, msdev, setupfilename = 'Setup', workspacename = 'workspace'):
        self.workspacename = workspacename
        self.msdev = msdev
        print "Building workspace (.dsw) file for", self.workspacename
        # read the setup file
        if setupfilename == 'Setup':
            try:
                lines = open(setupfilename, 'r').readlines()
            except:
                lines = open(setupfilename+'.in', 'r').readlines()
        else:
            lines = open(setupfilename, 'r').readlines()
        # filter the comments out (watching out for the VC50 commands)
        lines = filter(lambda line: line[0] != '#' or line[:7] == '#[VC50]', lines)
        # filter the extra whitespace
        lines = map(string.rstrip, lines)
        # filter out the blank lines
        lines = filter(None, lines)
        # filter out the *setup* lines
        lines = filter(lambda line: line != '*shared*', lines)
        lines = filter(lambda line: line != '*noconfig*', lines)

        # join the lines which need joining
        newlines = [lines[0]]
        lastline = lines[0]
        for line in lines[1:]:
            if lastline[-1] == '\\':
                newlines[:-1] = newlines[:-1] + line
            else:
                newlines.append(line)
            lastline = line

        self.projects = []
        
        # get value for location of python15.lib
        self.libsdir = get_pylibsdir()
        
        # get value for location of Python's include directory(ies)
        self.incsdir = get_pyincludedir()
        
        # since #[VC50] things apply to the *next* module, keep track
        # of them.
        extra_stuff = ''
        for line in lines:
            if line[:7] == '#[VC50]':
                extra_stuff = extra_stuff + '\n'+ line[7:]
            else:
                # create project file
                project = Project(self, self.msdev, line, extra_stuff) 
                self.projects.append(project)
                extra_stuff = ''  # new project does not inherit last
                                  # one's flags etc.

        self.output_workspace_file()

    def output_workspace_file(self):
        # the only tricky thing here is the fact that each project
        # depends (in the sense of a Makefile dependency) on the last
        # one), so that e.g. the modules in NumPy can be built after
        # _numpy has been built (otherwise DevStudio goes in
        # *alphabetical* order!). 

        projectinfo = ""
        oldp = None
        for project in self.projects:
            dependency_part = ""
            projectname = project.projectname
            projectfilepath = projectname+".dsp"
            # making thedir be this value prevents the creation of a random
            # directory (see template for project file).
            # [it'd be nice to understand exactly what this is about
            thedir = os.path.join(os.pardir, os.path.split(os.getcwd())[1])
            if oldp:
                dependency_part = 'Begin Project Dependency\nProject_Dep_Name '+oldp.projectname+'\nEnd Project Dependency\n'
            else:
                dependency_part = ''
            project_file_text = projectInWorkspaceTemplate % locals()
            projectinfo = projectinfo + project_file_text + '\n'
            oldp = project

        template_dict = locals().copy()
        template_dict.update(self.msdev.__dict__)
        workspace_text = workspacetemplate % template_dict
        workspace_file = open(self.workspacename+'.dsw', 'w')
        workspace_file.write(workspace_text)

def compile_unix():
    import os, sys
    import compileall

    lib = os.path.join(os.path.join(sys.exec_prefix, 'lib'),
                   'python'+sys.version[:3])

    if not os.path.exists('Makefile.pre.in'):
        source = os.path.join(os.path.join(lib, 'config'), 'Makefile.pre.in')
        if os.path.exists(source):
            os.system('cp ' + source + ' .')
        else:
            print "Copy Misc/Makefile.pre.in from the Python distribution"
            print "to this directory and try again."

    compileall.compile_dir(".")
    os.system("make -f Makefile.pre.in boot")
    os.system("make")

if sys.platform == 'win32':
    swig_found = find_swig()
    msdev = Msdev()
    ws = Workspace(msdev)
    msdev.build(ws)
    #msdev.Quit()
else:
    compile_unix()
