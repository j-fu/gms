/* done by subst_insert_configh.sh */
#if HAVE_CONFIG_H
#include <config.h>
#endif
/*
 * $Id$
 * $Log$
 * Revision 1.9  2004/07/01 13:41:16  strecken
 * AUTOCONF
 *
 * Revision 1.8  2003/03/28 16:47:51  fuhrmann
 * pdelib1.15_alpha1
 *
 * Revision 1.7  2003/03/28 11:20:29  fuhrmann
 * pdelib2.0_alpha1
 *
 * Revision 1.6  2000/03/01 16:45:08  fuhrmann
 * color allocation problems appear to be understood
 *
 * Revision 1.5  2000/02/29 13:28:50  fuhrmann
 * Visual&Color choice becomes more clear
 *
 * Revision 1.4  2000/02/28 18:05:12  fuhrmann
 * interim test
 *
 * Revision 1.3  2000/02/28 13:15:58  fuhrmann
 * version vor Linux-Test
 *
 * Revision 1.2  1998/11/18 13:11:15  fuhrmann
 * misc stuff
 *
 * Revision 1.1  1995/10/20  15:21:38  fuhrmann
 * Initial revision
 *
 */

/*-----------------------------------------------------------------
-------------------------------------------------------------------
X11 Release >=4  (Xlib unter DEC Windows, OpenWindows, Motif)
-------------------------------------------------------------------
-----------------------------------------------------------------*/
/*-> Resources: *polygonshape (Convex...) 

*/

#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef X11


#ifdef __convexc__
#include <sys/types.h>
#endif
#ifndef vms
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#else
#include <decw$include:Xlib.h>
#include <decw$include:Xutil.h>
#include <decw$include:Xos.h>
#include <decw$include:Xatom.h>
#endif

#define X11_iwidth 40
#define X11_iheight 40
#define XUNDEF 0xfff9
static char X11_ibits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00,
   0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00, 0x00,
   0x20, 0x08, 0x00, 0x00, 0x00, 0x30, 0x08, 0x00, 0x00, 0x00, 0x10, 0x08,
   0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00,
   0x00, 0x18, 0x08, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x0c,
   0x08, 0x00, 0x00, 0x00, 0x04, 0x08, 0x00, 0x00, 0x00, 0x06, 0x08, 0x00,
   0x3e, 0x00, 0x02, 0x08, 0x00, 0x63, 0x00, 0x02, 0x08, 0x80, 0xc1, 0x00,
   0x02, 0x08, 0x80, 0x80, 0x01, 0x03, 0x08, 0xc0, 0x00, 0x83, 0x01, 0x08,
   0x60, 0x00, 0xc6, 0x00, 0x08, 0x20, 0x00, 0x7c, 0x00, 0x08, 0x20, 0x00,
   0x00, 0x00, 0x08, 0x30, 0x00, 0x00, 0x00, 0x08, 0x10, 0x00, 0x00, 0x00,
   0x08, 0x18, 0x00, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x08, 0x0c,
   0x00, 0x00, 0x00, 0x08, 0x06, 0x00, 0x00, 0x00, 0x08, 0x02, 0x00, 0x00,
   0x00, 0x08, 0x03, 0x00, 0x00, 0x00, 0x88, 0x01, 0x00, 0x00, 0x00, 0x88,
   0x00, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00,
   0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x20,
   0xfe, 0xff, 0xff, 0xff, 0x7f, 0x08, 0x00, 0x00, 0x00, 0x20, 0x08, 0x00,
   0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};



typedef struct{
                 Display * display;
                 Window window;
                 Screen *screen;
                 XSetWindowAttributes winattr;
                 XWindowAttributes gwinattr;
                 Pixmap icon_pixmap;
                 Pixmap pixmap;
                 XWMHints wm_hints;
                 XTextProperty window_name,icon_name;
                 XEvent event;
                 GC gc;
                 int winchg;
                 XGCValues values;
                 unsigned long valuemask;

                 char *displayname;
                 int screen_num;
                 char *poutput[1];
                 XPoint fillpoints[FILLSIZE];
                 XFontStruct *font_struct;

                 int default_depth;
                 XVisualInfo *default_visual;
                 XColor colorcell_def;  
                 Colormap default_cmap;
                 unsigned long *colors;
                 char *colorall;
                 XVisualInfo visual_info;
                 int class;
                 int actcolor;
                 int background_color;
                 int xmax,ymax;
                 
                 char * default_font;
                 int server_does_no_backing_store;
                 int wait4expose;
              } X11_info;

static XVisualInfo *get_visual_from_id( Display *dpy, int scr,int id)
{
  XVisualInfo temp,*xtemp;
  long mask;
  int n;
  int default_depth;
  int default_class;
  
   mask = VisualIDMask;
 
    temp.visualid = id;
  
  xtemp= XGetVisualInfo( dpy, mask, &temp, &n );
  return xtemp;

}

/* 
 * The following two routines have been borrowed and modified 
 * from the MESA distribution.
 * 
 */

/*
 * Try to get an X visual which matches the given arguments.
 */

static XVisualInfo *get_visual( Display *dpy, int scr,
			        unsigned int depth, int xclass )
{
  XVisualInfo temp;
  long mask;
  int n;
  int default_depth;
  int default_class;
  
  mask = VisualScreenMask | VisualDepthMask | VisualClassMask;
  temp.screen = scr;
  temp.depth = depth;
#if defined(__cplusplus) || defined(c_plusplus)
  temp.c_class = xclass;
#else
  temp.class = xclass;
#endif
  
  default_depth = DefaultDepth(dpy,scr);
#if defined(__cplusplus) || defined(c_plusplus)
  default_class = DefaultVisual(dpy,scr)->c_class;
#else
  default_class = DefaultVisual(dpy,scr)->class;
#endif
  
  if (depth==default_depth && xclass==default_class) {
    /* try to get root window's visual */
    temp.visualid = DefaultVisual(dpy,scr)->visualid;
    mask |= VisualIDMask;
  }
  
  return XGetVisualInfo( dpy, mask, &temp, &n );
}


static XVisualInfo *choose_x_visual( Display *dpy, int screen,
				     int min_depth )
{
  XVisualInfo *vis;
  int xclass, visclass;
  int depth;
  int rgba=1;

  
  /* Otherwise, search for a suitable visual */
  for (xclass=0;xclass<6;xclass++) 
    {
      switch (xclass) {
      case 0:  visclass = TrueColor;    break;
      case 1:  visclass = DirectColor;  break;
      case 2:  visclass = PseudoColor;  break;
      case 3:  visclass = StaticColor;  break;
      case 4:  visclass = GrayScale;    break;
      case 5:  visclass = StaticGray;   break;
      }
      if (min_depth==0) 
	{
	  /* start with shallowest */
	  for (depth=0;depth<=24;depth++) {
	    if (visclass==TrueColor && depth==8) {
	      /* Special case:  try to get 8-bit PseudoColor before */
	      /* 8-bit TrueColor */
	      vis = get_visual( dpy, screen, 8, PseudoColor );
	      if (vis) {
		return vis;
	      }
	    }
	    vis = get_visual( dpy, screen, depth, visclass );
	    if (vis) {
	      return vis;
	    }
	  }
	}
      else 
	{
	  /* start with deepest */
	  for (depth=24;depth>=min_depth;depth--) {
	    if (visclass==TrueColor && depth==8) {
	      /* Special case:  try to get 8-bit PseudoColor before */
	      /* 8-bit TrueColor */
	      vis = get_visual( dpy, screen, 8, PseudoColor );
	      if (vis) {
		return vis;
	      }
	    }
	    vis = get_visual( dpy, screen, depth, visclass );
	    if (vis) {
	      return vis;
	    }
	  }
	}
    }
  /* didn't find a visual */
  return NULL;
}

static void X11_moveto(ws,x,y) wsdsc*ws; double x,y;
{
}

static void X11_lineto(ws,x,y)wsdsc*ws; double x,y;
{
 X11_info *info=(X11_info*)ws->info;
 XDrawLine(info->display,info->pixmap,info->gc,
    (int)ws->xpos,(int)ws->ypos,(int)x,(int)y);
}

