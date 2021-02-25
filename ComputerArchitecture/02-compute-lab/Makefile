#
# Makefile for the Compute Lab
#

CC		= gcc
CFLAGS	= -g -O2 -Wall

TARGET	= compute
CSRCS	= ctest.c compute.c
HEADERS	= compute.h
OBJS	= $(CSRCS:.c=.o)

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS) 

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) *~
