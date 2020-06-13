all: main
	./main
main: main.cpp
	g++-10 -o main -std=c++2a -Wall -Wextra -Werror -O2 main.cpp -lpthread
