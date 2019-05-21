#!/usr/bin/env python
import sys

def main( argv ):
    target = argv[1]
    version = argv[2]
    revision = argv[3]
    specfile = argv[4]

    if target == 'cli':
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
    spec_vars = {'TARGET': target
                ,'VERSION': version
                ,'REVISION': revision
                ,'PYTHON':  python}
    spec_file = fmt_spec_file % spec_vars

    with open( specfile, 'w' ) as f:
        f.write( spec_file )

    return 0

# use ./. in place of %

spec_file_head = '''
Name:           bemacs
Version:        ./.(VERSION)s
Release:        ./.(REVISION)s%{?dist}
Summary:        Barry's Emacs

License:        ASL 2.0
URL:            http://barrys-emacs.org/
Source0:        http://barrys-emacs.org/source_kits/%{name}-%{version}.tar.gz
'''

spec_file_requires_common = '''
BuildRequires:  unicode-ucd
'''

spec_file_requires_cli = '''
Requires:       bemacs-cli
'''

spec_file_requires_gui = '''
Requires:       bemacs-gui
Requires:       python3 >= 3.4
Requires:       sqlite

BuildRequires:  python3-devel >= 3.4
BuildRequires:  python3-qt5 >= 5.5.1
BuildRequires:  gcc-c++
BuildRequires:  python3-pycxx-devel >= 7.1.2
BuildRequires:  sqlite-devel
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
export PYTHON=./.(PYTHON)s

cd ${BUILDER_TOP_DIR}/Builder
export DESTDIR=%{buildroot}
%if 0%{?centos_ver} == 6
# Centos 6 has old unicode-ucd and no pycxx
./.(PYTHON)s build_bemacs.py ./.(TARGET)s
%else
# tell Editor/build-linux.sh to use Unicode UCD and PyCXX from system
./.(PYTHON)s build_bemacs.py ./.(TARGET)s --system-ucd --system-pycxx
find %{buildroot}/usr -ls
%endif

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

BuildRequires: python3-xml-preferences

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
/usr/share/bemacs/doc/*
/usr/share/bemacs/lib/*
/usr/share/applications/bemacs.desktop
%attr(0644,root,root) %{_mandir}/man1/bemacs.1.gz
'''

spec_file_tail = '''
%changelog
* Mon Apr 15 2019 Barry Scott <barry@barrys-emacs.org> - 8.5.3-1
- prep for copr release
'''

if __name__ == '__main__':
    sys.exit( main( sys.argv ) )
