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
 * Revision 1.1  1995/10/20 15:21:38  fuhrmann
 * Initial revision
 *
 */

/*-----------------------------------------------------------------
-------------------------------------------------------------------
  REGIS
-------------------------------------------------------------------
-----------------------------------------------------------------*/
#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef REGIS
/* Control sequence fragments taken from DPLOT */

static void regis_moveto(ws,x,y)wsdsc *ws; double x,y;
{  fprintf(ws->output,"P[%d,%d]",(int)x,(int)y);
}

static void regis_lineto(ws,x,y) wsdsc *ws;double x,y;
{ fprintf(ws->output,"v[]v[%d,%d]",(int)x,(int)y);
}

static void regis_fill(ws,n,x,y) wsdsc *ws;int n;double *x,*y;
{ int i;
  fprintf(ws->output,"P[%d,%d]F(",(int)x[0],(int)y[0]);
  for (i=1;i<n;i++)
     fprintf(ws->output,"V[%d,%d]",(int)x[i],(int)y[i]);
  fprintf(ws->output,"V[%d,%d])",(int)x[0],(int)y[0]);
}

static void regis_setcolor(ws,icol) wsdsc*ws; int icol;
{ fprintf(ws->output,"W(I%d)",icol+1);
  ws->icol=icol;
}

static void regis_defcolor(ws,r,g,b) wsdsc*ws;
double r,g,b;
{ double norm;
  double *min;
  int hue,lightness,sat;
 
  r=MIN(1.0,MAX(0.0,r));
  g=MIN(1.0,MAX(0.0,g));
  b=MIN(1.0,MAX(0.0,b));

  norm=sqrt(r*r+g*g+b*b);
  if (norm>0.0)
  {  
   lightness=(int)(100.0*norm/3.0);
 
   sat=MIN(100,(int)(100.0*((r-g)*(r-g)+(r-b)*(r-b)+(g-b)*(g-b))/(norm)));

   r=r/norm;
   b=b/norm;
   g=g/norm;

  }
  else 
  { sat=0.0;
    lightness=0.0;
  };
  min= &r;
  if (g< *min) min= &g;
  if (b< *min) min= &b;  

  if (&b==min)
  { if ((g+r) >1.0e-10)
      hue=(int)(180.0 + 60.0*(g-r)/(g+r)); 
    else
    { if (r>g) 
         hue= 120; 
       else hue=240;
    } 
  }
            
  else if (&g==min) 
  {  if ((b+r) >1.0e-10)
         hue=(int)(60.0 + 60.0*(r-b)/(r+b));
     else
     {  if (r>b)
          hue= 120; 
        else
          hue=0;
     }
   }
  else if (&r==min) 
  {  if ((g+b) >1.0e-10) 
        hue=(int)(300.0 + 60.0*(b-g)/(g+b));   
     else
       {  if (b>g) 
             hue= 0; 
          else
             hue=240;
       }
   }

  fprintf(ws->output,"S(M%d(AH%dL%dS%d))",ws->icol+1,hue,lightness,sat);

}

static void regis_marker(ws, mktype)wsdsc *ws;int mktype;
{
 switch(mktype)
 { case GMS_MARKER_PIXEL : fprintf(ws->output,"v[]");
                           break;
     case GMS_MARKER_DIAMOND  : fprintf(ws->output,
                     "v(S)P[-%d,-0]v(W(M%d))1357(E)",
                          (int)ws->markersize,(int)ws->markersize);
                    break;
   case GMS_MARKER_X  : fprintf(ws->output,"v(W(M%d))15513773",
                         (int)ws->markersize);
                         break;
   case GMS_MARKER_PLUS    : fprintf(ws->output,"v(W(M%d))04402662",
                         (int)ws->markersize);
                    break;
  case GMS_MARKER_ASTERISK : fprintf(ws->output,"v(W(M%d))0440622615513773",
                            (int)ws->markersize);
                    break;
   default :gms_generic_marker(ws,mktype);

  };
}

static void regis_text(ws,str) wsdsc *ws;char *str;
{
        (void) putc('T',ws->output); (void) putc('\'',ws->output);
        while (*str) {
                (void) putc(*str,ws->output);
                if (*str == '\'')
                        (void) putc('\'',ws->output);        /* send out another one */
                str++;
        }
        (void) putc('\'',ws->output);
}

#endif

int regis_ctrl(ws,action)wsdsc *ws;int action;
{
#ifdef REGIS

/*!!!                    if (ws->output!=stdout)
                    { if (!gms_getopt('\0',ws->connid)) gms_getopt('O',ws->connid);
                      sprintf(info->filename,"%s-%d.ps", optval,info->picno);
*/
  switch (action)
  { case GMS_CTRL_OPEN :

                    if ((strcmp(ws->connid,"")==0)||
                        (strcmp(ws->connid,"tty")==0)||
                        (strcmp(ws->connid,"stdout")==0))  
                    ws->output=stdout;
                    else ws->output=fopen(ws->connid,"w");

                    ws->asp=0.60;
                    ws->xmax=800.0;
                    ws->ymax=400.0;
                    
                    ws->moveto=regis_moveto; 
                    ws->lineto=regis_lineto;
                    ws->marker=regis_marker;
		    if (!gms_fontload()) fprintf(gms_errorfile,"%%GMS: No text avaliable");
                    ws->fill=regis_fill;
                    switch (ws->mode)
                    { case 1: ws->maxcolors=3;
                              ws->setcolor=regis_setcolor;
                              break;
                      case 2: ws->maxcolors=7;
                              ws->setcolor=regis_setcolor;
                              break;
                      case 3: ws->maxcolors=15;
                              ws->setcolor=regis_setcolor;
                              break;
                      case 4: ws->maxcolors=15;
                              ws->setcolor=regis_setcolor;
                              ws->defcolor=regis_defcolor;
                              break;
                      default: ws->maxcolors=1;
                              break;
                    }  
                    ws->ydir= -1;
                    break;
   case GMS_CTRL_BEGIN:        fprintf(ws->output,"\033[r\033[24;1H");
                      /*                   1     2
                              1. reset scrolling region
                              2. locate cursor on bottom line
                      */ 
   case GMS_CTRL_CLEAR:
              fprintf(ws->output,"\033[2J\033P1pS(C0)S(E)",/*S(W(M%d))S(W(I5))"*/
                ws->markersize);
/*                   1      2      3        4   5      6        7
        1. clear screen
        2. enter ReGIS graphics
        3. turn off graphics diamond cursor
        4. clear graphics screen
        6. Markersize
        7. color
*/
  fprintf(ws->output,"S(M%d(AH%dL%dS%d))",0,0,100,0);
                   break;
  case GMS_CTRL_FLUSH:  
                     fflush(ws->output);
                      break;
   case GMS_CTRL_END:
                       fprintf(ws->output,"\033\\\033[24;1H"); 
                     fflush(ws->output);
                      break;
   case GMS_CTRL_CLOSE: if (ws->output != stdout) fclose(ws->output);
   default : break;
  };

  return GMS_RETURN_OK;
#else
  return GMS_RETURN_ERROR;
#endif
}
