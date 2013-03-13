CXX = icpc
CXXFLAGS = -O3 -no-prec-div -xHost -vec-report2 -fno-alias -ansi-alias -opt-subscript-in-range
#CXXFLAGS += -D_GLIBCXX_USE_SCHED_YIELD
#CXXFLAGS += -profile-functions -profile-loops=all -profile-loops-report=2
LDFLAGS = $(CXXFLAGS) -pthread

-include local.mk

INITIAL = warm-circular-wave
BOUNDARY = periodic
EXTERNAL = Faddeeva

include build/Makefile
