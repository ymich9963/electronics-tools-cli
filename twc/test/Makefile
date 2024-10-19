BUILD_DIR = build/
UNITY_DIR = unity/
SRC_DIR = ../
RESULTS_DIR = results/
TEST_DIR = ./

ifndef VERBOSE
.SILENT:
endif

ifndef FUNCTIONS
COVFLAGS := -m -l
else
COVFLAGS := -m -l -f
endif

.DELETE_ON_ERROR:

CC := clang 
test: CFLAGS := -Wall  -I$(SRC_DIR) -I$(UNITY_DIR) -I$(TEST_DIR)
coverage: CFLAGS := -Wall  -I$(SRC_DIR) -I$(UNITY_DIR) -I$(TEST_DIR) --coverage

TEST_OBJ = $(patsubst Test%.c,Test%.o, $(wildcard *.c))
TEST_EXE = $(patsubst Test%.c,Test%.exe, $(wildcard *.c))
RESULTS = $(patsubst Test%.c,$(RESULTS_DIR)Test%.txt, $(wildcard *.c))
COV_FILES = $(wildcard *.gc*)

# If grep doesn't find the output, output a blank line
PASSED = grep -s :PASS $(RESULTS_DIR)*.txt || echo ""
FAIL = grep -s :FAIL $(RESULTS_DIR)*.txt || echo ""
IGNORE = grep -s :IGNORE $(RESULTS_DIR)*.txt || echo ""

.PHONY: test clean coverage
test: $(RESULTS)
	echo $(RESULTS)
	echo -----------------------
	echo IGNORES:
	$(IGNORE)
	echo -----------------------
	echo -----------------------
	echo FAILURES:
	$(FAIL)
	echo -----------------------
	echo -----------------------
	echo PASSED:
	$(PASSED)
	echo -----------------------
	echo 
	echo DONE
	echo 

# Execute the files and pipe the results (stdout & stderr) to the text files.
$(RESULTS):: $(TEST_EXE)
	-./$(BUILD_DIR)$< > $@ 2>&1
	echo "Results piped..."

$(TEST_EXE) :: unity.o twc.o $(TEST_OBJ) 
	cd build/; \
		$(CC) -o $@  $(patsubst ../%, ./% ,$^) $(CFLAGS); 
	echo "Built executable $@ with $^ in $(BUILD_DIR)"

$(TEST_OBJ):: %.o : %.c 
	$(CC) $< -c $(CFLAGS) 
	mv $@ build/
	echo "Object $@ in $(TEST_DIR) compiled to $(BUILD_DIR)."

twc.o : $(SRC_DIR)twc.c
	$(CC) $< -c $(CFLAGS) 
	mv $(patsubst ../%, ./% , $@) build/
	echo "Object $@ in $(SRC_DIR) compiled to $(BUILD_DIR)."

unity.o:: $(UNITY_DIR)unity.c $(BUILD_DIR) $(RESULTS_DIR)
	$(CC) $< -c $(CFLAGS) 
	mv $@ $(BUILD_DIR)
	echo "Object $@ in $(UNITY_DIR) compiled to $(BUILD_DIR)."

$(BUILD_DIR):
	mkdir build/
	echo "Created a build/ directory."

$(RESULTS_DIR):
	mkdir results/
	echo "Created a results/ directory."

coverage:: $(RESULTS) 
	echo "Program executed."
	echo
	llvm-cov gcov twc.c $(COVFLAGS)
	mv *.gc* $(BUILD_DIR)
	echo "Coverage files moved to $(BUILD_DIR)."

clean:
	rm -f $(BUILD_DIR) -r
	rm -f $(RESULTS_DIR) -r
