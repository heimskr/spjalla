SOURCES_TESTS += src/tests/test_config.cpp src/tests/test_util.cpp src/tests/test_aliases.cpp
NO_MAIN = $(filter-out build/core/main.o,$(OBJECTS))

build/test_config: build/tests/TestConfig.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_util: build/tests/TestUtil.o build/lib/formicine/futil.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_aliases: build/tests/TestAliases.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_backwards_line: build/tests/TestBackwardsLine.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_ansi: build/tests/TestAnsi.o $(NO_MAIN)
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

cfgtest: build/test_config
	./$^

utiltest: build/test_util
	./$^
	rm -f build/test_util build/tests/TestUtil.o

atest: build/test_aliases
	./$^

bltest: build/test_backwards_line
	./$^

antest: build/test_ansi
	./$^
