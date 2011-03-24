
all :
	g++ -o server -lboost_system src/*.cpp src/network/*.cpp src/network/packet/*.cpp
