#
# $Log$
# Revision 1.6  2003/03/28 16:47:50  fuhrmann
# pdelib1.15_alpha1
#
# Revision 1.5  2003/03/28 11:20:28  fuhrmann
# pdelib2.0_alpha1
#
# Revision 1.4  1998/04/27 13:22:12  fuhrmann
# Makefiles fixed, included into pdelib tree
#
# Revision 1.3  1996/07/09  09:52:21  fuhrmann
# *** empty log message ***
#
# Revision 1.2  1996/02/15  20:11:31  fuhrmann
# *** empty log message ***
#
#

#**********************************************************************
#*  This software is distributed in the hope that it will be useful,  *
#*  but WITHOUT ANY WARRANTY; without even the implied  warranty  of  *
#*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
#*                                                                    *
#**********************************************************************/
#DEFAULT:
GOPTION= -DX11 
GLIBS= -lX11

# SGI:
#GOPTION= -DX11 -DGL 
#GLIBS= -lgl_s

# SPARC:
GOPTION= -DX11 -DNOPROTO



LIBS= -lm $(GLIBS)
CFLAGS= -g $(GOPTION)
FFLAGS= -i4 -r8 

GMS_SOURCES=gmsdrv.h gmsstub.c gmsker.c gmsx11.c gmsps.c gmsgl.c gmsregis.c\
       gmshpgl.c gmssun.c gmstek.c gmsmsc.c gmsbgi.c



gms.c: $(GMS_SOURCES)
	cat $(GMS_SOURCES) > gms.c        	 


gms.o:gms.c gms.h

tgmsc:gms.o tgmsc.c 
	$(CC)  $(CFLAGS) $(GOPTION) $(LDFLAGS)\
          -o tgmsc tgmsc.c gms.o $(LIBS) 

tgmsf:gms.o tgmsf.f 
	$(FC)  $(FFLAGS)  $(LDFLAGS)\
           -o tgmsf tgmsf.f  gms.o $(LIBS)  
clean:
	-rm tgmsc tgmsf *.o *.ps








