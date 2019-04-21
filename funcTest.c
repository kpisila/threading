#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define maxQueueLength 128
#define bufferSize 512

/////////////////////// QUEUE DEFINITION AND CREATION /////////////////////////////
typedef struct queue_struct queue;
struct queue_struct{

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

void reader(const char* fileName);
void character(char newChar);
void toUpper();
void writer();
/*********************************************************************************/


//////////////////////////////// MAIN ////////////////////////////////////////////
int main(int argc, char** argv){

  initializeQueues();

  printf("Running reader...\nread queue:\n");
  reader("testFile.txt");
  printQueue(&read);
  printf("\n");

  printf("Running character...\ncharReplaced queue:\n");
  character('_');
  printQueue(&charReplaced);
  printf("\n");

  printf("Running toUpper...\nuppered queue:\n");
  toUpper();
  printQueue(&uppered);
  printf("\n");

  printf("Running toUpper...\nqueue final stats:\n");
  writer();
  printf("read:\n");
  printQueue(&read);
  printf("charReplaced:\n");
  printQueue(&charReplaced);
  printf("uppered:\n");
  printQueue(&uppered);
  return 0;
}
/*********************************************************************************/


//////////////////////////// INITIALIZE QUEUES ///////////////////////////////////
void initializeQueues(){
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
      printf("Unable to add data, Queue full\n");
      return -1;
    }else{

      Queue->back = 0;
    }
  }
  if(Queue->front == Queue->back){             //check for full queue
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
    //printf("No data available, Queue empty\n");
    return NULL;
  }else{
    if(Queue->front == maxQueueLength - 1){
      Queue->front = 0;
      return Queue->data[maxQueueLength];
    }else{
      Queue->front++;
      return Queue->data[Queue->front - 1];
    }
  }
}
/*********************************************************************************/


///////////////////////// READ FROM FILE TO FIRST QUEUE //////////////////////////
void reader(const char* fileName)
{
  FILE* fp = fopen(fileName, "r");
  char* temp;
  char buff[bufferSize];
  while(fgets(buff, bufferSize, fp) != NULL){
    temp = malloc(sizeof(char) * (strlen(buff) + 1));
    strcpy(temp, buff);
    enqueue(temp, &read);
  }

  return;
}
/*********************************************************************************/


////////////////// REPLACE EACH SPACE WITH SPECIFIED CHAR ////////////////////////
void character(char newChar)
{
  char* temp;
  while(temp = dequeue(&read)){
    int i;
    for(i = 0; i < strlen(temp) - 1; i++){
      if(isspace(temp[i])){
        temp[i] = newChar;
      }
    }
    enqueue(temp, &charReplaced);
  }
  return;
}
/*********************************************************************************/


////////////////////// MAKE EACH CHARACTER UPPERCASE /////////////////////////////
void toUpper()
{
  char* temp;
  while(temp = dequeue(&charReplaced)){
    int i;
    for(i = 0; i < strlen(temp) - 1; i++){
        temp[i] = toupper(temp[i]);
    }
    enqueue(temp, &uppered);
  }
  return;
}
/*********************************************************************************/


////////////////// WRITE FINISHED LINES TO OUTPUT FILE ///////////////////////////
void writer()
{
  char* temp;
  FILE* fp = fopen("output.txt", "w");
  while(temp = dequeue(&uppered)){
    fprintf(fp, "%s", temp);
    free(temp);
  }
  return;
}
/*********************************************************************************/
