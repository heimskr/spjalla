COMPILER		:= clang++
CFLAGS			:= -std=c++17 -g -O3 -Wall -Wextra -fdiagnostics-color=always
LDFLAGS			:=
CC				 = $(COMPILER) $(CFLAGS) $(CHECKFLAGS)
CHECKFLAGS		:=
MKBUILD			:= mkdir -p build
OUTPUT			:= build/spjalla

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
SRC				:=
include $(patsubst %,%/module.mk,$(MODULES))
SRC				+= $(COMMONSRC)
COMMONOBJ_PP	:= $(patsubst %.cpp,pingpong/%.o, $(filter %.cpp,$(COMMONSRC)))
OBJ_PP			:= $(patsubst %.cpp,pingpong/%.o, $(filter %.cpp,$(SRC)))
sinclude $(patsubst %,%/targets.mk,$(MODULES))
SRC_PP			:= $(patsubst %,pingpong/%,$(SRC))

MODULES			:= core
COMMONSRC		:=
SRC				:=
include $(patsubst %,%/module.mk,$(MODULES))
SRC				+= $(COMMONSRC)
COMMONOBJ		:= $(patsubst %.cpp,%.o, $(filter %.cpp,$(COMMONSRC)))
OBJ				:= $(patsubst %.cpp,%.o, $(filter %.cpp,$(SRC)))

OBJ_ALL			:= $(OBJ) $(OBJ_PP)
SRC_ALL			:= $(SRC) $(SRC_PP)

sinclude $(patsubst %,%/targets.mk,$(MODULES))

include pingpong/conan.mk

all: $(COMMONOBJ)

test: $(OUTPUT)
	./$(OUTPUT)

grind: $(OUTPUT)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./$(OUTPUT)

clean:
	rm -f $(OUTPUT) **/*.o *.o
	$(MAKE) -C pingpong clean

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

DEPFILE  = .dep
DEPTOKEN = "\# MAKEDEPENDS"
DEPFLAGS = -f $(DEPFILE) -s $(DEPTOKEN)

depend:
	@ echo $(DEPTOKEN) > $(DEPFILE)
	makedepend $(DEPFLAGS) -- $(CC) -- $(SRC_ALL)
	@ rm $(DEPFILE).bak

sinclude $(DEPFILE)
