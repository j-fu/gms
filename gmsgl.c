/* done by subst_insert_configh.sh */
#if HAVE_CONFIG_H
#include <config.h>
#endif
/*
 * $Id$
 * $Log$
 * Revision 1.4  2004/07/01 13:41:16  strecken
 * AUTOCONF
 *
 * Revision 1.3  2003/03/28 16:47:51  fuhrmann
 * pdelib1.15_alpha1
 *
 * Revision 1.2  2003/03/28 11:20:28  fuhrmann
 * pdelib2.0_alpha1
 *
 * Revision 1.1  1995/10/20 15:21:38  fuhrmann
 * Initial revision
 *
 */

/*----------------------------------------------------------------*/
/*------------- IRIS GL ------------------------------------------*/
/*----------------------------------------------------------------*/

#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef GL
#include <gl/gl.h>
#include <gl/device.h>
#ifndef GLCOLORS
#define GLCOLORS 64
#endif
#ifndef PI
#define PI 3.141592683
#endif
typedef struct{ int actcol;              
                long wnum;
                int xmax,ymax;
                float col[GLCOLORS][3];
                float xpos,ypos,zpos;
                Matrix matrix; int is3d;
              } GL_info;


static int lmbound=0;
static Matrix Identity = { 1, 0, 0, 0,  
                    0, 1, 0, 0,  
                    0, 0, 1, 0,  
                    0, 0, 0, 1 };

static float mat[] = {
	AMBIENT, 0.7, 0.5, 0.5,
	DIFFUSE, 0.7, 0.5, 0.5,
        ALPHA, 0.7,
        SHININESS,99.9,
	SPECULAR, 0.99, 0.99, 0.99,
	LMNULL
};

static float lm[] = {
	AMBIENT, .3, .3, .3,
	LOCALVIEWER, 0,
        TWOSIDE,1,
	LMNULL
};

static float lt[] = {
	POSITION, 1.0,-1.0, 1.0,  0.0,
     	LCOLOR, 0.8, 0.5, 0.8,
	LMNULL
};

/* Achtung :: Schlechte Erfahrung mit Skalierung in GL bei kleinen Werten !!! */

static void GL_triangle3d(ws,xc,yc,zc) wsdsc*ws; double *xc,*yc,*zc;
{

  float x1,x2,y1,y2,z1,z2,n[3];
  float v[3];



   x1=xc[1]-xc[0];
   y1=yc[1]-yc[0];
   z1=zc[1]-zc[0];
   x2=xc[2]-xc[0];
   y2=yc[2]-yc[0];
   z2=zc[2]-zc[0];

   n[0]=(y1*z2-z1*y2);
   n[1]=(z1*x2-x1*z2); 
   n[2]=(x1*y2-y1*x2); 

   
   
   bgntmesh();
   n3f(n);
      v[0]=xc[0];
      v[1]=yc[0];
      v[2]=zc[0];
   v3f(v);
      v[0]=xc[1];
      v[1]=yc[1];
      v[2]=zc[1];
   v3f(v);
      v[0]=xc[2];
      v[1]=yc[2];
      v[2]=zc[2];
   v3f(v);
   endtmesh();
   

          
} 

static void GL_shaded_triangle3d(ws,xc,yc,zc,f) wsdsc*ws; double *xc,*yc,*zc,*f;
{

  float v[3];
float col[3];
float x1,x2,y1,y2,z1,z2,n[3];


   
    
    bgntmesh();
      getscalecolor(f[0],col);
      v[0]=xc[0],v[1]=yc[0],v[2]=zc[0];
      c3f(col);
      v3f(v);
      getscalecolor(f[1],col);
      v[0]=xc[1];v[1]=yc[1];v[2]=zc[1];c3f(col);
      v3f(v);
      getscalecolor(f[2],col);
      v[0]=xc[2];v[1]=yc[2];v[2]=zc[2];c3f(col);
      v3f(v);
      endtmesh();
   

          
} 


static void GL_initran2d(ws,vxmin,vxmax,vymin,vymax,
			      wxmin,wxmax,wymin,wymax) 
     wsdsc *ws;
     double wxmin,wxmax,wymin,wymax,vxmin,vxmax,vymin,vymax; 
{ 
GL_info *info=(GL_info*)ws->info;

  ws->vxmin =MAX(0,vxmin*(ws->xmax));
  ws->vxmax =MIN(ws->xmax,vxmax*(ws->xmax));
  ws->vymin =MAX(0,vymin*(ws->ymax));
  ws->vymax =MIN(ws->ymax,vymax*(ws->ymax));
  ortho2((Coord)wxmin,(Coord)wxmax,(Coord)wymin,(Coord)wymax);
  ws->markersize=0.0075*(wxmax-wxmin);
  ws->textsize=0.02*(wxmax-wxmin);
  
 viewport((Screencoord)ws->vxmin,(Screencoord)ws->vxmax,
             (Screencoord)ws->vymin,(Screencoord)ws->vymax);

info->is3d=0;
}

static void GL_moveto(ws,x,y) wsdsc*ws;double x,y;
{
}

static Matrix GL_Identity = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };


void GL_initran3d(ws,alpha,beta,wxmin,wxmax,wymin,wymax,wzmin,wzmax) 
     wsdsc *ws;
     double wxmin,wxmax,wymin,wymax,wzmin,wzmax,alpha,beta;
{ 
GL_info *info=(GL_info*)ws->info;
double scalfac;
  
    if (!lmbound)
      {
	mmode(MVIEWING);  
        nmode(NNORMALIZE);             

	lmdef(DEFLIGHT, 3, 10, lt);
	lmdef(DEFLMODEL, 2, 9, lm);
	lmdef(DEFMATERIAL, 1, 17, mat);


	lmbind(LIGHT0, 3);
	lmbind(LMODEL, 2);
	lmbind(MATERIAL, 1);
        lmbound=1;
        zfunction(ZF_LESS); 
        shademodel(GOURAUD);
	lsetdepth(getgdesc(GD_ZMIN), getgdesc(GD_ZMAX));
	zbuffer(TRUE);
        czclear(0xffffff, getgdesc(GD_ZMAX));
	gconfig();
      }
   czclear(0xffffff, getgdesc(GD_ZMAX));
  
  alpha=((double)((int)alpha%360    ));
  beta= ((double)((int)beta %360));

  scalfac=MAX((wxmax-wxmin),(wymax-wymin));
  
 
  loadmatrix(GL_Identity);
  viewport((Screencoord)0.0,(Screencoord)ws->xmax,
             (Screencoord)0.0,(Screencoord)ws->ymax);

  perspective(300,1.0,1.5,4.0);
  lookat(-0.5,-3.0,-0.5, 0.0,0.0,0.0,2250);

 

 rot(beta,'x'); 
 rot(-alpha,'z');
 scale( 1.0/scalfac,
        1.0/scalfac,
        1.0/(wzmax-wzmin));

  translate( -(wxmin+wxmax)/2.0,
             -(wymin+wymax)/2.0,
             -(wzmin+wzmax)/2.0);
 info->is3d=1;
      
}

void GL_moveto3d(ws,x,y,z) 
double x,y,z;
wsdsc *ws;
{
 GL_info *info=(GL_info*)ws->info;
     info->xpos=x;info->ypos=y;info->zpos=z;
    
}

void GL_lineto3d(ws,x,y,z) 
double x,  y, z;
wsdsc *ws;
{
float vert[3];
 GL_info *info=(GL_info*)ws->info;
 
 bgnline();
  vert[0]=info->xpos;vert[1]=info->ypos;vert[2]=info->zpos;
  v3f(vert);
  vert[0]=x;vert[1]=y;vert[2]=z;
  v3f(vert);
 endline();
 info->xpos=x;info->ypos=y;info->zpos=z;

}


void GL_lineto(ws,x,y)wsdsc*ws; double x,y;
{float vert[2];
 GL_info *info=(GL_info*)ws->info;
 bgnline();
  vert[0]=ws->xpos;vert[1]=ws->ypos;
  v2f(vert);
  vert[0]=x;vert[1]=y;
  v2f(vert);
 endline();
   
}

static void GL_text(ws,str, xalign,yalign) wsdsc *ws;char *str;int xalign,yalign;
{ GL_info *info=(GL_info*)ws->info;
  if (info->is3d) 
  { cmov(info->xpos,info->ypos,info->zpos);
    charstr(str);
  }
  else gms_generic_text(ws,str,xalign,yalign);
}

static void GL_marker(ws, mktype)wsdsc*ws; int mktype;
{float vert[2];
 GL_info *info=(GL_info*)ws->info;
  if (mktype== GMS_MARKER_PIXEL)
  {  bgnpoint();
       vert[0]=ws->xpos;vert[1]=ws->ypos;
        v2f(vert);
     endpoint();
   }
  
   else gms_generic_marker(ws,mktype);
}

static void GL_fill(ws,n,x,y)wsdsc*ws; int n; double *x,*y;
{float vert[2];
 int i;
 GL_info *info=(GL_info*)ws->info;
 bgnpolygon();
  for(i=0;i<n;i++)
  { vert[0]=x[i];
    vert[1]=y[i];
    v2f(vert);
  } 
 endpolygon();  
}


