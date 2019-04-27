#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>


#define maxQueueLength 10
#define bufferSize 512
#define NUM_THREADS 4

char newChar;
char fileName[32];
bool readIsDone = 0;
bool charIsDone = 0;
bool upperIsDone = 0;

pthread_mutex_t readMutex;
pthread_mutex_t charReplacedMutex;
pthread_mutex_t upperedMutex;

/////////////////////// QUEUE DEFINITION AND CREATION /////////////////////////////
typedef struct queue_struct queue;
struct queue_struct
{
  char* data[maxQueueLength];
  int front;                ////front indicates the next index to be read
                              //if it is -1 this indicates the queue is empty

  int back;                 ////back indicates the next empty index to store data
  //enqueue sets back to 0 if it would be set to maxQueueLength
  //and also checks if front = back, which means the queue is full
  //and the calling function must wait for space to becomes available.


};
queue read;
queue charReplaced;
queue uppered;
/*********************************************************************************/


////////////////////////// FUNCTION PROTOTYPES ////////////////////////////////////
void initializeQueues();
void printQueue(queue* pq);
int enqueue(char* string, queue* Queue); //returns 0 on success, -1 on failure
char* dequeue(queue* Queue); //returns NULL on failure

void* reader();
void* character();
void* toUpper();
void* writer();
/*********************************************************************************/


//////////////////////////////// MAIN ////////////////////////////////////////////
int main(int argc, char** argv)
{
  pthread_t producerThreads[NUM_THREADS];


  strcpy(fileName, argv[1]);
  newChar = argv[2][0];
  printf("File: %s\tChar: %c\n", fileName, newChar);

  initializeQueues();

  // printf("Running reader...\nread queue:\n");
  // reader("testFile.txt");
  // printQueue(&read);
  // printf("\n");
  //
  // printf("Running character...\ncharReplaced queue:\n");
  // character('_');
  // printQueue(&charReplaced);
  // printf("\n");
  //
  // printf("Running toUpper...\nuppered queue:\n");
  // toUpper();
  // printQueue(&uppered);
  // printf("\n");
  //
  // printf("Running toUpper...\nqueue final stats:\n");
  // writer();
  // printf("read:\n");
  // printQueue(&read);
  // printf("charReplaced:\n");
  // printQueue(&charReplaced);
  // printf("uppered:\n");
  // printQueue(&uppered);

  pthread_create(&producerThreads[0], NULL, reader, NULL);
  pthread_create(&producerThreads[1], NULL, character, NULL);
  pthread_create(&producerThreads[2], NULL, toUpper, NULL);
  pthread_create(&producerThreads[3], NULL, writer, NULL);

  int i;
  for(i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(producerThreads[i], NULL);
  }

  printf("Threads Done!\n");

  return 0;
}
/*********************************************************************************/


//////////////////////////// INITIALIZE QUEUES ///////////////////////////////////
void initializeQueues()
{
  read.front = -1;
  read.back  =  0;

  charReplaced.front = -1;
  charReplaced.back  =  0;

  uppered.front = -1;
  uppered.back  =  0;
}
/*********************************************************************************/


//////////////////// PRINT OUT A QUEUE FOR TESTING PURPOSES //////////////////////
void printQueue(queue* pq){
  int temp = pq->front;
  if(temp == -1){
    printf("Queue is empty\n");
    return;
  }else if(temp == pq->back){
    printf("Queue has been emptied\n");
  }else{
    while(temp != pq->back){
      printf("At index %d: %s", temp, pq->data[temp]);
      temp++;
    }
  }
  return;
}
/*********************************************************************************/


///////////////////////// ADD DATA TO THE FRONT OF A QUEUE ///////////////////////
int enqueue(char* string, queue* Queue){

  if(Queue->back == maxQueueLength){            //provide queue wrapping
    if(Queue->front == 0){
      printf("Unable to add data, Queue full at end\n");
      return -1;
    }else{

      Queue->back = 0;
    }
  }
  if(Queue->front - 1 == Queue->back){             //check for full queue
    printf("Unable to add data, Queue full\n");
    return -1;
  }else{
    if(Queue->front == -1){
      Queue->front = 0;
    }

    Queue->data[Queue->back] = string;
    Queue->back++;
  }
  return 0;                                //returns 0 on success -1 on failure
}
/*********************************************************************************/


