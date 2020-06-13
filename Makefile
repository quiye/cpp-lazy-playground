all: main
	./main
main: main.cpp
	clang++ -std=c++17 -Wall -Wextra -Werror -O2 -o main main.cpp
