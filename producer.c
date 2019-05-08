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
bool readIsDone = 0;  //used to tell other threads that reader is done
bool charIsDone = 0;  //used to tell other threads that character is done
bool upperIsDone = 0; //used to tell other threads that toUpper is done

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

  initializeQueues();

  pthread_create(&producerThreads[0], NULL, reader, NULL);
  pthread_create(&producerThreads[1], NULL, character, NULL);
  pthread_create(&producerThreads[2], NULL, toUpper, NULL);
  pthread_create(&producerThreads[3], NULL, writer, NULL);

  int i;
  for(i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(producerThreads[i], NULL);
  }

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
      return -1;
    }else{

      Queue->back = 0;
    }
  }
  if(Queue->front - 1 == Queue->back){             //check for full queue
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
  if(Queue->front == -1 || Queue->front == Queue->back){ //check for empty queue
    return NULL;
  }else{
    if(Queue->front == maxQueueLength - 1){ //check for queue wrapping
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
  { //read until eof 1 line at a time
    temp = malloc(sizeof(char) * (strlen(buff) + 1));
    strcpy(temp, buff);

    pthread_mutex_lock(&readMutex); //lock while utilizing queue (shared resource)
    while(enqueue(temp, &read) == -1)
    { //try to enqueue until it succedes
      pthread_mutex_unlock(&readMutex);
      //allow other thread to take mutex
      //to make space in queue
      
      pthread_mutex_lock(&readMutex);
      //regrab mutex
      
    }
    pthread_mutex_unlock(&readMutex); 
    //free mutex
  }
  
  readIsDone = TRUE; 
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
      pthread_mutex_lock(&readMutex); //lock while utilizing queue (shared resource)
      if(!(temp = dequeue(&read)) )
      { //try to read, and if empty, release mutex & retry
        pthread_mutex_unlock(&readMutex);
        break;
      }
      pthread_mutex_unlock(&readMutex);
      //free mutex

      int i;
      for(i = 0; i < strlen(temp) - 1; i++)
      { //iterates through line
        if(isspace(temp[i]))
        { //replaces spaces with specified character
          temp[i] = newChar;
        }
      }
      
      pthread_mutex_lock(&charReplacedMutex); //lock while utilizing queue (shared resource)
      while(enqueue(temp, &charReplaced) == -1)
      { //try to enqueue until it works
          pthread_mutex_unlock(&charReplacedMutex);
          //allow other thread to take mutex
          //to make space in queue
        
          pthread_mutex_lock(&charReplacedMutex);
          //regrab mutex
      }
      pthread_mutex_unlock(&charReplacedMutex);
      //free mutex
      
    }
  } while(!readIsDone); //end thread after reader thread completes
  
  charIsDone = TRUE;
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
      pthread_mutex_lock(&charReplacedMutex); //lock while utilizing queue (shared resource)
      if(!(temp = dequeue(&charReplaced)) )
      { //try to read, if empty, release mutex & try again
        pthread_mutex_unlock(&charReplacedMutex);
        break;
      }
      pthread_mutex_unlock(&charReplacedMutex);
      //free mutex

      int i;
      for(i = 0; i < strlen(temp) - 1; i++)
      { //make characters uppercase
          temp[i] = toupper(temp[i]);
      }

      pthread_mutex_lock(&upperedMutex); //lock while utilizing queue (shared resource)
      while(enqueue(temp, &uppered) == -1)
      { //try to enqueue until it works 
        pthread_mutex_unlock(&upperedMutex);
        //allow other thread to take mutex
        //to make space in queue
        
        pthread_mutex_lock(&upperedMutex);
        //regrab mutex
      }
      pthread_mutex_unlock(&upperedMutex);
      //free mutex
      
    }
  } while(!charIsDone);
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
      pthread_mutex_lock(&upperedMutex); //lock while utilizing queue (shared resource)
      if(!(temp = dequeue(&uppered)) )
      { //try to read, if empty, release mutex & try again
        pthread_mutex_unlock(&upperedMutex);
        break;
      }
      pthread_mutex_unlock(&upperedMutex);
      //free mutex

      fprintf(fp, "%s", temp);
    }
    free(temp);
    
  } while(!upperIsDone);
  fclose(fp);
  return NULL;
}
/*********************************************************************************/
