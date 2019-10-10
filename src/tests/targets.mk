SOURCES_TESTS += src/tests/test_config.cpp src/tests/test_util.cpp src/tests/test_aliases.cpp
NO_MAIN = $(filter-out build/core/main.o,$(OBJECTS))

build/test_config: build/tests/test_config.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_util: build/tests/test_util.o build/lib/formicine/futil.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_aliases: build/tests/test_aliases.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_backwards_line: build/tests/test_backwards_line.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_ansi: build/tests/test_ansi.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

cfgtest: build/test_config
	./$^

utiltest: build/test_util
	./$^
	rm -f build/test_util build/tests/test_util.o

atest: build/test_aliases
	./$^

bltest: build/test_backwards_line
	./$^

antest: build/test_ansi
	./$^
