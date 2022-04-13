.SILENT:
	
CC = gcc
# no unused parameter suppresses the unused parameter warnings
CFLAGS = -Wall -Wextra -Wcast-align -g -Wno-unused-parameter
LFLAGS = -lm -lmpfr -lgmp
SRC_INCLUDE = -Isrc

BUILD_DIR = build
SRC_DIR = src
TESTS_DIR = tests

OBJS_FILE_NAMES = jc_util.o sub_stream.o math_parser.o iof_num.o history.o syntax_parser.o
OBJS_PATHS=$(patsubst %,$(BUILD_DIR)/%,$(OBJS_FILE_NAMES))

_TEST_OBJS = sub_stream_tests.o josh_tests.o jc_util_tests.o
TEST_OBJS=$(patsubst %,$(TESTS_DIR)/%,$(_TEST_OBJS))

DEBUG_STR = $@


all: math_parser


math_parser: $(SRC_DIR)/math_parser_driver.c math_parser.o history.o
	$(CC) $(CFLAGS) $(SRC_DIR)/math_parser_driver.c $(BUILD_DIR)/math_parser.o $(BUILD_DIR)/iof_num.o $(BUILD_DIR)/jc_util.o $(BUILD_DIR)/history.o $(LFLAGS) -o math_parser

math_parser.o: $(SRC_DIR)/math_parser.c $(SRC_DIR)/math_parser.h iof_num.o jc_util.o
	$(CC) $(CFLAGS) -c $(SRC_DIR)/math_parser.c -o $(BUILD_DIR)/math_parser.o

jc_util.o: $(SRC_DIR)/jc_util.h $(SRC_DIR)/jc_util.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/jc_util.c -o $(BUILD_DIR)/$@

iof_num.o: $(SRC_DIR)/iof_num.h $(SRC_DIR)/iof_num.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/iof_num.c -o $(BUILD_DIR)/$@

history.o: $(SRC_DIR)/history.h $(SRC_DIR)/history.c $(SRC_DIR)/math_parser.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/history.c -o $(BUILD_DIR)/history.o

syntax_parser.o: $(SRC_DIR)/syntax_parser.h $(SRC_DIR)/syntax_parser.c math_parser.o
	$(CC) $(CFLAGS) -c $(SRC_DIR)/syntax_parser.c -o $(BUILD_DIR)/syntax_parser.o



# I couldn't figure out how to make visual studio code detect that header
# files were included from another directory, so I just manually include headers from src
# for test files for now. Not good, but it works for now...
tests:	jc_util_tests math_parser_tests

jc_util_tests: $(TESTS_DIR)/jc_util_tests.c jc_util.o
	$(CC) $(CFLAGS) -c $(TESTS_DIR)/$@.c -o $(BUILD_DIR)/$@.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/jc_util.o $(BUILD_DIR)/$@.o -o $(BUILD_DIR)/$@
	$(BUILD_DIR)/$@

math_parser_tests: $(TESTS_DIR)/math_parser_tests.c jc_util.o iof_num.o math_parser.o history.o
	$(CC) $(CFLAGS) -c $(TESTS_DIR)/$@.c  -o $(BUILD_DIR)/$@.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/math_parser.o $(BUILD_DIR)/jc_util.o $(BUILD_DIR)/iof_num.o $(BUILD_DIR)/history.o $(BUILD_DIR)/$@.o $(LFLAGS) -o $(BUILD_DIR)/$@ -lm
	$(BUILD_DIR)/$@

syntax_parser_tests: $(TESTS_DIR)/syntax_parser_tests.c syntax_parser.o math_parser.o iof_num.o
	$(CC) $(CFLAGS) -c $(TESTS_DIR)/$@.c  -o $(BUILD_DIR)/$@.o
	$(CC) $(CFLAGS) $(BUILD_DIR)/syntax_parser.o $(BUILD_DIR)/math_parser.o $(BUILD_DIR)/jc_util.o $(BUILD_DIR)/iof_num.o $(BUILD_DIR)/$@.o $(LFLAGS) -o $(BUILD_DIR)/$@
	$(BUILD_DIR)/$@

clean:
	rm -f BIN_DIR/*

debug_make:
	echo $(DEBUG_STR)
