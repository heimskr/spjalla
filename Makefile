COMPILER		:= g++
ifeq ($(OFLAGS),)
	OFLAGS		:= -g -O0
endif
CFLAGS			:= -std=c++2a $(OFLAGS) -Wall -Wextra -fPIC
CFLAGS_ORIG		:= $(CFLAGS)
INCLUDE			:= 
LDFLAGS			:= -pthread -ldl
CC				 = $(COMPILER) $(strip $(CFLAGS) $(CHECKFLAGS))
CHECKFLAGS		:=
MKBUILD			:= mkdir -p build
OUTPUT			:= build/spjalla
MAINLIB			 = build/libspjalla.$(SHARED_EXT)
SHARED_EXT		:= so
SHARED_FLAG		:= -shared
LIBPATHVAR		:= LD_LIBRARY_PATH

ifeq ($(shell uname -s), Darwin)
	SDKFLAGS	:= --sysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
	SHARED_EXT	:= dylib
	SHARED_FLAG	:= -dynamiclib
	LIBPATHVAR	:= DYLD_LIBRARY_PATH
endif

ifeq ($(CHECK), asan)
	CHECKFLAGS := -fsanitize=address -fno-common
else ifeq ($(CHECK), msan)
	CHECKFLAGS := -fsanitize=memory -fno-common
endif

.PHONY: all test clean depend spotless count plugins

all: $(OBJECTS) $(OUTPUT)

INCLUDE_PP		:= -Ipingpong/include
SOURCES_PP		:= $(shell find pingpong/src -name '*.cpp' | sed -nE '/(tests?|test_.+|ansi|futil)\.cpp$$/!p')
OBJECTS_PP		:= $(patsubst pingpong/src/%.cpp,pingpong/build/%.o,$(SOURCES_PP))

INCLUDE_HN		:= -Ihaunted/include
SOURCES_HN		:= $(shell find haunted/src -name '*.cpp' | sed -nE '/(tests?|test_.+|ansi|futil)\.cpp$$/!p')
OBJECTS_HN		:= $(patsubst haunted/src/%.cpp,haunted/build/%.o,$(SOURCES_HN))

INCLUDE_SP		:= -Iinclude
SOURCES_SP		:= $(shell find -L src -name '*.cpp' | sed -nE '/(^src\/plugins\/)|((tests?|test_.+)\.cpp$$)/!p')
OBJECTS_SP		:= $(patsubst src/%.cpp,build/%.o,$(SOURCES_SP))
INCLUDE_LIBS	:= $(INCLUDE_PP) $(INCLUDE_HN)

SOURCES_PL		:= $(shell find -L src/plugins -name '*.cpp')
OBJECTS_PL		:= $(patsubst src/plugins/%.cpp,build/plugins/%.$(SHARED_EXT),$(SOURCES_PL))

OBJECTS			= $(OBJECTS_PP) $(OBJECTS_HN) $(OBJECTS_SP)

sinclude $(shell find src -name 'targets.mk')

ALLFLAGS		 = $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS)

plugins: $(OBJECTS_PL)

pingpong/build/%.o: pingpong/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(ALLFLAGS) $(INCLUDE_PP)) -c $< -o $@

haunted/build/%.o: haunted/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(ALLFLAGS) $(INCLUDE_HN)) -c $< -o $@

build/%.o: src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(ALLFLAGS) $(INCLUDE_SP) $(INCLUDE_LIBS)) -c $< -o $@

build/plugins/%.$(SHARED_EXT): src/plugins/%.cpp $(MAINLIB)
	@ mkdir -p build/plugins
	$(CC) $(strip $(ALLFLAGS) $(INCLUDE_SP) $(INCLUDE_LIBS)) -c $< -o $(addsuffix .o,$(basename $@))
	$(CC) $(SHARED_FLAG) $(addsuffix .o,$(basename $@)) $(MAINLIB) -o $@
	@ rm $(addsuffix .o,$(basename $@))


test: $(OUTPUT)
	$(LIBPATHVAR)="`pwd`/build" ./$(OUTPUT) --plugins build/plugins

dbg: $(OUTPUT)
	$(LIBPATHVAR)="`pwd`/build" lldb $(OUTPUT) -- --plugins build/plugins

grind: $(OUTPUT)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=no ./$(OUTPUT)

strip:
	strip -x {$(OUTPUT),$(MAINLIB),build/plugins/*.$(SHARED_EXT)}
	strip -x build/test_* 2>/dev/null || true

clean:
	rm -rf build
	$(MAKE) -C pingpong clean
	$(MAKE) -C haunted clean
	if [ -e .log ]; then > .log; fi

spotless:
	rm -rf build pingpong/build haunted/build .log $(DEPFILE)

count:
	cloc src include pingpong/src pingpong/include haunted/src haunted/include formicine

countbf:
	cloc --by-file src include pingpong/src pingpong/include haunted/src haunted/include formicine

DEPFILE  = .dep
DEPTOKEN = "\# MAKEDEPENDS"
DEPFLAGS = -f $(DEPFILE) -s $(DEPTOKEN)

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CC) $(INCLUDE_LIBS) $(INCLUDE_SP) -- $(SOURCES_PP) $(SOURCES_HN) $(SOURCES_SP) $(SOURCES_TESTS) 2>/dev/null
	@ sed -i.sed 's/^src\//build\//' $(DEPFILE)
	@ sed -i.sed -E 's/^(pingpong|haunted)\/src\//\1\/build\//' $(DEPFILE)
	@ sed -i.sed '/\/usr\/include\//d' $(DEPFILE)
	@ rm $(DEPFILE).bak $(DEPFILE).sed

sinclude $(DEPFILE)
