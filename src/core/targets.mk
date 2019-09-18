build/spjalla: build/core/main.o $(OBJECTS)
	@ $(MKBUILD)
	$(CC) $< $(filter-out $<,$+) -o $@ $(LDFLAGS) $(LDLIBS)
