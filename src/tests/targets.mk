build/test_boxes: build/tests/test_boxes.o $(COMMONOBJ)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

boxtest: build/test_boxes
	./$^

build/test_haunted: build/tests/test_haunted.o $(COMMONOBJ)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

hauntedtest: build/test_haunted
