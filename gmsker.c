/* done by subst_insert_configh.sh */
#if HAVE_CONFIG_H
#include <config.h>
#endif
/*
 * $Id$
 * $Log$
 * Revision 1.7  2004/07/01 13:41:16  strecken
 * AUTOCONF
 *
 * Revision 1.6  2003/03/28 16:47:51  fuhrmann
 * pdelib1.15_alpha1
 *
 * Revision 1.5  2003/03/28 11:20:28  fuhrmann
 * pdelib2.0_alpha1
 *
 * Revision 1.4  2000/02/29 13:28:50  fuhrmann
 * Visual&Color choice becomes more clear
 *
 * Revision 1.3  1999/07/05 18:29:21  fuhrmann
 * quick & dirty ansi port
 *
 * Revision 1.2  1997/02/28 14:25:41  fuhrmann
 * fortran stuff for triangles, code not checked
 *
 * Revision 1.1  1995/10/20  15:21:38  fuhrmann
 * Initial revision
 *
 */

/**********************************************************************
 *                                                                    *
 *  I             A          A            S                           *
 *  Institut fuer Angewandte Analysis und Stochastik  Berlin          *
 *                                                                    *
 *  D-10117 Berlin, Mohrenstr. 39                                     *
 *                                                                    *
 *  This software is distributed in the hope that it will be useful,  *
 *  but WITHOUT ANY WARRANTY; without even the implied  warranty  of  *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *                                                                    *
 **********************************************************************/
/* GMSKERNEL.C*/
#include "gms.h"
#ifndef GMS_C
#include "gmsdrv.h"
#endif
/* exported variables */
#ifndef PI
#define PI 3.141592683
#endif


char gms_optval[64]="";
FILE *gms_errorfile=NULL;
char gms_buffer[GMS_BUFLEN];
/*------------------------------------------------------------------------------------*/
/* static function prototypes */

static wsdsc wstable[WSTABLESIZE],*first_open=NULL;
static int initflag=0;
static void init_module PARM((void));
static int  init_ctrl PARM((struct wsdsc_*,int));

/*------------------------------------------------------------------------------------*/
/* some "tools"*/

/*
parse str to extract value belonging to option character opt
   returns 1 if char has been found, value of option is in gms_optval.
*/
#ifdef NOPROTO
int gms_getopt(opt,str)
char opt;char *str;
#else
int gms_getopt(char opt,char *str)
#endif
{ int i;
  if (str==NULL) return 0;
  if (opt=='\0')
/* get the first part of the string until '-'
  strip trailing and leading ' '*/
  {   for(;(*str)&&(*str==' ');str++);
      for(i=0;(*str)&&(*str!='-');i++,str++) gms_optval[i]= *str;
      gms_optval[i]='\0';
      if (i==0) return 0;
      for(i--;(i>0)&&(gms_optval[i]==' ');i--) gms_optval[i]='\0';
      return 1;
    }
  for(;;)
  { if (*str == '\0') { gms_optval[0]='\0'; return 0;}
    if (*str == '-')
    { str++;
      if (*str == '\0') {gms_optval[0]='\0'; return 0;}
      if (*str==opt)
        { str++;
          for(;(*str)&&(*str==' ');str++);
          for(i=0;(*str)&&(*str!='-');i++,str++) gms_optval[i]= *str;
          gms_optval[i]='\0';
          for(i--;(i>0)&&(gms_optval[i]==' ');i--) gms_optval[i]='\0';
          return 1;
           }
    }
    str++;
  }
}

/* generate filename from connid  and picno with extension fext  */
void gms_genfilename(fname,connid,fext,picno)
char *fname,*connid,*fext;
int picno;
{
 if (!gms_getopt('\0',connid)) gms_getopt('O',connid);
 sprintf(fname,"%s-%d.%s", gms_optval,picno-1,fext);
}

void gms_openoutput(ws,fname)
wsdsc *ws; char *fname;
{ if (ws->demand_newoutput)
    ws->output=ws->new_output;
  else
    ws->output=fopen(fname,"w");

	      if (ws->output==NULL)
{		fprintf(gms_errorfile,
                        "%%GMS: cannot open output file %s\n",fname);
	      }
}

void gms_closeoutput(ws)
wsdsc *ws;
{
 if (ws->demand_newoutput)
 {   ws->output=NULL;
     ws->new_output=NULL;
   }
  else
   fclose(ws->output);

}
/*--------------------------------------------------------*/
/*--------------- Dummy ----------------------------------*/
 void  gms_dummy_querypoint(ws,x,y)wsdsc* ws;  double *y, *x;{}
 void  gms_dummy_querypair(ws,i,xx,yy,xx1,yy1)wsdsc* ws; int i;double *xx,*yy,*xx1,*yy1; {}
 void  gms_dummy_draw(ws,i1,i2)wsdsc* ws; double i1, i2;{}
 void  gms_dummy_fill(ws,i1,i2,i3)wsdsc* ws; int i1; double *i2, *i3;{}
 void  gms_dummy_settruecolormap(ws,i1,i2,i3,i4)wsdsc* ws; int i1; double *i2, *i3,*i4;{}
 void  gms_dummy_defcolor(ws,i2,i3,i4)wsdsc* ws;  double i2, i3,i4;{}
 void  gms_dummy_setcolor(ws,i1)wsdsc* ws;int i1; {}

static void init_module()
{ int i;
  char * name;
  name=getenv("GMSERROR");
  if (name==NULL)
    gms_errorfile=stderr;
  else
  {   gms_errorfile=fopen(name,"w");
      if (gms_errorfile==NULL)
        { gms_errorfile=stderr;
          fprintf(gms_errorfile,"%%GMS: couldn't open error file '%s'!\n",name);
        }
    }
  for (i=0;i<WSTABLESIZE;i++)
     init_ctrl(&wstable[i],GMS_CTRL_OPEN);
  initflag=1;
}

static int init_ctrl(ws,action)int action; wsdsc *ws;
{if (action==GMS_CTRL_OPEN)
 {                  ws->state=GMS_STATE_CLOSED;
                    ws->asp=1.0;
                    ws->xmax=1;
                    ws->ymax=1;
                    ws->maxcolors=1;
                    ws->moveto=gms_dummy_draw;
                    ws->lineto=gms_dummy_draw;
                    ws->marker=gms_generic_marker;
                    ws->text=gms_generic_text;
                    ws->fill=gms_dummy_fill;
                    ws->setcolor=gms_dummy_setcolor;
                    ws->defcolor=gms_dummy_defcolor;
                    ws->shaded_triangle2d=gms_generic_shaded_triangle2d;
                    ws->shaded_triangle3d=gms_generic_shaded_triangle3d;
                    ws->isoline_triangle2d=gms_generic_isoline_triangle2d;
                    ws->triangle3d=gms_generic_triangle3d;

                    ws->querypair=gms_dummy_querypair;
                    ws->querypoint=gms_dummy_querypoint;


#ifdef HIDDNLINE
                    ws->hiddenline_triangle3d=gms_generic_hiddenline_triangle3d;
#endif
                    ws->tran2d=gms_generic_tran2d;
                    ws->initran2d=gms_generic_initran2d;
                    ws->tran3d=gms_generic_tran3d;
                    ws->initran3d=gms_generic_initran3d;
                    ws->moveto3d=gms_generic_moveto3d;
                    ws->lineto3d=gms_generic_lineto3d;
                    ws->ydir=1;
                    ws->info=NULL;
                    ws->charwidth=8;
                    return GMS_RETURN_ERROR;
 }
 return GMS_RETURN_ERROR;
}

/*-------------------------------------------------------------------------*/
/* GMS_GENERIC ROUTINES */
/*-------------------------------------------------------------------------*/

void gms_generic_tran2d(ws,x,y,world_x,world_y)
wsdsc *ws;
double *x,*y,world_x,world_y;
{
 *x=(world_x*ws->ax+ws->bx);
 *y=(world_y*ws->ay+ws->by);
}

 void gms_generic_initran2d(ws,vxmin,vxmax,vymin,vymax,
			      wxmin,wxmax,wymin,wymax)
     wsdsc *ws;
     double wxmin,wxmax,wymin,wymax,vxmin,vxmax,vymin,vymax;
{
  ws->vxmin =MAX(0,vxmin*(ws->xmax)*ws->asp);
  ws->vxmax =MIN(ws->xmax,vxmax*(ws->xmax)*ws->asp);
if (ws->ydir<0.0)
 {  ws->vymin =MAX(0,(1.0-vymax)*(ws->ymax));
    ws->vymax =MIN(ws->ymax,(1.0-vymin)*(ws->ymax));
 } else
 {  ws->vymin =MAX(0,vymax*(ws->ymax));
    ws->vymax =MIN(ws->ymax,vymin*(ws->ymax));
  }
  ws->ax=(ws->vxmax-ws->vxmin)/(wxmax-wxmin);
  ws->ay=(ws->vymin-ws->vymax)/(wymax-wymin);
  ws->bx=ws->vxmin- ws->ax *wxmin;
  ws->by=ws->vymax- ws->ay *wymin;
  ws->xpos=0.0;
  ws->ypos=0.0;
  if (ws->tran2d!=NULL)  ws->tran2d(ws,&ws->xpos,&ws->ypos,0.0,0.0);
}
/*-----------------------------------------------------------------------*/

