build/spjalla: core/spjalla.o $(COMMONOBJ)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)
