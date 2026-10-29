CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter -g
LDFLAGS  = -lftxui-component -lftxui-dom -lftxui-screen -lpthread

# ---- test build ----
TARGET = test_builder
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
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# -MMD -MP writes a .d file so header changes trigger rebuilds
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: all
	./$(TARGET)

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# ---- TUI build ----
# Remove ConcreteStrategies.cpp and EngineMediator.cpp from this filter
# once the new facade (which uses them) is in.
TUI_SRCS = $(filter-out test%.cpp %Handler.cpp ApprovalRequest.cpp ConcreteStrategies.cpp EngineMediator.cpp,$(wildcard *.cpp))
TUI_OBJS = $(TUI_SRCS:.cpp=.tui.o)

tividy: $(TUI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.tui.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -DTIVIDY_TUI -c $< -o $@

tui: tividy
	./tividy

-include $(OBJS:.o=.d) $(TUI_OBJS:.o=.d)

# Wildcards, so files that were filtered out earlier are cleaned too
clean:
	rm -f *.o *.d $(TARGET) tividy

.PHONY: all run valgrind tui clean