CC = $(CXX)
CXX = clang++
CPPFLAGS += -g3 -MMD -MP -MF .dep/$(basename $(notdir $@)).d
CPPFLAGS += $(shell pkg-config --cflags libsodium)
CXXFLAGS += -O2 -std=c++17 -Wall -Wextra
LDFLAGS += -g3
LOADLIBES += $(shell pkg-config --libs libsodium)

####

%.out: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
%_und.o: CXXFLAGS += -fsanitize=undefined -O0
%_und.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%_und.out: LDFLAGS += -fsanitize=undefined
%_adr.o: CXXFLAGS += -fsanitize=address,leak -O0
%_adr.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%_adr.out: LDFLAGS += -fsanitize=address,leak
%_mem.o: CXXFLAGS += -fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -O0
%_mem.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%_mem.out: LDFLAGS += -fsanitize=memory

.dep:
	mkdir -p $@
.PHONY: do
do: .dep test_sodium_drbg.out
.PHONY: clean
clean:
	$(RM) *.o *.out
.PHONY: check
check:
	time ./test_sodium_drbg.out
.PHONY: indent
indent:
	which $(CLANG_FORMAT)
	git ls-files | grep -i '\.[ch]pp' | xargs -n 1 $(CLANG_FORMAT) -style=file -i

ifneq ($(MAKECMDGOALS),clean)
-include $(shell ls .dep/*.d)
endif
