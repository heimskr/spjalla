COMPILER		:= g++
CFLAGS			:= -std=c++17 -g -O0 -Wall -Wextra
CFLAGS_ORIG		:= $(CFLAGS)
INCLUDE			:= 
LDFLAGS			:= -pthread
CC				 = $(COMPILER) $(strip $(CFLAGS) $(CHECKFLAGS))
CHECKFLAGS		:=
MKBUILD			:= mkdir -p build
OUTPUT			:= build/spjalla

ifeq ($(shell uname -s), Darwin)
	SDKFLAGS	:= --sysroot /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
endif

ifeq ($(CHECK), asan)
	CHECKFLAGS := -fsanitize=address -fno-common
else ifeq ($(CHECK), msan)
	CHECKFLAGS := -fsanitize=memory -fno-common
endif

.PHONY: all test clean depend spotless


INCLUDE_PP		:= -Ipingpong/include
SOURCES_PP		:= $(shell find pingpong/src -name '*.cpp' | sed -nE '/(tests?|test_.+|ansi)\.cpp$$/!p')
OBJECTS_PP		:= $(patsubst pingpong/src/%.cpp,pingpong/build/%.o, $(SOURCES_PP))

INCLUDE_HN		:= -Ihaunted/include -Ihaunted/src
SOURCES_HN		:= $(shell find haunted/src -name '*.cpp' | sed -nE '/(tests?|test_.+|ansi)\.cpp$$/!p')
OBJECTS_HN		:= $(patsubst haunted/src/%.cpp,haunted/build/%.o, $(SOURCES_HN))

INCLUDE_SP		:= -Iinclude -Iinclude/lib
SOURCES_SP		:= $(shell find -L src -name '*.cpp' | sed -nE '/(tests?|test_.+)\.cpp$$/!p')
OBJECTS_SP		:= $(patsubst src/%.cpp,build/%.o, $(SOURCES_SP))
INCLUDE_LIBS	:= $(INCLUDE_PP) $(INCLUDE_HN)

OBJECTS			= $(OBJECTS_PP) $(OBJECTS_HN) $(OBJECTS_SP)
sinclude $(shell find src -name 'targets.mk')


all: $(OBJECTS) $(OUTPUT)

pingpong/build/%.o: pingpong/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_PP)) -c $< -o $@

haunted/build/%.o: haunted/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_HN)) -c $< -o $@

build/%.o: src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_LIBS) $(INCLUDE_SP)) -c $< -o $@

test: $(OUTPUT)
	./$(OUTPUT)

grind: $(OUTPUT)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=no ./$(OUTPUT)

clean:
	rm -rf build
	if [ -e .log ]; then > .log; fi

spotless:
	$(MAKE) -C pingpong clean
	$(MAKE) -C haunted clean
	rm -rf build .log $(DEPFILE)

DEPFILE  = .dep
DEPTOKEN = "\# MAKEDEPENDS"
DEPFLAGS = -f $(DEPFILE) -s $(DEPTOKEN)

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CC) $(INCLUDE_LIBS) $(INCLUDE_SP) -- $(SOURCES_PP) $(SOURCES_HN) $(SOURCED_SP) 2>/dev/null
	@ sed -i.sed 's/^src\//build\//' $(DEPFILE)
	@ sed -i.sed -E 's/^(pingpong|haunted)\/src\//\1\/build\//' $(DEPFILE)
	@ sed -i.sed '/\/usr\/include\//d' $(DEPFILE)
	@ rm $(DEPFILE).bak $(DEPFILE).sed

sinclude $(DEPFILE)
