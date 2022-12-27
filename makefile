.PHONY:all
all: parser debug http_server 

SLFLAG=-ljsoncpp  -lboost_filesystem -lboost_system -lpthread -lmysqlclient 
PLFLAG=-lboost_system -lboost_filesystem 
CFLAG=-std=c++11
HFLAG=-I ./spdlog/include
cc=g++


http_server:http_server.cc
	$(cc) $< -o $@  $(SLFLAG) $(CFLAG) $(HFLAG) -g  

parser:parser.cc
	$(cc) $< -o $@ $(PLFLAG) $(CFLAG) $(HFLAG) -g

debug:debug.cc
	$(cc) $< -o $@ $(CFLAG) $(HFLAG) $(SLFLAG)  -g

.PHONY:clean
clean:	
	rm -f parser debug http_server
