CXXFLAGS += -std=c++11
CXXFLAGS += -MMD

BUILD = $(PWD)/build

common = main global config barrier grid boundaries kick \
	 deposit ohm faraday drift output misc diagnostics \
	 initial-conditions/$(INITIAL) \
	 boundary-conditions/$(BOUNDARY) \
	 external/$(EXTERNAL)

objects = $(addprefix $(BUILD)/, $(addsuffix .o, $(common)))

deps := $(objects:.o=.d)

vf-pic: $(objects)
	$(CXX) $(LDFLAGS) $(objects) $(LOADLIBES) $(LDLIBS) -o vf-pic

-include $(deps)

%.s: %.cpp
	$(CXX) -S $(CXXFLAGS) $<

clean:
	rm -f vf-pic $(objects) $(deps)
