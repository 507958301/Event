CC = g++
CFLAGS = -g -Wall -std=c++17

OBJSDIR = dist
TARGET = ./main
VPATH = $(OBJSDIR)

OBJS := $(patsubst %.cpp,$(OBJSDIR)/%.o,$(wildcard *.cpp))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJSDIR)/%.o: %.cpp | $(OBJSDIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJSDIR):
	mkdir $@

clean:
	rm -rf $(OBJSDIR) $(TARGET)
	rm -rf $(OBJSDIR)/*.o
