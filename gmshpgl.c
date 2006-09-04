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
 * Revision 1.1  1995/10/20 15:22:26  fuhrmann
 * Initial revision
 *
 */

#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef HPGL
#define HPCOLORS 6
typedef struct { int picno; 
                 char filename[32];
               } hpgl_info;

static void hpgl_moveto(ws,x,y) wsdsc *ws;double x,y;
{  fprintf(ws->output,"PU%8d,%8d;\n",(int)x,(int)y);
}

static void hpgl_lineto(ws,x,y) wsdsc *ws;double x,y;
{ fprintf(ws->output,"PD%8d,%8d;\n",(int)x,(int)y);
}

static void hpgl_setcolor(ws,icol) wsdsc*ws; int icol;
{  fprintf(ws->output,"PU;SP%d;\n",icol);
}
#endif
int hpgl_ctrl(ws,action)wsdsc *ws;int action;
{
#ifdef HPGL
 hpgl_info *info=(hpgl_info*) ws->info;
  switch (action)
  { case GMS_CTRL_OPEN :
                    if (!(gms_getopt('\0',ws->connid)||gms_getopt('O',ws->connid)))
                    ws->output=stdout;
                    else ws->output=NULL;

                    ws->info=info=NEW(hpgl_info);
                    ws->asp=1.0;
                    ws->xmax=9000; /*originally 15200*/
                    ws->ymax=9000; /*originally 10000*/                    
                    ws->moveto=hpgl_moveto; 
                    ws->lineto=hpgl_lineto;
                    if (!gms_fontload())fprintf(gms_errorfile,"%%GMS: No text avaliable");
                    ws->setcolor=hpgl_setcolor;

                    ws->maxcolors=HPCOLORS;
                    ws->ydir= 1;
                    info->picno=1;
                    break;
   case GMS_CTRL_CLEAR:
                    fprintf(ws->output,"SP0;\n");
                    fprintf(ws->output,
            "\33.N;19:\33.I100;;17:IN;SS;DT\3;CS0;SI0.2,0.3;SP1;VS9;PA;\n");
                      break;
   case GMS_CTRL_FLUSH :  
                       fflush(ws->output); 
                       break;
   case GMS_CTRL_BEGIN:
                    if (ws->output!=stdout)
                    { gms_genfilename(info->filename,ws->connid,"hpgl",info->picno);
                     gms_openoutput(ws,info->filename);
                      if (ws->output==NULL)  return GMS_RETURN_ERROR;
                    }
                    fprintf(ws->output,
            "\33.N;19:\33.I100;;17:IN;SS;DT\3;CS0;SI0.2,0.3;SP1;VS9;PA;\n");
                     info->picno++; 
                     break;
   case GMS_CTRL_END: 
                    fprintf(ws->output,"SP0;");
                     if (ws->output!=stdout)  gms_closeoutput(ws);
                     break;
   case GMS_CTRL_CLOSE:free(info);
                       break;
    default : break;
  };
  return GMS_RETURN_OK;
#else
  return GMS_RETURN_ERROR;
#endif
}