static void t3Identity(    double M[4][4])
{int i,j;
  for(i=0;i<4;i++) for(j=0;j<4;j++) M[i][j]=0.0;
  for(i=0;i<4;i++)                  M[i][i]=1.0;
}

/* M = M0 = M1*M2  */
static void t3Combine(double M[4][4],
		      double M1[4][4],
		      double M2[4][4]
		      )
{int i,j;
  double M0[4][4];
  for(i=0;i<4;i++)
    for(j=0;j<4;j++)
      M0[i][j]=
	M1[i][0]*M2[0][j]+M1[i][1]*M2[1][j]+M1[i][2]*M2[2][j]+M1[i][3]*M2[3][j];

  for(i=0;i<4;i++) for(j=0;j<4;j++)
    M[i][j]=M0[i][j];
}

/* xyz = xyz0 * M */
static void t3Apply(
     double M[4][4], 
     int num, 
     double x0[],
     double y0[],
     double z0[], 
     double x[],
     double y[],
     double z[])
{
  double t; int n;
  for(n=0;n<num;n++){
    t   = x0[n]*M[3][0]+y0[n]*M[3][1]+z0[n]*M[3][2]+M[3][3];
    if(t>1E-16) t=1/t; else t=1.0;
    x[n]=(x0[n]*M[0][0]+y0[n]*M[0][1]+z0[n]*M[0][2]+M[0][3])*t;
    y[n]=(x0[n]*M[1][0]+y0[n]*M[1][1]+z0[n]*M[1][2]+M[1][3])*t;
    z[n]=(x0[n]*M[2][0]+y0[n]*M[2][1]+z0[n]*M[2][2]+M[2][3])*t;
  }
}

/* M = M *RotX */
static void t3XRotation(
     double M[4][4],
     double alpha)
{
  double M2[4][4];
  t3Identity(M2);
  M2[1][1]=  cos(alpha); M2[1][2]=  -sin(alpha);
  M2[2][1]= -M2[1][2];   M2[2][2]=  M2[1][1];
  t3Combine(M,M2,M);
}

/* M = M*RotY */
static void t3YRotation(
     double M[4][4],
     double alpha)
{
  double M2[4][4];
  t3Identity(M2);
  M2[2][2]=  cos(alpha); M2[2][0]=  -sin(alpha);
  M2[0][2]= -M2[2][0];    M2[0][0]=  M2[2][2];
  t3Combine(M,M2,M);
}

/* M = M*RotZ */
static void t3ZRotation(
     double M[4][4],
     double alpha)
{
  double M2[4][4];
  t3Identity(M2);
  M2[0][0]=  cos(alpha); M2[0][1]=  -sin(alpha);
  M2[1][0]= -M2[0][1];   M2[1][1]=  M2[0][0];
  t3Combine(M,M2,M);
}

/* M = M*Shift */
static void t3Shift(
     double M[4][4],
     double shiftx,
     double shifty,
     double shiftz)
{
  double M2[4][4];
  t3Identity(M2);
  M2[0][3]=shiftx; M2[1][3]=shifty; M2[2][3]=shiftz;
  t3Combine(M,M2,M);
}

/* M = M*Scale */
static void t3Scale(
     double M[4][4],
     double scalex,
     double scaley,
     double scalez)
{
  double M2[4][4];
  t3Identity(M2);
  M2[0][0]=scalex; M2[1][1]=scaley; M2[2][2]=scalez;
  t3Combine(M,M2,M);
}

/* M = M*Perspective */
static void t3Perspective(
     double M[4][4],
     double distance)  /* distance=0 is identical with distance=infinity */
{
  double M2[4][4];
  t3Identity(M2);
  if(distance>1E-16) M2[3][2]=1.0/distance;
  t3Combine(M,M2,M);
}

/* example:

   real M[4][4];

   t3Identity(M);
   t3Shift(M,-(xmin+xmax)/2,-(ymin+ymax)/2,-(zmin+zmax)/2);
   t3ZRotation(M,delta);
   t3XRotation(M,-phi);
   t3Perspective(M,distance);
*/


void gms_generic_tran3d(ws,u,v,w,world_x,world_y,world_z)
wsdsc *ws;
double *u,*v,*w,world_x,world_y,world_z;
{ double t,*line;
   line=ws->Matrix3d[3];
    t=1.0/(line[0]*world_x+line[1]*world_y+line[2]*world_z+line[3]);

   line=ws->Matrix3d[0];
   *u=(line[0]*world_x+line[1]*world_y+line[2]*world_z+line[3])*t;

   line=ws->Matrix3d[1];
   *v=(line[0]*world_x+line[1]*world_y+line[2]*world_z+line[3])*t;

    line=ws->Matrix3d[2];
   *w=(line[0]*world_x+line[1]*world_y+line[2]*world_z+line[3])*t;
}


void gms_generic_initran3d(ws,alpha,beta,
			      wxmin,wxmax,wymin,wymax,wzmin,wzmax)
     wsdsc *ws;
     double wxmin,wxmax,wymin,wymax,wzmin,wzmax,alpha,beta;
{
  double scalfac;
  alpha=((double)((int)alpha%360    ))* PI/180.0;
  beta= ((double)((int)beta %180 -90))* PI/180.0;

  t3Identity(ws->Matrix3d);
  t3Shift(ws->Matrix3d,-(wxmin+wxmax)/2.0,
                       -(wymin+wymax)/2.0,
                       -(wzmin+wzmax)/2.0);
  scalfac=MAX((wxmax-wxmin),(wymax-wymin));

  t3Scale(ws->Matrix3d,0.6/scalfac,
                       -0.6/scalfac,
                       0.6/(wzmax-wzmin));
  t3ZRotation(ws->Matrix3d,alpha);
  t3XRotation(ws->Matrix3d,-beta);
  t3Shift(ws->Matrix3d,0.5,0.5,0.5);

  scalfac=MIN(ws->xmax,ws->ymax);
  if (ws->ydir>0)
  { t3Scale(ws->Matrix3d,1.0,-1.0,1.0);
    t3Shift(ws->Matrix3d,0.0,1.0,0.0);
   }
 t3Scale(ws->Matrix3d,scalfac,scalfac,scalfac);


/*
  t3Perspective(ws->Matrix3d,distance);
*/

}

void gms_generic_moveto3d(ws,x,y,z)
double x,y,z;
wsdsc *ws;
{
      ws->tran3d(ws,&x,&y,&z,x,y,z);
      ws->moveto(ws,x,y);
      ws->xpos=x;ws->ypos=y;

}

void gms_generic_lineto3d(ws,x,y,z)
double x,  y, z;
wsdsc *ws;
{
      ws->tran3d(ws,&x,&y,&z,x,y,z);
      ws->lineto(ws,x,y);
      ws->xpos=x;ws->ypos=y;
}

