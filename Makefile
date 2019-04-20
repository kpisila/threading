LFLAG=-lpthread

lab7: funcTest.c
	gcc funcTest.c -o test $(LFLAG)
	./test
clean:
	rm -rf test test.dSYM
