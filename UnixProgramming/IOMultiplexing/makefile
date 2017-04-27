client:IOmultiplexingClient.o
	g++ IOmultiplexingClient.o  -o client

IOmultiplexingClient.o:IOmultiplexingClient.cpp pollClient.h
	g++ -c IOmultiplexingClient.cpp pollClient.h

clean:
	rm IOmultiplexingClient.o client