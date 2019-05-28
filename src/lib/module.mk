UNISRC := uniset uobject uvector uvectr32 uvectr64 unifilt unifunct unistr ustring cmemory bmpset unisetspan appendable\
          stringpiece util patternprops uinvchar ustrtrns udataswp cstring umath uarrsort utf_impl
CFLAGS += -Iicu/icu4c/source/common -Iicu/icu4c/source/common/unicode
SRC    += $(patsubst %,src/lib/unicode/%.cpp,$(UNISRC))
