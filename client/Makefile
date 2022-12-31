CC = g++
CFLAGS = -Wall
DEPS = client_header.h
OBJ = client.o clientHandler.o downloadUtility.o clientToClientUtility.o uploadUtility.o hashing.o groupUtility.o clientAsServer.o variables.o logger.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

client: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -pthread
 
clean:
	rm -rf *o client