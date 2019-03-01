CC = $(CXX)
CXX = clang++
CPPFLAGS += -g3 -MMD -MP -MF .dep/$(basename $(notdir $@)).d
CPPFLAGS += $(shell pkg-config --cflags libsodium)
CPPFLAGS += $(shell pkg-config --cflags openssl)
CXXFLAGS += -O2 -std=c++17 -Wall -Wextra
LDFLAGS += -g3
LOADLIBES += $(shell pkg-config --libs libsodium)
LOADLIBES += $(shell pkg-config --libs openssl)

####

%.out: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
%_und.o: CXXFLAGS += -fsanitize=undefined -O0
%_und.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%_und.out: LDFLAGS += -fsanitize=undefined
%_adr.o: CXXFLAGS += -fsanitize=address -O0
%_adr.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%_adr.out: LDFLAGS += -fsanitize=address
%_mem.o: CXXFLAGS += -fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -O0
%_mem.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%_mem.out: LDFLAGS += -fsanitize=memory

.dep:
	mkdir -p $@
.PHONY: do
do: .dep $(addprefix test_sodium_drbg,.out _adr.out) $(addprefix test_openssl_aes_drbg,.out _adr.out)
.PHONY: clean
clean:
	$(RM) *.o *.out
.PHONY: check
check:
	set -xe; \
	for t in $$(ls -1 test_*.out); do \
		./$$t; \
	done; \
	set +xe
.PHONY: indent
indent:
	which $(CLANG_FORMAT)
	git ls-files | grep -i '\.[ch]pp' | xargs -n 1 $(CLANG_FORMAT) -style=file -i

ifneq ($(MAKECMDGOALS),clean)
-include $(shell ls .dep/*.d)
endif
