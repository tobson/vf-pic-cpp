-include local.mk

CXXFLAGS += -std=c++11
CXXFLAGS += -MMD

all: byers jacobi test

byers = barrier.o byers.o config.o global.o
deps := $(deps) $(byers:.o=.d)
byers: $(byers)
	$(CXX) $(LDFLAGS) $(byers) $(LDLIBS) -o byers


jacobi = barrier.o config.o global.o jacobi.o
deps := $(deps) $(jacobi:.o=.d)
jacobi: $(jacobi)
	$(CXX) $(LDFLAGS) $(jacobi) $(LDLIBS) -o jacobi

test = config.o $(fields) $(particles) test.o
deps := $(deps) $(test:.o=.d)
test: $(test)
	$(CXX) $(LDFLAGS) $(test) $(LDLIBS) -o test

-include $(deps)

%.s: %.cc
	$(CXX) -S $(CXXFLAGS) $<

clean:
	rm -f byers jacobi test
	rm -f *.o *.d
	rm -rf *.dSYM
