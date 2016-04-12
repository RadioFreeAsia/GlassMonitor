## automake.am
##
## Makefile for the glassmonitor(1) Stream Monitor
##
## (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing @QT4_CFLAGS@
MOC = @QT4_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	@MOC@ $< -o $@


bin_PROGRAMS = glassmonitor

dist_glassmonitor_SOURCES = cmdswitch.cpp cmdswitch.h\
                            config.cpp config.h\
                            glassmonitor.cpp glassmonitor.h\
                            jack.cpp\
                            monitor.cpp monitor.h\
                            playmeter.cpp playmeter.h\
                            profile.cpp profile.h\
                            segmeter.cpp segmeter.h

nodist_glassmonitor_SOURCES = moc_glassmonitor.cpp\
                              moc_monitor.cpp\
                              moc_playmeter.cpp\
                              moc_segmeter.cpp

glassmonitor_LDADD = @GUILIBS@ @LIBJACK@
glassmonitor_LDFLAGS = @GUIFLAGS@

CLEANFILES = *~\
             moc_*\
             *.obj\
             *.idb\
             *.pdb\
             *ilk

MAINTAINERCLEANFILES = *~\
                       Makefile.in
