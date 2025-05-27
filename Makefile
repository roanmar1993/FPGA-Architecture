# Define the compiler
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# Define the executable file name
TARGET = mapper

# Define the source files
SRCS = main.cpp

# Default rule to build the program
all:

	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the generated files
clean:
	rm -f $(TARGET)