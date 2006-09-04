#
# $Id$
#

SHELL=/bin/shSHELL=  /bin/sh
VPATH=../include
INCDIR=../include
BINDIR=../bin/$(config)
LIBDIR=../lib/$(config)
GMS_OPTIONS=-DUSE_OS_MESA

LIB=$(LIBDIR)/libgms.a
include ../config/modules.mk
include ../config/$(config).mk

CFLAGS=$(cflags) $(GMS_OPTIONS)
GMS_SOURCES=gmsdrv.h gmsstub.c gmsker.c gmsx11.c gmsps.c gmsgl.c gmsregis.c\
       gmshpgl.c gmssun.c gmstek.c gmsmsc.c gmsbgi.c


tgmsc: tgmsc.c gms.c
tgmsf: tgmsf.f $(OBJECTS)



gms.c: $(GMS_SOURCES)
	cat $(GMS_SOURCES) > gms.c        	 

tfont: tfont.c 


OBJECTS=$(LIB)(gmsker.o) $(LIB)(gmsx11.o) $(LIB)(gmsps.o) $(LIB)(gmsgl.o) $(LIB)(gmsregis.o) $(LIB)(gmshpgl.o) $(LIB)(gmssun.o) $(LIB)(gmstek.o) $(LIB)(gmsmsc.o) $(LIB)(gmsbgi.o) 
$(LIB)(gmsker.o): gmsker.c $(HEADERS)
$(LIB)(gmsx11.o): gmsx11.c $(HEADERS)
$(LIB)(gmsps.o): gmsps.c $(HEADERS)
$(LIB)(gmsgl.o): gmsgl.c $(HEADERS)
$(LIB)(gmsregis.o): gmsregis.c $(HEADERS)
$(LIB)(gmshpgl.o): gmshpgl.c $(HEADERS)
$(LIB)(gmssun.o): gmssun.c $(HEADERS)
$(LIB)(gmstek.o): gmstek.c $(HEADERS)
$(LIB)(gmsmsc.o): gmsmsc.c $(HEADERS)
$(LIB)(gmsbgi.o): gmsbgi.c $(HEADERS)

lib: $(OBJECTS)
	-$(AR) rv $(LIB) *.o
	-rm -f *.o
#
# $Log$
# Revision 1.5  2003/03/28 16:47:50  fuhrmann
# pdelib1.15_alpha1
#
# Revision 1.4  2003/03/28 11:20:28  fuhrmann
# pdelib2.0_alpha1
#
# Revision 1.3  1997/04/15 08:08:34  fuhrmann
# *** empty log message ***
#
# Revision 1.2  1997/02/28  15:05:52  fuhrmann
# vpath
#
# Revision 1.1  1996/10/16  15:51:07  darcy
# forget revision numbers less than 1.1
#
# Revision 0.1  1996/07/17  18:25:20  fuhrmann
# GMS_OPTIONS parameter
#
# Revision 0.0  1996/07/10  13:08:16  fuhrmann
# *** empty log message ***
#
#
#
