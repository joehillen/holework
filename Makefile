
.PHONY: lc

all :
	g++ -o server -lboost_system src/*.cpp src/network/*.cpp src/network/packet/*.cpp

lc:
	find . -regex '.*\.\(cpp\|h\)' -exec grep -v '^\s*\*' {} \; | grep -v '^\s*/\*' | grep -v "^\s*//" | grep -v "^\s*$$" | wc -l
