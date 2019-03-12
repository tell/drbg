include ../../Makefile

vpath %.hpp ../..
vpath %.cpp ../..

CXXFLAGS += -fsanitize=memory -fsanitize-memory-track-origins -fno-omit-frame-pointer -O0
LDFLAGS += -fsanitize=memory
