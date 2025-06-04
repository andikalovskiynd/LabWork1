GTEST_INC_DIR ?= /usr/local/include
GTEST_LIB_DIR ?= /usr/local/lib

CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -Iinclude -I$(GTEST_INC_DIR) -fsanitize=address

LDFLAGS = -L$(GTEST_LIB_DIR) -lgtest_main -lgtest -pthread -fsanitize=address

TEST_TARGET = test_bmp
MAIN_TARGET = labwork1

FILE_SOURCES = Functions.cpp
TEST_MAIN_SOURCE = test.cpp

MAIN_APP_SOURCE = Main.cpp 

FILE_OBJECTS = $(patsubst %.cpp,%.o,$(FILE_SOURCES))
TEST_OBJECTS = $(patsubst %.cpp,%.o,$(TEST_MAIN_SOURCE))
MAIN_APP_OBJECTS = $(patsubst %.cpp,%.o,$(MAIN_APP_SOURCE)) 

all: $(TEST_TARGET) $(MAIN_TARGET)

$(TEST_TARGET): $(FILE_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(MAIN_TARGET): $(FILE_OBJECTS) $(MAIN_APP_OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


test: $(TEST_TARGET)
	./$(TEST_TARGET)


lab: $(MAIN_TARGET)
	./$(MAIN_TARGET)

clean:
	rm -f $(TEST_TARGET) $(MAIN_TARGET) $(FILE_OBJECTS) $(TEST_OBJECTS) $(MAIN_APP_OBJECTS)
	rm -f saved_4pixel.bmp 
	
.PHONY: all test run clean