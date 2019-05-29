build/test_boxes: build/tests/boxes.o $(COMMONOBJ)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

boxtest: build/test_boxes
	./$^
