all: main
	./main
main: main.cpp
	g++ -lpthread -std=c++17 -Wall -Wextra -Werror -O2 -o main main.cpp
