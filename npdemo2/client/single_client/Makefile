CC = clang++
CPPFLAGS = -Wall -O2 -pedantic -std=c++11

CLIENT_SRC = src/delayclient.cpp
CLIENT = $(patsubst src/%.cpp,%,$(CLIENT_SRC))


all: $(CLIENT)

%: src/%.cpp
	$(CC) $< $(CPPFLAGS) -o $@

clean:
	rm -f $(CLIENT)
