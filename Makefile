-include local.mk

CXXFLAGS += -std=c++11
CXXFLAGS += -MMD

all: vf-pic jacobi test

main = config.o global.o grid.o main.o
deps := $(deps) $(main:.o=.d)
vf-pic: $(main)
	$(CXX) $(LDFLAGS) $(main) $(LDLIBS) -o vf-pic

jacobi = barrier.o config.o global.o jacobi.o
deps := $(deps) $(jacobi:.o=.d)
jacobi: $(jacobi)
	$(CXX) $(LDFLAGS) $(jacobi) $(LDLIBS) -o jacobi

test = config.o global.o test.o
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
