build/test_boxes: build/tests/test_boxes.o $(COMMONOBJ)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)
