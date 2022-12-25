.PHONY:all
all: parser debug http_server 

http_server:http_server.cc
	g++ http_server.cc -o http_server -ljsoncpp -std=c++11 -lboost_filesystem -lboost_system -lpthread -g

parser:parser.cc
	g++ parser.cc -o parser -std=c++11 -lboost_system -lboost_filesystem -g

debug:debug.cc
	g++ debug.cc -o debug -ljsoncpp -std=c++11 -lboost_filesystem -lboost_system -g
.PHONY:clean
clean:	
	rm -f parser debug http_server
