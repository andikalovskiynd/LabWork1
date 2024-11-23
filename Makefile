PROJECT = labwork
LIBPROJECT = $(PROJECT).a
TESTPROJECT = test-$(PROJECT)

CXX = g++
A = ar
AFLAGS = rcs

CXXFLAGS = -std=c++17 -Wall -Werror -Wpedantic -g
LDFLAGS = $(CXXFLAGS)
LDGTESTFLAGS = $(LDFLAGS) -lgtest -lgtest_main -lpthread

DEPS = $(wildcard *.h)

SRCS = Main.cpp Functions.cpp
OBJS = $(SRCS:.cpp=.o)

TEST-OBJS = test-main.o test-functions.o

.PHONY: default
default: all

all: $(PROJECT)

$(PROJECT): Main.o $(LIBPROJECT)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(LIBPROJECT): $(OBJS)
	$(A) $(AFLAGS) $@ $^

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TESTPROJECT): $(LIBPROJECT) $(TEST-OBJS)
	$(CXX) -o $@ $(TEST-OBJS) $(LIBPROJECT) $(LDGTESTFLAGS)

test: $(TESTPROJECT)

.PHONY: clean
clean:
	rm -f *.o

cleanall: clean
	rm -f $(PROJECT)
	rm -f $(LIBPROJECT)
	rm -f $(TESTPROJECT)
