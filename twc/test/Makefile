BUILD_DIR = build/
UNITY_DIR = unity/
SRC_DIR = ../
RESULTS_DIR = results/
TEST_DIR = ./

ifndef VERBOSE
.SILENT:
endif

CC := gcc
CFLAGS := -Wall  -I$(SRC_DIR) -I$(UNITY_DIR) -I$(TEST_DIR)

TEST_OBJ = $(patsubst Test%.c,Test%.o, $(wildcard *.c))
TEST_EXE = $(patsubst Test%.c,Test%.exe, $(wildcard *.c))
# SRC_OBJ = $(patsubst $(SRC_DIR)%.c, $(SRC_DIR)%.o, $(wildcard $(SRC_DIR)*.c))
# DEPEND_OBJ = $(patsubst Test%.c,Test%.d, $(wildcard *.c)) 
RESULTS = $(patsubst Test%.c,$(RESULTS_DIR)Test%.txt, $(wildcard *.c))

PASSED = `grep -s PASS $(RESULTS_DIR)*.txt`
FAIL = `grep -s FAIL $(RESULTS_DIR)*.txt`
IGNORE = `grep -s IGNORE $(RESULTS_DIR)*.txt`

.DELETE_ON_ERROR:

.PHONY: test
test: $(RESULTS)
	echo $(RESULTS)
	echo -----------------------
	echo IGNORES:
	echo "$(IGNORE)"
	echo -----------------------
	echo -----------------------
	echo FAILURES:
	echo "$(FAIL)"
	echo -----------------------
	echo -----------------------
	echo PASSED:
	echo "$(PASSED)"
	echo -----------------------
	echo 
	echo DONE

# Execute the files and pipe the results (stdout & stderr) to the text files.
$(RESULTS):: $(TEST_EXE)
	-./$(BUILD_DIR)$< > $@ 2>&1
	echo "Results piped..."

$(TEST_EXE) :: unity.o twc.o $(TEST_OBJ)
	cd build/; \
	$(CC) -o $@  $(patsubst ../%, ./% ,$^); 
	echo "Built executable $@ with $^ in $(BUILD_DIR)"

$(DEPEND_OBJ):: %.d : %.c
	$(CC) -MM -MF -MG $@ $<
	echo "Dependencies compiled in $(BUILD_DIR)..."

$(TEST_OBJ):: %.o : %.c $(BUILD_DIR) $(RESULTS_DIR) 
	$(CC) $< -c $(CFLAGS) 
	mv $@ build/
	echo "Object $@ in $(TEST_DIR) compiled to $(BUILD_DIR)."

# Can use this line instead to use all .c files in the directory
# $(SRC_OBJ):: %.o : %.c $(BUILD_DIR) $(RESULTS_DIR)
twc.o : $(SRC_DIR)twc.c $(BUILD_DIR) $(RESULTS_DIR)
	$(CC) $< -c $(CFLAGS) 
	mv $(patsubst ../%, ./% , $@) build/
	echo "Object $@ in $(SRC_DIR) compiled to $(BUILD_DIR)."

unity.o:: $(UNITY_DIR)unity.c $(BUILD_DIR) $(RESULTS_DIR)
	$(CC) $< -c $(CFLAGS) 
	mv $@ build/
	echo "Object $@ in $(UNITY_DIR) compiled to $(BUILD_DIR)."

$(BUILD_DIR):
	mkdir build/
	echo "Created a build/ directory."

$(RESULTS_DIR):
	mkdir results/
	echo "Created a results/ directory."

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(BUILD_DIR) -r
	rm -f $(RESULTS_DIR) -r

.PRECIOUS: *.o
.PRECIOUS: $(BUILD_DIR)*.$(TARGET_EXTENSION)
.PRECIOUS: $(RESULTS_DIR)*.txt
.PRECIOUS: *.d
