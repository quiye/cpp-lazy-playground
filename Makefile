all: main
	./main
main: main.cpp
	g++ -o main -std=c++2a -Wall -Wextra -Werror -O2 main.cpp -lpthread
