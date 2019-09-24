build/libspjalla.$(SHARED_EXT): $(OBJECTS)
	@ $(MKBUILD)
	$(CC) $< $(filter-out $<,$+) $(SHARED_FLAG) -o $@ $(LDFLAGS) $(LDLIBS)

build/spjalla: build/core/main.o build/libspjalla.$(SHARED_EXT)
	@ $(MKBUILD)
	$(CC) $< $(filter-out $<,$+) -o $@ $(LDFLAGS) $(LDLIBS)
