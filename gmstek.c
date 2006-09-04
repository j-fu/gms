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

#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef TEK

/* Control sequence fragments and number conversion taken from DPLOT */

#define EMU     (GMS_DRIVER_TEK4014_EMU - GMS_DRIVER_TEK4014)


#define HX 0x20                /* bit pattern to OR over 5-bit data */
#define HY 0x20
#define LX 0x40
#define LY 0x60


#define UPPER5 (31<<5)
#define LOWER5 31
static void tek4014_lineto PARM((wsdsc *ws,double x, double y));

static void tek4014_lineto(ws,x,y)wsdsc *ws;
double x,y;
{  int ix=(int)x;
   int iy=(int)y;

        (void) putc((HY | (iy & UPPER5)>>5), ws->output);
        (void) putc((LY | (iy & LOWER5)), ws->output);
        (void) putc((HX | (ix & UPPER5)>>5), ws->output);
        (void) putc((LX | (ix & LOWER5)), ws->output);
}

static  void tek4014_moveto(ws,x,y) wsdsc *ws;
 double x,y;
{
        (void) putc('\035', ws->output);        /* into graphics */
        tek4014_lineto(ws,x,y);

}

#endif
int tek4014_ctrl(ws,ctrl) wsdsc *ws; int ctrl;
{
#ifdef TEK
 switch (ctrl)
    { case GMS_CTRL_OPEN : 
                    if ((strcmp(ws->connid,"")==0)||
                        (strcmp(ws->connid,"tty")==0)||
                        (strcmp(ws->connid,"stdout")==0))  
                        ws->output=stdout;
                    else 
                         ws->output=fopen(ws->connid,"w");

                    ws->asp=0.90;
                    ws->xmax=1024.0;
                    ws->ymax=780.0;
                    
                    ws->moveto=tek4014_moveto;
                    ws->lineto=tek4014_lineto;
		    if (!gms_fontload()) fprintf(gms_errorfile,"%%GMS: No text avaliable");
                    ws->ydir= 1;
                    break;

    case GMS_CTRL_BEGIN:
                        if (ws->mode==EMU)
                        fprintf(ws->output,"\33[?38h"); /*enter TEK mode*/

    case GMS_CTRL_CLEAR:

                           fprintf(ws->output,"\033\014");
                      /*                        1
                                             1. clear screen
                      */
                         fflush(ws->output);
       /*        sleep(1);  */
                          /* sleep 1 second to allow screen time to clear on real
                         tektronix terminals */
                        break;

  case GMS_CTRL_FLUSH:  
                     fflush(ws->output);
                      break;
  case GMS_CTRL_END:  
                         fprintf(ws->output,"\037");
                /*                   1
                           1. into alphanumerics
                */
                      fprintf(ws->output,"\33[?38l"); /*leave TEK mode*/
                     fflush(ws->output);
  case GMS_CTRL_CLOSE: if (ws->output != stdout) fclose(ws->output);
  default:
                 break;
  }
  return GMS_RETURN_OK;
#else
  return GMS_RETURN_OK;
#endif
}

