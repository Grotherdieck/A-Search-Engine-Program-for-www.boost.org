all:parser debug http_server update
parser:parser.cc
	g++ -o $@ $^ -std=c++11 -lboost_system -lboost_filesystem
debug:debug.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp
http_server:http_server.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp -lpthread
update:update.cc
	g++ -o $@ $^ -std=c++11 -lboost_system -lboost_filesystem
.PHONY:clean
clean:
	rm -rf parser debug http_server update
