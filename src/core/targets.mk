build/libspjalla.$(SHARED_EXT): $(OBJECTS)
	@ $(MKBUILD)
	cd build && $(CC) $(addprefix ../,$+) $(SHARED_FLAG) -o $(notdir $@) $(LDFLAGS) $(LDLIBS)

build/spjalla: build/core/main.o build/libspjalla.$(SHARED_EXT)
	@ $(MKBUILD)
	cd build && $(CC) core/main.o libspjalla.$(SHARED_EXT) -o $(notdir $@) $(LDFLAGS) $(LDLIBS)
