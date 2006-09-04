/* done by subst_insert_configh.sh */
#if HAVE_CONFIG_H
#include <config.h>
#endif
/*
 * $Id$
 * $Log$
 * Revision 1.5  2004/07/01 13:41:16  strecken
 * AUTOCONF
 *
 * Revision 1.4  2003/03/28 16:47:50  fuhrmann
 * pdelib1.15_alpha1
 *
 * Revision 1.3  2003/03/28 11:20:28  fuhrmann
 * pdelib2.0_alpha1
 *
 * Revision 1.2  1995/11/06 18:58:56  fuhrmann
 * BGI search first in home dir
 * gmserr -> gms_errorfile
 *
 * Revision 1.1  1995/10/20  15:22:26  fuhrmann
 * Initial revision
 *
 */

/*-----------------------------------------------------------------
-------------------------------------------------------------------
  Borland's Graphics Interface (Turbo C, Turbo C++)
-------------------------------------------------------------------
-----------------------------------------------------------------*/
#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef __TURBOC__
#include <graphics.h>
typedef struct { int mode;
                 int color;
                 int fill[2*FILLSIZE];
               } bgi_info;

static void bgi_marker(wsdsc* ws,int mktype)
{ int mks=(int)ws->markersize;
 switch(mktype)
 { case GMS_MARKER_PIXEL : putpixel((int)ws->xpos,(int)ws->ypos,1);  break;
   case GMS_MARKER_DIAMOND: line(ws->xpos-mks,ws->ypos,ws->xpos,ws->ypos+mks);
                           line(ws->xpos,ws->ypos+mks,ws->xpos+mks,ws->ypos);
                           line(ws->xpos+mks,ws->ypos,ws->xpos,ws->ypos-mks);
                           line(ws->xpos,ws->ypos-mks,ws->xpos-mks,ws->ypos);
                    break;
   case GMS_MARKER_PLUS  : line(ws->xpos-mks,ws->ypos,ws->xpos+mks,ws->ypos);
                          line(ws->xpos,ws->ypos-mks,ws->xpos,ws->ypos+mks);
                    break;
   case GMS_MARKER_X    :  line(ws->xpos-mks,ws->ypos-mks,ws->xpos+mks,ws->ypos+mks);
                          line(ws->xpos+mks,ws->ypos-mks,ws->xpos-mks,ws->ypos+mks);
                    break;
   case GMS_MARKER_ASTERISK:bgi_marker(ws,GMS_MARKER_X);
                           bgi_marker(ws,GMS_MARKER_PLUS);
   default : gms_generic_marker(ws,mktype);

  };
}

static void bgi_moveto(wsdsc *ws,double x, double y)
{ moveto((int)(x),(int)(y));
}

static void bgi_lineto(wsdsc *ws,double x, double y)
{lineto((int)(x),(int)(y));}


static void bgi_fill(wsdsc *ws,int n,double *x, double *y)
{ bgi_info*info=(bgi_info*)ws->info;
   int i,j;
  for (i=j=0;i<n;i++)
  {  info->fill[j++]=(int)x[i];
     info->fill[j++]=(int)y[i];
   }
  fillpoly(n,info->fill);
}

static void  bgi_setcolor(wsdsc *ws,int c)
{ bgi_info *info=(bgi_info*)ws->info;
  if (info->mode==HERCMONOHI) c=(c+1)%2;
  setcolor(c);
  setfillstyle(SOLID_FILL,c-1);
  info->color=c;
}

static void bgi_defcolor(ws,r,g,b) wsdsc*ws;
double r,g,b;
{/* int color;
  bgi_info *info=(bgi_info*)ws->info;
  color=(int)(4.0*r);
  setpalette(info->color,color);
*/
}


static void bgi_text(wsdsc *ws,char *s,int xalign,int yalign)
{ int xxa=LEFT_TEXT,yya=BOTTOM_TEXT;
  switch(xalign)
  { case GMS_ALIGN_LEFT   : xxa=LEFT_TEXT; break;
    case GMS_ALIGN_RIGHT  : xxa=RIGHT_TEXT; break;
    case GMS_ALIGN_CENTER : xxa=CENTER_TEXT; break;
    default:break;
  }
  switch(yalign)
  { case GMS_ALIGN_TOP    : yya=TOP_TEXT; break;
    case GMS_ALIGN_BOTTOM : yya=BOTTOM_TEXT; break;
    case GMS_ALIGN_CENTER : yya=CENTER_TEXT; break;
    default:break;
  }
  settextjustify(xxa,yya);
  outtext(s);
}

#endif
int bgi_ctrl(ws,action)
wsdsc* ws;int action;
{
#ifdef __TURBOC__
  int mode;
  int gdriver=DETECT;
  int xasp,yasp;
  char * gpath;
  bgi_info *info;

  info=(bgi_info*)ws->info;
  switch (action)
  { case GMS_CTRL_OPEN : detectgraph(&gdriver,&mode);
                  
                      /* search graphics driver in actual directory*/ 
                     initgraph(&gdriver,&mode,gpath);
                     if (graphresult()!=0)
                     { 
                       /* retry with BGI path */
                         gpath=getenv("BGIPATH");
                         if (gpath==NULL)
                         { 
                          fprintf(gms_errorfile,"no BGI path\n");
                          return GMS_RETURN_ERROR;
                         };

                       initgraph(&gdriver,&mode,gpath);
	               if (graphresult()!=0)
                       { 
                         fprintf(gms_errorfile,"BGI initialization failed\n");
                         return GMS_RETURN_ERROR;
                       }
		     }
                    info=NEW(bgi_info);
                    ws->info=info;
                    info->mode=mode;
                    getaspectratio(&xasp,&yasp);
                    ws->asp=((double)xasp/(double)yasp);
                    ws->xmax=getmaxx();
                    ws->ymax=getmaxy();
                    ws->ydir= -1;
                    ws->maxcolors=getmaxcolor();
                    ws->setcolor=bgi_setcolor;
                    ws->defcolor=bgi_defcolor;
                    ws->moveto=bgi_moveto;
                    ws->lineto=bgi_lineto;
                    ws->marker=bgi_marker;
                    if (!gms_fontload())  ws->text=bgi_text;
                    ws->fill=bgi_fill;
   case GMS_CTRL_END:   restorecrtmode(); break;
   case GMS_CTRL_BEGIN: setgraphmode(info->mode);
   case GMS_CTRL_CLEAR: cleardevice();
                        break;
   case GMS_CTRL_CLOSE: free(info);
                        closegraph();
   default :            break;
  };
  return GMS_RETURN_OK;
#else
  return GMS_RETURN_ERROR;
#endif
}

