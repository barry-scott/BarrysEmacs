""" 
Script to install the Numeric files.  Assumes shared objects have
been built already.

Expects these names to be set:
    PACKAGENAME: the name under which to install. (or set as argument)
    PTHFILE: the name of the .pth file to install


Version 2. Uses .pth file; very much simplified.

Please let us know of bugs or failures.

David Ascher - July 21, 1998;
Paul Dubois - March 1999;
David Ascher - May 1999;
"""

import glob, shutil, os, sys, string

def mkrecursivedir(libdir):
    path, tail = os.path.split(libdir)
    if tail != '':
        mkrecursivedir(path)
    if not os.path.exists(libdir):
        print "#making directory", libdir
        os.mkdir(libdir)
    elif not os.path.isdir(libdir):
        print "Oops! recursive mkdir failed: %s is not a directory!" % libdir
        type, value = sys.exc_info[:2]
        raise type, value
    
def do_install():
    global MODE
    if MODE == None:
        # determime the mode of the files in their target directory
        origumask = os.umask(0) ; os.umask(origumask)  # leave unchanged
        MODE = 0666 ^ origumask    # this will be 0644 or 0664 in most cases

    if sys.platform in ('win32', 'mac'):
        installdir = sys.exec_prefix
        idir = os.path.join(sys.prefix, 'include')
    else:
        installdir = os.path.join(sys.exec_prefix, 'lib',
            'python'+sys.version[:3],'site-packages')
        idir = os.path.join(sys.prefix, 'include',
            'python'+sys.version[:3])

    if len(sys.argv) == 1 and globals().has_key('PACKAGENAME'):
        pname = PACKAGENAME
    elif len(sys.argv) == 2:
        pname = sys.argv[1]
    elif len(sys.argv) == 3:
        pname = sys.argv[1]
        prefixdir = sys.argv[2]
        installdir = os.path.join(prefixdir, 'lib',
            'python'+sys.version[:3],'site-packages')
        idir = os.path.join(prefixdir, 'include',
            'python'+sys.version[:3])
    else:
        print "Usage: python installthis.py [package_name [install_prefix] ]"
        raise SystemExit
    libdir = os.path.join(installdir, pname)
    idir = os.path.join(idir, pname)

    installdir = os.path.normpath(installdir)
    libdir = os.path.normpath(libdir)
    idir = os.path.normpath(idir)

    print "Will install .py files and binaries to ", libdir
    print "Will install include files to ", idir
    print "OK? (y/n)",
    f = sys.stdin.readline()
    if f[0] == "y" or f[0] == "Y":
        pass
    else:
        print "Installation aborted."
        raise SystemExit

    if not os.path.exists (libdir):
        mkrecursivedir(libdir)

    if not os.path.exists (idir):
        mkrecursivedir(idir)

    if sys.platform == 'win32':
        for fname in glob.glob(os.path.join('pyds', '*.pyd')):
            target = os.path.normpath(\
                os.path.join(libdir, os.path.split(fname)[1]))
            os.chmod(fname, MODE)
            shutil.copy(fname, target)

    else:
        dsodir = libdir
        for dso in glob.glob('*.so'):
            target = os.path.normpath(\
                os.path.join(dsodir, os.path.split(dso)[1]))
            shutil.copy(dso, target)
        for dso in glob.glob('*.sl'):
            target = os.path.normpath(\
                os.path.join(dsodir, os.path.split(dso)[1]))
            shutil.copy(dso, target)

    for dir in DIRS:
        d1 = os.path.normpath(\
                os.path.join(dir, '*.py'))
        d2 = os.path.normpath(\
                os.path.join(dir, '*.pyc'))
        # problem: maybe not all the pyc's exist...
        fnames = glob.glob(d1) + glob.glob(d2)
        for fname in fnames:
            os.chmod(fname, MODE)
            target = os.path.normpath(\
                os.path.join(libdir, os.path.split(fname)[1]))
            shutil.copy(fname, target)
# Install include files to PYTHON/Include/PACKAGENAME
    d1 = os.path.normpath(\
            os.path.join('Include', '*.h'))
    d2 = os.path.normpath(\
            os.path.join('Include', '*.hh'))
    fnames = glob.glob(d1) + glob.glob(d2)
    for fname in fnames:
        os.chmod(fname, MODE)
        target = os.path.normpath(\
            os.path.join(idir, os.path.split(fname)[1]))
        shutil.copy(fname, target)

    # Now put in the pth file
    target = os.path.normpath(\
        os.path.join(installdir, pname + '.pth'))
    f = open(target, 'w')
    f.write(pname+"\n")
    f.close()
    os.chmod(target, MODE)
    print "Installation to ", libdir, "completed."
    print "Include files installed to ", idir, "."

do_install()

