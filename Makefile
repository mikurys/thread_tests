make:
	g++ -std=c++14 -g -pthread -lrt -o main main.cpp

tests:
	g++ -std=c++14 -g -pthread -lrt -o xor_test xor_test.cpp
	./xor_test -n 100000 ${ARGS}

all_tests:
	g++ -std=c++14 -g -pthread -lrt -o xor_test xor_test.cpp
	./xor_test -n 100000 ${ARGS}
	./xor_test -n 100000 -mb ${ARGS}
	./xor_test -n 100000 -tp ${ARGS}
	./xor_test -n 100000 -tp -mb ${ARGS}

clang:
	clang++ -std=c++14 -g -pthread -lrt -o main main.cpp

fsanitize:
	clang++ -fsanitize=thread -std=c++14 -g -pthread -lrt -o main main.cpp

asm:
	g++ -std=c++14 -pthread -o main.S main.cpp -S

run:
	./main ${ARGS}

helgrind:
	valgrind --read-var-info=yes --tool=helgrind ./main ${ARGS}

clean:
	rm main main.S xor_test chart.jpg t*
