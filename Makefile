.DEFAULT_GOAL := do

CC = $(CXX)
CXX = clang++
CPPFLAGS += -g3 -MMD -MP -MF .dep/$(basename $(notdir $@)).d
CPPFLAGS += $(shell pkg-config --cflags libsodium)
CPPFLAGS += $(shell pkg-config --cflags openssl)
CXXFLAGS += -O2 -std=c++14 -Wall -Wextra
LDFLAGS += -g3
LOADLIBES += $(shell pkg-config --libs libsodium)
LOADLIBES += $(shell pkg-config --libs openssl)

####

%.out: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

.dep:
	mkdir -p $@
.PHONY: do
do: .dep test_sodium_drbg.out test_openssl_aes_drbg.out
.PHONY: clean
clean:
	$(RM) *.o *.out
.PHONY: check
check: do
	set -xe; \
	for t in $$(ls -1 test_*.out | grep -v __); do \
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