static void GL_setcolor(ws,icol) wsdsc*ws; int icol;
{ GL_info *info=(GL_info*)ws->info;
  info->actcol=icol;
  c3f(info->col[icol]);
}



static void GL_shaded_triangle2d(ws,xc,yc,f) wsdsc*ws; double *xc,*yc,*f;
{ GL_info *info=(GL_info*)ws->info;
  float vert[2];
  float col[3];
  int i;
 
 bgnpolygon();
  for(i=0;i<3;i++)
  { getscalecolor(f[i],col);
    c3f(col);
    vert[0]=(float)xc[i];
    vert[1]=(float)yc[i];
    v2f(vert);
  } 
 endpolygon();  


}


static void GL_defcolor(ws,r,g,b) wsdsc*ws; 
double r,g,b;
{ GL_info *info=(GL_info*)ws->info;
  info->col[info->actcol][0]=r;
  info->col[info->actcol][1]=g;
  info->col[info->actcol][2]=b;
  c3f(info->col[info->actcol]);
}


#endif
int GL_ctrl(ws,action)wsdsc*ws;int action;
{ 
#ifdef GL
int i;
  GL_info *info=(GL_info*)ws->info;
  char *disp;
 
 switch (action)
  { case GMS_CTRL_OPEN :
                   info=NEW(GL_info);
                   if (info==NULL) return GMS_RETURN_ERROR;
                    
                   ws->info=info;             
                   ws->maxcolors=GLCOLORS;
                   ws->setcolor=GL_setcolor;
                   ws->defcolor=GL_defcolor;
                   ws->initran2d=GL_initran2d;
                   ws->initran3d=GL_initran3d;
                   ws->tran2d=NULL;

		   ws->triangle3d=GL_triangle3d;
		   ws->shaded_triangle2d=GL_shaded_triangle2d;
        	   ws->shaded_triangle3d=GL_shaded_triangle3d;
                  
                   ws->moveto=GL_moveto; 
                   ws->lineto=GL_lineto;
                   ws->moveto3d=GL_moveto3d; 
                   ws->lineto3d=GL_lineto3d;
		   if (!gms_fontload()) fprintf(gms_errorfile,"%%GMS: No text avaliable");
                   ws->text=GL_text;
                   ws->fill=GL_fill;
                   ws->ydir= 1.0;
                   ws->asp=1.0;

/* Window Creation */
                   info->xmax=600;
                   info->ymax=600;
                   if (gms_getopt('W',ws->connid) ) info->xmax=atoi(gms_optval);
                   if (gms_getopt('H',ws->connid) ) info->ymax=atoi(gms_optval);
                   prefsize(info->xmax,info->ymax);
                   foreground();
                   if (gms_getopt('T',ws->connid) )
                      info->wnum=winopen(gms_optval);
                   else 
                      info->wnum=winopen("gms");
                    if (info->wnum==NULL) 
                    { fprintf(gms_errorfile,"%%GMS: Unable to create GL window\n");
                       abort();
                    } 
                       info->is3d=0;
                     ws->xmax=(double)info->xmax;
                     ws->ymax=(double)info->ymax;
                     if (ws->mode) doublebuffer(); else singlebuffer(); 
                     RGBmode();
                     shademodel(GOURAUD);
                     mmode(MVIEWING);  
                     gconfig();
                                                        
                     ws->setcolor(ws,0);
                     ws->defcolor(ws,1.0,1.0,1.0);
                      clear(); 
                     swapbuffers();
                     gflush();                         
                     ws->setcolor(ws,1);
                     ws->defcolor(ws,0.0,0.0,0.0);
                     
case GMS_CTRL_CLEAR:winset(info->wnum);
                     ws->setcolor(ws,0);
                     ws->defcolor(ws,1.0,1.0,1.0);
                     viewport((Screencoord)0,(Screencoord)ws->xmax,
                              (Screencoord)0,(Screencoord)ws->ymax);
                     clear(); 
                     ws->setcolor(ws,0);
                     ws->defcolor(ws,0.0,0.0,0.0);
                     break;
case GMS_CTRL_FLUSH: winset(info->wnum);
                      swapbuffers();
                     gflush(); 

case GMS_CTRL_BEGIN: winset(info->wnum);
                      ws->setcolor(ws,0);
                      
                      break; 

case GMS_CTRL_END:  
                    break;
case GMS_CTRL_CLOSE:
                winset(info->wnum);
                winclose(info->wnum);
                free(info);
                default : break;
  }
 return GMS_RETURN_OK;
#else
 return GMS_RETURN_ERROR;
#endif

}
