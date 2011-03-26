
.PHONY: lc

all :
	g++ -Wall -Wno-unused-function -o server -lboost_system src/*.cpp src/network/*.cpp 

lc:
	find . -regex '.*\.\(cpp\|h\)' -exec grep -v '^\s*\*' {} \; | grep -v '^\s*/\*' | grep -v "^\s*//" | grep -v "^\s*$$" | wc -l
