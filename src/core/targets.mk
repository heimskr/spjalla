build/spjalla: build/core/main.o $(OBJ) $(COMMONOBJ_LIBS)
	@ $(MKBUILD)
	$(CC) $< $(filter-out $<,$+) -o $@ $(LDFLAGS) $(LDLIBS)
