## automake.am
##
## Top level automake.am
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##

SUBDIRS = conf\
          icons\
          src\
          xdg

rpm:	dist
	cp glassmonitor-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux glassmonitor.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/glassmonitor-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/glassmonitor-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/glassmonitor-$(VERSION).tar.gz
	rm -rf $(HOME)/rpmbuild/BUILD/glassmonitor-$(VERSION)

EXTRA_DIST = glassmonitor.spec.in\
             autogen.sh\
             get_distro.sh\
             get_target.sh\
             PACKAGE_VERSION

CLEANFILES = *~\
             *.deb\
             *.mo\
             *.rpm\
             *.tar.gz\
             *.tgz\
             *.zip

DISTCLEANFILES = glassmonitor.spec\
                 config.guess\
                 config.sub\
                 ltmain.sh

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       config.guess\
                       config.status\
                       config.sub\
                       configure\
                       depcomp\
                       install-sh\
                       libtool\
                       ltmain.sh\
                       Makefile.in\
                       missing\
                       mkinstalldirs
