# ======================================================================
# TIVIDY makefile        (run `make help` to list the targets)
#
#   app/                 TUI front end (FTXUI)
#   src/                 engine sources (plain C++11)
#   include/             all headers
#   tests/               integration and unit tests (plus older pattern demos)
#   build/               every compiled file; safe to delete (make clean)
# ======================================================================

CXX  ?= g++
WARN := -Wall -Wextra -Wno-unused-parameter
INC  := -Iinclude

SRC_DIR   := src
APP_DIR   := app
BUILD_DIR := build

ENGINE_SRCS := $(wildcard $(SRC_DIR)/*.cpp)
APP_SRCS    := $(wildcard $(APP_DIR)/*.cpp)

INTEGRATION_TEST := tests/integration_tests/test_integration.cpp
UNIT_TEST        := tests/mediator_strategy_tests/test_mediator_strategy.cpp

# ---------------------------------------------------------------- TUI
# The front end needs C++17 and FTXUI. TIVIDY_TUI silences the engine's
# console logging so it does not draw over the screen.
TUI_FLAGS := -std=c++17 $(WARN) -g $(INC) -DTIVIDY_TUI
TUI_LIBS  := -lftxui-component -lftxui-dom -lftxui-screen -lpthread
TUI_OBJS  := $(patsubst %.cpp,$(BUILD_DIR)/tui/%.o,$(ENGINE_SRCS) $(APP_SRCS))

all: tividy

tividy: $(TUI_OBJS)
	$(CXX) $(TUI_FLAGS) -o $@ $^ $(TUI_LIBS)

$(BUILD_DIR)/tui/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(TUI_FLAGS) -MMD -MP -c $< -o $@

tui run: tividy
	./tividy

# -------------------------------------------------------------- tests
# Engine objects are built once with AddressSanitizer and UBSan and shared
# by every test program.
TEST_FLAGS := -std=c++11 $(WARN) -g -fsanitize=address,undefined $(INC)
SAN_OBJS   := $(patsubst %.cpp,$(BUILD_DIR)/san/%.o,$(ENGINE_SRCS))

$(BUILD_DIR)/san/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/bin/test_integration: $(BUILD_DIR)/san/$(INTEGRATION_TEST:.cpp=.o) $(SAN_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_FLAGS) -o $@ $^

$(BUILD_DIR)/bin/test_mediator_strategy: $(BUILD_DIR)/san/$(UNIT_TEST:.cpp=.o) $(SAN_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_FLAGS) -o $@ $^

# integration tests: mediator and strategies driving the real composites
test: $(BUILD_DIR)/bin/test_integration
	./$<

# unit tests for the Mediator and the Strategy classes
unit: $(BUILD_DIR)/bin/test_mediator_strategy
	./$<

# proves the engine is plain C++11 (only the front end needs C++17)
check11:
	@for f in $(ENGINE_SRCS); do $(CXX) -std=c++11 $(WARN) -fsyntax-only $(INC) $$f || exit 1; done; echo "engine compiles as C++11"

# ----------------------------------------------------------- valgrind
# Valgrind cannot run next to AddressSanitizer, so this uses its own build.
VG_FLAGS := -std=c++11 $(WARN) -g -O0 $(INC)
VG_OBJS  := $(patsubst %.cpp,$(BUILD_DIR)/vg/%.o,$(ENGINE_SRCS) $(INTEGRATION_TEST))

$(BUILD_DIR)/vg/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(VG_FLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/bin/test_integration_vg: $(VG_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(VG_FLAGS) -o $@ $^

valgrind: $(BUILD_DIR)/bin/test_integration_vg
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$<

# -------------------------------------------------------------- misc
clean:
	rm -rf $(BUILD_DIR) tividy

help:
	@echo "make            build the TUI (tividy)"
	@echo "make tui        build and run the TUI"
	@echo "make test       integration tests (sanitizers)"
	@echo "make unit       mediator and strategy unit tests (sanitizers)"
	@echo "make check11    check the engine compiles as C++11"
	@echo "make valgrind   integration tests under valgrind"
	@echo "make clean      delete build output"

DEPS := $(TUI_OBJS:.o=.d) $(SAN_OBJS:.o=.d) $(VG_OBJS:.o=.d)
-include $(DEPS)

.PHONY: all tui run test unit check11 valgrind clean help