/*------------------------------------------------------------------------*/
 void gms_generic_marker(ws,mktype) wsdsc* ws; int mktype;
{ double xpos=ws->xpos,ypos=ws->ypos,x,y; /* "position stack" */
  switch(mktype)
 { case GMS_MARKER_PIXEL : ws->lineto(ws,ws->xpos,ws->ypos);
                   break;
   case GMS_MARKER_DIAMOND:
                 ws->moveto(ws,x=xpos-ws->markersize,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos,y=ypos-ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos+ws->markersize,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos,y=ypos+ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos-ws->markersize,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                       break;
   case GMS_MARKER_PLUS  :
                 ws->moveto(ws,x=xpos-ws->markersize,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos+ws->markersize,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos-ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos,y=ypos+ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                         break;
  case GMS_MARKER_X    :
                 ws->moveto(ws,x=xpos-ws->markersize,y=ypos-ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos+ws->markersize,y=ypos+ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos+ws->markersize,y=ypos-ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos-ws->markersize,y=ypos+ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 break ;
  case GMS_MARKER_DOWNARROW    :
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=xpos;ws->ypos=ypos;
                 ws->lineto(ws,x=xpos-ws->markersize,y=ypos+ws->markersize*ws->ydir);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos+ws->markersize,y=ypos+ws->markersize*ws->ydir);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 break;
  case GMS_MARKER_UPARROW    :
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=xpos;ws->ypos=ypos;
                 ws->lineto(ws,x=xpos-ws->markersize,y=ypos-ws->markersize*ws->ydir);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos+ws->markersize,y=ypos-ws->markersize*ws->ydir);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 break;
  case GMS_MARKER_RIGHTARROW    :
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=xpos;ws->ypos=ypos;
                 ws->lineto(ws,x=xpos-ws->markersize,y=ypos+ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos-ws->markersize,y=ypos-ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 break;
  case GMS_MARKER_LEFTARROW    :
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=xpos;ws->ypos=ypos;
                 ws->lineto(ws,x=xpos+ws->markersize,y=ypos+ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 ws->lineto(ws,x=xpos+ws->markersize,y=ypos-ws->markersize);
                 ws->xpos=x;ws->ypos=y;
                 ws->moveto(ws,x=xpos,y=ypos);
                 ws->xpos=x;ws->ypos=y;
                 break;

 case GMS_MARKER_ASTERISK: gms_generic_marker(ws,GMS_MARKER_PLUS);
                           gms_generic_marker(ws,GMS_MARKER_X);
   default :return ;
  };
}



/*-------------------------------------------------------------------------*/
static double *colorscale_v=NULL,
              *colorscale_r=NULL,
              *colorscale_g=NULL,
              *colorscale_b=NULL;

static       int colorscale_size=0;



void getscalecolor(
double f, float col[])
{
if (f<=colorscale_v[0])
{ col[0]=colorscale_r[0];
  col[1]=colorscale_g[0];
  col[2]=colorscale_b[0];
}

else if (f>=colorscale_v[colorscale_size-1])
{ col[0]=colorscale_r[colorscale_size-1];
  col[1]=colorscale_g[colorscale_size-1];
  col[2]=colorscale_b[colorscale_size-1];
}

else
{ int ilev; float dcol;
  for(ilev=1;ilev<colorscale_size;ilev++)
      if (f<=colorscale_v[ilev]&&(f>colorscale_v[ilev-1])) break;

  dcol=(f-colorscale_v[ilev-1])/(colorscale_v[ilev]-colorscale_v[ilev-1]);

  col[0]=colorscale_r[ilev-1]+dcol*(colorscale_r[ilev]-colorscale_r[ilev-1]);
  col[1]=colorscale_g[ilev-1]+dcol*(colorscale_g[ilev]-colorscale_g[ilev-1]);
  col[2]=colorscale_b[ilev-1]+dcol*(colorscale_b[ilev]-colorscale_b[ilev-1]);

}

}


void  gms_generic_shaded_triangle2d(ws,x,y,f)wsdsc *ws;double *x, *y,*f;
{register int  i, i0,i1,i2,ncol;
 double alpha,xx,yy;
 double xf[5],yf[5],*cv,cval;
 double idf20,idf21,idf10,dx20,dx21,dx10,dy21,dy20,dy10;

  if (f[0] <=f[1]) {i0=0; i1=1;} else {i0=1; i1=0;};
  if (f[i1]<=f[2]) {i2=2;      } else {i2=i1;i1=2;};
  if (f[i0] >f[i1]){i=i0; i0=i1; i1=i ;};

  xf[0]=x[i2];
  yf[0]=y[i2];
  xf[3]=x[i1];
  yf[3]=y[i1];

  if (f[i2]!=f[i0]) idf20=1.0/(f[i2]-f[i0]); else idf20=1.0e10;
  if (f[i1]!=f[i0]) idf10=1.0/(f[i1]-f[i0]); else idf10=1.0e10;
  if (f[i2]!=f[i1]) idf21=1.0/(f[i2]-f[i1]); else idf21=1.0e10;

  dx20=x[i2]-x[i0];
  dx10=x[i1]-x[i0];
  dx21=x[i2]-x[i1];

  dy20=y[i2]-y[i0];
  dy10=y[i1]-y[i0];
  dy21=y[i2]-y[i1];

  ncol=ws->colorscale_size;
  cv=ws->colorscale_v;

  for( i=1 ; i<ncol; i++ ) /* kleinster Wert hat Farbe i */
  { cval=cv[i];
    if( ((i==(ncol-1)||(f[i0]<cval))) &&
        ( (i==1) || (cv[i-1]<=f[i0]) ))
    {
        ws->setcolor(ws,i);
       ws->fill(ws,3,x,y);
    }

    if( (f[i0]<=cval) && (cval<=f[i2]) ) /* Hoehenlinie mit Farbe i
                                           im Dreieck*/
    {
      ws->setcolor(ws,i+1);
      alpha=(cval-f[i0])*idf20;
      xf[1]=x[i0]+alpha*dx20;
      yf[1]=y[i0]+alpha*dy20;

      if( cval<=f[i1])
      { alpha=(cval-f[i0])*idf10;
        xf[2]=x[i0]+alpha*dx10;
        yf[2]=y[i0]+alpha*dy10;

#ifdef ALPHA_XSERVER_BUG
        xx=xf[2]-xf[1];
        yy=yf[2]-yf[1];
        if ((xx*xx+yy*yy)<1.0e-3)
        { xf[2]=xf[3];
          yf[2]=yf[3];
          ws->fill(ws,3,xf,yf);
         }
	 else
           ws->fill(ws,4,xf,yf);
#else
          ws->fill(ws,4,xf,yf);
#endif
      }
    else
      { alpha=(cval-f[i1])*idf21;
        xf[2]=x[i1]+alpha*dx21;
        yf[2]=y[i1]+alpha*dy21;
         ws->fill(ws,3,xf,yf);

      }

    }
  }
}


/*--------------------------------------------------------------*/
static double *isolevels=NULL;
static int nisolevels=0;
 void  gms_generic_isoline_triangle2d(ws,x,y,f)wsdsc *ws;double *x, *y,*f;
{register int  i, i0,i1,i2;
 double alpha;
 double x0,y0,x1,y1;
 double df20,df21,df10,dx20,dx21,dx10,dy21,dy20,dy10;

  if (f[0] <=f[1]) {i0=0; i1=1 ;} else {i0=1; i1=0;};
  if (f[i1]<=f[2]) {i2=2       ;} else {i2=i1;i1=2;};
  if (f[i0] >f[i1]){i=i0; i0=i1; i1=i ;};

  if (f[i2]!=f[i0]) df20=1.0/(f[i2]-f[i0]); else df20=0.0;
  if (f[i1]!=f[i0]) df10=1.0/(f[i1]-f[i0]); else df10=0.0;
  if (f[i2]!=f[i1]) df21=1.0/(f[i2]-f[i1]); else df21=0.0;

  dx20=x[i2]-x[i0];
  dx10=x[i1]-x[i0];
  dx21=x[i2]-x[i1];

  dy20=y[i2]-y[i0];
  dy10=y[i1]-y[i0];
  dy21=y[i2]-y[i1];

  for( i=0 ; i< nisolevels; i++ )
  { if( (f[i0]<=isolevels[i]) && (isolevels[i]<f[i2]) )
    { alpha=(isolevels[i]-f[i0])*df20;
      x0=x[i0]+alpha*dx20;
      y0=y[i0]+alpha*dy20;

      if (isolevels[i]<f[i1])
      { alpha=(isolevels[i]-f[i0])*df10;
        x1=x[i0]+alpha*dx10;
        y1=y[i0]+alpha*dy10;
      } else
      {
        alpha=(isolevels[i]-f[i1])*df21;
        x1=x[i1]+alpha*dx21;
        y1=y[i1]+alpha*dy21;
      }
      ws->moveto(ws,x0,y0);
      ws->xpos=x0;ws->ypos=y0;
      ws->lineto(ws,x1,y1);
     }
   }
 }

/*--------------------------------------------------------------*/

typedef struct {double coord[3][3];} triangle;
typedef struct {double val[3];} trvalue;

static triangle *triangles=NULL;
static trvalue *trval=NULL;
static float *trdist;
static int *trindex;
static int ntria=1000;
static int itria=0;

void  gms_generic_shaded_triangle3d(ws,x,y,z,f)wsdsc *ws;
double *x, *y,*z,*f;
{
  if (itria==0) triangles=(triangle*)malloc(ntria*sizeof(triangle));
  if (itria==0) trval=(trvalue*)malloc(ntria*sizeof(trvalue));


  triangles[itria].coord[0][0]=x[0];
  triangles[itria].coord[1][0]=x[1];
  triangles[itria].coord[2][0]=x[2];

  triangles[itria].coord[0][1]=y[0];
  triangles[itria].coord[1][1]=y[1];
  triangles[itria].coord[2][1]=y[2];

  triangles[itria].coord[0][2]=z[0];
  triangles[itria].coord[1][2]=z[1];
  triangles[itria].coord[2][2]=z[2];

  trval[itria].val[0]=f[0];
  trval[itria].val[1]=f[1];
  trval[itria].val[2]=f[2];

  itria++;
  if(itria>=ntria)
  { ntria*=2;
    triangles=(triangle*)realloc(triangles,ntria*sizeof(triangle));
    trval=(trvalue*)realloc(trval,ntria*sizeof(trvalue));
  }
}


#ifdef HIDDENLINE
static int hdlin=0;

void  gms_generic_hiddenline_triangle3d(ws,x,y,z)wsdsc *ws;double *x, *y,*z;
{
  if (itria==0) triangles=(triangle*)malloc(ntria*sizeof(triangle));
  hdlin=1;

  triangles[itria].coord[0][0]=x[0];
  triangles[itria].coord[1][0]=x[1];
  triangles[itria].coord[2][0]=x[2];

  triangles[itria].coord[0][1]=y[0];
  triangles[itria].coord[1][1]=y[1];
  triangles[itria].coord[2][1]=y[2];

  triangles[itria].coord[0][2]=z[0];
  triangles[itria].coord[1][2]=z[1];
  triangles[itria].coord[2][2]=z[2];

  itria++;
  if(itria>=ntria)
  { ntria*=2;
    triangles=(triangle*)realloc(triangles,ntria*sizeof(triangle));
  }
}
#endif

void  gms_generic_triangle3d(ws,x,y,z)wsdsc *ws;double *x, *y,*z;
{
  if (itria==0) triangles=(triangle*)malloc(ntria*sizeof(triangle));


  triangles[itria].coord[0][0]=x[0];
  triangles[itria].coord[1][0]=x[1];
  triangles[itria].coord[2][0]=x[2];

  triangles[itria].coord[0][1]=y[0];
  triangles[itria].coord[1][1]=y[1];
  triangles[itria].coord[2][1]=y[2];

  triangles[itria].coord[0][2]=z[0];
  triangles[itria].coord[1][2]=z[1];
  triangles[itria].coord[2][2]=z[2];

  itria++;
  if(itria>=ntria)
  { ntria*=2;
    triangles=(triangle*)realloc(triangles,ntria*sizeof(triangle));
  }
}

#ifdef HIDDENLINE

#ifndef RESOLUTION
#define RESOLUTION 2048
#endif


static int nh;
static double *uh=NULL, *lh=NULL;
static double ufac;



void gms3_begin_horizon(ws) wsdsc *ws;
{
  double x,y;
  int i;
    uh=(double*)malloc((10+RESOLUTION)*sizeof(double))+5;
    if (uh==NULL) abort() ;
    lh=(double*)malloc((10+RESOLUTION)*sizeof(double))+5;
    if (lh==NULL) abort() ;
    nh=RESOLUTION;
    x=ws->vymax+0.1;
    y=ws->vymin+0.1;

    for(i= -2;i<nh+2;i++) {lh[i]=x;uh[i]=y;};
    ufac=((double)nh)/(ws->vxmax-ws->vxmin);
 }

void gms3_end_horizon(ws) wsdsc *ws;
{
  free(uh-5);
  free(lh-5);
  uh=lh=NULL;
}


static void xmoveto(ws,u,v)
wsdsc *ws;
double u,v;
{
 ws->moveto(ws,u,v);
ws->xpos=u;
ws->ypos=v;

}


static  void xlineto(ws,u,v)
wsdsc *ws;
double u,v;
{
 ws->lineto(ws,u,v);
ws->xpos=u;
ws->ypos=v;

}

void gms3_plot_horizon(ws,u0,v0,u1,v1)
double u0,v0,u1,v1; wsdsc *ws;
{ register int i,pen;
  int n,i0,i1;
  double u,v,hu,hv;

  if (u1<u0) {v=v0; v0=v1;v1=v; u=u0;u0=u1;u1=u;}

  i0=(int)((u0-ws->vxmin)*ufac);
  n=(int)((u1-u0)*ufac);

  if (n==0)
  { if (v1<v0) {v=v0; v0=v1;v1=v; u=u0;u0=u1;u1=u;}
      if (uh[i0]<lh[i0]) {lh[i0]=v1;uh[i0]=v1;}
      if (v0<=lh[i0]) { xmoveto(ws,u0,v0);
		       xlineto(ws,u0,lh[i0]);
		       lh[i0]=v0;}
      if (v1>=uh[i0]) { xmoveto(ws,u1,v1);
		       xlineto(ws,u1,uh[i0]);
		       uh[i0]=v1;}
      return;
    }

  i1=i0+n;
  if (i1>=nh) i1=nh-1;
  u=(double)(n-1);
  hv=(v1-v0)/u;
  hu=(u1-u0)/u;


  pen=0;

  for (i=i0, u=u0, v=v0; i<i1;i++, u+=hu, v+=hv )
    {

    if (v<=lh[i])
	{ if (pen==0) {pen= -1; xmoveto(ws,u,v);}
	  lh[i]=v;
	  continue;
	}
      else
	if (pen== -1)
	  { pen=0;
	    xlineto(ws,u,v);
	    continue;
	  }

    if (v>=uh[i])
      { if (pen==0) { pen=1; xmoveto(ws,u,v);}
	uh[i]=v;
	continue;
      }
    else
      if (pen==1)
	{ pen=0;
	  xlineto(ws,u,v);
	  continue;
	}

	  xmoveto(ws,u,v);
    ws->marker(ws,GMS_MARKER_ASTERISK);


    }


  if (v1<=lh[i1])
    {
      if (pen==0) xmoveto(ws,u1-hu,v1-hv);
      xlineto(ws,u1,v1);
      lh[i1]=v1;
      return;
   }

  if (v1>=uh[i1])
    { if (pen==0) xmoveto(ws,u1-hu,v1-hv);
      xlineto(ws,u1,v1);
      uh[i1]=v1;
      return;
    }


}


#endif /*HIDDENLINE*/


#ifdef NOPROTO
static int compare_triangles(i0,i1)  void *i0; void*i1;
#else
static int compare_triangles(const void *i0, const void*i1)
#endif
{ if (trdist[*((int*)i0)]<trdist[*((int*)i1)]) return -1;
  else if (trdist[*((int*)i0)]==trdist[*((int*)i1)]) return 0;
  else return 1;

}

static double  red=0.85, green= 0.85, blue=1.0;


static void gms3_flush_triangles(wsdsc *ws)
{ register int i,j,icol;
  double u[4],v[4],w[4],*coord;
  double x1,x2,y1,y2,z1,z2,z3,xn,yn,zn,nnorm;
  triangle *tr;
  float cx,cy,cz;


  trindex=(int *)malloc(sizeof(int)*itria);
  trdist=(float *)malloc(sizeof(float)*itria);
  for (i=0;i<itria;i++)
  { trindex[i]=i;
    tr= &triangles[i];
    trdist[i]=0.0;

    coord=tr->coord[0];
    ws->tran3d(ws,&coord[0],&coord[1],&coord[2],coord[0], coord[1], coord[2]);
    trdist[i]+=coord[2];

    coord=tr->coord[1];
    ws->tran3d(ws,&coord[0],&coord[1],&coord[2],coord[0], coord[1], coord[2]);
    trdist[i]+=coord[2];

    coord=tr->coord[2];
    ws->tran3d(ws,&coord[0],&coord[1],&coord[2],coord[0], coord[1], coord[2]);
    trdist[i]+=coord[2];

  }


   qsort(trindex,(size_t)itria,sizeof(int),compare_triangles);

  if (trval==NULL
#ifdef HIDDENLINE
&&!hdlin
#endif
              )
    {
      double r,g,b,rh,bh,gh;
      rh=0.999*red/(double)(ws->maxcolors-1);
      gh=0.999*green/(double)(ws->maxcolors-1);
      bh=0.999*blue/(double)(ws->maxcolors-1);
      r=0.001*red;
      g=0.001*green;
      b=0.001*blue;


      for (i=1;i<ws->maxcolors;i++)
        {  ws->setcolor(ws,i);
           ws->defcolor(ws,r,g,b);
           r+=rh,b+=bh,g+=gh;
         }
    }

  if (trval==0
#ifdef HIDDENLINE
&&!hdlin
#endif
    )
  for (i=0;i<itria;i++)
  {
   tr= &triangles[trindex[i]];

   x1=tr->coord[1][0]-tr->coord[0][0];
   y1=tr->coord[1][1]-tr->coord[0][1];
   z1=tr->coord[1][2]-tr->coord[0][2];
   x2=tr->coord[2][0]-tr->coord[0][0];
   y2=tr->coord[2][1]-tr->coord[0][1];
   z2=tr->coord[2][2]-tr->coord[0][2];

   xn=(y1*z2-z1*y2);
   yn=(z1*x2-x1*z2);
   zn=(x1*y2-y1*x2);


   nnorm=sqrt(xn*xn+yn*yn+zn*zn);
   xn=xn/nnorm;
   yn=yn/nnorm;
   zn=zn/nnorm;
   nnorm=fabs(0.64*(xn+yn)+0.42*zn);
   icol=(int)(nnorm*(double)(ws->maxcolors-1))+1;
   u[0]=tr->coord[0][0];
   u[1]=tr->coord[1][0];
   u[2]=tr->coord[2][0];
   v[0]=tr->coord[0][1];
   v[1]=tr->coord[1][1];
   v[2]=tr->coord[2][1];


   ws->setcolor(ws,icol);
   ws->fill(ws,3,u,v);

}
#ifdef HIDDENLINE
else if (hdlin)
  {
  gms3_begin_horizon(ws);
   for (i=itria-1;i>-0;i--)
     {
      tr= &triangles[trindex[i]];

      gms3_plot_horizon(ws,tr->coord[0][0],tr->coord[0][1],tr->coord[1][0],tr->coord[1][1]);
      gms3_plot_horizon(ws,tr->coord[1][0],tr->coord[1][1],tr->coord[2][0],tr->coord[2][1]);
      gms3_plot_horizon(ws,tr->coord[0][0],tr->coord[0][1],tr->coord[2][0],tr->coord[2][1]);

    }

      gms3_end_horizon(ws);
      hdlin=0;
    }
#endif
else  for (i=0;i<itria;i++)
  {
   tr= &triangles[trindex[i]];

   u[0]=tr->coord[0][0];
   u[1]=tr->coord[1][0];
   u[2]=tr->coord[2][0];
   v[0]=tr->coord[0][1];
   v[1]=tr->coord[1][1];
   v[2]=tr->coord[2][1];


   ws->shaded_triangle2d(ws,u,v,trval[trindex[i]].val);


}

free(triangles); triangles=NULL;
free(trindex);trindex=NULL;
free(trdist);trdist=NULL;
free(trval);trval=NULL;
itria=0;
}

/*-------------------------------------------------------------------------*/
/* gms_generic text - polyline fonts based on Hershey font coding
   from the ghostscript distribution
*/
typedef struct { int lmarg;
                 int rmarg;
                 char * data;
               } fontchar;

static fontchar gmsfont[256];
static double fonttop= -9.0,
       fontbot=12.0;
static char fontbiaschar='R';
static char fontpenupchar=' ';
static char *charnames[256]=
{ "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "space",         "exclam",        "numbersign",    "quotedbl",
  "dollar",        "percent",       "ampersand",     "qouteleft",
  "parenleft",     "parenright",    "asterisk",      "plus",
  "comma",         "hyphen",        "period",        "slash",

  "zero",          "one",           "two",           "three",
  "four",          "five",          "six",           "seven",
  "eight",         "nine",          "colon",         "semicolon",
  "less",          "equal",         "greater",       "question",

  "at",            "A",             "B",             "C",
  "D",             "E",             "F",             "G",
  "H",             "I",             "J",             "K",
  "L",             "M",             "N",             "O",

  "P",             "Q",             "R",             "S",
  "T",             "U",             "V",             "W",
  "X",             "Y",             "Z",             "bracketleft",
  "backslash",     "bracketright",  "asciicircum",   "underscore",

  "quoteright",    "a",             "b",             "c",
  "d",             "e",             "f",             "g",
  "h",             "i",             "j",             "k",
  "l",             "m",             "n",             "o",

  "p",             "q",             "r",             "s",
  "t",             "u",             "v",             "w",
  "x",             "y",             "z",             "braceleft",
  "",              "braceright",    "asciitilde",    "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",

  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "",
  "",              "",              "",              "" };

int gms_fontloaded=0;


 int gms_fontload PARM((void))
{ FILE *fontfile;
  int i,j,ichar;
  char charname[16],*fnam;
  if (gms_fontloaded) return gms_fontloaded;
  fontfile=NULL;

  if (fontfile==NULL)
  { fnam=getenv("GMSFONT");
    if (fnam!=NULL)
      fontfile=fopen(fnam,"r");
  }

  if (fontfile==NULL)
  { return gms_fontloaded;
  }

/* default: original gs Hershey fonts:*/

  fonttop= -9.0,
  fontbot=12.0;
  fontbiaschar='R';
  fontpenupchar=' ';

  for(;;)
  {  fgets(gms_buffer,256,fontfile);
     if (feof(fontfile)) break;
     i=0;
     if (gms_buffer[i]=='/')
     { i++;
       j=0;
       while( gms_buffer[i]!=' ') charname[j++]=gms_buffer[i++];
       charname[j]='\0';
       if (!strcmp(charname,"_fontbiaschar"))
       { while(gms_buffer[i]==' ') i++;
         fontbiaschar=(char)atoi(&gms_buffer[i]);
         continue;
       }
       if (!strcmp(charname,"_fontpenupchar"))
       { while(gms_buffer[i]==' ') i++;
         fontpenupchar=(char)atoi(&gms_buffer[i]);
         continue;
       }
       if (!strcmp(charname,"_fonttop"))
       { while(gms_buffer[i]==' ') i++;
         fonttop=(double)atoi(&gms_buffer[i]);
         continue;
       }
       if (!strcmp(charname,"_fontbottom"))
       { while(gms_buffer[i]==' ') i++;
         fontbot=(double)atoi(&gms_buffer[i]);
         continue;
       }

       if (!strcmp(charname,"_fontbottom"))
       { while(gms_buffer[i]==' ') i++;
         fonttop=atoi(&gms_buffer[i]);
         continue;
       }

       for(ichar=0;ichar<256;ichar++)
         if (!strcmp(charname,charnames[ichar]))
          {
            while (gms_buffer[i]==' ') i++;
            gmsfont[ichar].lmarg=atoi(&gms_buffer[i]);
            while (gms_buffer[i]!=' ') i++;
            while (gms_buffer[i]==' ') i++;
            gmsfont[ichar].rmarg=atoi(&gms_buffer[i]);
            while (gms_buffer[i]!=' ') i++;
            while (gms_buffer[i]==' ') i++;

            j=i;
            while(gms_buffer[j]!='\n') j++;
            gmsfont[ichar].data=malloc((j-i+2)*sizeof(char));
            j=0;
            while(gms_buffer[i]!='\n')
            { gmsfont[ichar].data[j++]=gms_buffer[i++];
            }
            gmsfont[ichar].data[j]='\0';
          break;
         }
       }
     }
     fclose(fontfile);
     gms_fontloaded=1;
     return gms_fontloaded;
}

static double fontxscale,fontyscale;
static void draw_char(wsdsc *ws, char c)
{ int i,movepen;
  double x,y;
  /* Assert position has been saved, pen is at center of char*/
  double xpos=ws->xpos,ypos=ws->ypos;
  i=0;
  movepen=1;
  while (gmsfont[c].data[i]!='\0')
  { if (gmsfont[c].data[i]==fontpenupchar){ movepen=1; i++;}
    x= xpos + ((double)((int)gmsfont[c].data[i++]-(int)fontbiaschar))*fontxscale;
    y= ypos + ((double)((int)gmsfont[c].data[i++]-(int)fontbiaschar))*fontyscale;
    if (movepen) ws->moveto(ws,x,y); else ws->lineto(ws,x,y);
    ws->xpos=x;ws->ypos=y;
    movepen=0;
  }
  ws->moveto(ws,xpos,ypos);
  ws->xpos=xpos; ws->ypos=ypos;
}


 void gms_generic_text(ws,s,xalign,yalign) wsdsc *ws; char *s;int xalign,yalign;
{ double xpos=ws->xpos,ypos=ws->ypos; /* "position stack" */
  double x,y,textlen;
  int i;
  if (!gms_fontloaded) return;
  fontxscale= ws->textsize/((double)fonttop-(double)fontbot);
  fontyscale= ws->textsize/((double)fonttop-(double)fontbot);
  fontyscale *= -ws->ydir;
  switch (yalign)
  { case GMS_ALIGN_CENTER: y=ws->ypos;break;
    case GMS_ALIGN_TOP:    y=ws->ypos-((double)fontbot)*fontyscale;break;
    case GMS_ALIGN_BOTTOM: y=ws->ypos-((double)fonttop)*fontyscale;break;
    default:break;
  }

  textlen=0.0;
  for(i=0;s[i]!='\0';i++) textlen+=gmsfont[s[i]].rmarg-gmsfont[s[i]].lmarg;
  switch(xalign)
  { case GMS_ALIGN_LEFT: x=ws->xpos;break;
    case GMS_ALIGN_RIGHT: x=ws->xpos-textlen*fontxscale;break;
    case GMS_ALIGN_CENTER: x=ws->xpos-0.5*textlen*fontxscale;break;
    default:break;
  }
 for (i=0;s[i]!='\0';i++)
 { x-=gmsfont[s[i]].lmarg*fontxscale;
   ws->moveto(ws,x,y);
   ws->xpos=x;ws->ypos=y;
   draw_char(ws,s[i]);
   x+=gmsfont[s[i]].rmarg*fontxscale;
 }
 ws->xpos=xpos;ws->ypos=ypos;
}




/*-----------------------------------------------------------------
-------------------------------------------------------------------
  EXPORTED PROCEDURES - C INTERFACE
-------------------------------------------------------------------
-----------------------------------------------------------------*/
static double gms2_wxmin=0.0,gms2_wxmax=1.0,gms2_wymin=0.0,gms2_wymax=1.0;
static double gms2_vxmin=0.0,gms2_vxmax=1.0,gms2_vymin=0.0,gms2_vymax=1.0;
static double gms2_xpos,gms2_ypos;


void  gms_open(iws,deviceid,connid)
int iws;   int deviceid;char *connid;
{ wsdsc *ws;
  char *name;
  ws= &wstable[iws];
  if (!initflag) init_module();

  if (deviceid==0)
    { name=getenv("GMSDEVICE");
      if (name==NULL)
        fprintf(gms_errorfile,"%%GMS: No device specifier\n");
      else
        deviceid=atoi(name);
    }

  if ((connid==NULL)||(connid[0]=='\0'))
    { name=getenv("GMSCONNID");
      if (name==NULL)
        strcpy(ws->connid,"");
      else
        strcpy(ws->connid,name);
    }
  else
    strcpy(ws->connid,connid);


 if ((iws<0)||(iws>=WSTABLESIZE))
  {    fprintf(gms_errorfile,"%%GMS: wrong workstation number: %d\n",iws);
  }

  if (ws->state!=GMS_STATE_CLOSED)
  {   fprintf(gms_errorfile,"%%GMS: wrong workstation state in gms_open for ws no %d\n",iws);
  }

  ws->mode=deviceid%10;
  deviceid=10*(deviceid/10);
  ws->wstype=deviceid;

  switch(deviceid)
    {
    case GMS_DRIVER_REGIS:     ws->ctrl=regis_ctrl; break;
    case GMS_DRIVER_BGI:       ws->ctrl=bgi_ctrl; break;
    case GMS_DRIVER_GL:        ws->ctrl=GL_ctrl; break;
    case GMS_DRIVER_X11:       ws->ctrl=X11_ctrl; break;
    case GMS_DRIVER_TEK4014:   ws->ctrl=tek4014_ctrl; break;
    case GMS_DRIVER_POSTSCRIPT:ws->ctrl=postscript_ctrl; break;
    case GMS_DRIVER_HPGL:      ws->ctrl=hpgl_ctrl; break;
    case GMS_DRIVER_SUNVIEW:   ws->ctrl=sunview_ctrl; break;
    default:                                ws->ctrl=init_ctrl;
      fprintf(gms_errorfile,"%%GMS: wrong workstion number\n");
      if (gms_errorfile!=stderr)
	fprintf(stderr,"%%GMS: wrong workstion number\n");

    }
  if (ws->ctrl(ws,GMS_CTRL_OPEN))
    {
      ws->state=GMS_STATE_OPEN;
      ws->demand_newoutput=0;
      ws->new_output=NULL;
      if (gms_getopt('O',ws->connid))
	{ if (strcmp(gms_optval,"0")==0) ws->demand_newoutput=1;
	}
      if (first_open==NULL)
	{  first_open=ws;
	   ws->next_open=NULL;
	 }
      else
	{ ws->next_open=first_open;
	  first_open=ws;
	}
    }
 else
  { fprintf(gms_errorfile,"%%GMS: unable to open workstation\n");
    if (gms_errorfile!=stderr)
     fprintf(stderr,"%%GMS: unable to open workstation\n");
  }
}



void gms_close(iws) int iws;
{ wsdsc *ws= &wstable[iws],*xws;
  if (ws->state!=GMS_STATE_OPEN)
    {   fprintf(gms_errorfile,"%%GMS: wrong workstation state in gms_close for ws  %d\n",iws);
	return;
      }
  if (ws->ctrl(ws,GMS_CTRL_CLOSE))
    { ws->state=GMS_STATE_CLOSED;
      if (ws==first_open)
	{ first_open=ws->next_open;
	  ws->next_open=NULL;
      } else
	{ xws=first_open;
	  while (xws->next_open!=ws) xws=xws->next_open;
	  xws->next_open=ws->next_open;
	  ws->next_open=NULL;
	}
    }
  else fprintf(gms_errorfile,"%%GMS: error closing workstation %d",iws);
}

void gms_begin_ws(wsdsc *ws)
{
  if (ws->ctrl(ws,GMS_CTRL_BEGIN))
    { 
      ws->state=GMS_STATE_ACTIVE;
      ws->markersize=0.0075*MIN(ws->xmax,ws->ymax);
      ws->textsize=0.015*MIN(ws->xmax,ws->ymax);
      ws->vxmin=0.0; ws->vxmax=1.0;
      ws->vymin=0.0; ws->vymax=1.0;
      ws->initran2d(ws,gms2_vxmin,gms2_vxmax,gms2_vymin,gms2_vymax,
		    gms2_wxmin,gms2_wxmax,gms2_wymin,gms2_wymax);
      ws->xpos=gms2_xpos;
      ws->ypos=gms2_ypos;
      
      if (ws->tran2d!=NULL)  ws->tran2d(ws,&ws->xpos,&ws->ypos,gms2_xpos,gms2_ypos);
    }
}

void gms_begin(iws) int iws;
{ wsdsc *ws;
  if (iws==-1)
    {
      for(ws=first_open;ws!=NULL;ws=ws->next_open)
	gms_begin_ws(ws);
      
    }
  else
    {
      ws= &wstable[iws];
      if(ws->state==GMS_STATE_OPEN)
	gms_begin_ws(ws);
      
    }
}

void gms_output(iws,f) int iws; FILE *f;
{ wsdsc *ws;
  if (iws==-1)
    return;
  ws= &wstable[iws];
  if(ws->state==GMS_STATE_OPEN);
  { if (ws->demand_newoutput) ws->new_output=f;
     }

}

void gms_end(iws) int iws;
{ wsdsc *ws;
  if (iws== -1)
    { for(ws=first_open;ws!=NULL;ws=ws->next_open)
	if (ws->state==GMS_STATE_ACTIVE)
	  {   if (ws->ctrl(ws,GMS_CTRL_END))
		ws->state=GMS_STATE_OPEN;
	    }
    }
  else
    { ws= &wstable[iws];
      if (ws->state==GMS_STATE_ACTIVE)
	{   if (ws->ctrl(ws,GMS_CTRL_END))
	      ws->state=GMS_STATE_OPEN;
	  }
    }
}

void gms_hold()
{
  gms_flush();
  fgetc(stdin);
}

void gms_flush()
{ wsdsc *ws;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  {  if (ws->state==GMS_STATE_ACTIVE)
       { if (itria) gms3_flush_triangles(ws);
	 ws->ctrl(ws,GMS_CTRL_FLUSH);
       }
   }
}


void gms_clear()
{ wsdsc *ws;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
   if (ws->state==GMS_STATE_ACTIVE) ws->ctrl(ws,GMS_CTRL_CLEAR);
}




void gms_window(xmin,xmax,ymin,ymax)
double xmin;double xmax;double ymin;double ymax;
{ wsdsc *ws;
  gms2_wxmin=xmin;
  gms2_wxmax=xmax;
  gms2_wymin=ymin;
  gms2_wymax=ymax;
  if (EQ(ymax,ymin) || EQ(xmax,xmin)) return;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
    ws->initran2d(ws,gms2_vxmin,gms2_vxmax,gms2_vymin,gms2_vymax,
		  gms2_wxmin,gms2_wxmax,gms2_wymin,gms2_wymax);
}



void gms_viewport(xmin,xmax,ymin,ymax)
double xmin;double xmax;double ymin;double ymax;
{ wsdsc *ws;
  gms2_vxmin=xmin;
  gms2_vxmax=xmax;
  gms2_vymin=ymin;
  gms2_vymax=ymax;
  if (EQ(ymax,ymin) || EQ(xmax,xmin)) return;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
    ws->initran2d(ws,gms2_vxmin,gms2_vxmax,gms2_vymin,gms2_vymax,
		  gms2_wxmin,gms2_wxmax,gms2_wymin,gms2_wymax);
}



#ifndef PI
#define PI 3.141592683
#endif

#ifndef RESOLUTION
#define RESOLUTION 2048
#endif


static double gms3_wxmin=0.0,
              gms3_wxmax=1.0,
              gms3_wymin=0.0,
              gms3_wymax=1.0,
              gms3_wzmin=0.0,
              gms3_wzmax=1.0;
static   double gms3_alpha,gms3_beta;





void gms3_window(xmin,xmax,ymin,ymax,zmin,zmax)
double xmin; double xmax;
               double ymin; double ymax;
                 double zmin; double zmax;
{ wsdsc *ws;
  gms3_wxmin=xmin;
  gms3_wxmax=xmax;
  gms3_wymin=ymin;
  gms3_wymax=ymax;
  gms3_wzmin=zmin;
  gms3_wzmax=zmax;
  if (EQ(ymax,ymin) || EQ(xmax,xmin)|| EQ(zmax,zmin)) return;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  ws->initran3d(ws,gms3_alpha,gms3_beta,
  gms3_wxmin,  gms3_wxmax,  gms3_wymin,  gms3_wymax,  gms3_wzmin,  gms3_wzmax);

}

void gms3_perspective(alpha,beta)double alpha; double beta;
{ wsdsc *ws;
  gms3_alpha=alpha;
  gms3_beta=beta;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  ws->initran3d(ws,gms3_alpha,gms3_beta,
  gms3_wxmin,  gms3_wxmax,  gms3_wymin,  gms3_wymax,  gms3_wzmin,  gms3_wzmax);

}

/*--- Ausgaberoutinen ------------------------------------------------------*/



void gms_moveto(x,y)double x; double y;
{ wsdsc *ws;
  double wx,wy;
  gms2_xpos=x;gms2_ypos=y;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {
    if (ws->tran2d!=NULL) ws->tran2d(ws,&wx,&wy,x,y);
    ws->moveto(ws,wx,wy);
    ws->xpos=wx;ws->ypos=wy;
  }
}

void gms_lineto(x,y)double x; double y;
{ wsdsc *ws;
  double wx,wy;
  gms2_xpos=x;gms2_ypos=y;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {
    if (ws->tran2d!=NULL) ws->tran2d(ws,&wx,&wy,x,y);
     ws->lineto(ws,wx,wy);
     ws->xpos=wx;ws->ypos=wy;
  }
}

void gms_polyline(n,x,y)
     int n; real * x; real* y;
{ wsdsc *ws;
  double xx,yy;
  int i;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
    if (ws->state==GMS_STATE_ACTIVE)
      {  if (ws->tran2d!=NULL)
	   { ws->tran2d(ws,&xx,&yy,x[0],y[0]);
	     ws->moveto(ws,xx,yy);
	     ws->xpos=xx;ws->ypos=yy;
	     for(i=1;i<n;i++)
	       {
		 ws->tran2d(ws,&xx,&yy,x[i],y[i]);
		 ws->lineto(ws,xx,yy);

	 ws->xpos=xx;ws->ypos=yy;
	       }
	   }
      else
	{
	  ws->moveto(ws,x[0],y[0]);
	  ws->xpos=x[0];ws->ypos=y[0];
	  for(i=1;i<n;i++)
	    {
	      ws->lineto(ws,x[i],y[i]);
	      ws->xpos=x[i];ws->ypos=y[i];
	    }
	}
       }
  gms2_xpos=x[n-1];gms2_ypos=y[n-1];
}



void gms_marker(mktype)int mktype;
{ wsdsc *ws;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE) ws->marker(ws,mktype);
}


static double xfill[FILLSIZE],yfill[FILLSIZE],zfill[FILLSIZE];

void gms_fill(n,x,y)int n; double *x; double *y;
{ wsdsc *ws;
  int i;
  if (n>FILLSIZE)
  { fprintf(gms_errorfile,"GMS: Fill buffer exceeded. Recompile gms with -DFILLSIZE=%d\n",n);
    return;
  }
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  { if ( ws->tran2d!=NULL)
    { for (i=0;i<n;i++)  ws->tran2d(ws,xfill+i,yfill+i,x[i],y[i]);
      ws->fill(ws,n,xfill,yfill);
    }
  else
    ws->fill(ws,n,x,y);
  }
}

static int xalign=GMS_ALIGN_LEFT,yalign=GMS_ALIGN_BOTTOM;

void gms_aligntext(xa,ya) int xa,ya;
{xalign=xa;
 yalign=ya;
}

void gms_text(s)
char *s;
{ wsdsc *ws;
  int i;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE) ws->text(ws,s,xalign,yalign);

}




void gms_defcolor(r,g,b)
double r,g,b;
{ wsdsc *ws;
 double xr=r,xg=g,xb=b;

  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)     ws->defcolor(ws,xr,xg,xb);
}



