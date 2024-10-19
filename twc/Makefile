OBJECTS := twc.o main.o
BUILD_DIR := ./build
TARGET := twc
CFLAGS := -Wall 
CC := clang

ifndef VERBOSE
.SILENT:
endif

.DELETE_ON_ERROR:

.PHONY: all
all: $(OBJECTS)
	cd $(BUILD_DIR); \
	$(CC) $^ -o $(TARGET) $(CFLAGS);
	echo "Executable compiled with $(CC) $(CFLAGS)"

$(OBJECTS): %.o: %.c build
	cd $(BUILD_DIR); \
	$(CC) ../$< -c -I. $(CFLAGS);
	echo "$@ compiled with $<"

build:
	mkdir $(BUILD_DIR)
	echo "Created /build in" `pwd` 

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	echo "Project /build cleaned."

.PHONY: test
test:: 
	echo "Running tests..."
	# $(MAKE) test -C ./test
	cd ./test; \
	$(MAKE) test;

.PHONY: coverage 
coverage: 
	echo "Running coverage..."
	$(MAKE) coverage -C ./test
