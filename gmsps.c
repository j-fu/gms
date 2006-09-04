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
 * Revision 1.4  2003/03/28 16:47:51  fuhrmann
 * pdelib1.15_alpha1
 *
 * Revision 1.3  2003/03/28 11:20:29  fuhrmann
 * pdelib2.0_alpha1
 *
 * Revision 1.2  1999/07/05 18:29:21  fuhrmann
 * quick & dirty ansi port
 *
 * Revision 1.1  1995/10/20  15:21:38  fuhrmann
 * Initial revision
 *
 */

/*-----------------------------------------------------------------
-------------------------------------------------------------------
  POSTSCRIPT
-----------------------------------------------------------------
-----------------------------------------------------------------*/
#ifndef GMS_C
#include "gmsdrv.h"
#endif 
#ifdef POSTSCRIPT
#define PS_UPI 72.0
#define A4_XINCH 8.27
#define A4_YINCH 11.69

#ifndef PSCOLORS
#define PSCOLORS 64
#endif
typedef struct { int pageno,picno,actcolor; 
                 double r[PSCOLORS],g[PSCOLORS],b[PSCOLORS];
                 char filename[32];
                 char fontname[32];
                 int page_empty;
                 double xtran,ytran,scale;
                 char closed,moved;
         } postscript_info;

static void postscript_moveto(wsdsc *ws,double x,double y)
{postscript_info *info=(postscript_info*) ws->info;
 fprintf(ws->output,"%d %d S M\n",(int)x,(int)y);
 info->page_empty=0;
 info->moved=1; /** IB **/
}

static void postscript_lineto(wsdsc *ws,double x, double y)
{postscript_info *info=(postscript_info*) ws->info;

if (!info->moved) postscript_moveto(ws,ws->xpos,ws->ypos); /** IB **/
  fprintf(ws->output,"%d %d L\n",(int)x,(int)y);
 info->page_empty=0;
}

static void postscript_fill(wsdsc *ws,int n,double *x,double *y)
{postscript_info *info=(postscript_info*) ws->info;
   int i;
  fprintf(ws->output,"NP\n");
  fprintf(ws->output,"%d %d M\n",(int)(x[0]+0.5),(int)(y[0]+0.5));
  for (i=1;i<n;i++)
    fprintf(ws->output,"%d %d L\n",(int)(x[i]+0.5),(int)(y[i]+0.5));
  fprintf(ws->output,"FP\n");
 info->page_empty=0;
 info->moved=0; /** IB **/
 
}


static void postscript_text(ws,str, xalign,yalign) wsdsc *ws;char *str;int xalign,yalign;
{ postscript_info *info=(postscript_info*)ws->info;
 register int i,j;
  char xa,ya;
  switch (xalign)
  { case GMS_ALIGN_LEFT:xa='L';break;
    case GMS_ALIGN_RIGHT:xa='R';break;
    case GMS_ALIGN_CENTER:xa='C';break;
    default:break;
   }
  switch (yalign)
  { case GMS_ALIGN_TOP:ya='T';break;
    case GMS_ALIGN_BOTTOM:ya='B';break;
    case GMS_ALIGN_CENTER:ya='C';break;
    default:break;
   }

  for(i=j=0;str[i]!='\0';i++)
  { switch (str[i])
    { case '(' : gms_buffer[j++]='\\';gms_buffer[j++]='(';  break;
      case ')' : gms_buffer[j++]='\\';gms_buffer[j++]=')';  break;
      case '\\': gms_buffer[j++]='\\';gms_buffer[j++]='\\'; break;
      default  : gms_buffer[j++]=str[i];
    }
    if (i>=GMS_BUFLEN) break;
  }
  gms_buffer[j]='\0';
  fprintf(ws->output,"(%s) %c%cshow\n",gms_buffer,xa,ya);
  info->moved=0; /** IB **/
}


