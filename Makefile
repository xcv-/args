CXX=/usr/bin/clang++
CXXFLAGS=-std=c++11 -Ilib
LDFLAGS=-static -Llib -largs

OUT=a.out

all: libargs test

libargs:
	make -C lib

test:
	$(CXX) $(CXXFLAGS) main.cpp -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
