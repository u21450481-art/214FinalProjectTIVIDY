# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g

# Executable name
TARGET   = test_builder

# Option A: Compile ONLY the Builder/Prototype/Composite files needed for this test
# (Uncomment Option B below if you prefer to compile every .cpp file in the root directory)
SRCS = testBuilder.cpp \
       CustomDefinitionBuilder.cpp \
       WorkflowDefinitionBuilder.cpp \
       Director.cpp \
       WorkflowDefinition.cpp \
       WorkflowInstance.cpp \
       DataPayload.cpp \
       WorkflowComponent.cpp \
       Subprocess.cpp \
       Task.cpp \
       Department.cpp \
       Employee.cpp \
       Stage.cpp \
       ConcreteStages.cpp \
       DepthFirstIterator.cpp

# Option B: Automatically grab all .cpp files in root directory
# SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files into executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile .cpp files into .o object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile and immediately run the test suite
run: all
	./$(TARGET)

# Check for memory leaks with Valgrind (run inside WSL Ubuntu)
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Remove compiled binaries and object files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run valgrind clean