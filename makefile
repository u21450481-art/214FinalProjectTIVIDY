# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter -g
LDFLAGS = -lftxui-component -lftxui-dom -lftxui-screen -lpthread #for ftxui

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
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile .cpp files into .o object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile and immediately run the test suite
run: all
	./$(TARGET)
# ---- TUI build ----
TUI_SRCS = $(filter-out test%.cpp %Handler.cpp ApprovalRequest.cpp ConcreteStrategies.cpp EngineMediator.cpp,$(wildcard *.cpp))
TUI_OBJS = $(TUI_SRCS:.cpp=.tui.o)

tividy: $(TUI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.tui.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -DTIVIDY_TUI -c $< -o $@

-include $(TUI_OBJS:.o=.d)

tui: tividy
	./tividy

# Check for memory leaks with Valgrind (run inside WSL Ubuntu)
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Remove compiled binaries and object files
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f $(TUI_OBJS) $(TUI_OBJS:.o=.d) tividy

.PHONY: all run valgrind clean