Name:           bemacs
Version:        8.2.1
Release:        1%{?dist}
Summary:        Barry's Emacs

License:        ASL 2.0
URL:            http://barrys-emacs.org/
Source0:        http://barrys-emacs.org/source_kits/%{name}-%{version}.tar.gz

BuildRequires:  python3-devel >= 3.4
BuildRequires:  python3-qt5 >= 5.5.1
BuildRequires:  unicode-ucd >= 7.0
Requires:       python3 >= 3.4
Requires:       python3-qt5 >= 5.5.1

%description
Barry's Emacs
* Easy to get start Emacs
* Uses original MockLisp extension language
* UI implemented using Python3 and PyQt

%prep
# unpack Source0
echo Info: prep start
%setup
echo Info: prep done

%build
echo Info: build PWD $( pwd )
true

%install
echo Info: Install PWD $( pwd )

export BUILDER_TOP_DIR=$( pwd )
export PYTHON_VERSION=$( python3 -c 'import sys; print( "%d.%d" % (sys.version_info[0], sys.version_info[1]) )' )

cd ${BUILDER_TOP_DIR}/Builder
make -f linux.mak PYTHON=/usr/bin/python3 DESTDIR=%{buildroot} build_Linux

mkdir -p %{buildroot}%{_mandir}/man1
gzip -c ${BUILDER_TOP_DIR}/Kits/Linux/bemacs.1 > %{buildroot}%{_mandir}/man1/bemacs.1.gz

mkdir -p %{buildroot}/usr/share/applications
cp ${BUILDER_TOP_DIR}/Kits/Linux/bemacs.desktop %{buildroot}/usr/share/applications

%files
%defattr(-, root, root, -)
/usr/bin/bemacs
/usr/bin/bemacs_server
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

%changelog
* Sat Apr 30 2016 barry scott <barry@barrys-emacs.org> - 8.2.1-1
- First version
