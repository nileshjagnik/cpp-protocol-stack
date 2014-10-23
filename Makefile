all: build

build:
	g++ main.cpp -o m

clean:
	rm -rf *~
	rm m
