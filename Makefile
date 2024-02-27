CC = g++
CFLAGS = -O2
LIBS = -lgmpxx -lgmp 

poly1305gen : poly1305gen.cpp 
	$(CC) $(CFLAGS) poly1305gen.cpp $(LIBS) -o poly1305-gen

poly1305test : poly1305test.cpp 
	$(CC) $(CFLAGS) poly1305test.cpp $(LIBS) -o poly1305-test

all : poly1305gen poly1305test

