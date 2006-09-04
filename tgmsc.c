/* done by subst_insert_configh.sh */
#if HAVE_CONFIG_H
#include <config.h>
#endif
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
#include <math.h> 
#include <stdio.h>
#include <stdlib.h>
#include "gms.h"

static char xbuf[128];

void test_level1(int icol)
{   int i;
   double t,h,xx[FILLSIZE],yy[FILLSIZE];
  if (icol>0)
  {

    h=1.0/(icol/2-1);
    
    for(t=1.0,i=1;i<icol/2;i++,t-=h)
      {
	xx[0]=t-h; yy[0]=0.1;
	xx[1]=t-h; yy[1]=0.2;
	xx[2]=t; yy[2]=0.2;
	xx[3]=t; yy[3]=0.1;
	xx[4]=0.1; yy[4]=0.1;
	
	gms_setcolor(i);
	gms_defcolor(1.0-t, t,1.0); 
	gms_fill(4,xx,yy);
      }
    h=1.0/(icol-i);
    for(t=1.0;i<icol;i++,t-=h)
      {
	xx[0]=t-h; yy[0]=0.8;
	xx[1]=t-h; yy[1]=0.9;
	xx[2]=t; yy[2]=0.9;
	xx[3]=t; yy[3]=0.8;
	xx[4]=0.1; yy[4]=0.1;
	
	gms_setcolor(i);
	gms_defcolor(1.0,1.0-t,t); 
	gms_fill(4,xx,yy);
      }
}
else
{  xx[0]=0.3; yy[0]=0.1;
   xx[1]=0.3; yy[1]=0.2;
   xx[2]=0.7; yy[2]=0.2;
   xx[3]=0.7; yy[3]=0.1;
   xx[4]=0.3; yy[4]=0.1;
   gms_fill(4,xx,yy);
}
  
   gms_setcolor(0);
   gms_defcolor(0.0,0.0,0.0);
   gms_moveto(0.0,0.0);
   gms_lineto(0.0,1.0);
   gms_lineto(1.0,1.0);
   gms_lineto(1.0,0.0);
   gms_lineto(0.0,0.0);

   gms_moveto(0.0,0.5);
   gms_lineto(0.5,1.0);
   gms_lineto(1.0,0.5);
   gms_lineto(0.5,0.0);
   gms_lineto(0.0,0.5);

   h=2*3.1415/100.0;
   t=0.0;
   gms_moveto(1.0,0.5);
   for(;t<6.2833;t+=h) gms_lineto(0.5*cos(t)+0.5,0.5*sin(t)+0.5);

  gms_moveto(0.1,0.9);
  gms_marker(GMS_MARKER_PIXEL);
  gms_moveto(0.3,0.9);
  gms_marker(GMS_MARKER_DIAMOND);
  gms_moveto(0.5,0.9);
  gms_marker(GMS_MARKER_PLUS);
  gms_moveto(0.7,0.9);
  gms_marker(GMS_MARKER_X);
  gms_moveto(0.9,0.9);
  gms_marker(GMS_MARKER_ASTERISK);

  gms_moveto(0.5,0.4);
  gms_lineto(0.5,0.45);
  gms_marker(GMS_MARKER_UPARROW);
  gms_moveto(0.5,0.4);
  gms_lineto(0.5,0.35);
  gms_marker(GMS_MARKER_DOWNARROW);
  gms_moveto(0.5,0.4);
  gms_lineto(0.4,0.4);
  gms_marker(GMS_MARKER_LEFTARROW);
  gms_moveto(0.5,0.4);
  gms_lineto(0.6,0.4);
  gms_marker(GMS_MARKER_RIGHTARROW);

  gms_aligntext(GMS_ALIGN_CENTER,GMS_ALIGN_CENTER);
  gms_moveto(0.5,0.7);
  gms_text("abcdefghijklmnopqrstuvwxyz");
  gms_moveto(0.5,0.5);
  gms_text("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  gms_moveto(0.5,0.3);
  gms_text("&/+-_(){}[] 1234567890.:?!$%&");
  gms_moveto(0.5,0.2);
  sprintf(xbuf,"%d color(s)",icol);
  gms_text(xbuf);


  gms_aligntext(GMS_ALIGN_CENTER,GMS_ALIGN_TOP);
  gms_moveto(0.5,1.0);
  gms_text("0,1");
  gms_aligntext(GMS_ALIGN_RIGHT,GMS_ALIGN_CENTER);
  gms_moveto(1.0,0.5);
  gms_text("1,0");
  gms_aligntext(GMS_ALIGN_CENTER,GMS_ALIGN_BOTTOM);
  gms_moveto(0.5,0.0);
  gms_text("0,-1");
  gms_aligntext(GMS_ALIGN_LEFT,GMS_ALIGN_CENTER);
  gms_moveto(0.0,0.5);
  gms_text("-1,0");

}

typedef double (*Function) PARM((double x, double y));

static double f(x,y) double x,y;
{

return (exp(x*x)*sin(6.2832*y));

}

static double f1(x,y) double x,y;
{

return (exp(y*y)*sin(6.2832*x));

}

static void Create1DGrid(
int n,
double min,
double max,
double **coord)
{double h;
 double *c;
 int i;


c=malloc(n*sizeof(double));
*coord=c;
c[0]=min;
if (n==1) return;

h=(max-min)/((double)(n-1));

for (i=1;i<n;i++) c[i]=c[i-1]+h;
}

CreateFunction(
int nx,int ny,
double *xc,double *yc,double **uu,
Function f)
{  int i,j,l,n;
double *u;
    n=nx*ny;
   u=malloc(n*sizeof(double));
*uu=u;

   l=0;
     for   ( j=0; j<ny;  j++)
       for ( i=0; i<nx;  i++,l++)
	 u[l]=f(xc[i],yc[j]);  
   
}



static  double xd[5],yd[5],ud[5],vd[5];
static  double *xc,*yc,*u,*u1,umin=1.0,umax= -1.0,*isolevels,
         r[3]={0.0,0.5,1.0},
         g[3]={0.0,0.2,0.0},
         b[3]={1.0,0.5,0.0},v[3];

static  int nx=170;
static  int ny=160;
static  int nlev=32;
 
void test_level2_1(void)
{ 
  int i,j,k;

  gms_window(-1.0,1.0,-1.0,1.0);
  gms_setisolevels(nlev,isolevels); 
  v[0]=umin;
  v[1]=0.0;
  v[2]=umax;
  gms_setcolorscale(3,r,g,b,v); 

    for(j=0;j<ny-1;j++)
      for(i=0;i<nx-1;i++)
      { k=j*nx+i;
        xd[4]=xd[0]=xc[i];   yd[4]=yd[0]=yc[j];     ud[4]=ud[0]=u[k];
        xd[1]=xc[i+1]; yd[1]=yc[j];      ud[1]=u[k+1];
        xd[2]=xc[i+1]; yd[2]=yc[j+1];    ud[2]=u[k+nx+1];
        xd[3]=xc[i];   yd[3]=yc[j+1];    ud[3]=u[k+nx];
        gms_isoline_triangle(xd+2,yd+2,ud+2);
        gms_isoline_triangle(xd,yd,ud);
      }

}

void test_level2_2(void)
{ 
  int i,j,k;

  gms_window(-1.0,1.0,-1.0,1.0);
   gms_setcolorscale(3,r,g,b,v); 

 
    for(j=0;j<ny-1;j++)
      for(i=0;i<nx-1;i++)
      { k=j*nx+i;
        xd[4]=xd[0]=xc[i];   yd[4]=yd[0]=yc[j];     ud[4]=ud[0]=u[k];
        xd[1]=xc[i+1]; yd[1]=yc[j];      ud[1]=u[k+1];
        xd[2]=xc[i+1]; yd[2]=yc[j+1];    ud[2]=u[k+nx+1];
        xd[3]=xc[i];   yd[3]=yc[j+1];    ud[3]=u[k+nx];
        gms_shaded_triangle(xd,yd,ud);
        gms_shaded_triangle(xd+2,yd+2,ud+2);
      }

}


void test_level3_1(void)
{ 
  int i,j,k;
 
  gms3_window(0.0,1.5,0.0,1.5,0.0,1.5);
  gms3_perspective(40.0,20.0);
  gms_setcolor(0);
  gms_defcolor(0.0,0.0,0.0);
  gms3_moveto(0.0,0.0,0.0);
  gms3_lineto(1.5,0.0,0.0);gms3_text("x"); 
  gms3_moveto(0.0,0.0,0.0);
  gms3_lineto(0.0,1.5,0.0);gms3_text("y");
  gms3_moveto(0.0,0.0,0.0);
  gms3_lineto(0.0,0.0,1.5);gms3_text("z");

  gms3_moveto(0.0,0.0,0.0);
  gms3_lineto(1.0,0.0,0.0);
  gms3_lineto(1.0,1.0,0.0);
  gms3_lineto(0.0,1.0,0.0);
  gms3_lineto(0.0,0.0,0.0);

  gms3_moveto(0.0,0.0,1.0);
  gms3_lineto(1.0,0.0,1.0);
  gms3_lineto(1.0,1.0,1.0);
  gms3_lineto(0.0,1.0,1.0);
  gms3_lineto(0.0,0.0,1.0);

  gms3_moveto(1.0,0.0,0.0);
  gms3_lineto(1.0,0.0,1.0);
  gms3_moveto(0.0,1.0,0.0);
  gms3_lineto(0.0,1.0,1.0);
  gms3_moveto(1.0,1.0,0.0);
  gms3_lineto(1.0,1.0,1.0);
}

void test_level3_2(void)
{ 
  int i,j,k;
  gms3_perspective(40.0,-20.0);
  gms3_window(-1.0,1.0,-1.0,1.0,umin,umax);
    for(j=0;j<ny-1;j++)
      for(i=0;i<nx-1;i++)
      { k=j*nx+i;
        gms3_moveto(xc[i],yc[j],u[k]);
        gms3_lineto(xc[i+1],yc[j],u[k+1]);
        gms3_lineto(xc[i+1],yc[j+1],u[k+nx+1]);
        gms3_lineto(xc[i],yc[j+1],u[k+nx]);
        gms3_lineto(xc[i],yc[j],u[k]);
      }
}
/*
void test_level3_3h(void)
{ 
  int i,j,k;

  gms3_window(-1.0,1.0,-1.0,1.0,umin,umax);
    for(j=0;j<ny-1;j++)
      for(i=0;i<nx-1;i++)
      { k=j*nx+i;
        xd[4]=xd[0]=xc[i];   yd[4]=yd[0]=yc[j];     ud[4]=ud[0]=u[k];
        xd[1]=xc[i+1]; yd[1]=yc[j];      ud[1]=u[k+1];
        xd[2]=xc[i+1]; yd[2]=yc[j+1];    ud[2]=u[k+nx+1];
        xd[3]=xc[i];   yd[3]=yc[j+1];    ud[3]=u[k+nx];
        gms3_hiddenline_triangle(xd,yd,ud);
        gms3_hiddenline_triangle(xd+2,yd+2,ud+2);
      }


}

*/
void test_level3_3(void)
{ 
  int i,j,k;

  gms3_window(-1.0,1.0,-1.0,1.0,umin,umax);
    for(j=0;j<ny-1;j++)
      for(i=0;i<nx-1;i++)
      { k=j*nx+i;
        xd[4]=xd[0]=xc[i];   yd[4]=yd[0]=yc[j];     ud[4]=ud[0]=u[k];
        xd[1]=xc[i+1]; yd[1]=yc[j];      ud[1]=u[k+1];
        xd[2]=xc[i+1]; yd[2]=yc[j+1];    ud[2]=u[k+nx+1];
        xd[3]=xc[i];   yd[3]=yc[j+1];    ud[3]=u[k+nx];
        gms3_triangle(xd,yd,ud);
        gms3_triangle(xd+2,yd+2,ud+2);
      }


}

void test_level3_4(void)
{ 
  int i,j,k;

  v[0]=umin;
  v[1]=0.0;
  v[2]=umax;
  gms_setcolorscale(3,r,g,b,v); 
  gms3_window(-1.0,1.0,-1.0,1.0,umin,umax);
    for(j=0;j<ny-1;j++)
      for(i=0;i<nx-1;i++)
      { k=j*nx+i;
        xd[4]=xd[0]=xc[i];   yd[4]=yd[0]=yc[j];     ud[4]=ud[0]=u[k]; vd[4]=vd[0]=u1[k];
        xd[1]=xc[i+1]; yd[1]=yc[j];      ud[1]=u[k+1];    vd[1]=u1[k+1];    
        xd[2]=xc[i+1]; yd[2]=yc[j+1];    ud[2]=u[k+nx+1]; vd[2]=u1[k+nx+1];
        xd[3]=xc[i];   yd[3]=yc[j+1];    ud[3]=u[k+nx];   vd[3]=u1[k+nx];
        gms3_shaded_triangle(xd,yd,ud,vd);
        gms3_shaded_triangle(xd+2,yd+2,ud+2,vd+2);
      }


}






main(argc,argv)
int argc;char*argv[];
{ int ws;
  char * connid=NULL;
  int icol;

  if (argc<2) ws=0; else ws=atoi(argv[1]);
  if (argc>=3) connid=argv[2];

  gms_open(1,ws,connid);
  gms_open(2,120,"-O xxx");
  icol=gms_querycolors(-1);
  if (icol==0) return;

  gms_begin(-1);
  gms_clear();
  gms_viewport(0.05,0.95,0.05,0.95);
  test_level1(icol);
  gms_hold();
  gms_end(-1);

  Create1DGrid(nx,-1.0,1.0,&xc);
  Create1DGrid(ny,-1.0,1.0,&yc);
  CreateFunction(nx,ny,xc,yc,&u,f);
  CreateFunction(nx,ny,xc,yc,&u1,f1);
  if (umin>umax)
  {int i;
    umin=u[0];
    umax=u[0];
    for(i=1;i<nx*ny;i++)
    { if (u[i]<umin) umin=u[i];
      if (u[i]>umax) umax=u[i];
    }
  }

  Create1DGrid(nlev,umin,umax,&isolevels);

  
  gms_begin(1);
  gms_clear();
  test_level2_1();
  gms_hold();
  gms_end(1);

  gms_begin(1);
  gms_clear();
  test_level2_2();
  gms_hold();
  gms_end(1);

  gms_aligntext(GMS_ALIGN_LEFT,GMS_ALIGN_BOTTOM);
  gms_begin(1);
  gms_clear();
  gms_setcolor(0);
  gms_defcolor(0.0,0.0,0.0);
  test_level3_1();
  gms_hold();
  gms_end(1);

  gms_begin(1);
  gms_clear();
  gms_setcolor(0);
  gms_defcolor(0.0,0.0,0.0);
  test_level3_2();
  gms_hold();
  gms_end(1);

/*
  gms3_perspective(40.0,40.0);
  gms_begin(1);
  gms_clear();
  test_level3_3h();
  gms_hold();
  gms_end(1);
*/
  gms3_perspective(40.0,40.0);
  gms_begin(1);
  gms_clear();
  test_level3_3();
  gms_hold();
  gms_end(1);

  gms3_perspective(40.0,40.0);
  gms_begin(1);
  gms_clear();
  test_level3_4();
  gms_hold();
  gms_end(1);

  gms_begin(1);
  gms_clear();
  gms_viewport(0.05,0.95,0.05,0.95);
  gms_window(0.0,1.0,0.0,1.0);
  test_level1(icol);
  gms_hold();
  gms_end(1);

  gms_close(1);
}


