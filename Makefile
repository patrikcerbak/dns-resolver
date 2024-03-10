#
# Author: Patrik Cerbak
# Login: xcerba00
#

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra
SRC_DIR = src/
TEST_DIR = test/

dns: $(SRC_DIR)arguments.o $(SRC_DIR)error.o $(SRC_DIR)message.o $(SRC_DIR)response.o $(SRC_DIR)sending.o
	$(CC) $(CFLAGS) $(SRC_DIR)arguments.o $(SRC_DIR)error.o $(SRC_DIR)message.o $(SRC_DIR)response.o $(SRC_DIR)sending.o $(SRC_DIR)main.c -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

test: $(SRC_DIR)arguments.o $(SRC_DIR)error.o $(SRC_DIR)message.o $(SRC_DIR)response.o $(SRC_DIR)sending.o dns
	$(CC) $(CFLAGS) $(SRC_DIR)arguments.o $(SRC_DIR)error.o $(SRC_DIR)message.o $(SRC_DIR)response.o $(SRC_DIR)sending.o $(TEST_DIR)tests.c -o test/tests
	test/tests
#   Uncomment, if Python 3 is availiable on the system:
#	python3 test/tests.py

clean:
	rm -f $(SRC_DIR)*.o dns $(TEST_DIR)tests

