# Makefile for RAID-Level-Simulator

CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = raid_simulation
SRC = raid_simulation.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
