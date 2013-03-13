CXX = icpc
CXXFLAGS = -fast -vec-report2 -fno-alias -ansi-alias -opt-subscript-in-range
LDFLAGS = $(CXXFLAGS)

-include local.mk

INITIAL = warm-circular-wave
BOUNDARY = periodic
EXTERNAL = Faddeeva

include build/Makefile
