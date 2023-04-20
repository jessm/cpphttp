CFLAGS = -std=c++17 -g -Isrc

CC = g++
LD = g++

SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,build/%.o,$(SRCS))
TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $^ -o build/$(TARGET)

build/%.o: src/%.cpp
	mkdir -p build
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY:
run: $(TARGET)
	./build/$(TARGET)

.PHONY:
test: $(TARGET)
	./build/$(TARGET) $(TEST_PARAMS)

.PHONY:
load: $(TARGET)
	rm -f perf.* out.svg
	perf record -F 1000 -g --call-graph dwarf ./build/server &
	locust -f locustfile.py -u 500 -r 100 -t 5 -H http://localhost:8080 --headless --only-summary || true
	killall -INT ./build/server
	sleep 1
	perf script | ./FlameGraph/stackcollapse-perf.pl | ./FlameGraph/flamegraph.pl > out.svg

.PHONY:
clean:
	rm -rf build out.svg
