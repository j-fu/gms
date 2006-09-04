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
 * Revision 1.2  2003/03/28 11:20:29  fuhrmann
 * pdelib2.0_alpha1
 *
 * Revision 1.1  1995/10/20 15:22:26  fuhrmann
 * Initial revision
 *
 */

/*-----------------------------------------------------------------
-------------------------------------------------------------------
Microsoft C Compiler, only VGA 16 color supported
-------------------------------------------------------------------
-----------------------------------------------------------------*/
#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef __MSC__
#include <graph.h>
typedef struct { int mode;
                 short color;
                 double fill[2*FILLSIZE];
		 struct _fontinfo fi;
		 long palette[16];
               } msc_info;

static void msc_line(double x1,double y1,double x2,double y2)
{ _moveto_w(x1,y1); _lineto_w(x2,y2);} 

static void msc_marker(wsdsc* ws,int mktype)
{ int mks=(int)ws->markersize;
  msc_info*info=(msc_info*)ws->info;
  double *work=info->fill;
 switch(mktype)
 { case GMS_MARKER_PIXEL : _setpixel_w(ws->xpos,ws->ypos);  break;
   case GMS_MARKER_DIAMOND: work[0]=ws->xpos-mks;
			    work[1]=ws->ypos;	  
			    work[2]=ws->xpos;	  
			    work[3]=ws->ypos+mks;	  
			    work[4]=ws->xpos+mks;	  
			    work[5]=ws->ypos;	  
			    work[6]=ws->xpos;	  
			    work[7]=ws->ypos-mks;	  
			    work[8]=work[0];	  
			    work[9]=work[1];	  
			  _polygon_w(_GBORDER,work,(short)5);
                    break;
   case GMS_MARKER_ASTERISK:
   case GMS_MARKER_PLUS  : msc_line(ws->xpos-mks,ws->ypos,
				    ws->xpos+mks,ws->ypos);
                           msc_line(ws->xpos,ws->ypos-mks,
				    ws->xpos,ws->ypos+mks);
                    if (mktype==GMS_MARKER_PLUS) break;
   case GMS_MARKER_X    :  msc_line(ws->xpos-mks,ws->ypos-mks,
				    ws->xpos+mks,ws->ypos+mks);
                           msc_line(ws->xpos+mks,ws->ypos-mks,
				    ws->xpos-mks,ws->ypos+mks);
                    break;
   default : gms_generic_marker(ws,mktype);

  };
}

static void msc_moveto(wsdsc *ws,double x, double y)
{ _moveto_w(x,y);}

static void msc_lineto(wsdsc *ws,double x, double y)
{ _lineto_w(x,y);}


static void msc_fill(wsdsc *ws,int n,double *x, double *y)
{ msc_info*info=(msc_info*)ws->info;
   int i,j;
  for (i=j=0;i<n;i++)
  {  info->fill[j++]=x[i];
     info->fill[j++]=y[i];
   }
  _polygon_w(_GFILLINTERIOR,info->fill,n);
}

static void  msc_setcolor(wsdsc *ws,int c)
{ msc_info *info=(msc_info*)ws->info;
  if (info->mode==_HERCMONO) c=(c+1)%2;
  _setcolor(c);
  info->color=c;
}
static long msc_rgb(r,g,b) double r,g,b;
{
long rgb;
short ih;
ih=floor(64.0*b); if (ih>63) ih=63; 
rgb=ih;
ih=floor(64.0*g); if (ih>63) ih=63; 
rgb*=256; rgb+=ih;
ih=floor(64.0*r); if (ih>63) ih=63; 
rgb*=256; rgb+=ih;
return rgb;
}

static void msc_defcolor(ws,r,g,b) wsdsc*ws;
double r,g,b;
{ msc_info *info=(msc_info*)ws->info;
info->palette[info->color]=msc_rgb(r,g,b);
_remappalette(info->color,info->palette[info->color]);
}

static void msc_text(wsdsc *ws,char *s,int xalign,int yalign)
{ double xxa=ws->xpos, yya=ws->ypos;
  switch(xalign)
  { case GMS_ALIGN_LEFT   : break;
    case GMS_ALIGN_RIGHT  : xxa-=_getgtextextent(s); break;
    case GMS_ALIGN_CENTER : xxa-=0.5*_getgtextextent(s); break;
    default:break;
  }
  switch(yalign)
  { case GMS_ALIGN_TOP    : break;
    /* don't work on vector fonts */
    case GMS_ALIGN_BOTTOM : yya-=((msc_info *)(ws->info))->fi.pixheight; break;
    case GMS_ALIGN_CENTER : yya-=0.5*((msc_info *)(ws->info))->fi.ascent; break;
    default:break;
  }
  _moveto_w(xxa,yya);
  _outgtext(s);
}
#endif
int msc_ctrl(ws,action)