static void X11_marker(ws, mktype)wsdsc*ws; int mktype;
{ X11_info *info=(X11_info*)ws->info;
  if (mktype== GMS_MARKER_PIXEL)
     XDrawPoint(info->display,info->pixmap,info->gc,
                (int)ws->xpos,(int)ws->ypos);
   else gms_generic_marker(ws,mktype);
}

static void X11_fill(ws,n,x,y)wsdsc*ws; int n; double *x,*y;
{  register int i;
   X11_info *info=(X11_info*)ws->info;
   XPoint *fillpoints=info->fillpoints;
   for(i=0;i<n;i++)
   { fillpoints[i].x=(short)(x[i]);
     fillpoints[i].y=(short)(y[i]);
   }
   
   XFillPolygon(info->display,info->pixmap,info->gc,
		info->fillpoints,n,Convex,CoordModeOrigin);
}

static void X11_text(ws,str,xalign,yalign)wsdsc*ws; char *str;
int xalign,yalign;
{ X11_info *info=(X11_info*)ws->info;
  int txpos,typos;
  int length;
  int direction,ascent,descent;
  XCharStruct overall;
  length=(int)strlen(str);
  XTextExtents(info->font_struct,str,length,&direction,&ascent,&descent,&overall);
 switch (xalign)
  { case GMS_ALIGN_LEFT:txpos=(int)ws->xpos;break;
    case GMS_ALIGN_RIGHT:txpos=(int)ws->xpos-overall.width;break;
    case GMS_ALIGN_CENTER:txpos=(int)ws->xpos-overall.width/2;break;
    default:break;
   }
  switch (yalign)
  { case GMS_ALIGN_TOP:typos=(int)ws->ypos+overall.ascent;break;
    case GMS_ALIGN_BOTTOM:typos=(int)ws->ypos-overall.descent+1;break;
    case GMS_ALIGN_CENTER:typos=(int)ws->ypos+overall.ascent/2;break;
    default:break;
   }

  XDrawString(info->display, info->pixmap, info->gc,
            txpos,typos, str,(int)strlen(str));
}


static void X11_setcolor(ws,icol) wsdsc*ws; int  icol;
{ X11_info *info=(X11_info*)ws->info;
  info->actcolor=icol;
  XSetForeground( info->display, info->gc,info->colors[icol]);
}

static void X11_defcolor(ws,r,g,b) wsdsc*ws; 
double r,g,b;
{ X11_info *info=(X11_info*)ws->info;

  info->colorcell_def.flags=DoRed|DoGreen|DoBlue;
  info->colorcell_def.red= (short int)(r*65535.0);
  info->colorcell_def.green=(short int)(g*65535.0);
  info->colorcell_def.blue=(short int)(b*65535.0);

    if (info->colorall[info->actcolor]) 
    { 
      XSetForeground( info->display,  info->gc,
		      BlackPixel(info->display,info->screen_num));
      
      XFreeColors(info->display,info->default_cmap,
		  &info->colors[info->actcolor],1,0);
      info->colorall[info->actcolor]=0;
      info->colors[info->actcolor]=BlackPixel(info->display,info->screen_num);
    }
    
    if (XAllocColor(info->display,info->default_cmap,&info->colorcell_def))
    {
      info->colorall[info->actcolor]=1;
      info->colors[info->actcolor]=info->colorcell_def.pixel;
    }

  XSetForeground( info->display,  info->gc,info->colors[info->actcolor]);
}



void     X11_querypoint(ws,x,y)
wsdsc *ws; double *x; double *y;
{
 X11_info *info=(X11_info*)ws->info;
 XFlush(info->display);
 XWarpPointer(info->display,
		None,info->window,
		0,0,0,0,info->xmax/2,info->ymax/2);
 XSelectInput(
                       info->display,
                       info->window,
		       ButtonPressMask|	
                       StructureNotifyMask);
 for(;;){
	XNextEvent(info->display,&info->event);
        if (info->event.type==ConfigureNotify){           
                info->xmax=info->event.xconfigure.width;
                info->ymax=info->event.xconfigure.height;
                ws->xmax=(real)info->xmax;
                ws->ymax=(real)info->ymax;
        }else if (info->event.type==ButtonPress){
		*x = ((double) info->event.xbutton.x - ws->bx) / ws->ax;
		*y = ((double) info->event.xbutton.y - ws->by) / ws->ay;
		break;
	}
 }
 XSelectInput(
                       info->display,
                       info->window,
                       ExposureMask|
                       StructureNotifyMask);
 XSetFunction(info->display,info->gc,GXinvert);
 XSetFunction(info->display,info->gc,GXcopy);
}

