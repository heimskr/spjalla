# Expects libcpr to be installed somewhere in the ld/include paths.
# For my setup, I have to set CFLAGS=-I/usr/local/include and LDFLAGS=-I/usr/local/lib.

build/plugins/slashdot/%.o: src/plugins/slashdot/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(ALLFLAGS) $(INCLUDE_SP) $(INCLUDE_LIBS) $(INCLUDE_FLAGS) $(CFLAGS_TINYXML2) -Icpr/include) -c $< -o $@

build/plugins/slashdot.dylib: $(patsubst %.cpp,%.o,$(addprefix build/plugins/slashdot/,$(filter %.cpp,$(shell ls "src/plugins/slashdot")))) $(MAINLIB)
	$(CC) $(SHARED_FLAG) $+ -o $@ $(LDFLAGS) $(LD_TINYXML2) -lcurl -Lcpr/build/lib -lcpr