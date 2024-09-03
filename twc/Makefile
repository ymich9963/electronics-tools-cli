OBJECTS := twc.o
BUILD_DIR := ./build
TARGET := twc
CFLAGS := -Wall 
CC := clang

.DELETE_ON_ERROR:

.PHONY: all
all: $(OBJECTS)
	@cd $(BUILD_DIR); \
	$(CC) $^ -o $(TARGET) $(CFLAGS);
	@echo "Executable compiled with $(CC) $(CFLAGS)"
	@#rm -f *.o

$(OBJECTS): %.o: %.c build
	@cd $(BUILD_DIR); \
	$(CC) ../$< -c -I. $(CFLAGS);
	@echo "$@ compiled with $<"

build:
	@mkdir $(BUILD_DIR); \
	echo "Created /build in" `pwd` 

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Project /build cleaned."