void gms_setcolor(icol)
int icol;
{ wsdsc *ws;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)  ws->setcolor(ws,MAX(0,MIN(icol,ws->maxcolors-1)));
}

void gms_setisolevels(n,lev)
int n;
double *lev;
{ if (n<=1) return;
  if (lev==NULL) return;
  nisolevels=n;
  isolevels=lev;
}

void gmsilv(n,v)
     int *n; double*v;
{
  gms_setisolevels(*n,v);
  
}
void gms_setcolorscale(n,r,g,b,v)
double *r,*g,*b,*v;
int n;
{ wsdsc *ws;
  int i;
  float col[3],val,vh;
  if (r==NULL) return;
  if (b==NULL) return;
  if (g==NULL) return;
  if (v==NULL) return;
  colorscale_r=r;
  colorscale_b=b;
  colorscale_g=g;
  colorscale_v=v;
  colorscale_size=n;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
    if (ws->state>=GMS_STATE_OPEN)
      {  if ((ws->shaded_triangle2d==gms_generic_shaded_triangle2d)||
	     (ws->triangle3d==gms_generic_triangle3d))
	   {
	     ws->colorscale_size=MIN(ws->maxcolors-1,(MAX_COLORSCALE_SIZE-1));
	     val=colorscale_v[0];
	     vh= (colorscale_v[colorscale_size-1]-colorscale_v[0])/
	       ((double)ws->colorscale_size);
	     for (i=0;i<ws->colorscale_size;i++)
	       { getscalecolor(val,col);
		 ws->setcolor(ws,i+1);
		 ws->defcolor(ws,col[0],col[1],col[2]);
		 ws->colorscale_v[i]=val;
                 val+=vh;
	       }
	     ws->colorscale_v[i]=val;
	   }
       }

}

