.PHONY: clean build all

# compilation settings
CC = gcc -lm
CFLAGS = -I$(INCLUDE_DIR) -Wall -Wextra -Werror -pedantic -std=gnu99 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -g

# directory paths
INCLUDE_DIR = ./
SRC_DIR = ./
OBJ_DIR = ./

# file lists
CFILES = hashmap.c 
OBJS = hashmap.o 

# binary
BIN = search

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<
 	
$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS)
 		
clean:
	rm -f $(OBJS) $(BIN) *~
