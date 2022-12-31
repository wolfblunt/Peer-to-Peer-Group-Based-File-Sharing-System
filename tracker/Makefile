CC = g++
CFLAGS = -Wall
DEPS = trackerHeader.h
OBJ = tracker.o trackerHandler.o featureDefinition.o variables.o logger.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

tracker: $(OBJ)
	$(CC) $(CFLAGS) -pthread -o $@ $^ 

clean:
	rm -rf *o tracker