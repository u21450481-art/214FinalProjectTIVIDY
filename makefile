CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wno-unused-parameter -g
LDFLAGS  = -lftxui-component -lftxui-dom -lftxui-screen -lpthread

# ---- test build ----
TARGET = test_builder
SRCS = main.cpp \
       WorkflowEngine.cpp \
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
       ConcreteStrategies.cpp \
       DepthFirstIterator.cpp \
       EngineMediator.cpp \
       Command.cpp \
       CommandHistory.cpp \
       MacroCommand.cpp \
       SetDataCommand.cpp \
       StartCommand.cpp \
       CompleteCommand.cpp \
       RejectCommand.cpp \
       CancelCommand.cpp \
       AssignTaskCommand.cpp \
       StateTransitionCommand.cpp \
       RecordDecisionCommand.cpp \
       DataAdapter.cpp \
       ClaimsDataAdapter.cpp \
       ClaimsSystem.cpp \
       HrDataAdapter.cpp \
       LegacyHrSystem.cpp \
       ReturnsDataAdapter.cpp \
       ReturnsSystem.cpp \
       ApprovalRequest.cpp \
       ManagerHandler.cpp \
       DirectorHandler.cpp
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
# ---- TUI build ----
# Include all source files without any exclusions
TUI_SRCS = main.cpp \
       WorkflowEngine.cpp \
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
       ConcreteStrategies.cpp \
       DepthFirstIterator.cpp \
       EngineMediator.cpp \
       Command.cpp \
       CommandHistory.cpp \
       MacroCommand.cpp \
       SetDataCommand.cpp \
       StartCommand.cpp \
       CompleteCommand.cpp \
       RejectCommand.cpp \
       CancelCommand.cpp \
       AssignTaskCommand.cpp \
       StateTransitionCommand.cpp \
       RecordDecisionCommand.cpp \
       DataAdapter.cpp \
       ClaimsDataAdapter.cpp \
       ClaimsSystem.cpp \
       HrDataAdapter.cpp \
       LegacyHrSystem.cpp \
       ReturnsDataAdapter.cpp \
       ReturnsSystem.cpp \
       ApprovalRequest.cpp \
       ManagerHandler.cpp \
       DirectorHandler.cpp

TUI_OBJS = $(TUI_SRCS:.cpp=.tui.o)

tividy: $(TUI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.tui.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -DTIVIDY_TUI -c $< -o $@

tui: tividy
	./tividy
# Wildcards, so files that were filtered out earlier are cleaned too
clean:
	rm -f *.o *.d $(TARGET) tividy

.PHONY: all run valgrind tui clean

# ---- checks added with the Mediator/Strategy integration fixes ----
# engine sources only (everything except the FTXUI front end)
ENGINE_SRCS = $(filter-out main.cpp,$(SRCS))

# integration tests: mediator + strategies driving the real composites (run: make test)
test: tests/test_integration.cpp $(ENGINE_SRCS)
	$(CXX) -std=c++11 -Wall -Wextra -Wno-unused-parameter -g -fsanitize=address,undefined -I. -o test_integration $^
	./test_integration

# proves the engine is plain C++11 (the FTXUI front end needs a newer standard)
check11:
	@for f in $(ENGINE_SRCS); do $(CXX) -std=c++11 -Wall -Wextra -Wno-unused-parameter -fsyntax-only -I. $$f || exit 1; done; echo "engine compiles as C++11"

.PHONY: test check11