wsdsc* ws;int action;
{
#ifdef __MSC__
  int mode, regf;
  msc_info *info;
  struct videoconfig *vc;
  double wxmax,wymax;
  char *fonpath;
  short i;

  info=(msc_info*)ws->info;
  switch (action)
  { case GMS_CTRL_OPEN :
		    if (FILLSIZE<5)
  			{ fprintf(gms_errorfile,
	"%%GMS: Fill buffer exceeded. Recompile gms with -DFILLSIZE=%d\n",5);
			return GMS_RETURN_ERROR;
			}
		    mode=_VRES16COLOR;
		    _setvideomode(mode);
		    if (_grstatus < _GROK)
		    { fprintf(gms_errorfile,"Setvideomode to VRES16COLOR failed\n");
                      return GMS_RETURN_ERROR;
		    }
		    vc=NEW(struct videoconfig);
		    _getvideoconfig(vc);
                    info=NEW(msc_info);
                    ws->info=info;
                    info->mode=mode;
		    info->palette[0]=	_BRIGHTWHITE  ;
		    info->palette[1]=	_BLACK        ;
		    info->palette[2]=	_BLUE         ;
		    info->palette[3]=	_GREEN        ;
		    info->palette[4]=	_CYAN         ;
		    info->palette[5]=	_RED          ;
		    info->palette[6]=	_MAGENTA      ;
		    info->palette[7]=	_BROWN        ;
		    info->palette[8]=	_WHITE        ;
		    info->palette[9]=	_GRAY         ;
		    info->palette[10]=	_LIGHTBLUE    ;
		    info->palette[11]=	_LIGHTGREEN   ;
		    info->palette[12]=	_LIGHTCYAN    ;
		    info->palette[13]=	_LIGHTRED     ;
		    info->palette[14]=	_LIGHTMAGENTA ;
		    info->palette[15]=	_YELLOW       ;
                    ws->asp=1.0*vc->numypixels/vc->numxpixels;  
                    ws->xmax=vc->numxpixels -1;
                    ws->ymax=vc->numypixels -1;
                    ws->ydir= -1; /* Koord. Urspr. links oben */
                    ws->maxcolors=vc->numcolors;
                    ws->setcolor=msc_setcolor;
                    ws->defcolor=msc_defcolor;
                    ws->moveto=msc_moveto;
                    ws->lineto=msc_lineto;
                    ws->marker=msc_marker;
		    regf=_registerfonts("HELVB.FON");
		    if (regf <= 0)
			{
			fonpath=malloc(_MAX_PATH+1);
                    	strcpy(fonpath,getenv("MSCFONPATH"));
			strcat(fonpath,"\\HELVB.FON");
			regf=_registerfonts(fonpath);
			free(fonpath);
			}
		    if (regf <= 0)
				fprintf(gms_errorfile,"%s%s","HELVB.FON not found,",
				" gms_generic textdriver used\n");
				else
				if (_setfont("t'helv'h15w8b")>= 0)
					{ 
					if (_getfontinfo(&(info->fi)))
						{
						fprintf(gms_errorfile,"%s%s",
						"can't get fontinfo,"
						" gms_generic textdriver used\n");
						_unregisterfonts();
						}
						else
						{
						ws->text=msc_text;
						ws->charwidth=info->fi.avgwidth;
						}
					}
					else
					{
					fprintf(gms_errorfile,"%s%s","can't set font,",
					" gms_generic textdriver used\n");
					_unregisterfonts();
                                        if (!gms_fontload())
                                        fprintf(gms_errorfile,"%s%s","can't set font,",
					" gms_dummy textdriver used\n");
		     					}
                    ws->fill=msc_fill;
		    free(vc);
   case GMS_CTRL_END:   _setvideomode(_DEFAULTMODE); break;
   case GMS_CTRL_BEGIN: _setvideomode(info->mode);
			/* world coordinates to physical coordinates*/
			wxmax=ws->xmax; wymax=ws->ymax;
			_setwindow((short)0,(double)0.0,(double)0.0,
					wxmax,wymax);
			_remapallpalette(info->palette);
			_setcolor(1);
			info->color=1;
			break;
   case GMS_CTRL_CLEAR: _clearscreen(_GCLEARSCREEN);
                        break;
   case GMS_CTRL_CLOSE: free(info);
			if (ws->text == msc_text)
				_unregisterfonts();
   default :            break;
  };
  return GMS_RETURN_OK;
#else
  return GMS_RETURN_ERROR;
#endif
}
