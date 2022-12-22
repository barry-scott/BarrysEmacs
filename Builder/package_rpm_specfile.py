#!/usr/bin/env python
import sys
import os
import time

def if_opt( opt, yes, no ):
    if opt:
        return yes
    else:
        return no

def createRpmSpecFile( opt, spec_filename ):
    all_requires_base = set()
    all_requires_gui = set()
    all_requires_cli = set()
    all_build_requires = set()
    all_config_options = set()

    all_sources = []

    kit_pycxx_basename = ''
    kit_sqlite_basename = ''
    kit_xml_preferences_basename = ''

    # options for cli
    all_requires_base.add( 'bemacs-cli = %{version}-%{release}' )
    all_requires_cli.add( 'bemacs-common = %{version}-%{release}' )
    all_build_requires.add( 'gcc-c++' )

    if opt.opt_system_ucd:
        all_build_requires.add( 'unicode-ucd' )
        all_config_options.add( '--system-ucd' )

    if opt.opt_hunspell and opt.opt_system_hunspell:
        all_requires_cli.add( 'hunspell' )
        all_requires_gui.add( 'hunspell' )
        all_build_requires.add( 'hunspell' )
        all_build_requires.add( 'hunspell-devel' )
        all_config_options.add( '--system-hunspell' )
    else:
        all_config_options.add( '--no-hunspell' )

    if opt.opt_sqlite:
        if opt.opt_kit_sqlite is None:
            all_requires_cli.add( 'sqlite' )
            all_requires_gui.add( 'sqlite' )
            all_build_requires.add( 'sqlite-devel' )
            all_config_options.add( '--system-sqlite' )

        else:
            all_sources.append( opt.opt_kit_sqlite )
            kit_sqlite_basename = os.path.basename( opt.opt_kit_sqlite )
            all_build_requires.add( 'unzip' )

    else:
        all_config_options.add( '--no-sqlite' )

    if opt.opt_sftp:
        all_requires_cli.add( 'libssh' )
        all_requires_gui.add( 'libssh' )
        all_build_requires.add( 'libssh-devel' )

    else:
        all_build_requires.add( '--no-sftp' )

    if not opt.opt_warnings_as_errors:
        all_config_options.add( '--no-warnings-as-errors' )

    if opt.opt_gui:
        # options for gui
        all_requires_gui.add( 'mozilla-fira-mono-fonts' )
        if opt.opt_mock_target.startswith( 'epel-7-' ):
            # centos 7 uses python36 not python3
            python = '/usr/bin/python3'
            all_requires_gui.add( 'bemacs-common = %{version}-%{release}' )
            all_requires_gui.add( 'python36' )
            all_requires_gui.add( 'python36-qt5' )

            all_requires_base.add( 'bemacs-gui = %{version}-%{release}' )
            all_requires_base.add( 'python36' )

            all_build_requires.add( 'python36' )
            all_build_requires.add( 'python36-devel' )
            all_build_requires.add( 'python36-qt5' )

        else:
            python = '/usr/bin/python3'
            all_requires_gui.add( 'bemacs-common = %{version}-%{release}' )
            all_requires_gui.add( 'python3 >= 3.4' )
            all_requires_gui.add( 'python3-qt6' )

            all_requires_base.add( 'bemacs-gui = %{version}-%{release}' )
            all_requires_base.add( 'python3' )

            all_build_requires.add( 'python3 >= 3.4' )
            all_build_requires.add( 'python3-devel >= 3.4' )
            all_build_requires.add( 'python3-qt6' )

        if opt.opt_kit_xml_preferences is None:
            all_requires_gui.add( 'python3-xml-preferences' )
            all_build_requires.add( 'python3-xml-preferences' )

        else:
            kit_xml_preferences_basename = os.path.basename( opt.opt_kit_xml_preferences )
            all_sources.append( opt.opt_kit_xml_preferences )

        if opt.opt_kit_pycxx is None:
            if opt.opt_mock_target.startswith( 'epel-7-' ):
                # use the systems pycxx
                all_build_requires.add( 'python-pycxx-devel >= 7.1.3' )
            else:
                # use the systems pycxx
                all_build_requires.add( 'python3-pycxx-devel >= 7.1.3' )

            all_config_options.add( '--system-pycxx' )

        else:
            all_sources.append( opt.opt_kit_pycxx )
            kit_pycxx_basename = os.path.basename( opt.opt_kit_pycxx )

    else:
        all_build_requires.add( 'python' )
        python = '/usr/bin/python2'

    fmt_spec_file = ''.join( (
                        spec_file_head,
                        spec_file_prep,
                        if_opt( kit_sqlite_basename != '', spec_file_prep_sqlite, '' ),
                        if_opt( kit_pycxx_basename != '', spec_file_prep_pycxx, '' ),
                        if_opt( kit_xml_preferences_basename != '', spec_file_prep_xml_preferences, '' ),
                        spec_file_build,
                        spec_file_install,
                        spec_file_description,
                        if_opt( opt.opt_gui, spec_file_package_gui, '' ),
                        spec_file_package_cli,
                        if_opt( opt.opt_gui, spec_file_files_gui, '' ),
                        spec_file_files_cli,
                        spec_file_files_common,
                        if_opt( opt.opt_sqlite, '', spec_file_files_no_sqlite ),
                        spec_file_tail) )

    fmt_spec_file = fmt_spec_file.replace( '%', '%%' )
    fmt_spec_file = fmt_spec_file.replace( './.', '%' )
    spec_vars = {'TARGET':              if_opt( opt.opt_gui, 'gui', 'cli' )
                ,'VERSION':             opt.version
                ,'RELEASE':             opt.opt_release
                ,'COMMIT_ID':           opt.commit_id
                ,'DATE':                time.strftime( '%a %b %d %Y' )
                ,'PYTHON':              python
                ,'SOURCES':             '\n'.join( 'Source%d: %s' % (index, os.path.basename( src )) for index, src in enumerate( all_sources, 1 ) )
                ,'REQUIRES_BASE':       '\n'.join( 'Requires: %s' % (req,) for req in sorted( all_requires_base ) )
                ,'REQUIRES_CLI':        '\n'.join( 'Requires: %s' % (req,) for req in sorted( all_requires_cli ) )
                ,'REQUIRES_GUI':        '\n'.join( 'Requires: %s' % (req,) for req in sorted( all_requires_gui ) )
                ,'BUILD_REQUIRES':      '\n'.join( 'BuildRequires: %s' % (req,) for req in sorted( all_build_requires ) )
                ,'KIT_PYCXX':           kit_pycxx_basename
                ,'KIT_SQLITE':          kit_sqlite_basename
                ,'KIT_XML_PREFERENCES': kit_xml_preferences_basename
                ,'CONFIG_OPTIONS':      ' '.join( sorted( all_config_options ) )
                }
    spec_file = fmt_spec_file % spec_vars

    with open( spec_filename, 'w' ) as f:
        f.write( spec_file )

    return 0

