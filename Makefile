all: build

build:
	g++ -g driver1.cc -o d1 -lpthread
	g++ driver2.cc -o d2 -lpthread
	g++ driver3.cc -o d3 -lpthread

clean:
	rm -rf *~
	rm d1
	rm d2
	rm d3
