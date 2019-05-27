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
import build_log
import build_utils

log = build_log.BuildLog()

# setup build_utils
build_utils.log = log
# alias run()
run = build_utils.run
BuildError = build_utils.BuildError

class PackageBEmacs(object):
    valid_cmds = ('srpm', 'mock', 'copr-release', 'copr-testing', 'list-release', 'list-testing')

    def __init__( self ):
        self.KITNAME = 'bemacs'

        self.opt_colour = False
        self.opt_verbose = False
        self.opt_sqlite = True
        self.opt_gui = True
        self.opt_system_pycxx = False
        self.opt_system_ucd = False
        self.opt_system_sqlite = False
        self.opt_warnings_as_errors = False

        self.opt_kit_sqlite = None
        self.opt_kit_pycxx = None

        self.cmd = None
        self.release = 'auto'
        self.mock_target = None
        self.arch = None
        self.install = False

    def main( self, argv ):
        try:
            self.parseArgs( argv )
            log.setColour( self.opt_colour )
            self.setupVars()

            if self.cmd == 'srpm':
                self.buildSrpm()

            elif self.cmd == 'mock':
                self.buildMock()

            elif self.cmd == 'copr-release':
                self.buildCoprRelease()

            elif self.cmd == 'copr-testing':
                self.buildCoprTesting()

            elif self.cmd == 'list-release':
                self.listCoprRelease()

            elif self.cmd == 'list-testing':
                self.listCoprTesting()

        except KeyboardInterrupt:
            return 2

        except BuildError as e:
            log.error( str(e) )
            return 1

        return 0

    def setupVars( self ):
        # expects to be run from the Builder folder
        self.BUILDER_TOP_DIR = os.path.abspath( '..' )

        import brand_version
        vi = brand_version.VersionInfo( self.BUILDER_TOP_DIR, log.info )
        vi.parseVersionInfo( os.path.join( self.BUILDER_TOP_DIR, 'Builder/version_info.txt' ) )

        self.version = '%s.%s.%s' % (vi.get( 'major' ), vi.get( 'minor' ), vi.get( 'patch' ))

        self.MOCK_COPR_REPO_FILENAME = '/etc/yum.repos.d/_copr:copr.fedorainfracloud.org:barryascott:tools.repo'

    def parseArgs( self, argv ):
        try:
            args = iter( argv )
            next(args)

            self.cmd = next(args)
            if self.cmd not in self.valid_cmds:
                raise BuildError( 'Unknown command %r - pick on of: %s' %
                                    (self.cmd, ', '.join( self.valid_cmds,)) )

            while True:
                arg = next(args)
                if arg == '--debug':
                    log.setDebug( True )

                elif arg == '--verbose':
                    self.opt_verbose = True

                elif arg == '--colour':
                    self.opt_colour = True

                elif arg == '--no-sqlite':
                    self.opt_sqlite = False

                elif arg == '--system-sqlite':
                    self.opt_system_sqlite = True

                elif arg.startswith( '--kit-sqlite=' ):
                    self.opt_kit_sqlite = arg[len('--kit-sqlite='):]

                elif arg == '--no-gui':
                    self.opt_gui = False

                elif arg == '--system-pycxx':
                    self.opt_system_pycxx = True

                elif arg.startswith( '--kit-pycxx=' ):
                    self.opt_kit_pycxx = arg[len('--kit-pycxx='):]

                elif arg == '--system-ucd':
                    self.opt_system_ucd = True

                elif arg == '--no-warnings-as-errors':
                    self.opt_warnings_as_errors = False

                elif arg.startswith('--release='):
                    self.release = arg[len('--release='):]

                elif arg.startswith('--mock-target='):
                    self.mock_target = arg[len('--mock-target='):]

                elif arg.startswith('--arch='):
                    self.arch = arg[len('--arch='):]

                elif arg.startswith('--install'):
                    self.install = True

                else:
                    raise BuildError( 'Unknown option %r' % (arg,) )

        except StopIteration:
            pass

    def fedoraVersion( self ):
        with open( '/etc/os-release', 'r' ) as f:
            for line in f:
                if line.startswith( 'VERSION_ID=' ):
                    return int( line.strip()[len('VERSION_ID='):] )

        raise BuildError( 'Expected /etc/os-release to have a VERSION_ID= field' )

    def buildSrpm( self ):
        if self.release == 'auto':
            self.release = 1

        run( ('rm', '-rf', 'tmp') )
        run( ('mkdir', 'tmp') )
        run( ('mkdir', 'tmp/sources') )

        self.makeTarBall()

        if self.opt_kit_pycxx is not None:
            log.info( 'Add PyCXX kit to sources from %s' % (self.opt_kit_pycxx,) )
            shutil.copyfile( self.opt_kit_pycxx, os.path.join( 'tmp/sources', os.path.basename( self.opt_kit_pycxx ) ) )

        if self.opt_kit_sqlite is not None:
            log.info( 'Add sqlite kit to sources from %s' % (self.opt_kit_sqlite,) )
            shutil.copyfile( self.opt_kit_sqlite, os.path.join( 'tmp/sources', os.path.basename( self.opt_kit_sqlite ) ) )

        self.ensureMockSetup()
        self.makeSrpm()
        log.info( 'SRPM is %s' % (self.SRPM_FILENAME,) )

    def buildMock( self ):
        self.buildSrpm()

        log.info( 'Creating RPM' )
        run( ('mock',
                    '--root=%s' % (self.MOCK_TARGET_FILENAME,),
                    '--enablerepo=barryascott-tools',
                    '--rebuild',
                    self.SRPM_FILENAME) )

        all_bin_kitnames = [
            self.KITNAME,
            '%s-cli' % (self.KITNAME,),
            '%s-common' % (self.KITNAME,),
            '%s-cli-debuginfo' % (self.KITNAME,),
            '%s-debugsource' % (self.KITNAME,),
            '%s-debuginfo' % (self.KITNAME,),
            ]

        if self.opt_gui:
            all_bin_kitnames.extend( [
                '%s-gui' % (self.KITNAME,),
                '%s-gui-debuginfo' % (self.KITNAME,),
                ] )

        run( ('mkdir','-p', 'built') )

        for bin_kitname in all_bin_kitnames:
            basename = '%s-%s-%s.%s.%s.rpm' % (bin_kitname, self.version, self.release, self.dist_tag, self.arch)
            src = '%s/RPMS/%s' % (self.MOCK_BUILD_DIR, basename)
            if os.path.exists( src ):
                log.info( 'Copying %s' % (basename,) )
                shutil.copyfile( src, 'built/%s' % (basename,) )

        log.info( 'Results in %s/built:' % (os.getcwd(),) )

        if self.install:
            log.info( 'Installing RPMs' )

            for bin_kitname in all_bin_kitnames:
                cmd = ('rpm', '-q', bin_kitname)
                p = run( cmd, check=False )
                if p.returncode == 0:
                    run( ('sudo', 'dnf', '-y', 'remove', bin_kitname) )

            cmd = ['sudo', 'dnf', '-y', 'install']
            cmd.extend( glob.glob( 'tmp/%s*.%s.rpm' % (self.KITNAME, self.arch) ) )
            run( cmd )

    def buildCoprRelease( self ):
        self.buildCopr( 'tools' )

    def buildCoprTesting( self ):
        self.buildCopr( 'tools-testing' )

    def buildCopr( self, copr_repo ):
        if self.release == 'auto':
            p = self.listCopr( copr_repo )
            for line in p.stdout.split('\n'):
                if line.startswith( '%s.src' % (self.KITNAME,) ):
                    parts = line.split()
                    version, rel_distro = parts[1].split('-')
                    release, distro = rel_distro.split('.')

                    if version == self.version and distro == self.dist_tag:
                        self.release = int(release) + 1

                    else:
                        self.release = 1

                    break

        self.buildSrpm()
        run( ('copr-cli', 'build', '-r', self.mock_target, copr_repo, self.SRPM_FILENAME) )

    def listCoprRelease( self ):
        p = self.listCopr( 'tools' )
        print( p.stdout )

    def listCoprTesting( self ):
        p = self.listCopr( 'tools-testing' )
        print( p.stdout )

    def listCopr( self, copr_repo ):
        return run( ('dnf', 'list', 'available', '--refresh', '--disablerepo=*', '--enablerepo=barryascott-%s' % (copr_repo,)), output=True )

    def ensureMockSetup( self ):
        log.info( 'Creating mock target file' )
        self.makeMockTargetFile()

        p = run( ('mock', '--root=%s' % (self.MOCK_TARGET_FILENAME,), '--print-root-path'), output=True )

        self.MOCK_ROOT = p.stdout.strip()
        self.MOCK_BUILD_DIR = '%s/builddir/build' % (self.MOCK_ROOT,)

        if os.path.exists( self.MOCK_ROOT ):
            log.info( 'Using existing mock for %s' % (self.mock_target,) )

        else:
            log.info( 'Init mock for %s' % (self.MOCK_TARGET_FILENAME,) )
            run( ('mock', '--root=%s' % (self.MOCK_TARGET_FILENAME,), '--init') )

    def makeMockTargetFile( self ):
        if self.mock_target is None:
            self.mock_target = 'fedora-%d-%s' % (self.fedoraVersion(), platform.machine())
            log.info( 'Defaulting --mock-target=%s' % (self.mock_target,) )

        self.MOCK_TARGET_FILENAME = 'tmp/%s-%s.cfg' % (self.KITNAME, self.mock_target)

        mock_cfg = '/etc/mock/%s.cfg' % (self. mock_target,)
        if not os.path.exists( mock_cfg ):
            raise BuildError( 'Mock CFG files does not exist %s' % (mock_cfg,) )

        with open( mock_cfg, 'r' ) as f:
            cfg_code = compile( f.read(), 'mock_cfg', 'exec' )
            config_opts = {}
            exec( cfg_code )

        # set to match the mock target
        self.arch = config_opts[ 'target_arch' ]
        self.dist_tag = config_opts[ 'dist' ]

        with open( self.MOCK_COPR_REPO_FILENAME, 'r' ) as f:
            repo = f.read()

            config_opts['yum.conf'] += '\n'
            config_opts['yum.conf'] += repo
            config_opts['root'] = os.path.splitext( os.path.basename( self.MOCK_TARGET_FILENAME ) )[0]

        with open( self.MOCK_TARGET_FILENAME, 'w' ) as f:
            for k in config_opts:
                if k == 'yum.conf':
                    print( 'config_opts[\'yum.conf\'] = """', end='', file=f )
                    print( config_opts['yum.conf'], file=f )
                    print( '"""', file=f )

                else:
                    print( 'config_opts[%r] = %r' % (k, config_opts[k]), file=f )

        # prevent mock from rebuilding the mock cache on each build.
        # mock uses the timestamp on the CFG file and compares to the
        # cache timestamp. Use the timestamp of the input cfg to avoid
        # rebuilding the cache unless the original CFG file changes.
        st = os.stat( mock_cfg )
        os.utime( self.MOCK_TARGET_FILENAME, (st.st_atime, st.st_mtime) )

    def makeTarBall( self ):
        self.KIT_BASENAME = '%s-%s' % (self.KITNAME, self.version)

        log.info( 'Exporting source code' )

        cmd = '(cd ${BUILDER_TOP_DIR}; git archive --format=tar --prefix=%s/ master) | tar xf - -C tmp ' % (self.KIT_BASENAME,)
        run( cmd )

        p = run( ('git', 'show-ref', '--head', '--hash', 'head'), output=True, cwd=os.environ['BUILDER_TOP_DIR'] )

        with open( os.path.join( 'tmp', self.KIT_BASENAME, 'Builder/commit_id.txt' ), 'w' ) as f:
            f.write( p.stdout )

        run( ('tar', 'czf', 'sources/%s.tar.gz' % (self.KIT_BASENAME,), self.KIT_BASENAME), cwd='tmp' )

    def makeSrpm( self ):
        log.info( 'creating %s.spec' % (self.KITNAME,) )
        import package_rpm_specfile
        package_rpm_specfile.createRpmSpecFile( self, 'tmp/%s.spec' % (self.KITNAME,) )

        log.info( 'Creating SRPM for %s' % (self.KIT_BASENAME,) )

        run( ('mock',
                '--root=%s' % (self.MOCK_TARGET_FILENAME,),
                '--buildsrpm',
                '--spec', 'tmp/%s.spec' % (self.KITNAME,),
                '--sources', 'tmp/sources') )

        SRPM_BASENAME = '%s-%s.%s' % (self.KIT_BASENAME, self.release, self.dist_tag)
        self.SRPM_FILENAME = 'tmp/%s.src.rpm' % (SRPM_BASENAME,)

        src = '%s/SRPMS/%s.src.rpm' % (self.MOCK_BUILD_DIR, SRPM_BASENAME)
        log.info( 'copy %s %s' % (src, self.SRPM_FILENAME) )
        shutil.copyfile( src, self.SRPM_FILENAME )

if __name__ == '__main__':
    sys.exit( PackageBEmacs().main( sys.argv ) )
