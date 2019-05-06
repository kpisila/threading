LFLAG=-lpthread

producer: producer.c
	gcc producer.c -o producer $(LFLAG)
	gcc consumer.c -o consumer $(LFLAG)
clean:
	rm -rf producer producer.dSYM
	rm -rf consumer consumer.dSYM
