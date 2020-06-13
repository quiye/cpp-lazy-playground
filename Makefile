all: main
	./main
main: main.cpp
	g++-10 -lpthread -std=c++2a -Wall -Wextra -Werror -O2 -o main main.cpp
