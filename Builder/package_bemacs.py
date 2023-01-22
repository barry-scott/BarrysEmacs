#!/usr/bin/env python3
#
#   package_bemacs.py
#
from __future__ import print_function   # needed to allow python3 only error message

import sys
import os
import re
import time
import subprocess
import platform
import glob
import build_log
import build_utils
import package_list_repo

log = build_log.BuildLog()

SUPPORTED_PYQT_VERSIONS = ('6', '5')

# setup build_utils
build_utils.log = log
# alias run()
run = build_utils.run
BuildError = build_utils.BuildError

class PackageBEmacs(object):
    valid_cmds = ('srpm-release', 'srpm-testing'
                 ,'mock-release', 'mock-testing', 'mock-standalone'
                 ,'copr-release', 'copr-testing'
                 ,'list-release', 'list-testing'
                 ,'debian-test-build', 'debian-sbuild')

    def __init__( self ):
        self.KITNAME = 'bemacs'

        self.opt_colour = False
        self.opt_verbose = False
        self.opt_sqlite = True
        self.opt_sftp = True
        self.opt_gui = True
        self.opt_pyqt_version = SUPPORTED_PYQT_VERSIONS[0]
        self.opt_system_ucd = False
        self.opt_hunspell = True
        self.opt_system_hunspell = False
        self.opt_warnings_as_errors = False
        self.opt_kit_sqlite = None
        self.opt_kit_pycxx = None
        self.opt_kit_xml_preferences = None

        self.os_release_info = None

        self.copr_repo = None
        self.copr_repo_other = None
        self.COPR_REPO_URL = None
        self.COPR_REPO_OTHER_URL = None
        self.MOCK_COPR_REPO_FILENAME = None

        self.copr_repo_pyqt6 = 'copr:copr.fedorainfracloud.org:barryascott:python-qt6'
        self.MOCK_COPR_REPO_PYQT6_FILENAME = '/etc/yum.repos.d/_%s.repo' % (self.copr_repo_pyqt6,)

        self.cmd = None
        self.opt_release = 'auto'
        self.opt_debian_repos = None
        self.commit_id = 'unknown'
        self.opt_mock_target = None
        self.opt_arch = None
        self.install = False

    def main( self, argv ):
        try:
            if sys.version_info[0] != 3:
                raise BuildError( '%s requires python3' % (os.path.basename( argv[0] ),) )

            self.parseArgs( argv )
            log.setColour( self.opt_colour )
            self.setupVars()

            if self.cmd in ('srpm-release', 'srpm-testing'):
                self.buildSrpm()

            elif self.cmd in ('mock-testing', 'mock-release', 'mock-standalone'):
                self.buildMock()

            elif self.cmd in ('copr-release', 'copr-testing' ):
                self.buildCopr()

            elif self.cmd in ('list-release', 'list-testing'):
                self.listCopr()

            elif self.cmd in ('debian-test-build',):
                self.buildDebianSourcePackage( sbuild=False )

            elif self.cmd in ('debian-sbuild',):
                self.buildDebianSourcePackage( sbuild=True )

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

        self.os_release_info = self.loadOsReleaseInfo()

        if self.os_release_info['ID'] == 'fedora':
            if self.copr_repo:
                self.MOCK_COPR_REPO_FILENAME = '/etc/yum.repos.d/_copr:copr.fedorainfracloud.org:barryascott:%s.repo' % (self.copr_repo,)

            if self.opt_mock_target is None:
                self.opt_mock_target = 'fedora-%s-%s' % (self.os_release_info['VERSION_ID'], platform.machine())
                log.info( 'Defaulting --mock-target=%s' % (self.opt_mock_target,) )

            if self.copr_repo:
                with open( self.MOCK_COPR_REPO_FILENAME, 'r' ) as f:
                    for line in f:
                        if line.startswith('baseurl='):
                            baseurl = line.strip().split('=', 1)[1]
                            self.COPR_REPO_URL = baseurl.replace('fedora-$releasever-$basearch', self.opt_mock_target)

            if self.copr_repo_other:
                self.COPR_REPO_OTHER_URL = 'https://download.copr.fedorainfracloud.org/results/barryascott/%s/%s' % (self.copr_repo_other, self.opt_mock_target)

            if self.opt_release == 'auto':
                all_packages = package_list_repo.listRepo( self.COPR_REPO_URL )
                all_other_packages = package_list_repo.listRepo( self.COPR_REPO_OTHER_URL )

                package_ver = 0
                other_package_ver = 0

                if self.KITNAME in all_packages:
                    key, ver, rel, build_time = all_packages[ self.KITNAME ]
                    if ver == self.version:
                        package_ver = int( rel.split('.')[0] )
                        log.info( 'Release %d found in %s' % (package_ver, self.copr_repo) )

                if self.KITNAME in all_other_packages:
                    key, ver, rel, build_time = all_other_packages[ self.KITNAME ]
                    if ver == self.version:
                        other_package_ver = int( rel.split('.')[0] )
                        log.info( 'Release %d found in %s' % (package_ver, self.copr_repo_other) )

                self.opt_release = 1 + max( package_ver, other_package_ver )

                log.info( 'Release set to %d' % (self.opt_release,) )

            else:
                log.info( 'Building for release %s' % (self.opt_release,) )

            return

        if self.os_release_info['ID'] in ('ubuntu', 'debian'):
            return

        raise BuildError( 'Unsupported OS %r' % (self.os_release_info['ID'],) )

    def parseArgs( self, argv ):
        try:
            args = iter( argv )
            next(args)

            self.cmd = next(args)
            if self.cmd not in self.valid_cmds:
                raise BuildError( 'Unknown command %r - pick on of: %s' %
                                    (self.cmd, ', '.join( self.valid_cmds,)) )

            if self.cmd in ('srpm-release', 'mock-release', 'list-release', 'copr-release'):
                self.copr_repo = 'tools'
                self.copr_repo_other = 'tools-testing'

            elif self.cmd in ('srpm-testing', 'mock-testing', 'list-testing', 'copr-testing'):
                self.copr_repo = 'tools-testing'
                self.copr_repo_other = 'tools'

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

                elif arg == '--no-sftp':
                    self.opt_sftp = False

                elif arg.startswith( '--kit-sqlite=' ):
                    self.opt_kit_sqlite = arg[len('--kit-sqlite='):]

                elif arg == '--no-gui':
                    self.opt_gui = False

                elif arg.startswith('--pyqt-version='):
                    self.opt_pyqt_version = arg[len('--pyqt-version='):]
                    if self.opt_pyqt_version not in SUPPORTED_PYQT_VERSIONS:
                        BuildError( 'Unsupported PyQt version %r' % (arg,) )

                elif arg.startswith('--kit-pycxx='):
                    self.opt_kit_pycxx = arg[len('--kit-pycxx='):]

                elif arg.startswith('--kit-xml-preferences='):
                    self.opt_kit_xml_preferences = arg[len('--kit-xml-preferences='):]

                elif arg == '--system-ucd':
                    self.opt_system_ucd = True

                elif arg == '--system-hunspell':
                    self.opt_system_hunspell = True

                elif arg == '--no-hunspell':
                    self.opt_hunspell = False

                elif arg == '--no-warnings-as-errors':
                    self.opt_warnings_as_errors = False

                elif arg.startswith('--release='):
                    self.opt_release = arg[len('--release='):]

                elif arg.startswith('--debian-repos='):
                    self.opt_debian_repos = arg[len('--debian-repos='):]

                elif arg.startswith('--mock-target='):
                    self.opt_mock_target = arg[len('--mock-target='):]

                elif arg.startswith('--arch='):
                    self.opt_arch = arg[len('--arch='):]

                elif arg.startswith('--install'):
                    self.install = True

                else:
                    raise BuildError( 'Unknown option in package_bemacs %r' % (arg,) )

        except StopIteration:
            pass

    def loadOsReleaseInfo( self ):
        info = {}
        with open( '/etc/os-release', 'r' ) as f:
            for line in f:
                key, value = line.strip().split( '=', 1 )
                if value.startswith('"') and value.endswith('"'):
                    value = value[1:-1]

                info[ key ] = value

        return info

    def buildSrpm( self ):
        run( ('rm', '-rf', 'tmp') )
        run( ('mkdir', 'tmp') )
        run( ('mkdir', 'tmp/sources') )

        self.makeTarBall()

        if self.opt_kit_pycxx is not None:
            log.info( 'Add PyCXX kit to sources from %s' % (self.opt_kit_pycxx,) )
            build_utils.copyFile( self.opt_kit_pycxx, 'tmp/sources', 0o600 )

        if self.opt_kit_sqlite is not None:
            log.info( 'Add sqlite kit to sources from %s' % (self.opt_kit_sqlite,) )
            build_utils.copyFile( self.opt_kit_sqlite, 'tmp/sources', 0o600 )

        if self.opt_kit_xml_preferences is not None:
            log.info( 'Add xml-preferences to sources from %s' % (self.opt_kit_xml_preferences,) )
            build_utils.copyFile( self.opt_kit_xml_preferences, 'tmp/sources', 0o600 )

        self.ensureMockSetup()
        self.makeSrpm()
        log.info( 'SRPM is %s' % (self.SRPM_FILENAME,) )

    def buildDebianSourcePackage( self, sbuild ):
        run( ('rm', '-rf', 'tmp') )
        run( ('mkdir', 'tmp') )
        run( ('mkdir', 'tmp/sources') )

        self.makeTarBall()

        run( ('mkdir', 'tmp/%s/debian' % (self.KIT_BASENAME,)) )
        run( ('mkdir', 'tmp/%s/debian/source' % (self.KIT_BASENAME,)) )

        # figure out the debian release
        if self.opt_release == 'auto':
            if self.opt_debian_repos is None:
                raise BuildError( '--release=auto requires --debian-repos=<repos-dir>' )

            # assume debian release 1
            self.opt_release = '1'

            if not os.path.exists( self.opt_debian_repos ):
                log.info( 'debian repos not found %s' % (self.opt_debian_repos,) )

            else:
                debian_release = 0
                for deb in glob.glob( '%s/bemacs_%s-*.deb' % (self.opt_debian_repos, self.version) ):
                    debian_release = max( debian_release, int( deb.split('_')[1].split('-')[1] ) )

                self.opt_release = '%d' % (debian_release + 1,)

        log.info( log.colourFormat('Building version <>em %s-%s<>') % (self.version, self.opt_release) )

        # debian/changelog
        with open( 'tmp/%s/debian/changelog' % (self.KIT_BASENAME,), 'w' ) as f:
            changelog_args = {
                'date':
                    time.strftime('%a, %d %b %Y %H:%M:%S +0000'),
                'email':
                    'Barry Scott <barry@barrys-emacs.org>',
                'version':
                    self.version,
                'release':
                    self.opt_release,
                }
            f.write(
'''bemacs (%(version)s-%(release)s) UNRELEASED; urgency=medium

  * Initial release.

 -- %(email)s  %(date)s

''' % changelog_args )

        with open( 'tmp/%s/debian/changelog' % (self.KIT_BASENAME,), 'r' ) as f:
            changelog = f.read()

        with open( 'tmp/%s/debian/changelog' % (self.KIT_BASENAME,), 'w' ) as f:
            changelog = changelog.replace( ' (Closes: #XXXXXX)', ' (Closes:)' )
            f.write( changelog )

        # debian/compat
        with open( 'tmp/%s/debian/compat' % (self.KIT_BASENAME,), 'w' ) as f:
            f.write( '10\n' )

        # debian/control
        build_depends = [
            'debhelper (>= 13)',
            'libhunspell-dev',
            'python3-cxx-dev',
            'libsqlite3-dev',
            'libssh-dev',
            'unicode-data',
            ]
        depends = [
            '${shlibs:Depends}',
            '${misc:Depends}',
            'python3',
            'python3-pyqt%s' % (self.opt_pyqt_version,),
            'fonts-noto-mono',
            ]

        control_args = {
            'Standards-Version':    # version of a debian packaging standard?
                '3.9.2',
            'Version':
                '%s-%s' % (self.version, self.opt_release),
            'Depends':
                ', '.join( depends ),
            'Build-Depends':
                ', '.join( build_depends ),
            }

        with open( 'tmp/%s/debian/control' % (self.KIT_BASENAME,), 'w' ) as f:
            f.write(
'''Source: bemacs
Maintainer: Barry scott <barry@barrys-emacs.org>
Section: misc
Priority: optional
Standards-Version: %(Standards-Version)s
Build-Depends: %(Build-Depends)s

Package: bemacs
Architecture: any
Depends: %(Depends)s
Description: Barry's Emacs text editor
 Easy to use text editor
''' % control_args )

        # debian/copyright
        with open( 'tmp/%s/debian/copyright' % (self.KIT_BASENAME,), 'w' ) as f:
            f.write(
'''Files:
 *
Copyright: 1980-2023 Barry A. Scott
License: Apache-2.0
''' )

        # debian/format
        with open( 'tmp/%s/debian/source/format' % (self.KIT_BASENAME,), 'w' ) as f:
            f.write( '3.0 (quilt)\n' )

        # debian/rules
        with open( 'tmp/%s/debian/rules' % (self.KIT_BASENAME,), 'w' ) as f:
            f.write(
f'''#!/usr/bin/make -f
%:
        dh $@

override_dh_auto_install:
        Builder/debian-package-dh-build.sh $$(pwd)/debian/bemacs "{self.opt_pyqt_version}"
'''.replace('\n        ', '\n\t') )
        os.chmod( 'tmp/%s/debian/rules' % (self.KIT_BASENAME,), 0o775)

        # debian/source/lintian-overrides
        with open( 'tmp/%s/debian/source/lintian-overrides' % (self.KIT_BASENAME,), 'w' ) as f:
            f.write(
'''# overrides for bemacs
bemacs source: source-is-missing [HTML/extn_intro.html]
bemacs source: source-is-missing [HTML/extn_libraries.html]
bemacs source: source-is-missing [HTML/pg_extension_facilities.html]
bemacs source: source-is-missing [HTML/pg_external_programing.html]
bemacs source: source-is-missing [HTML/pg_macros.html]
bemacs source: source-is-missing [HTML/pg_mlisp.html]
bemacs source: source-is-missing [HTML/ug_advancededit.html]
bemacs source: source-is-missing [HTML/ug_basicedit.html]
bemacs source: source-is-missing [HTML/ug_correct.html]
bemacs source: source-is-missing [HTML/ug_customise.html]
bemacs source: source-is-missing [HTML/ug_specificedit.html]
bemacs source: source-is-missing [HTML/ug_top.html]
''' )

        if not sbuild:
            # build using system installed dependencies
            run( ('debuild', '--unsigned-source', '--unsigned-changes'),
                cwd='tmp/%s' % (self.KIT_BASENAME,) )

        else:
            # build using chroot
            run( ('debuild',
                    '--build=source',
                    '--unsigned-source',
                    '--unsigned-changes',
                    '--no-check-builddeps' ),
                cwd='tmp/%s' % (self.KIT_BASENAME,) )

    def buildMock( self ):
        self.buildSrpm()

        log.info( 'Creating RPM' )
        if self.copr_repo is not None:
            run( ('mock',
                        '--root=%s' % (self.MOCK_TARGET_FILENAME,),
                        '--enablerepo=copr:copr.fedorainfracloud.org:barryascott:%s' % (self.copr_repo,),
                        '--enablerepo=%s' % (self.copr_repo_pyqt6,),
                        '--rebuild',
                        self.SRPM_FILENAME) )
        else:
            run( ('mock',
                        '--root=%s' % (self.MOCK_TARGET_FILENAME,),
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
            basename = '%s-%s-%s.%s.%s.rpm' % (bin_kitname, self.version, self.opt_release, self.dist_tag, self.opt_arch)
            src = '%s/RPMS/%s' % (self.MOCK_BUILD_DIR, basename)
            if os.path.exists( src ):
                log.info( 'Copying %s' % (basename,) )
                build_utils.copyFile( src, 'built/%s' % (basename,), 0o600 )

        log.info( 'Results in %s/built:' % (os.getcwd(),) )

        if self.install:
            log.info( 'Installing RPMs' )

            for bin_kitname in all_bin_kitnames:
                cmd = ('rpm', '-q', bin_kitname)
                p = run( cmd, check=False )
                if p.returncode == 0:
                    run( ('sudo', 'dnf', '-y', 'remove', bin_kitname) )

            cmd = ['sudo', 'dnf', '-y', 'install']
            cmd.extend( glob.glob( 'tmp/%s*.%s.rpm' % (self.KITNAME, self.opt_arch) ) )
            run( cmd )

    def buildCopr( self ):
        # setup vars based on mock config
        self.readMockConfig()

        self.buildSrpm()
        run( ('copr-cli', 'build', '-r', self.opt_mock_target, self.copr_repo, self.SRPM_FILENAME) )

    def listCopr( self ):
        all_packages = package_list_repo.listRepo( self.COPR_REPO_URL )
        print( 'Packages for %s %s' % (self.copr_repo, self.opt_mock_target) )

        now = time.time()

        for name in sorted( all_packages.keys() ):
            key, ver, rel, build_time = all_packages[ name ]

            build_age = self.formatTimeDelta( now - build_time )

            build_time_str = time.strftime( '%Y-%m-%d %H:%M:%S', time.localtime( build_time ) )
            print( '  %30s: %-8s %-8s %s - %s' % (name, ver, rel, build_time_str, build_age ) )

    def formatTimeDelta( self, age ):
        r, s = divmod( age, 60 )
        r, m = divmod( r, 60 )
        d, h = divmod( r, 24 )

        return '%3dd %2.2d:%2.2d' % (d, h, m)

    def ensureMockSetup( self ):
        log.info( 'Creating mock target file' )
        self.makeMockTargetFile()

        p = run( ('mock', '--root=%s' % (self.MOCK_TARGET_FILENAME,), '--print-root-path'), output=True )

        self.MOCK_ROOT = p.stdout.strip()
        self.MOCK_BUILD_DIR = '%s/builddir/build' % (self.MOCK_ROOT,)

        if os.path.exists( self.MOCK_ROOT ):
            log.info( 'Using existing mock for %s' % (self.opt_mock_target,) )

        else:
            log.info( 'Init mock for %s' % (self.MOCK_TARGET_FILENAME,) )
            run( ('mock', '--root=%s' % (self.MOCK_TARGET_FILENAME,), '--init') )

    def readMockConfig( self ):
        if self.opt_mock_target.startswith( '/' ):
            mock_cfg = self.opt_mock_target + '.cfg'
        else:
            mock_cfg = '/etc/mock/%s.cfg' % (self.opt_mock_target,)
        if not os.path.exists( mock_cfg ):
            raise BuildError( 'Mock CFG files does not exist %s' % (mock_cfg,) )

        with open( mock_cfg, 'r' ) as f:
            # starting with Fedora 31 mock uses the include('template') statement
            config_opts = {'yum_install_command': 'install yum yum-utils'
                          ,'plugin_conf':
                            {'root_cache_opts': {}
                            ,'selinux_enable': False}}
            cfg_locals = {'config_opts': config_opts}

            def include( tpl ):
                abs_tpl = os.path.join( '/etc/mock', tpl )
                with open( abs_tpl, 'r' ) as t:
                    tpl_code = compile( t.read(), 'mock_tpl', 'exec' )
                    exec( tpl_code , globals(), cfg_locals)

            cfg_locals['include'] = include

            cfg_code = compile( f.read(), 'mock_cfg', 'exec' )
            exec( cfg_code, globals(), cfg_locals )

        def expandMockCfgVars( key ):
            value = config_opts[ key ]
            value = value.replace( '{{ releasever }}', config_opts[ 'releasever' ] )
            assert '{{' not in value, 'Key %s: Found {{ in %r' % (key, value)
            return value

        # set to match the mock target
        self.opt_arch = config_opts[ 'target_arch' ]
        self.dist_tag = expandMockCfgVars( 'dist' )
        if self.dist_tag == 'centos6':
            self.dist_tag = 'el6'

        return config_opts

    def makeMockTargetFile( self ):
        self.MOCK_TARGET_FILENAME = 'tmp/%s-%s-%s.cfg' % (
                self.KITNAME, self.copr_repo, os.path.basename( self.opt_mock_target ))

        config_opts = self.readMockConfig()

        if 'yum.conf' in config_opts:
            conf_key = 'yum.conf'

        elif 'dnf.conf' in config_opts:
            conf_key = 'dnf.conf'

        else:
            assert False, 'config_opts missing yum.conf or dnf.conf section'

        config_opts['root'] = os.path.splitext( os.path.basename( self.MOCK_TARGET_FILENAME ) )[0]

        if self.MOCK_COPR_REPO_FILENAME:
            with open( self.MOCK_COPR_REPO_FILENAME, 'r' ) as f:
                repo = f.read()

                if self.opt_mock_target.startswith( 'epel-' ):
                    repo = repo.replace( '/fedora-$releasever-$basearch/', '/epel-$releasever-$basearch/' )

                config_opts[conf_key] += '\n'
                config_opts[conf_key] += repo

        if self.MOCK_COPR_REPO_PYQT6_FILENAME:
            with open( self.MOCK_COPR_REPO_PYQT6_FILENAME, 'r' ) as f:
                repo = f.read()

                if self.opt_mock_target.startswith( 'epel-' ):
                    repo = repo.replace( '/fedora-$releasever-$basearch/', '/epel-$releasever-$basearch/' )

                config_opts[conf_key] += '\n'
                config_opts[conf_key] += repo

        with open( self.MOCK_TARGET_FILENAME, 'w' ) as f:
            for k in config_opts:
                if k == conf_key:
                    print( '''config_opts['%s'] = """''' % (conf_key,), end='', file=f )
                    print( config_opts[conf_key], file=f )
                    print( '"""', file=f )

                else:
                    print( '''config_opts['%s'] = %r''' % (k, config_opts[k]), file=f )

        # prevent mock from rebuilding the mock cache on each build.
        # mock uses the timestamp on the CFG file and compares to the
        # cache timestamp. Use the timestamp of the input cfg to avoid
        # rebuilding the cache unless the original CFG file changes.
        if self.opt_mock_target.startswith( '/' ):
            mock_cfg = self.opt_mock_target + '.cfg'
        else:
            mock_cfg = '/etc/mock/%s.cfg' % (self.opt_mock_target,)

        st = os.stat( mock_cfg )
        os.utime( self.MOCK_TARGET_FILENAME, (st.st_atime, st.st_mtime) )

    def makeTarBall( self ):
        self.KIT_BASENAME = '%s-%s' % (self.KITNAME, self.version)

        log.info( 'Exporting source code' )

        p = run( ('git', 'branch', '--show-current'), output=True, cwd=os.environ['BUILDER_TOP_DIR'] )
        git_branch = p.stdout.strip()

        cmd = '(cd ${BUILDER_TOP_DIR}; git archive --format=tar --prefix=%s/ %s) | tar xf - -C tmp ' % (self.KIT_BASENAME, git_branch)
        run( cmd )

        p = run( ('git', 'show-ref', '--head', '--hash', 'head'), output=True, cwd=os.environ['BUILDER_TOP_DIR'] )
        self.commit_id = p.stdout.strip()

        with open( os.path.join( 'tmp', self.KIT_BASENAME, 'Builder/commit_id.txt' ), 'w' ) as f:
            f.write( self.commit_id )
            f.write( '\n' )

        log.info( 'Commit ID %s' % (self.commit_id,) )

        if self.os_release_info['ID'] == 'fedora':
            run( ('tar', 'czf', 'sources/%s.tar.gz' % (self.KIT_BASENAME,), self.KIT_BASENAME), cwd='tmp' )

        elif self.os_release_info['ID'] in ('ubuntu', 'debian'):
            run( ('tar', 'czf', '%s_%s.orig.tar.gz' % (self.KITNAME, self.version), self.KIT_BASENAME), cwd='tmp' )

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

        SRPM_BASENAME = '%s-%s.%s' % (self.KIT_BASENAME, self.opt_release, self.dist_tag)
        self.SRPM_FILENAME = 'tmp/%s.src.rpm' % (SRPM_BASENAME,)

        src = '%s/SRPMS/%s.src.rpm' % (self.MOCK_BUILD_DIR, SRPM_BASENAME)
        log.info( 'copy %s %s' % (src, self.SRPM_FILENAME) )
        build_utils.copyFile( src, self.SRPM_FILENAME, 0o600 )

if __name__ == '__main__':
    sys.exit( PackageBEmacs().main( sys.argv ) )
