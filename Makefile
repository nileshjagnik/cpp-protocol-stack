all: build

build:
	g++ driver1.cc -o d1 -lpthread
	g++ driver2.cc -o d2 -lpthread
	g++ driver3.cc -o d3 -lpthread
	g++ driver4.cc -o d4 -lpthread
	g++ driver5.cc -o d5 -lpthread

clean:
	rm -rf *~
	rm d1
	rm d2
	rm d3
	rm d4
	rm d5
