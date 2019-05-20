COMPILER		:= clang++
CFLAGS			:= -std=c++17 -g -O3 -Wall -Wextra -fdiagnostics-color=always
LDFLAGS			:=
CC				 = $(COMPILER) $(CFLAGS) $(CHECKFLAGS)
CHECKFLAGS		:=
MKBUILD			:= mkdir -p build

CHECK			:= asan

ifeq ($(CHECK), asan)
	CHECKFLAGS += -fsanitize=address -fno-common
else ifeq ($(CHECK), msan)
	CHECKFLAGS += -fsanitize=memory -fno-common
else
	CHECKFLAGS +=
endif

.PHONY: all test clean depend
all: Makefile

# Peter Miller, "Recursive Make Considered Harmful" (http://aegis.sourceforge.net/auug97.pdf)
MODULES			:= pingpong/core pingpong/test pingpong/commands pingpong/messages pingpong/lib
COMMONSRC		:=
CFLAGS			+= -I. -Ipingpong/include
LIBS			:=
SRC				:=
include $(patsubst %,%/module.mk,$(MODULES))
SRC				+= $(COMMONSRC)
COMMONOBJ_PP	:= $(patsubst %.cpp,pingpong/%.o, $(filter %.cpp,$(COMMONSRC)))
OBJ_PP			:= $(patsubst %.cpp,pingpong/%.o, $(filter %.cpp,$(SRC)))
sinclude $(patsubst %,%/targets.mk,$(MODULES))

include pingpong/conan.mk

all: $(COMMONOBJ_PP)

what:
	echo $(patsubst pingpong/%,%/%.o,$(MODULES))

test: spjalla
	./spjalla

spjalla: core/main.cpp
	$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

grind: build/tests
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./build/tests

clean:
	rm -f spjalla
	rm -f **/*.o
	rm -f *.o
	$(MAKE) -C pingpong clean

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

DEPFILE  = .dep
DEPTOKEN = "\# MAKEDEPENDS"
DEPFLAGS = -f $(DEPFILE) -s $(DEPTOKEN)

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CC) -- $(SRC)
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
