.PHONY:all
all: parser server


parser:parser.cc
	g++ parser.cc -o parser -std=c++11 -lboost_system -lboost_filesystem -g

server:server.cc
	g++ server.cc -o server -ljsoncpp -std=c++11 -lboost_filesystem -lboost_system -g
.PHONY:clean
clean:	
	rm -f parser