void     X11_querypair(ws, modus, x0, y0, x1, y1)
wsdsc *ws; int modus;
		double *x0; double *y0; double *x1; double *y1;
{GC gcx;
 int xx0,yy0,xx1,yy1,istat=0,rold,gold,bold;
 X11_info *info=(X11_info*)ws->info;
 XFlush(info->display);
 gcx = info->gc;
 if(gcx==NULL){
	gcx=XCreateGC(info->display, info->pixmap,
                                       (unsigned long)0, &info->values);
	XSetForeground(info->display, gcx,
		WhitePixel(info->display,(info->screen_num)));
	XSetLineAttributes( info->display, gcx,
                                             0,
                                            LineSolid,
                                             CapRound,
                                            JoinRound 
                                          );
	info->gc = gcx;
 }else{
 }
/* gcx = DefaultGC(info->display,info->screen_num);
*/
/* XSetForeground( info->display,  gcx,   
                     info->colors[ws->maxcolors-1]);
*/
 XSetFunction(info->display,gcx,GXxor);
 XWarpPointer(info->display,
		None,info->window,
		0,0,0,0,info->xmax/2,info->ymax/2);
/* XGrabPointer(info->display,
		info->window,True,
	  	ButtonPressMask|	
		       ButtonReleaseMask|	
		       ButtonMotionMask|
                       StructureNotifyMask,
		GrabModeAsync,GrabModeAsync,
		None,None,CurrentTime);
*/
 XSelectInput(
                       info->display,
                       info->window,
		       ButtonPressMask|	
		       ButtonReleaseMask|	
		       ButtonMotionMask|/*PointerMotionHintMask|*/
                       StructureNotifyMask);
 for(;;){
	XNextEvent(info->display,&info->event);
        if (info->event.type==ConfigureNotify){           
                info->xmax=info->event.xconfigure.width;
                info->ymax=info->event.xconfigure.height;
                ws->xmax=(real)info->xmax;
                ws->ymax=(real)info->ymax;
        }else if (info->event.type==ButtonPress){
		xx0 = xx1 = info->event.xbutton.x;
		yy0 = yy1 = info->event.xbutton.y;
		if(modus){
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx0,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy0);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy1,xx1,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx1,yy0,xx1,yy1);
		}else{
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy1);
		}
		istat=1;
        }else if (info->event.type==MotionNotify){
		if(istat==0) continue;
		while(XCheckMaskEvent(info->display,ButtonMotionMask,
			&info->event));

		if(modus){
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx0,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy0);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy1,xx1,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx1,yy0,xx1,yy1);
		}else{
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy1);
		}
		xx1 = info->event.xmotion.x;
		yy1 = info->event.xmotion.y;
		if(modus){
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx0,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy0);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy1,xx1,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx1,yy0,xx1,yy1);
		}else{
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy1);
		}
                XClearWindow(info->display,info->window);
		XFlush(info->display);
        }else if (info->event.type==ButtonRelease){
 		if(istat==0) continue;
		if(modus){
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx0,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy0);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy1,xx1,yy1);
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx1,yy0,xx1,yy1);
		}else{
			XDrawLine(info->display,info->pixmap,gcx,
		 		xx0,yy0,xx1,yy1);
		}
                XClearWindow(info->display,info->window);
		XFlush(info->display);
		xx1 = info->event.xbutton.x;
		yy1 = info->event.xbutton.y;
	      	*x0 = ((double)xx0 - ws->bx) / ws->ax;
		*x1 = ((double)xx1 - ws->bx) / ws->ax;
	 	*y0 = ((double)yy0 - ws->by) / ws->ay;
  		*y1 = ((double)yy1 - ws->by) / ws->ay;
		if(modus){
			if(*x1 < *x0){double xx = *x0; *x0 = *x1; *x1 = xx;}
			if(*y1 < *y0){double yy = *y0; *y0 = *y1; *y1 = yy;}
		}
		break;
	}
 }
 XSelectInput(
                       info->display,
                       info->window,
                       ExposureMask|
                       StructureNotifyMask);
 XSetFunction(info->display,gcx,GXcopy);
