
$(install_prefix)/$(pak_subdir)/build_stamp:
	@echo "`date`" > $(install_prefix)/$(pak_subdir)/build_stamp

FCOMPILE = $(F77) $(FFLAGS_MIN) $(FFLAGS_OPTLEVEL)
CCOMPILE = $(CC)  $(CFLAGS_MIN) $(CFLAGS_OPTLEVEL)
libgms_a_AR = $(AR) $(ARFLAGS)


libgms_a_OBJECTS = gmsker.$(OBJEXT) \
                   gmsx11.$(OBJEXT) \
                   gmsps.$(OBJEXT) \
                   gmsgl.$(OBJEXT) \
                   gmsregis.$(OBJEXT) \
                   gmshpgl.$(OBJEXT) \
                   gmssun.$(OBJEXT) \
                   gmstek.$(OBJEXT) \
                   gmsmsc.$(OBJEXT) \
                   gmsbgi.$(OBJEXT)


PKG = $(install_dir_lib)/libgms.a

.SUFFIXES: .f .o .obj


libgms.a: $(libgms_a_OBJECTS)
	@echo "Creating $@..."
	@-rm -f libgms.a
	@$(libgms_a_AR) libgms.a $(libgms_a_OBJECTS)
	@$(RANLIB) libgms.a


$(PKG): $(install_prefix)/$(pak_subdir)/build_stamp
	$(MAKE) -f $(pkg_dir)/$(pak_subdir)_Makefile libgms.a
	cp ./libgms.a $(PKG)

clean:
	-rm -f libgms.a
	-rm -f *.o

uninstall: clean
	-rm -f $(PKG)
	-rm -f $(install_prefix)/$(pak_subdir)/build_stamp


all: $(PKG)


.f.o:
	@echo "Compiling $<..."
	@if $(FCOMPILE) -c -o $@ $< ; then true ; else echo "failed with cmd-line: $(FCOMPILE) -c -o $@ $<" ; exit 1 ; fi

.f.obj:
	@echo "Compiling `$(CYGPATH_W) '$<'`..."
	@if $(FCOMPILE) -c -o $@ `$(CYGPATH_W) '$<'` ; then true ; else echo "failed with cmd-line: $(FCOMPILE) -c -o $@ `$(CYGPATH_W) '$<'`" ; exit 1 ; fi

.c.o:
	@echo "Compiling $<..."
	@if $(CCOMPILE) -c -o $@ $< ; then true ; else echo "failed with cmd-line: $(CCOMPILE) -c -o $@ $<" ; exit 1 ; fi

.c.obj:
	@echo "Compiling $<..."
	@if $(CCOMPILE) -c -o $@ $< ; then true ; else echo "failed with cmd-line: $(CCOMPILE) -c -o $@ $<" ; exit 1 ; fi


.PHONY: all clean uninstall
.NOEXPORT:










