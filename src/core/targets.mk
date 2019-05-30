build/spjalla: build/core/client.o $(OBJ) $(COMMONOBJ_LIBS)
	@ $(MKBUILD)
	$(CC) $< $(filter-out $<,$+) -o $@ $(LDFLAGS) $(LDLIBS)