# use ./. in place of %

spec_file_head = '''
Name:           bemacs
Version:        ./.(VERSION)s
Release:        ./.(RELEASE)s%{?dist}
Summary:        Barry's Emacs

License:        ASL 2.0
URL:            http://barrys-emacs.org/
Source0:        http://barrys-emacs.org/source_kits/%{name}-%{version}.tar.gz
./.(SOURCES)s

./.(REQUIRES_BASE)s

./.(BUILD_REQUIRES)s
'''

spec_file_prep = '''
%prep
# unpack Source0
%setup
'''

spec_file_prep_sqlite = '''
mkdir -p Imports/sqlite
unzip -j "%_sourcedir/./.(KIT_SQLITE)s" -d Imports/sqlite
'''

spec_file_prep_pycxx = '''
gunzip -c "%_sourcedir/./.(KIT_PYCXX)s" | tar xf - -C Imports
'''

spec_file_prep_xml_preferences = '''
gunzip -c "%_sourcedir/./.(KIT_XML_PREFERENCES)s" | tar xf - -C Imports
# make build_bemacs code happy
ln -s $PWD/Imports/xml-preferences-*/Source/xml_preferences Builder
# make PyQtBEmacs code happy
ln -s $PWD/Imports/xml-preferences-*/Source/xml_preferences Editor/PyQtBEmacs
'''

