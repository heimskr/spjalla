build/test_config: build/tests/test_config.o $(filter-out build/core/main.o,$(OBJECTS))
	@ $(MKBUILD)
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

cfgtest: build/test_config
	./$^