static void postscript_setcolor(ws,icol) wsdsc*ws; int icol;
{ postscript_info *info=(postscript_info*)ws->info;
  info->actcolor=icol;
  if (!info->closed) /** IB **/
  {  fprintf(ws->output,"S %.2f %.2f %.2f C\n",
        info->r[icol],info->g[icol],info->b[icol]);
   info->moved=0; /** IB **/
   }
}

static void postscript_defcolor(ws,r,g,b) wsdsc*ws; 
double r,g,b;
{ postscript_info *info=(postscript_info*)ws->info;
  int icol=info->actcolor;
  info->r[icol]=r;
  info->g[icol]=g;
  info->b[icol]=b;
  if (!info->closed) /** IB **/
  { fprintf(ws->output,"%.2f %.2f %.2f C\n",
        info->r[icol],info->g[icol],info->b[icol]);
   info->moved=0; /** IB **/
  }
}



#endif
int postscript_ctrl(ws,action)wsdsc *ws;int action;
{
#ifdef POSTSCRIPT  
 postscript_info *info=(postscript_info*) ws->info;
  time_t t;
  int i; double xcol,hc;
  t=time(&t);
  switch (action)
    { case GMS_CTRL_OPEN :
	if (!(gms_getopt('\0',ws->connid)||gms_getopt('O',ws->connid)))
	  ws->output= stdout;
	else 
	  ws->output=NULL;
	
	ws->info=info=NEW(postscript_info);
	ws->asp=1.0;
        info->scale=0.1;
	ws->xmax=(A4_XINCH-1.0)*PS_UPI/info->scale;
	ws->ymax=ws->xmax;  

	if (gms_getopt('W',ws->connid) ) ws->xmax=PS_UPI*((double)atoi(gms_optval))/info->scale;
	if (gms_getopt('H',ws->connid) ) ws->ymax=PS_UPI*((double)atoi(gms_optval))/info->scale;
        
          
	info->xtran=(A4_XINCH*PS_UPI-ws->xmax*info->scale)/2.0;
	info->ytran=((A4_YINCH-0.5)*PS_UPI-ws->ymax*info->scale);
        if (info->xtran<0.0) info->xtran=0.0;
        if (info->ytran<0.0) info->ytran=0.0;
   
	ws->moveto=postscript_moveto; 
	ws->lineto=postscript_lineto;
	ws->text=postscript_text;
	ws->fill=postscript_fill;
	switch(ws->mode)
	  { case 1:  ws->maxcolors=1;
	    break;
	  default: ws->defcolor=postscript_defcolor;
	    ws->setcolor=postscript_setcolor;
            ws->maxcolors=PSCOLORS;
	    hc=1.0/PSCOLORS;
	    xcol=0.0;
	    for (i=0;i<PSCOLORS;i++,xcol+=hc) 
	      info->r[i]=info->g[i]=info->b[i]=xcol;
	  }
	info->actcolor=1;
	ws->ydir= 1;
	ws->charwidth=12;
	info->picno=1;
	info->page_empty=1;
	info->closed=1;
	info->moved=0; /** IB **/
	break;
      case GMS_CTRL_CLEAR:
	if (!info->page_empty)
	  {
	    fprintf(ws->output,
 "S showpage grestore\n%%%%Page:%d %d\n gsave %f %f translate %f %f scale\n",
                    info->pageno,info->pageno,
                    info->xtran,info->ytran, 
                    info->scale, info->scale
		    );info->pageno++;
	  }  
	info->page_empty=1;
	
	break;
      case GMS_CTRL_FLUSH :  
	fflush(ws->output); 
	break;
      case GMS_CTRL_BEGIN:
	if (info->closed){
	  if (ws->output!=stdout)
	    { gms_genfilename(info->filename,ws->connid,"ps",info->picno);
                     gms_openoutput(ws,info->filename);
                      if (ws->output==NULL)  return GMS_RETURN_ERROR;
 	    }
	  fprintf(ws->output,
		  "%%!PS-Adobe-2.0\n");
	  fprintf(ws->output,
		  "%%%%Creator:GMS - Graphics Minimal System\n");
	  fprintf(ws->output,
		  "%%%%Title:%s\n", gms_getopt('T',ws->connid) ? 
		  "GMS output" : gms_optval);
	  fprintf(ws->output,"%%%%CreationDate:%s",ctime(&t));
	  fprintf(ws->output,"%%%%Pages:(atend)\n");
	  fprintf(ws->output,"%%%%BoundingBox:%f %f %f %f\n",
                            info->xtran,info->ytran,
		            ws->xmax*info->scale+info->xtran,
                            ws->ymax*info->scale+info->ytran);
	  fprintf(ws->output,"%%%%DocumentFonts:%s\n",
		  gms_getopt('F',ws->connid)?gms_optval:"Helvetica");
	  fprintf(ws->output,"%%%%EndComments\n");
	  fprintf(ws->output,"%%\n%%Edit here to modify font!\n");
	  fprintf(ws->output,"/Fontscale %g def\n",14.0/info->scale);
	  fprintf(ws->output,"/Fontname  /%s def\n%%\n",gms_getopt('F',ws->connid)?gms_optval:"Helvetica");
	  fprintf(ws->output,"/BD {bind def} bind def\n");
	  fprintf(ws->output,"/M  {moveto} BD\n");
	  fprintf(ws->output,"/L  {lineto} BD\n");
	  fprintf(ws->output,"/S  {stroke} BD\n");
	  fprintf(ws->output,"/C  {setrgbcolor} BD\n");
	  fprintf(ws->output,"/NP {newpath} BD\n");
	  fprintf(ws->output,"/FP {closepath fill stroke} BD\n");
	  fprintf(ws->output,"/LBshow {0  0  rmoveto show} BD\n");
	  fprintf(ws->output,"/LCshow {0 Fontscale 2 div neg   rmoveto show} BD\n");
	  fprintf(ws->output,"/LTshow {0 Fontscale neg rmoveto show} BD\n");
	  fprintf(ws->output,"/CBshow {dup stringwidth pop 2 div neg   0 rmoveto  show} BD\n");
	  fprintf(ws->output,"/CCshow {dup stringwidth pop 2 div neg  Fontscale 2 div neg  rmoveto  show} BD\n");
	  fprintf(ws->output,"/CTshow {dup stringwidth pop 2 div neg  Fontscale neg rmoveto  show} BD\n");
	  fprintf(ws->output,"/RBshow {dup stringwidth pop neg  0  rmoveto  show} BD\n");
	  fprintf(ws->output,"/RCshow {dup stringwidth pop neg Fontscale 2 div neg   rmoveto  show} BD\n");
	  fprintf(ws->output,"/RTshow {dup stringwidth pop neg Fontscale neg rmoveto  show} BD\n");
	  
	  fprintf(ws->output,"Fontname findfont  Fontscale scalefont setfont\n");
	  fprintf(ws->output,"0 setlinewidth\n");
	  
	  fprintf(ws->output,"%%%%EndProlog\n");
	  info->pageno=1;
	  info->picno++;
	  fprintf(ws->output,
       "%%%%Page:%d %d\n gsave %f %f  translate %f %f scale\n",
                     info->pageno,info->pageno,
                     info->xtran,info->ytran, info->scale,info->scale);
	  info->page_empty=1; 
	  info->closed=0;
	  info->moved=0; /** IB **/
	}
	break;
      case GMS_CTRL_END: 
	if (!info->page_empty)
	  { fprintf(ws->output,
                    "stroke showpage grestore\n%%%%Trailer\n%%%%Pages:%d\n",
		    info->pageno);
	    if (ws->output!=stdout) gms_closeoutput(ws);
	    info->closed=1; 
	}  else info->closed=0;
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