void gmscsc(n,r,g,b,v)
     int*n; double *r, *g,*b,*v;
{
  gms_setcolorscale(*n,r,g,b,v);
}

void    gms_shaded_triangle (xc,yc,f)
double *xc,*yc,*f;
{ wsdsc *ws;
  int i;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {  if (ws->tran2d!=NULL)
    {  for (i=0;i<3;i++)   ws->tran2d(ws,xfill+i,yfill+i,xc[i],yc[i]);
       ws->shaded_triangle2d (ws,xfill,yfill,f);
     }

  else
    ws->shaded_triangle2d (ws,xc,yc,f);
   }
}

void    gms_isoline_triangle (xc,yc,f)
double *xc,*yc,*f;
{ wsdsc *ws;
  int i;
if (isolevels==NULL) return;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {
  if (ws->tran2d!=NULL)
    {  for (i=0;i<3;i++)   ws->tran2d(ws,xfill+i,yfill+i,xc[i],yc[i]);
       ws->isoline_triangle2d (ws,xfill,yfill,f);
     }

  else
    ws->isoline_triangle2d (ws,xc,yc,f);
     }
}



void gms3_text(s)
char *s;
{ wsdsc *ws;
  int i;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {if(ws->text!=gms_generic_text) ws->text(ws,s,xalign,yalign);
  }
}

