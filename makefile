# CC = g++
# TARGET = output
# $(TARGET):obj.o
# 	$(CC) -o $< $@
# obj.o : %o:%cpp
# 	$(CC) -c $< -0 $@
# clean:
# 	rm -f $(TARGET) obj.o
# CC = g++
# CFLAGS=-c -Wall -std=c++17
# LFLAGS = -Wall -std=c++17
# CXXFLAGS = -Wall -std=c++17

# TARGET = main
# $(TARGET): $(TARGET).o
# 	$(CC) -g -std=c++17 -o $(TARGET) $(TARGET).o

# $(TARGET).o: $(TARGET).cpp
# 	$(CC) -g -std=c++17 -o $(TARGET).o -c $(TARGET).cpp
	
# clean:
# 	rm -rf $(TARGET)
# 	rm -rf *.o


# CC = g++
# CFLAGS= -g -Wall -std=c++17
# LFLAGS = -Wall -std=c++17
# CXXFLAGS = -Wall -std=c++17

# OBJSDIR = dist
# TARGET = main
# VPATH = $(OBJSDIR)
# $(TARGET): $(OBJSDIR) $(TARGET).o
# 	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o

# $(TARGET).o: $(TARGET).cpp
# 	$(CC) -c $(TARGET).cpp $(CFLAGS) -o $(TARGET).o 
# $(OBJSDIR):
# 	mkdir $@
# clean:
# 	rm -rf $(TARGET)
# 	rm -rf *.o

CC = g++
CFLAGS= -g -Wall -std=c++17
LFLAGS = -Wall -std=c++17
CXXFLAGS = -Wall -std=c++17

OBJSDIR = dist
TARGET = main
# VPATH = $(OBJSDIR)
OBJS = $(TARGET).o
$(TARGET): $(OBJSDIR) $(OBJS)
	$(CC) -o $(TARGET) $(OBJSDIR)/*.o $(CFLAGS)

$(OBJS): %.o:%.cpp
	$(CC) -c $(CFLAGS) $^ -o $(OBJSDIR)/$@ 
$(OBJSDIR):
	mkdir $@
clean:
	rm -rf $(TARGET).exe
	rm -rf *.o
