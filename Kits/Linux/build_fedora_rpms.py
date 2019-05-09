#!/usr/bin/python3
#
#   build_fedora_rpms.py
#
#   cmd - srpm, mock, copr-release, copr-testing
#   --release=auto|<rel>
#   --target=<fedora-version>
#   --arch=<arch>
#   --install
#
import sys
import os
import shutil
import subprocess
import platform

valid_cmds = ('srpm', 'mock', 'copr-release', 'copr-testing')

def info( msg ):
    print( '\033[32mInfo:\033[m %s' % (msg,) )

def error( msg ):
    print( '\033[31;1mError: %s\033[m' % (msg,) )

class BuildError(Exception):
    def __init__( self, msg ):
        super().__init__( msg )

class BuildFedoraRpms:
    def __init__( self ):
        self.KITNAME = 'bemacs'
        self.cmd = None
        self.release = 1
        self.target = self.fedoraVersion()
        self.arch = platform.machine()
        self.version = None
        self.install = False

    def main( self, argv ):
        try:
            self.parseArgs( argv )
            self.setupVars()

            if self.cmd == 'srpm':
                self.buildSrpm()

            elif self.cmd == 'mock':
                self.buildMock()

            elif self.cmd == 'copr-release':
                self.buildCoprRelease()

            elif self.cmd == 'copr-testing':
                self.buildCoprTesting()

            return 0

        except BuildError as e:
            error( str(e) )
            return 1

    def parseArgs( self, argv ):
        try:
            args = iter( argv )
            next(args)
            self.cmd = next(args)
            if self.cmd not in valid_cmds:
                raise BuildError( 'Invalid cmd %r - pick on of %s' % (cmd, ', '.join( valid_cmds )) )

            while True:
                arg = next(args)
                if arg.startswith('--release='):
                    self.release = arg[len('--release='):]

                elif arg.startswith('--target='):
                    self.target = arg[len('--target='):]

                elif arg.startswith('--arch='):
                    self.arch = arg[len('--arch='):]

                elif arg.startswith('--version='):
                    self.version = arg[len('--version='):]

                elif arg.startswith('--install'):
                    self.install = True

                else:
                    raise BuildError( 'Unknown option %r' % (arg,) )

        except StopIteration:
            pass

        if self.cmd is None:
            raise BuildError( 'Require a cmd arg' )

        if self.version is None:
            raise BuildError( '--version must be provided' )

    def setupVars( self ):
        self.MOCK_ORIG_VERSION_NAME = 'fedora-%d-%s' % (self.target, self.arch)
        self.MOCK_COPR_REPO_FILE = '/etc/yum.repos.d/_copr:copr.fedorainfracloud.org:barryascott:tools.repo'
        self.MOCK_VERSION_NAME= 'tmp/bemacs-%s.cfg' % (self.MOCK_ORIG_VERSION_NAME,)

    def fedoraVersion( self ):
        with open( '/etc/os-release', 'r' ) as f:
            for line in f:
                if line.startswith( 'VERSION_ID=' ):
                    return int( line.strip()[len('VERSION_ID='):] )

        raise BuildError( 'Expected /etc/os-release to have a VERSION_ID= field' )

    def buildSrpm( self ):
        self.makeTarBall()
        self.ensureMockSetup()
        self.makeSrpm()
        info( 'SRPM is %s' % (self.SRPM_FILENAME,) )

    def buildMock( self ):
        self.buildSrpm()

        info( 'Creating RPM' )
        self.run( ('mock',
                    '--root=%s' % (self.MOCK_VERSION_NAME,),
                    '--enablerepo=barryascott-tools',
                    '--rebuild', '--dnf',
                    self.SRPM_FILENAME) )

        ALL_BIN_KITNAMES=[
            self.KITNAME,
            '%s-cli' % (self.KITNAME,),
            '%s-gui' % (self.KITNAME,),
            '%s-common' % (self.KITNAME,),
            '%s-debuginfo' % (self.KITNAME,),
            ]

        for BIN_KITNAME in ALL_BIN_KITNAMES:
            basename = '%s-%s-%s.%s.%s.rpm' % (BIN_KITNAME, self.version, self.release, self.DISTRO, self.arch)
            info( 'Copying %s' % (basename,) )
            shutil.copyfile( '%s/RPMS/%s' % (self.MOCK_BUILD_DIR, basename), 'tmp/%s' % (basename,) )

        info( 'Results in %s/tmp:' % (os.getcwd(),) )
        for filename in os.listdir( 'tmp' ):
            info( 'Kit %s' % (filename,) )

        if self.install:
            info( 'Installing RPMs' )

            for BIN_KITNAME in ALL_BIN_KITNAMES:
                cmd = ('rpm', '-q', BIN_KITNAME)
                p = self.run( cmd, check=False )
                if p.returncode == 0:
                    self.run( ('sudo', 'dnf', '-y', 'remove', BIN_KITNAME) )

            cmd = ['sudo', 'dnf', '-y', 'install']
            cmd.extend( glob.glob( 'tmp/%s*.%s.rpm' % (self.KITNAME, self.arch) ) )
            self.run( cmd )

    def buildCoprRelease( self ):
        self.buildCopr( 'tools' )

    def buildCoprTesting( self ):
        self.buildCopr( 'tools-testing' )

    def buildCorp( self, copr_repo ):
        self.buildSrpm()
        self.run( ('copr-cli', 'build', '-r', 'fedora-%s-%s' % (self.target, self.arch), copr_repo, self.SRPM_FILENAME) )

    def ensureMockSetup( self ):
        info( 'creating mock target file' )
        import bemacs_make_mock_cfg
        bemacs_make_mock_cfg.main( ['',
            '/etc/mock/%s.cfg' % (self.MOCK_ORIG_VERSION_NAME,),
            self.MOCK_COPR_REPO_FILE,
            self.MOCK_VERSION_NAME] )

        p = self.run( ('mock', '--root=%s' % (self.MOCK_VERSION_NAME,), '--print-root-path') )

        self.MOCK_ROOT = p.stdout.strip()
        self.MOCK_BUILD_DIR = '%s/builddir/build' % (self.MOCK_ROOT,)

        if os.path.exists( self.MOCK_ROOT ):
            info( 'Using existing mock for %s' % (self.MOCK_ORIG_VERSION_NAME,) )

        else:
            info( 'Init mock for %s' % (self.MOCK_ORIG_VERSION_NAME,) )
            self.run( ('mock', '--root=%s' % (self.MOCK_ORIG_VERSION_NAME,), '--init') )

    def makeTarBall( self ):
        self.KIT_BASENAME = '%s-%s' % (self.KITNAME, self.version)

        info( 'Exporting source code' )

        cmd = '(cd ${BUILDER_TOP_DIR}; git archive --format=tar --prefix=%s/ master) | tar xf - -C tmp ' % (self.KIT_BASENAME,)
        self.run( cmd )

        p = self.run( ('git', 'show-ref', '--head', '--hash', 'head'), output=True, cwd=os.environ['BUILDER_TOP_DIR'] )

        with open( os.path.join( 'tmp', self.KIT_BASENAME, 'Builder/commit_id.txt' ), 'w' ) as f:
            f.write( p.stdout )

        self.run( ('tar', 'czf', '%s.tar.gz' % (self.KIT_BASENAME,), self.KIT_BASENAME), cwd='tmp' )

    def makeSrpm( self ):
        info( 'creating bemacs.spec' )
        import bemacs_make_spec_file
        bemacs_make_spec_file.main( ['', 'gui', self.version, self.release, 'tmp/bemacs.spec'] )

        info( 'Creating SRPM for %s' % (self.KIT_BASENAME,) )

        self.run( ('mock',
                '--root=%s' % (self.MOCK_VERSION_NAME,),
                '--buildsrpm', '--dnf',
                '--spec', 'tmp/bemacs.spec',
                '--sources', 'tmp/%s.tar.gz' % (self.KIT_BASENAME,)) )

        self.DISTRO = 'fc%d' % (self.target,)
        SRPM_BASENAME = '%s-%s.%s' % (self.KIT_BASENAME, self.release, self.DISTRO)
        self.SRPM_FILENAME = 'tmp/%s.src.rpm' % (SRPM_BASENAME,)

        src = '%s/SRPMS/%s.src.rpm' % (self.MOCK_BUILD_DIR, SRPM_BASENAME)
        info( 'copy %s %s' % (src, self.SRPM_FILENAME) )
        shutil.copyfile( src, self.SRPM_FILENAME )

    def run( self, cmd, check=True, output=True, cwd=None ):
        kw = {}
        if type(cmd) is str:
            info( 'Running %s' % (cmd,) )
            kw['shell'] = True
        else:
            info( 'Running %s' % (' '.join( cmd ),) )

        if output:
            kw['capture_output'] = True
            kw['encoding'] = 'utf-8'

        if cwd:
            kw['cwd'] = cwd

        p = subprocess.run( cmd, **kw )
        if check and p.returncode != 0:
            raise BuildError( 'Cmd failed %r' % (cmd,) )

        return p

if __name__ == '__main__':
    sys.exit( BuildFedoraRpms().main( sys.argv ) )
