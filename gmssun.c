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
  SUNVIEW  
-----------------------------------------------------------------*/
#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef SUNVIEW
#include <suntool/sunview.h>
#include <suntool/canvas.h>

typedef struct {
                 Frame frame;
                 Canvas canvas;
                 Pixwin *pw;
                 int mmx,mmy;
} sunview_info;
static void sunview_moveto(ws,x,y)wsdsc *ws;
double x,y;
{
}

static void sunview_lineto(ws,x,y)wsdsc *ws;
double x,y;
{ sunview_info *info=(sunview_info*)ws->info;
  (void)pw_vector(info->pw, (int)ws->xpos,(int)ws->ypos, 
                            (int)x,(int)y, PIX_SRC, 1);
}

void sunview_text(ws,s)wsdsc*ws;
char *s;
{ sunview_info *info=(sunview_info*)ws->info;
  (void)pw_text(info->pw, (int)ws->xpos,(int)ws->ypos, PIX_SRC, NULL, s);
}
               


static SV_done;     /** ??? **/
static void canvasevent(canvas,event)
Canvas canvas;
Event *event;
{
  int ie;
  ie=event_action(event);
  if(ie==13) SV_done=1;
}

static void frameevent(frame,event)
Frame frame;
Event *event;
{
  int ie;
  ie=event_action(event);
  if(ie==MS_RIGHT) SV_done=1;
}
#endif

int sunview_ctrl(ws,action)wsdsc *ws;int action;
{
#ifdef  SUNVIEW
 int i;
 sunview_info *info=(sunview_info*)ws->info;

  switch (action)
  {case GMS_CTRL_OPEN :
         ws->asp=1.0;
         ws->moveto=sunview_moveto;
         ws->lineto=sunview_lineto;
	 if (!gms_fontload()) fprintf(gms_errorfile,"%%GMS: No text avaliable");
	 ws->ydir= -1;  
         ws->needhold=0;
         ws->info=info=NEW(sunview_info);
         info->frame= window_create(NULL,FRAME,
                              FRAME_LABEL,"GM OUTPUT",
                              WIN_CONSUME_KBD_EVENT, WIN_ASCII_EVENTS,
                              WIN_EVENT_PROC, frameevent,
                              0);
         info->canvas=window_create(info->frame,CANVAS,
                              WIN_CONSUME_KBD_EVENT, WIN_ASCII_EVENTS,
                              WIN_EVENT_PROC, canvasevent,
                              0);

         info->pw= (Pixwin*) canvas_pixwin(info->canvas);
     break;

   case GMS_CTRL_BEGIN: 
         info->mmx= (int) window_get(info->canvas, CANVAS_WIDTH);
         info->mmy= (int) window_get(info->canvas, CANVAS_HEIGHT);
         ws->xmax=(double)info->mmx; 
         ws->ymax=(double)info->mmy;
 
         window_set(info->frame,
                    WIN_HOLD,TRUE,
                    0);
         notify_dispatch(); 
   case GMS_CTRL_CLEAR:
         info->pw_writebackground(pw,0,0,info->mmx,info->mmy,0); 
     break;

   case GMS_CTRL_FLUSH :
         SV_done=0;
         while(!SV_done) notify_dispatch();
     break;

   case GMS_CTRL_END:
         window_set(info->frame, 
                    WIN_HOLD, FALSE,
                    0);
     break;
   case GMS_CTRL_CLOSE: /* window_delete(info->frame); */
                        free(info);
   default : break;

  };
  return GMS_RETURN_OK;
#else  
  return GMS_RETURN_ERROR;
#endif
}
 