void    gms3_shaded_triangle (xc,yc,zc,f)
double *xc,*yc,*zc,*f;
{ wsdsc *ws;
  int i;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {  ws->shaded_triangle3d(ws,xc,yc,zc,f);
   }
}

void    gms3_triangle (xc,yc,zc)
double *xc,*yc,*zc;
{ wsdsc *ws;
  int i;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {  ws->triangle3d(ws,xc,yc,zc);
   }
}

#ifdef HIDDENLINE
void    gms3_hiddenline_triangle (xc,yc,zc)
double *xc,*yc,*zc;
{ wsdsc *ws;
  int i;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {  ws->hiddenline_triangle3d(ws,xc,yc,zc);
   }
}
#endif

static double gms3_xpos,gms3_ypos,gms3_zpos;

void gms3_moveto(x,y,z)double x; double y; double z;
{ wsdsc *ws;double w;
  gms3_xpos=x;gms3_ypos=y;gms3_zpos=z;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {
    ws->moveto3d(ws,x,y,z);
  }
}

void gms3_lineto(x,y,z)double x; double y; double z;
{ wsdsc *ws;double w;
  gms3_xpos=x;gms3_ypos=y; gms3_zpos=z;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {
      ws->lineto3d(ws,x,y,z);
   }
}

int gms_querycolors(iws) int iws;
{ wsdsc*ws;
  int ncol;

  if (iws== -1)
  { ncol=123456;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  {
   if (ws->maxcolors <ncol) ncol=ws->maxcolors;
  }
  if (ncol==123456) ncol=1;
  return ncol;
  }
  else
    {
  ws= &wstable[iws];
  if(ws->state>0)  return ws->maxcolors; else return 0;
}
}

