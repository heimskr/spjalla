build/spjalla: build/core/spjalla.o $(OBJ) $(COMMONOBJ_PP)
	@ $(MKBUILD)
	$(CC) $< $(filter-out $<,$+) -o $@ $(LDFLAGS) $(LDLIBS)
