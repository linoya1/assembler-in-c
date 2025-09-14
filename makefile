CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
ASSEMBLER_DIR = assembler

INCLUDES = -I$(INCLUDE_DIR)

# רשימת קבצי המקור והאובייקט
SRCS = $(filter-out $(SRC_DIR)/error_warning.c $(SRC_DIR)/instruction_encoder.c, $(wildcard $(SRC_DIR)/*.c))
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# כלל לקבצי האובייקט
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/*.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# כלל להרכבת התוכנית
assembler_program: $(OBJS) $(ASSEMBLER_DIR)/assembler.o
	$(CC) $(CFLAGS) $(OBJS) $(ASSEMBLER_DIR)/assembler.o -o $(ASSEMBLER_DIR)/assembler_program

# כלל לקובץ האסמבלר
$(ASSEMBLER_DIR)/assembler.o: $(ASSEMBLER_DIR)/assembler.c $(INCLUDE_DIR)/*.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $(ASSEMBLER_DIR)/assembler.c -o $(ASSEMBLER_DIR)/assembler.o

# כלל לניקוי הקבצים המורכבים
clean:
	rm -f $(OBJ_DIR)/*.o $(ASSEMBLER_DIR)/assembler.o $(ASSEMBLER_DIR)/assembler_program