int gms_querytype(iws) int iws;
{ wsdsc*ws;
  if (iws== -1) return 0;
  ws= &wstable[iws];
  if(ws->state>0)  return ws->wstype;
  else return -1;
}

double gms_querycharwidth(iws) int iws;
{ wsdsc*ws;
  if ((iws<0)||(iws>=WSTABLESIZE)) {fprintf(gms_errorfile,"%%GMS: no such ws %d",iws);
                                   return -1.0;}
  ws= &wstable[iws];
  if(ws->state>0) return ws->charwidth/ws->ax;
  else return -1.0;
}

void gms_setaspect(iws,asp) int iws; double asp;
{ wsdsc*ws;
  if ((iws<0)||(iws>=WSTABLESIZE)) {fprintf(gms_errorfile,"%%GMS: no such ws %d",iws);
                                   return;}
  ws= &wstable[iws];
  if(ws->state>0)  ws->asp=asp;
}

int gms_querystate(iws) int iws;
{ wsdsc*ws;
  if ((iws<0)||(iws>=WSTABLESIZE)) {fprintf(gms_errorfile,"%%GMS: no such ws %d",iws);
                                   return -1;}
  ws= &wstable[iws];
  return ws->state;
}


/*---------------------------------------------------------------------*/
int     gms_querypoint (iws,x,y)
int iws; double *x; double *y;
                /*inquire point coordinates from a mouse click (x11 required) */
{wsdsc *ws;
 ws= &wstable[iws];
  if (ws->state!=GMS_STATE_ACTIVE)
  { fprintf(gms_errorfile,"%%GMS: wrong state of ws %d",iws);
    return GMS_RETURN_ERROR;
   }
    ws->querypoint(ws,x,y);
    return GMS_RETURN_OK;
}

