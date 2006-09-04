C**********************************************************************
C*                                                                    *
C*  I             A          A            S                           *
C*  Institut fuer Angewandte Analysis und Stochastik  Berlin          *
C*                                                                    *
C*  D-10117 Berlin, Mohrenstr. 39                                     *
C*                                                                    * 
C*  This software is distributed in the hope that it will be useful,  *
C*  but WITHOUT ANY WARRANTY; without even the implied  warranty  of  *
C*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
C*                                                                    *
C**********************************************************************
      subroutine  testbild(icol)
      integer i
      double precision  t,h,xx(50),yy(50)
      if (icol.ge.0) then
         h=1.0/icol
         t=1.0
         do 1 i=1,icol
            xx(1)=0.1D0 
            yy(1)=0.1D0
            xx(2)=0.1D0 
            yy(2)=0.2D0
            xx(3)=t 
            yy(3)=0.2D0
            xx(4)=t 
            yy(4)=0.1D0
            xx(5)=0.1D0 
            yy(5)=0.1D0
            call gmscol(i)
            call gmdcol(1.0D0-t, t,1.0D0) 
            call gmfill(4,xx,yy)
            t=t-h
 1       continue
      else
         xx(1)=0.3D0 
         yy(1)=0.1D0
         xx(2)=0.3D0 
         yy(2)=0.2D0
         xx(3)=0.7D0 
         yy(3)=0.2D0
         xx(4)=0.7D0 
         yy(4)=0.1D0
         xx(5)=0.3D0 
         yy(5)=0.1D0
         call gmfill(4,xx,yy)
      endif        
      call gmscol(0)
      call gmdcol(0.0D0,0.0D0,0.0D0)
      call gmmove(0.0D0,0.0D0)
      call gmline(0.0D0,1.0D0)
      call gmline(1.0D0,1.0D0)
      call gmline(1.0D0,0.0D0)
      call gmline(0.0D0,0.0D0)
      
      call gmmove(0.0D0,0.5D0)
      call gmline(0.5D0,1.0D0)
      call gmline(1.0D0,0.5D0)
      call gmline(0.5D0,0.0D0)
      call gmline(0.0D0,0.5D0)
      
      h=2.0*3.1415/100.0D0
      t=0.0D0
      call gmmove(1.0D0,0.5D0)
      do 2 ,i=1,10000
         if (t.gt.6.2833) then
            goto 3
         endif
         call gmline(0.5D0*cos(t)+0.5D0,0.5D0*sin(t)+0.5D0)
         t=t+h
 2    continue
 3    call gmmove(0.1D0,0.9D0)
      call gmmark(0)
      call gmmove(0.3D0,0.9D0)
      call gmmark(1)
      call gmmove(0.5D0,0.9D0)
      call gmmark(2)
      call gmmove(0.7D0,0.9D0)
      call gmmark(3)
      call gmmove(0.9D0,0.9D0)
      call gmmark(4)
      
      call gmmove(0.5D0,0.4D0)
      call gmline(0.5D0,0.45D0)
      call gmmark(11)
      call gmmove(0.5D0,0.4D0)
      call gmline(0.5D0,0.35D0)
      call gmmark(12)
      call gmmove(0.5D0,0.4D0)
      call gmline(0.4D0,0.4D0)
      call gmmark(13)
      call gmmove(0.5D0,0.4D0)
      call gmline(0.6D0,0.4D0)
      call gmmark(14)
      
      call gmaltx(3,3)
      call gmmove(0.5D0,0.7D0)
      call gmtext('abcdefghijklmnopqrstuvwxyz\0')
      call gmmove(0.5D0,0.5D0)
      call gmtext('ABCDEFGHIJKLMNOPQRSTUVWXYZ\0')
      call gmmove(0.5D0,0.3D0)
      call gmtext('&/+-_(){}() 1234567890.:? !$%&\0')
      call gmmove(0.5D0,0.2D0)
      if (icol.gt.1) call gmtext('color\0\0')
      call gmaltx(3,2)
      call gmmove(0.5D0,1.0D0)
      call gmtext('0,1\0')
      call gmaltx(2,3)
      call gmmove(1.0D0,0.5D0)
      call gmtext('1,0\0')
      call gmaltx(3,1)
      call gmmove(0.5D0,0.0D0)
      call gmtext('0,-1\0')
      call gmaltx(1,3)
      call gmmove(0.0D0,0.5D0)
      call gmtext('-1,0\0')
      end
      
      program tgmsf
      integer icol
      call gmop(1,110,0)
      call gmqcl(-1,icol)
      if (icol.eq.0) stop
      call gmbeg(-1)
      call gmvp(0.1D0,0.9D0,0.1D0,0.9D0)
      call testbild(icol)
      call gmhold()
      call gmend(-1)
      call gmcl(1)
      end
      

