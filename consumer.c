#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  int pid;

  char *producer = "./producer";
  char *args[4];

  args[1] = malloc(sizeof(char) * (strlen(argv[1]) + 1));
  args[2] = malloc(sizeof(char) * (strlen(argv[2]) + 1));

  if(argc != 3)
  {
    perror("Error: Wrong # of cmd line args");
  }

  args[0] = "./producer";
  strcpy(args[1], argv[1]);
  strcpy(args[2], argv[2]);
  args[3] = NULL;

  if( (pid = fork()) < 0)
  {
    perror("Forking error\n");
  }
  else if(pid == 0)
  { //CHILD PROCESS

    execvp(producer, args);

  }
  else
  { //PARENT PROCESS
    wait(NULL);

    char *cat = "cat";
    char *catArgs[3];
    catArgs[0] = "cat";
    catArgs[1] = "output.txt";
    catArgs[2] = NULL;

    execvp(cat, catArgs);

    free(args[1]);
    free(args[2]);
  }
}
