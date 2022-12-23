parser:parser.cc
	g++ parser.cc -o parser -std=c++11 -lboost_system -lboost_filesystem -g
.PHONY:clean
clean:	
	rm -f parser
