# threading

To run the program simply download producer.c consumer.c and Makefile to the same folder, cd to that folder, type Make then ./consumer (filename) (character) where the command line arguments are the file you want to use and the character you want to replace the spaces with. Your file is assumed to be in the same folder as the .c files.

If you wish, there is a testFile.txt available for download for testing. The test file contains 11 lines to show functionality with a potentially overflowing queue.

The producer creates a file "output.txt" which can be compared to the consumer's output to the terminal.
