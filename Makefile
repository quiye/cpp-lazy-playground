all: main
	./main
main: main.cpp
	g++ -o main -std=c++20 -Wall -Wextra -Werror -O2 main.cpp -lpthread
