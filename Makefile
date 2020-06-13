all: main
	./main
main: main.cpp
	g++-10 -stdlib=libc++ -lpthread -std=c++17 -Wall -Wextra -Werror -O2 -o main main.cpp
