-include local.mk

CXXFLAGS += -std=c++11
CXXFLAGS += -MMD

all: vf-pic jacobi test

main = barrier.o boundaries.o config.o deposit.o drift-kick.o faraday.o global.o grid.o main.o ohm.o
deps := $(deps) $(main:.o=.d)
vf-pic: $(main)
	$(CXX) $(LDFLAGS) $(main) $(LDLIBS) -o vf-pic

jacobi = barrier.o boundaries.o config.o global.o jacobi.o
deps := $(deps) $(jacobi:.o=.d)
jacobi: $(jacobi)
	$(CXX) $(LDFLAGS) $(jacobi) $(LDLIBS) -o jacobi

test = config.o global.o test.o
deps := $(deps) $(test:.o=.d)
test: $(test)
	$(CXX) $(LDFLAGS) $(test) $(LDLIBS) -o test

sse = config.o global.o sse.o
deps := $(deps) $(sse:.o=.d)
sse: $(sse)
	$(CXX) $(LDFLAGS) $(sse) $(LDLIBS) -o sse

-include $(deps)

%.s: %.cc
	$(CXX) -S $(CXXFLAGS) $<

clean:
	rm -f byers jacobi test sse
	rm -f *.o *.d
	rm -rf *.dSYM
