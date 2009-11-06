%define sp_rel 1
%define sp_ver 0.95.3
Summary: Sinclair ZX Spectrum emulator
Name: spectemu
Version: %{sp_ver}
Release: %{sp_rel}
Copyright: GPL
Group: Applications/Emulators
Source: http://www.inf.bme.hu/~mszeredi/spectemu/spectemu-%{sp_ver}.tar.gz
URL: http://www.inf.bme.hu/~mszeredi/spectemu

%description
This package contains a 48k ZX-Spectrum emulator for Linux, with full
Z80 instruction set, comprehensive screen, sound and tape emulation,
and snapshot file saving and loading.

%prep
%setup

%build
./configure --prefix=/usr
make

%install
rm -rf "$RPM_BUILD_ROOT"
make install_root="$RPM_BUILD_ROOT" install

%clean
rm -rf "$RPM_BUILD_ROOT"

%files
%doc README COPYING ChangeLog TODO spectemu.lsm
/usr/bin/vgaspect
/usr/bin/xspect
/usr/man/man1/xspect.1
/usr/man/man1/vgaspect.1

