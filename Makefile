COMPILER		:= g++
CFLAGS			:= -std=c++17 -g -ggdb -O0 -Wall -Wextra
CFLAGS_ORIG		:= $(CFLAGS)
INCLUDE			:= -Iinclude -Iinclude/lib
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

.PHONY: all test clean depend spotless destroy haunted
all: Makefile

# Peter Miller, "Recursive Make Considered Harmful" (http://aegis.sourceforge.net/auug97.pdf)
SRCDIR_PP		:= pingpong/src
MODULES			:= core test commands messages lib events net
INCLUDE_PP		:= -Ipingpong/include
INCLUDE			+= $(INCLUDE_PP)
CFLAGS			:= $(CFLAGS_ORIG) $(INCLUDE_PP)
COMMONSRC		:=
SRC				:=
include $(patsubst %,$(SRCDIR_PP)/%/module.mk,$(MODULES))
SRC				+= $(COMMONSRC)
COMMONSRC_PP	:= $(COMMONSRC)
COMMONOBJ_PP	:= $(patsubst src/%.cpp,pingpong/build/%.o, $(filter %.cpp,$(COMMONSRC)))
OBJ_PP			:= $(patsubst src/%.cpp,pingpong/build/%.o, $(filter %.cpp,$(SRC)))
sinclude $(patsubst %,$(SRCDIR_PP)/%/targets.mk,$(MODULES))
SRC_PP			:= $(patsubst %,pingpong/%,$(SRC))

SRCDIR_H		:= haunted
MODULES			:= src/core src/ui src/ui/boxes lib
INCLUDE_H		:= -Ihaunted/include -Ihaunted/lib -Ihaunted
INCLUDE			+= $(INCLUDE_H)
CFLAGS			:= $(CFLAGS_ORIG) $(INCLUDE_H)
COMMONSRC		:=
SRC				:=
include $(patsubst %,$(SRCDIR_H)/%/module.mk,$(MODULES))
SRC				+= $(COMMONSRC)
COMMONSRC_H		:= $(COMMONSRC)
COMMONOBJ_H		:= $(patsubst lib/%.cpp,haunted/build/lib/%.o, $(patsubst src/%.cpp,haunted/build/%.o, $(filter %.cpp,$(COMMONSRC))))
OBJ_H			:= $(patsubst src/%.cpp,haunted/build/%.o, $(filter %.cpp,$(SRC)))
sinclude $(patsubst %,$(SRCDIR_H)/%/targets.mk,$(MODULES))
SRC_H			:= $(patsubst %,haunted/%,$(SRC))

MODULES			:= core ui lib lines tests
COMMONSRC		:=
SRC				:=
CFLAGS			:= $(CFLAGS_ORIG)
include $(patsubst %,src/%/module.mk,$(MODULES))
SRC				+= $(COMMONSRC)
COMMONOBJ		:= $(patsubst src/%.cpp,build/%.o, $(filter %.cpp,$(COMMONSRC)))
COMMONOBJ_LIBS	:= $(COMMONOBJ_PP) $(COMMONOBJ_H)
OBJ				:= $(patsubst src/%.cpp,build/%.o, $(filter %.cpp,$(SRC)))
sinclude $(patsubst %,src/%/targets.mk,$(MODULES))

OBJ_ALL			:= $(OBJ) $(OBJ_PP) $(OBJ_H)
SRC_ALL			:= $(SRC) $(SRC_PP) $(SRC_H)
CFLAGS			:= $(CFLAGS_ORIG)

all: $(COMMONOBJ) $(OUTPUT)

pingpong/build/%.o: pingpong/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_PP)) -c $< -o $@

haunted/build/%.o: haunted/src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_H)) -c $< -o $@

haunted/build/lib/%.o: haunted/lib/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_H)) -c $< -o $@

build/%.o: src/%.cpp
	@ mkdir -p "$(shell dirname "$@")"
	$(CC) $(strip $(SDKFLAGS) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE)) -c $< -o $@

test: $(OUTPUT)
	./$(OUTPUT) irchost

grind: $(OUTPUT)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=no ./$(OUTPUT)

clean:
	rm -rf build
	if [ -e .log ]; then > .log; fi

# Don't completely wipe the build directories of the libraries.
# They might contain libraries that take a while to compile...
spotless:
	$(MAKE) -C pingpong clean
	$(MAKE) -C haunted clean
	rm -rf build .log $(DEPFILE)

# Not a great idea.
destroy: spotless
	rm -rf haunted/build/unicode

haunted:
	@ make -C haunted

DEPFILE  = .dep
DEPTOKEN = "\# MAKEDEPENDS"
DEPFLAGS = -f $(DEPFILE) -s $(DEPTOKEN)

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CC) $(INCLUDE) -- $(SRC_ALL) 2>/dev/null
	@ sed -i.sed 's/^src\//build\//' $(DEPFILE)
	@ sed -i.sed 's/^pingpong\/src\//pingpong\/build\//' $(DEPFILE)
	@ sed -i.sed '/\/usr\/include\//d' $(DEPFILE)
	@ rm $(DEPFILE).bak $(DEPFILE).sed

sinclude $(DEPFILE)
