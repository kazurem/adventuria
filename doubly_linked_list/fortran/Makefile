# Compilers and Preprocessors
FC = gfortran
FYPP = fypp
# Compilation Flags
FFLAGS = -fcheck=all -fsanitize=address -g

# Change or remove the paths appropriately.
# `-lfortran_stdlib_core` should not be changed.
INCLUDES = -I/usr/local/include/fortran_stdlib/GNU-16.1.1
LDFLAGS = -L/usr/local/lib -lfortran_stdlib_core

# Build Directory (keeps your working directory clean)
BUILD_DIR = build

# Source Files (all expected in the same directory as this Makefile)
FYPP_SRC = doubly_linked_list.fypp
MOD_SRC = $(BUILD_DIR)/doubly_linked_list.f90
TEST_SRC = test_doubly_linked_list.f90

# FORD project file
FORD_PROJECT_FILE = project_file.md

# Output Executable
TARGET = $(BUILD_DIR)/test_runner

.PHONY: all clean run docs

# Default target: compile everything
all: $(TARGET)

# Rule to preprocess the .fypp file into .f90 inside the build directory
$(MOD_SRC): $(FYPP_SRC)
	@mkdir -p $(BUILD_DIR)
	$(FYPP) $< $@

# Rule to compile the executable
# The -J flag tells gfortran to put/look for .mod files inside the build directory
$(TARGET): $(MOD_SRC) $(TEST_SRC)
	$(FC) $(FFLAGS) $(INCLUDES) -J $(BUILD_DIR) $(MOD_SRC) $(TEST_SRC) -o $(TARGET) $(LDFLAGS)

docs: $(MOD_SRC)
	ford $(FORD_PROJECT_FILE)

# Rule to run the test suite
run: $(TARGET)
	./$(TARGET)

# Clean up all generated build artifacts
clean:
	rm -rf $(BUILD_DIR) *.mod doc/