spec_file_build = '''
%build
true
'''

spec_file_install = '''
%install
echo Info: Install PWD $( pwd )

export BUILDER_TOP_DIR=$( pwd )
export PYTHON=./.(PYTHON)s

cd ${BUILDER_TOP_DIR}/Builder
export DESTDIR=%{buildroot}
./.(PYTHON)s build_bemacs.py ./.(TARGET)s ./.(CONFIG_OPTIONS)s

# creating the debug info RPM uses objcopy that needs the files to be writeable.
if [ -e %{buildroot}/usr/bin/bemacs_server ]
then
    chmod +w %{buildroot}/usr/bin/bemacs
    chmod +w %{buildroot}/usr/bin/bemacs_server
    chmod +w %{buildroot}/usr/lib/bemacs/_bemacs.so
fi
chmod +w %{buildroot}/usr/bin/bemacs-cli

mkdir -p %{buildroot}%{_mandir}/man1
gzip -c ${BUILDER_TOP_DIR}/Kits/Linux/bemacs.1 > %{buildroot}%{_mandir}/man1/bemacs.1.gz

mkdir -p %{buildroot}/usr/share/bemacs
cp ${BUILDER_TOP_DIR}/Editor/PyQtBEmacs/org.barrys-emacs.editor.png %{buildroot}/usr/share/bemacs/org.barrys-emacs.editor.png
mkdir -p %{buildroot}/usr/share/applications
cp ${BUILDER_TOP_DIR}/Kits/Linux/org.barrys-emacs.editor.desktop %{buildroot}/usr/share/applications/org.barrys-emacs.editor.desktop

'''

spec_file_description = '''
%description
Barry's Emacs
* Easy to get started with Emacs
* Uses original MockLisp extension language
* UI implemented using Python3 and PyQt

Built from ./.(COMMIT_ID)s
'''

spec_file_package_gui = '''
%package gui
Summary: Barry's Emacs GUI version
Group: Applications/Editors
./.(REQUIRES_GUI)s

%description gui
Barry's Emacs
* Easy to get started with Emacs
* Uses original MockLisp extension language
* UI implemented using Python3 and PyQt

Built from ./.(COMMIT_ID)s
'''

spec_file_package_cli = '''
%package cli
Summary: Barry's Emacs CLI version
Group: Applications/Editors
./.(REQUIRES_CLI)s

%description cli
Barry's Emacs
* Easy to get start with Emacs
* Uses original MockLisp extension language
* Command line version for terminal

Built from ./.(COMMIT_ID)s

%package common
Summary: Barry's Emacs common files
Group: Applications/Editors

%description common
Barry's Emacs common files used by
bemacs-gui and bemacs-cli

%files
'''

spec_file_files_gui = '''
%files gui
%defattr(-, root, root, -)
/usr/bin/bemacs
/usr/bin/bemacs_server
/usr/lib/bemacs/*.py
/usr/lib/bemacs/_bemacs.so
/usr/lib/bemacs/__pycache__/*
/usr/share/bemacs/org.barrys-emacs.editor.png
/usr/share/applications/org.barrys-emacs.editor.desktop
'''

spec_file_files_cli = '''
%files cli
%defattr(-, root, root, -)
/usr/bin/bemacs-cli
'''

spec_file_files_common = '''
%files common
%defattr(-, root, root, -)
/usr/share/bemacs/doc/*
/usr/lib/bemacs/*.db
/usr/lib/bemacs/*.ml
%attr(0644,root,root) %{_mandir}/man1/bemacs.1.gz
'''

spec_file_files_no_sqlite = '''
/usr/bin/bemacs-dbadd
/usr/bin/bemacs-dbcreate
/usr/bin/bemacs-dbdel
/usr/bin/bemacs-dblist
/usr/bin/bemacs-dbprint
/usr/bin/bemacs-mll2db
'''

spec_file_tail = '''
%changelog
* ./.(DATE)s Barry Scott <barry@barrys-emacs.org> - ./.(VERSION)s-./.(RELEASE)s
- Specfile create by package_bemacs.py
'''
