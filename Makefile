all: build

build:
	g++ main.cpp -o m
	#g++ driver1.cc -o d1
	#g++ driver2.cc -o d2
	#g++ driver3.cc -o d3

clean:
	rm -rf *~
	#rm d1
	#rm d2
	#rm d3
	rm m