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
%__und.o: CXXFLAGS += -fsanitize=undefined -O0
%__und.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%__und.out: LDFLAGS += -fsanitize=undefined
%__adr.o: CXXFLAGS += -fsanitize=address -O0
%__adr.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%__adr.out: LDFLAGS += -fsanitize=address
%__mem.o: CXXFLAGS += -fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -O0
%__mem.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
%__mem.out: LDFLAGS += -fsanitize=memory

.dep:
	mkdir -p $@
.PHONY: do
do: .dep $(addprefix test_sodium_drbg,.out __adr.out) $(addprefix test_openssl_aes_drbg,.out __adr.out)
.PHONY: clean
clean:
	$(RM) *.o *.out
.PHONY: check
check:
	set -xe; \
	for t in $$(ls -1 test_*.out | grep -v __); do \
		ldd ./$$t; \
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
