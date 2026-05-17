CC = clang
CFLAGS = -Wall
TARGET = clox

SRC_DIR = .
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

ALL_SRCS = $(wildcard $(SRC_DIR)/*.c)
PROJECT_SRCS = $(filter-out $(SRC_DIR)/main.c, $(ALL_SRCS))
PROJECT_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(PROJECT_SRCS))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_BINS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%, $(TEST_SRCS))

all: $(TARGET)

$(TARGET): $(OBJ_DIR)/main.o $(PROJECT_OBJS) $(SRC_DIR)/*.h
	$(CC) $(CFLAGS) $(OBJ_DIR)/main.o $(PROJECT_OBJS) -o $(TARGET)

debug: all
	leaks -atExit -- ./$(TARGET)

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%: $(TEST_DIR)/%.c $(PROJECT_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TEST_BINS)
	@echo "=================================================="
	@echo "Running all tests..."
	@echo "=================================================="
	@for test in $(TEST_BINS); do \
		echo "Running $$test..."; \
		./$$test 2>&1; \
		status=$$?; \
		if [ $$status -eq 0 ]; then \
			echo "🟢 $$test SUCCESS"; \
		else \
			echo "🔴 $$test FAIL (Exit Code: $$status)"; \
		fi; \
	done; \
	echo "=================================================="

clean:
	rm -f $(TARGET)
	rm -rf $(BUILD_DIR)
	rm -rf clox.dSYM

run: all
	./$(TARGET)