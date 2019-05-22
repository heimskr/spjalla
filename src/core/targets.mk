build/spjalla: build/core/client.o $(OBJ) $(COMMONOBJ_PP)
	@ $(MKBUILD)
	$(CC) $< $(filter-out $<,$+) -o $@ $(LDFLAGS) $(LDLIBS)
