SOURCES_TESTS += src/tests/test_config.cpp src/tests/test_util.cpp

build/test_config: build/tests/test_config.o $(filter-out build/core/main.o,$(OBJECTS))
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build/test_util: build/tests/test_util.o $(filter-out build/core/main.o,$(OBJECTS))
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

cfgtest: build/test_config
	./$^

utiltest: build/test_util
	./$^
