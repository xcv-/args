CXX=/usr/bin/clang++
CXXFLAGS=-std=c++11 -I.
LDFLAGS=

SRCS=$(shell find . -name '*.cpp')
OUT_LIB=libargs.so
OUT_STATIC=libargs.a

all: build

cleanup:
	rm -f $(OBJS)

build: $(SRCS)
	for f in $(SRCS); do \
		echo $(CXX) $(CXXFLAGS) -c $$f; \
		$(CXX) $(CXXFLAGS) -c $$f || exit 1; \
	done
	$(CXX) *.o -shared -o $(OUT_LIB) $(LDFLAGS)
	ar rcs $(OUT_STATIC) *.o
	rm *.o

clean:
	rm -f $(OUT_LIB) $(OUT_STATIC)
