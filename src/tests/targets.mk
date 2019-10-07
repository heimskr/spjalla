SOURCES_TESTS += src/tests/test_config.cpp src/tests/test_util.cpp src/tests/test_aliases.cpp

build/test_config: build/tests/test_config.o $(filter-out build/core/main.o,$(OBJECTS))
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_util: build/tests/test_util.o build/lib/formicine/futil.o $(filter-out build/core/main.o,$(OBJECTS))
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_aliases: build/tests/test_aliases.o $(filter-out build/core/main.o,$(OBJECTS))
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

cfgtest: build/test_config
	./$^

utiltest: build/test_util
	./$^
	rm -f build/test_util build/tests/test_util.o

atest: build/test_aliases
	./$^