/////////////////////// RETURN THE FIRST ITEM FROM A QUEUE ///////////////////////
char* dequeue(queue* Queue){
  if(Queue->front == -1 || Queue->front == Queue->back){
    printf("No data available, Queue empty\n");
    return NULL;
  }else{
    if(Queue->front == maxQueueLength - 1){
      Queue->front = 0;
      return Queue->data[maxQueueLength - 1];
    }else{
      Queue->front++;
      return Queue->data[Queue->front - 1];
    }
  }
}
/*********************************************************************************/


///////////////////////// READ FROM FILE TO FIRST QUEUE //////////////////////////
void* reader()
{
  FILE* fp = fopen(fileName, "r");
  char* temp;
  char buff[bufferSize];

  while(fgets(buff, bufferSize, fp) != NULL)
  {
    temp = malloc(sizeof(char) * (strlen(buff) + 1));
    strcpy(temp, buff);

    pthread_mutex_lock(&readMutex);
    printf("reader in critial area\n");
    while(enqueue(temp, &read) == -1){
      pthread_mutex_unlock(&readMutex);
      pthread_mutex_lock(&readMutex);
      printf("reader back in critial area\n");
    };
    //printQueue(&read);
    pthread_mutex_unlock(&readMutex);
  }

  readIsDone = 1;
  fclose(fp);
  return NULL;
}
/*********************************************************************************/


////////////////// REPLACE EACH SPACE WITH SPECIFIED CHAR ////////////////////////
void* character()
{
  char* temp;
  do
  {
    while(1)
    {
      pthread_mutex_lock(&readMutex);
      printf("character in critial area for read\n");
      if(!(temp = dequeue(&read)) )
      {
        pthread_mutex_unlock(&readMutex);
        break;
      }
      pthread_mutex_unlock(&readMutex);

      int i;
      for(i = 0; i < strlen(temp) - 1; i++)
      {
        if(isspace(temp[i]))
        {
          temp[i] = newChar;
        }
      }
      pthread_mutex_lock(&charReplacedMutex);
      printf("character in critial area for charReplaced\n");
      while(enqueue(temp, &charReplaced) == -1){
          pthread_mutex_unlock(&charReplacedMutex);
          pthread_mutex_lock(&charReplacedMutex);
      }
      pthread_mutex_unlock(&charReplacedMutex);
    }
  } while(!readIsDone);
  //printQueue(&charReplaced);
  charIsDone = 1;
  return NULL;
}
/*********************************************************************************/


////////////////////// MAKE EACH CHARACTER UPPERCASE /////////////////////////////
void* toUpper()
{
  char* temp;
  do
  {
    while(1)
    {
      pthread_mutex_lock(&charReplacedMutex);
      printf("toUpper in critical area for charReplaced\n");
      if(!(temp = dequeue(&charReplaced)) )
      {
        pthread_mutex_unlock(&charReplacedMutex);
        break;
      }
      pthread_mutex_unlock(&charReplacedMutex);

      int i;
      for(i = 0; i < strlen(temp) - 1; i++)
      {
          temp[i] = toupper(temp[i]);
      }

      pthread_mutex_lock(&upperedMutex);
      printf("toUpper in critical area for uppered\n");
      while(enqueue(temp, &uppered) == -1){
        pthread_mutex_unlock(&upperedMutex);
        pthread_mutex_lock(&upperedMutex);
      }
      pthread_mutex_unlock(&upperedMutex);
    }
  } while(!charIsDone);
  //printQueue(&uppered);
  upperIsDone = 1;
  return NULL;
}
/*********************************************************************************/


////////////////// WRITE FINISHED LINES TO OUTPUT FILE ///////////////////////////
void* writer()
{
  char* temp;
  FILE* fp = fopen("output.txt", "w");

  do
  {
    while(1)
    {
      pthread_mutex_lock(&upperedMutex);
      printf("writer in critical area for uppered\n");
      if(!(temp = dequeue(&uppered)) )
      {
        pthread_mutex_unlock(&upperedMutex);
        break;
      }
      pthread_mutex_unlock(&upperedMutex);

      fprintf(fp, "%s", temp);
      printf("freeing\n");

    }
    free(temp);
  } while(!upperIsDone);
  fclose(fp);
  printf("writer done\n");
  return NULL;
}
/*********************************************************************************/
