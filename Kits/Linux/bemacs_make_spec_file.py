#!/usr/bin/env python
import sys

def main( argv ):
    target = argv[1]
    version = argv[2]
    specfile = argv[3]

    if target == 'cli':
        spec_targets = 'build-cli'
        python = '/usr/bin/python2'
        all_parts = (
            spec_file_head,
            spec_file_requires_common,
            spec_file_requires_cli,
            spec_file_build,
            spec_file_description,
            spec_file_package_cli,
            spec_file_files_cli,
            spec_file_files_common,
            spec_file_tail)

    elif target == 'gui':
        spec_targets = 'build-gui build-cli'
        python = '/usr/bin/python3'
        all_parts = (
            spec_file_head,
            spec_file_requires_common,
            spec_file_requires_cli,
            spec_file_requires_gui,
            spec_file_build,
            spec_file_build_gui_extra,
            spec_file_description,
            spec_file_package_gui,
            spec_file_package_cli,
            spec_file_files_gui,
            spec_file_files_cli,
            spec_file_files_common,
            spec_file_tail)

    else:
        raise ValueError( 'need a target' )

    fmt_spec_file = ''.join( all_parts )
    fmt_spec_file = fmt_spec_file.replace( '%', '%%' )
    fmt_spec_file = fmt_spec_file.replace( './.', '%' )
    spec_vars = {'TARGETS': spec_targets
                ,'VERSION': version
                ,'PYTHON':  python}
    spec_file = fmt_spec_file % spec_vars

    with open( specfile, 'w' ) as f:
        f.write( spec_file )

    return 0

# use ./. in place of %

spec_file_head = '''
Name:           bemacs
Version:        ./.(VERSION)s
Release:        1%{?dist}
Summary:        Barry's Emacs

License:        ASL 2.0
URL:            http://barrys-emacs.org/
Source0:        http://barrys-emacs.org/source_kits/%{name}-%{version}.tar.gz
'''

spec_file_requires_common = '''
BuildRequires:  unicode-ucd >= 7.0
'''

spec_file_requires_cli = '''
Requires:       bemacs-cli
'''

spec_file_requires_gui = '''
Requires:       bemacs-gui

BuildRequires:  unicode-ucd >= 7.0
BuildRequires:  python3-devel >= 3.4
BuildRequires:  python3-qt5 >= 5.5.1
BuildRequires:  gcc-c++
BuildRequires:  python3-pycxx-devel >= 7.1.2
'''

spec_file_build = '''
%prep
# unpack Source0
echo Info: prep start
%setup
echo Info: prep done

%build
true

%install
echo Info: Install PWD $( pwd )

export BUILDER_TOP_DIR=$( pwd )

cd ${BUILDER_TOP_DIR}/Builder
# tell Editor/build-linux.sh to use Unicode UCD and PyCXX from system
export SETUP_OPTIONS="--system-ucd --system-pycxx"
make -f linux.mak PYTHON=./.(PYTHON)s DESTDIR=%{buildroot} ./.(TARGETS)s

mkdir -p %{buildroot}%{_mandir}/man1
gzip -c ${BUILDER_TOP_DIR}/Kits/Linux/bemacs.1 > %{buildroot}%{_mandir}/man1/bemacs.1.gz

mkdir -p %{buildroot}/usr/share/applications
cp ${BUILDER_TOP_DIR}/Kits/Linux/bemacs.desktop %{buildroot}/usr/share/applications
'''

spec_file_build_gui_extra = '''
'''

spec_file_description = '''
%description
Barry's Emacs
* Easy to get started with Emacs
* Uses original MockLisp extension language
* UI implemented using Python3 and PyQt

'''

spec_file_package_gui = '''
%package gui
Summary: Barry's Emacs GUI version
Group: Applications/Editors
Requires: bemacs-common
Requires: python3 >= 3.4
Requires: python3-qt5 >= 5.5.1
Requires: python3-xml-preferences

%description gui
Barry's Emacs
* Easy to get started with Emacs
* Uses original MockLisp extension language
* UI implemented using Python3 and PyQt

'''

spec_file_package_cli = '''
%package cli
Summary: Barry's Emacs CLI version
Group: Applications/Editors
Requires: bemacs-common
Requires: python3 >= 3.4

BuildRequires:  unicode-ucd >= 7.0

%description cli
Barry's Emacs
* Easy to get start with Emacs
* Uses original MockLisp extension language
* Command line version for terminal

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
'''

spec_file_files_cli = '''
%files cli
%defattr(-, root, root, -)
/usr/bin/bemacs-cli
'''

spec_file_files_common = '''
%files common
%defattr(-, root, root, -)
/usr/bin/bemacs-dbadd
/usr/bin/bemacs-dbcreate
/usr/bin/bemacs-dbdel
/usr/bin/bemacs-dblist
/usr/bin/bemacs-dbprint
/usr/bin/bemacs-mll2db
/usr/share/bemacs/doc/*
/usr/share/bemacs/lib/*
/usr/share/applications/bemacs.desktop
%attr(0644,root,root) %{_mandir}/man1/bemacs.1.gz
'''

spec_file_tail = '''
%changelog
* Mon Apr 15 2019 Barry Scott <barry@barrys-emacs.org> - 8.5.3-1
- prep for copr release
* Sat Apr 30 2016 barry scott <barry@barrys-emacs.org> - 8.2.1-1
- First version
'''

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