int     gms_queryline(iws,x0,y0,x1,y1)
int iws; double *x0; double *y0; double *x1; double *y1;
                /*inquire line coordinates from the mouse (x11 required) */
{
wsdsc *ws;
 ws= &wstable[iws];
  if (ws->state!=GMS_STATE_ACTIVE)
  { fprintf(gms_errorfile,"%%GMS: wrong state of ws %d",iws);
    return GMS_RETURN_ERROR;
   }
    ws->querypair(ws,0,x0,y0,x1,y1);
    return GMS_RETURN_OK;
}

int     gms_queryrectangle(iws,x0,x1,y0,y1)
int iws;double *x0;double *y0;double *x1;double *y1;
                /*inquire rectangle coordinates from the mouse (x11 required) */
{
wsdsc *ws;
 ws= &wstable[iws];
  if (ws->state!=GMS_STATE_ACTIVE)
  { fprintf(gms_errorfile,"%%GMS: wrong state of ws %d",iws);
                                   return GMS_RETURN_ERROR;
   }
       ws->querypair(ws,1,x0,y0,x1,y1);
    return GMS_RETURN_OK;
}


/*-----------------------------------------------------------------
-------------------------------------------------------------------
  FORTRAN INTERFACE
-------------------------------------------------------------------
-----------------------------------------------------------------*/
#ifdef vms
#include <descrip.h>
#endif
/* This is in uncritical in time, so do it always!*/
static char xbuf[GMS_BUFLEN];
char *  set_eos( char *s)
{ int i;
#ifdef vms
  struct dsc$descriptor_s  *sdsc =(struct dsc$descriptor_s *)s;
  s=sdsc->dsc$a_pointer;
#endif
  for(i=0;i<GMS_BUFLEN-1;i++)
    { xbuf[i]=s[i];
      if (s[i]=='\0') break;
      if ((s[i]=='\\')&&(s[i+1]=='0'))    break;
     }
  xbuf[i]='\0';
  return xbuf;
}

void  gmop(iws,deviceid,connid)
int *iws;   int *deviceid;int *connid;
{
 if (*connid==0)   gms_open(*iws,*deviceid,NULL);
 else   gms_open(*iws,*deviceid,set_eos((char*)connid));

}

void gmcl(iws) int *iws; {gms_close(*iws);}
void gmbeg(iws)int *iws; {gms_begin(*iws);}
void gmend(iws) int *iws; {gms_end(*iws);}
void gmhold(){gms_hold();}
void gmflush(){gms_flush();}
void gmclr() {gms_clear();}




void gmwn(xmin,xmax,ymin,ymax)
real *xmin;real *xmax;real *ymin;real *ymax;
{ gms_window((double)(*xmin),(double)(*xmax),(double)(*ymin),(double)(*ymax));
}

void gmvp(xmin,xmax,ymin,ymax)
real *xmin;real *xmax;real *ymin;real *ymax;
{ gms_viewport((double)(*xmin),(double)(*xmax),(double)(*ymin),(double)(*ymax));
}
void gmmove(x,y)real *x; real *y;
{
 gms_moveto((double)*x,(double)*y);
}

void gmline(x,y)real *x; real *y;
{
  gms_lineto((double)*x,(double)*y);
}

void gmpl(n,x,y) int *n; real * x; real* y;
{ wsdsc *ws;
  int i;
  double xx,yy;
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  if (ws->state==GMS_STATE_ACTIVE)
  {  ws->tran2d(ws,&xx,&yy,(double)x[0],(double)y[0]);
     ws->moveto(ws,xx,yy);
     ws->xpos=xx;ws->ypos=yy;
     for(i=1;i< *n;i++)
     {  ws->tran2d(ws,&xx,&yy,(double)x[i],(double)y[i]);
        ws->lineto(ws,xx,yy);
        ws->xpos=xx;ws->ypos=yy;
      }
  }
  gms2_xpos=(double)x[*n-1];gms2_ypos=(double)y[*n-1];
}


void gmmark(mktype)int *mktype;
{ gms_marker(*mktype);}

void gmfill(n,x,y)int *n; real *x; real *y;
{ wsdsc *ws;
  int i;
  if (*n>FILLSIZE)
  { fprintf(gms_errorfile,"Cannot fill ! Recompile gm with -DFILLSIZE=nnn");
    return;
  }
  for(ws=first_open;ws!=NULL;ws=ws->next_open)
  {  for (i=0;i< *n;i++)  ws->tran2d(ws, xfill+i,yfill+i,(double)x[i],(double)y[i]);
     ws->fill(ws,*n,xfill,yfill);
   }
}

void gmscol(icol)
int *icol;
{ gms_setcolor(*icol);
}

void gmdcol(r,g,b)
real *r,*g,*b;
{ gms_defcolor((double)*r,(double)*g,(double)*b);
}

void gmaltx(xalign,yalign) int *xalign,*yalign;
{ gms_aligntext(*xalign,*yalign);
}

void gmtext(s)
int *s;
{
 if (*s==0)  gms_text("");
 else gms_text(set_eos((char*)s));
}

void   gmqcl(iws,icol) int *iws; int *icol;
{ *icol=gms_querycolors(*iws);
}

void   gmqst(ws,ist)int *ws; int *ist;
{ *ist=gms_querystate(*ws);
}

void   gmqcw(ws,cw)int *ws; real  *cw;
{ *cw=(real)gms_querycharwidth(*ws);
}


void  gmsas (iws,asp)int *iws;real *asp;
{  gms_setaspect(*iws,*asp);
}

