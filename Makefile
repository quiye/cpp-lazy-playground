all: main
	./main
main: main.cpp
	clang++ -stdlib=libc++ -lpthread -std=c++17 -Wall -Wextra -Werror -O2 -o main main.cpp

gcc: gcc-main
	./main
gcc-main: main.cpp
	g++ -o main -std=c++20 -Wall -Wextra -Werror -O2 main.cpp -lpthread
