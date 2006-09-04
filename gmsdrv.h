/**
   \file gmsdrv.h
   \brief internal driver interface for gms
   \author Juergen Fuhrmann
 */

#ifndef GMSDRV
#define GMSDRV
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "gms.h"

#ifndef PARM
#ifdef NOPROTO
#define PARM(list) ()
#else
#define PARM(list) list
#endif
#endif

/* PREPROCESSOR DEFINES */
#ifndef MAX
#define MAX(x,y) ((x)>(y) ? (x):(y))
#endif

#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x):(y))
#endif

#define EQ(x,y) (fabs(x-y)<(1.0e-10*(fabs(x)+fabs(y))))  /* Not for time critical use!!*/

#define NEW(type) ((type *) malloc(sizeof(type)))
#define MAX_COLORSCALE_SIZE 48

/* workstation control commands */
#define GMS_CTRL_OPEN       1
#define GMS_CTRL_CLOSE      2
#define GMS_CTRL_BEGIN      3
#define GMS_CTRL_FLUSH      4
#define GMS_CTRL_END        5
#define GMS_CTRL_CLEAR      6


/* workstation control return codes */
#define GMS_RETURN_OK  1
#define GMS_RETURN_ERROR 0



typedef struct wsdsc_ {/* All coordinates here are given in workstation coordinates */ 
                       double  xpos,ypos,      /* actual position of drawing point*/
                              xmax,ymax,      /* maximum size*/
                              ydir;           /* direction of y-axis*/
                       double asp;            /* aspect ratio */
                       double markersize;     /* size of markers */
                       double textsize;       /* size of text for gms fonts */
                       FILE   *output;        /* output file if needed */
                       double  vxmin,vxmax,    /* viewport */
                              vymin,vymax;

                       double ax,bx,ay,by;    /* 2D transformation data*/
                       double Matrix3d[4][4];
                       int maxcolors,icol;    /* color number and act. color in table mode*/ 
                       int state;             /* workstation state*/ 
                       char connid[64];       /* workstation atttribute string */
                       int mode;              /* mode if used*/
                       int wstype;            /* type */
                       void (*moveto)PARM((struct wsdsc_*,double,double));
                       void (*lineto)PARM((struct wsdsc_*,double,double));
                       void (*marker)PARM((struct wsdsc_*,int ));
                       void (*fill)PARM((struct wsdsc_*,int, double *, double * ));
                       void (*text)PARM((struct wsdsc_*,char * , int, int));
                       void (*setcolor)PARM((struct wsdsc_*,int ));
                       void (*defcolor)PARM((struct wsdsc_*,double,double,double));
                       void (*shaded_triangle2d)PARM((struct wsdsc_*,double *, double *,double * ));
                       void (*isoline_triangle2d)PARM((struct wsdsc_*,double *, double *,double * ));
		       void (*initran2d)PARM((struct wsdsc_*,double,double,double,double,
					                     double,double,double,double));
		       void (*tran2d)PARM((struct wsdsc_*,double*,double*,double,double));
                       void (*initran3d)PARM((struct wsdsc_*,double,double,
                                                             double,double,
					                     double,double,double,double));
		       void (*tran3d)PARM((struct wsdsc_*,double*,double*,double *, double,double,double));
                       void (*moveto3d)PARM((struct wsdsc_*,double,double,double));
                       void (*lineto3d)PARM((struct wsdsc_*,double,double,double));
                       void (*triangle3d)PARM((struct wsdsc_*,double *, double *,double * ));
#ifdef HIDDENLINE
                       void (*hiddenline_triangle3d)PARM((struct wsdsc_*,double *, double *,double * ));
#endif
                       void (*shaded_triangle3d)PARM((struct wsdsc_*,double *, double *,double *,double * ));
                       
                       void (*querypoint) PARM((struct wsdsc_ *, double *, double *));
                       void (*querypair) PARM((struct wsdsc_ *, int,double *, double *,double*,double*));
                     

                       int  (*ctrl)PARM((struct wsdsc_*,int ));
                       double charwidth; 
                       double colorscale_v[MAX_COLORSCALE_SIZE];
                       int colorscale_size;  
                       void *info;   
                       struct wsdsc_ *next_open;

                       FILE * new_output;
                       int demand_newoutput;
                   } wsdsc;

 void  gms_dummy_draw      PARM((struct wsdsc_*,double,double ));
 void  gms_dummy_fill      PARM((struct wsdsc_*,int, double *, double * ));
 void  gms_dummy_setcolor  PARM((struct wsdsc_*,int ));
 void  gms_dummy_defcolor  PARM((struct wsdsc_*,double,double,double));

 void gms_dummy_querypoint PARM((struct wsdsc_*,double *,double * ));
 void gms_dummy_querypair  PARM((struct wsdsc_*,int, double *,double *,double*,double*));


 void gms_generic_marker PARM((struct wsdsc_*,int));
 void gms_generic_text PARM((struct wsdsc_*,char * , int, int));
 void gms_generic_shaded_triangle2d PARM((struct wsdsc_*,double *, double *,double * ));
#ifdef HIDDENLINE
 void gms_generic_hiddenline_triangle3d PARM((struct wsdsc_*,double *, double *,double * ));
#endif
 void gms_generic_triangle3d PARM((struct wsdsc_*,double *, double *,double * ));
 void gms_generic_shaded_triangle3d PARM((struct wsdsc_*,double *, double *,double *,double *));
 void gms_generic_isoline_triangle2d PARM((struct wsdsc_*,double *, double *,double * ));

 void gms_generic_initran2d PARM((struct wsdsc_*,double,double,double,double,
				                   double,double,double,double));
 void gms_generic_tran2d PARM((struct wsdsc_*,double*,double*,double,double));

 void gms_generic_initran3d PARM((struct wsdsc_*,double,double,double,double,
				                   double,double,double,double));
 void gms_generic_tran3d PARM((struct wsdsc_*,double*,double*,double*,double,double,double));
 void gms_generic_moveto3d PARM((struct wsdsc_*,double,double,double));
 void gms_generic_lineto3d PARM((struct wsdsc_*,double,double,double));

                     

int  regis_ctrl PARM((struct wsdsc_ *ws, int control));
int  bgi_ctrl PARM((struct wsdsc_ *ws, int control));
int  GL_ctrl PARM((struct wsdsc_ *ws, int control));
int  X11_ctrl PARM((struct wsdsc_ *ws, int control));
int  tek4014_ctrl PARM((struct wsdsc_ *ws, int control));
int  postscript_ctrl PARM((struct wsdsc_ *ws, int control));
int  hpgl_ctrl PARM((struct wsdsc_ *ws, int control));
int  sunview_ctrl PARM((struct wsdsc_ *ws, int control));

extern FILE *gms_errorfile;
extern char gms_optval[];
extern char gms_buffer[];
void gms_genfilename PARM(( char *fname, char *connid, char *fext, int picno));
int gms_getopt PARM((char opt,char *str));
int gms_fontload PARM((void));
void gms_openoutput PARM((struct wsdsc_ *ws, char *));
void gms_closeoutput PARM((struct wsdsc_ *ws));
 
#endif
/*
 * $Id$
 * $Log$
 * Revision 1.4  2003/09/16 08:06:32  fuhrmann
 * documentation update
 *
 * Revision 1.3  2003/03/28 16:47:50  fuhrmann
 * pdelib1.15_alpha1
 *
 * Revision 1.2  2003/03/28 11:20:28  fuhrmann
 * pdelib2.0_alpha1
 *
 * Revision 1.1  1995/10/20 15:21:38  fuhrmann
 * Initial revision
 *
 */