/* XUngrabPointer(info->display,CurrentTime);*/
}

#endif
int X11_ctrl(ws,action)wsdsc*ws;int action;
{ 
#ifdef X11
  int i,maxcol;
  X11_info *info=(X11_info*)ws->info;
  char *buffer;
  int x0,y0; 
  switch (action)
  { case GMS_CTRL_OPEN :
      info=NEW(X11_info);
      if (info==NULL) return GMS_RETURN_ERROR;
      info->display=NULL;
      if (gms_getopt('D',ws->connid))
	info->display=XOpenDisplay(gms_optval);  
      if (info->display==NULL)
	{ /* backward compatibility */
	  if (gms_getopt('\0',ws->connid)) 
	    info->display=XOpenDisplay(gms_optval);  
	}
      if (info->display==NULL)
	{
	  buffer=getenv("DISPLAY");
	  if (buffer!=NULL)
	    info->display=XOpenDisplay(buffer);  
	  else
#ifdef vms
	    info->display=XOpenDisplay(NULL);
#else
	  info->display=XOpenDisplay(":0");
#endif
	}
      if (info->display==NULL) 
	{   fprintf(gms_errorfile,"%%GMS: cannot connect to X11 server %s\n",
		    ws->connid);
            fprintf(gms_errorfile,"     check your DISPLAY environment variable!\n");
	    free(info);
	    return GMS_RETURN_ERROR;
	  }
      
      ws->info=info;             
      info->displayname=NULL;     
      info->screen_num=DefaultScreen(info->display);
      info->screen=DefaultScreenOfDisplay(info->display);
      info->pixmap=0;
      
      if (getenv("GMS_VISID")!=NULL)
	{
	  info->default_visual=
	    get_visual_from_id(info->display,info->screen_num,
			       atoi(getenv("GMS_VISID")));
	}
      else
      info->default_visual=choose_x_visual(info->display,info->screen_num,1);

      info->class=info->default_visual->class;
      info->default_depth=info->default_visual->depth;
   
    if (MaxCmapsOfScreen(DefaultScreenOfDisplay(info->display))==1
	&& info->default_visual->visual==DefaultVisual(info->display,info->screen_num)) 
	info->default_cmap = DefaultColormap(info->display,info->screen_num);
    else
     info->default_cmap = XCreateColormap(info->display,
					   RootWindow(info->display,
						      info->screen_num), 
					   info->default_visual->visual,
					   AllocNone);      
      

      info->default_font=XGetDefault(info->display,"gms","font");

      buffer=XGetDefault(info->display,"gms","backingstore");
      if (buffer!=NULL) /* let the user decide */
	{
	  if (!strcmp(buffer,"client"))
	    info->server_does_no_backing_store=1;
	  else
	   info->server_does_no_backing_store=0;
	}
      else /* let the server decide */
	{
	  if (DoesBackingStore(DefaultScreenOfDisplay(info->display)))
	    info->server_does_no_backing_store=0;
	  else
	    info->server_does_no_backing_store=1;
	}
	

      buffer=XGetDefault(info->display,"gms","wait4expose");
      if ((buffer!=NULL)&&(!strcmp(buffer,"no")))
         info->wait4expose=0;
      else
       info->wait4expose=1;
    
     
      buffer=XGetDefault(info->display,"gms","colors");
      if (buffer!=NULL)
	ws->maxcolors=atoi(buffer);
      else
        ws->maxcolors=XMAX_COLORS;
      
      maxcol=1;
      for(i=0;i<info->default_depth;i++) maxcol*=2;
      maxcol=MIN(maxcol,info->default_visual->colormap_size)-2;
      ws->maxcolors=MIN(maxcol,ws->maxcolors);
					    

      if (ws->maxcolors>1)
	{ ws->setcolor=X11_setcolor;
	  ws->defcolor=X11_defcolor;
          info->colorall=malloc(ws->maxcolors);
          info->colors=(unsigned long*)malloc(ws->maxcolors*sizeof(unsigned long));
	  for(i=0;i<ws->maxcolors;i++) info->colorall[i]=0;
	  for(i=0;i<ws->maxcolors;i++) info->colors[i]=0;
	}

      ws->moveto=X11_moveto; 
      ws->lineto=X11_lineto;
      ws->marker=X11_marker;
      ws->querypair=X11_querypair;
      ws->querypoint=X11_querypoint;

      if (!gms_fontload()) ws->text=X11_text;
      ws->fill=X11_fill;
      ws->ydir= -1;
      ws->asp=(
	       ((double)DisplayHeightMM(info->display,info->screen_num)/
		(double)DisplayHeight(info->display,info->screen_num))/
	       ((double)DisplayWidthMM(info->display,info->screen_num)/
		(double)DisplayWidth(info->display,info->screen_num))
	       );
      
      
      
      
      /* Window Creation */
      XGetWindowAttributes(info->display,RootWindow(info->display,info->screen_num), &info->gwinattr);
      x0=y0=0;
      info->xmax=info->ymax=2*info->gwinattr.width/5;
      if (gms_getopt('W',ws->connid) ) info->xmax=atoi(gms_optval);
      if (gms_getopt('H',ws->connid) ) info->ymax=atoi(gms_optval);
      if (gms_getopt('X',ws->connid) ) x0=atoi(gms_optval);
      if (gms_getopt('Y',ws->connid) ) y0=atoi(gms_optval);
 
      if (gms_getopt('F',ws->connid)) info->default_font=gms_optval;
      if (info->default_font!=NULL)
      { info->font_struct=XLoadQueryFont(info->display,info->default_font);
        if (info->font_struct==NULL) info->default_font=NULL;
      }
	
      
        
      {
	XSetWindowAttributes wa;
	wa.colormap = info->default_cmap;
	wa.background_pixmap = None;
	wa.border_pixel = 0;
	wa.backing_store=Always;
	wa.save_under=True;
	wa.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
	  ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
	info->window=XCreateWindow(
				   info->display,
				   RootWindow(info->display,info->screen_num),
				   x0,                              
				   y0,
				   info->xmax,
				   info->ymax,
				   0,
				   info->default_depth, 
				   InputOutput,
				   info->default_visual->visual,
				   CWBackPixmap|CWBorderPixel|CWEventMask|CWColormap,
				   &wa);     
	
	info->colorcell_def.flags=DoRed|DoGreen|DoBlue;
	info->colorcell_def.red=65535;
	info->colorcell_def.green=65535;
	info->colorcell_def.blue=65535;
	
	XAllocColor(info->display,info->default_cmap,
			 &info->colorcell_def);        
			 
	info->background_color=(int)info->colorcell_def.pixel;
	XSetWindowBackground(info->display, info->window, info->background_color);
      }


      if ((void*)info->window==NULL) 
	{ fprintf(gms_errorfile,"%%GMS: Unable to create X11 window\n");
	abort();
	} 
      
      
      
      /* Icon Generation */   
      info->icon_pixmap=XCreateBitmapFromData(info->display,info->window,X11_ibits,X11_iwidth,X11_iheight);
      
      /* Title ... */
      if (!gms_getopt('T',ws->connid) )  strcpy(gms_optval,"gms");
      *info->poutput=gms_optval;
      XStringListToTextProperty(info->poutput,1,&info->window_name);
      XStringListToTextProperty(info->poutput,1,&info->icon_name);
      
      info->wm_hints.initial_state=NormalState;
      info->wm_hints.input=0;
      info->wm_hints.icon_pixmap=info->icon_pixmap;
      info->wm_hints.flags=StateHint|IconPixmapHint|InputHint;
      
      XSetWMProperties(info->display,
		       info->window,
		       &info->window_name,
		       &info->icon_name,
		       NULL,/*argv*/
		       0,   /*argc*/
		       NULL,
		       &info->wm_hints,
		       NULL);
      XSelectInput(info->display,info->window,ExposureMask|StructureNotifyMask);
      XMapWindow(info->display,info->window);
      if (info->wait4expose)
        for(;;)
	{ XNextEvent(info->display,&info->event);
	  if (info->event.type==Expose) break;
	  if (info->event.type==ConfigureNotify)
	    {            
	      info->xmax=info->event.xconfigure.width;
	      info->ymax=info->event.xconfigure.height;
	      ws->xmax=(real)info->xmax;
	      ws->ymax=(real)info->ymax;
	    }
	}

      info->winchg=1;
      if (info->server_does_no_backing_store)  info->gc=NULL;
      ws->xmax=(real)info->xmax;
      ws->ymax=(real)info->ymax;
      
    case GMS_CTRL_BEGIN:
      while(XCheckTypedEvent(info->display,ConfigureNotify,&info->event))
	{            
	  info->xmax=info->event.xconfigure.width;
	  info->ymax=info->event.xconfigure.height;
	  ws->xmax=(real)info->xmax;
	  ws->ymax=(real)info->ymax;
	  info->winchg=1;
	}
      
      if ((info->server_does_no_backing_store)&&(info->winchg))
	{ 

	  if (info->gc!=NULL)
	    {  XFreeGC(info->display,info->gc);
	    if (info->pixmap!=0) XFreePixmap(info->display,info->pixmap);
	    info->pixmap=0;
	    info->gc=NULL;
	    }
	  
	  info->pixmap=XCreatePixmap(info->display, DefaultRootWindow(info->display),
				     info->xmax, info->ymax,
				     info->default_visual->depth); 
	  info->winattr.background_pixmap=info->pixmap;
	  XChangeWindowAttributes(info->display,info->window,CWBackPixmap,&info->winattr);   
	  info->winchg=0; 
	}
      
      if (info->pixmap==0) info->pixmap=info->window;              
      
      
      if (info->gc==NULL)
	{
	  info->gc=XCreateGC(info->display, info->window,(unsigned long)0, &info->values);
	  if (info->default_font==NULL) 
	    info->font_struct=XQueryFont(info->display,XGContextFromGC(info->gc));
	  else
	    XSetFont(info->display,info->gc,info->font_struct->fid);
	  XSetLineAttributes( info->display, info->gc,  0, 
			      LineSolid, CapRound,  JoinRound );  
	  XSetFillRule(info->display,info->gc,WindingRule);
	}

      if (ws->maxcolors>1)
	{ 
	  ws->setcolor=X11_setcolor;
	  ws->defcolor=X11_defcolor;
          info->colorall=malloc(ws->maxcolors);
          info->colors=(unsigned long*)malloc(ws->maxcolors*sizeof(unsigned long));
	  for(i=0;i<ws->maxcolors;i++) info->colorall[i]=0;
	  for(i=0;i<ws->maxcolors;i++) info->colors[i]=0;
	}

      if (action!=GMS_CTRL_OPEN) break;                     
      
  case GMS_CTRL_CLEAR:
	    XSetForeground(info->display, info->gc, info->background_color);
	    XFillRectangle(info->display, info->pixmap, info->gc, 0, 0, info->xmax, info->ymax);
	    if (info->server_does_no_backing_store)
	      XClearWindow(info->display,info->window);
	    XFlush(info->display);

	 
	    if (action!=GMS_CTRL_OPEN) break;                     
  case GMS_CTRL_END:

      if (ws->maxcolors>1)
	for(i=0;i<ws->maxcolors;i++)
	  if (info->colorall[i]) 
	    {
	      XFreeColors(info->display,info->default_cmap,
			  &info->colors[i],1,0);
	    }  
      
      free(info->colorall);
      free(info->colors);

  case GMS_CTRL_FLUSH :

          if (info->server_does_no_backing_store)
	    XClearWindow(info->display,info->window);
 /* XClearWindow makes the background pixmap appear!!! */
    XFlush(info->display);


   break;
    case GMS_CTRL_CLOSE:
      
      XFlush(info->display);
      XFreeGC(info->display,info->gc);
      if (info->pixmap!=info->window)
	{
	  XFreePixmap(info->display,info->pixmap);
	
	}
      XDestroyWindow(info->display,info->window);
      XFlush(info->display);
      XCloseDisplay(info->display);
      free(info);
    default : break;
    }
  return GMS_RETURN_OK;
#else
  return GMS_RETURN_ERROR;
#endif
}



