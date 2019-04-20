#include <stdio.h>
#include <stdlib.h>

#define maxQueueLength 128

/////////////////////// QUEUE DEFINITION AND CREATION /////////////////////////////
typedef struct queue_struct queue;
struct queue_struct
{
  char* data[maxQueueLength];
  int front;                ////front indicates the next index to be read
                              //if it is -1 this indicates the queue is empty
  int back;                 ////back indicates the next empty index to store data
  //enqueue sets back to 0 if it would be set to maxQueueLength
  //this also then checks if front = back, which means the queue is full
  //and the calling function must wait for space to becomes available.
};
queue read;
queue charReplaced;
queue uppered;
/*********************************************************************************/


////////////////////////// FUNCTION PROTOTYPES ////////////////////////////////////
int enqueue(char* string, queue* Queue); //returns 0 on success, -1 on failure
char* dequeue(queue* Queue); //returns NULL on failure
void printQueue(queue* pq);
void initializeQueues();
void reader(char* fileName);
void character(char newChar);
void toUpper();
void writer();
/*********************************************************************************/


//////////////////////////////// MAIN ////////////////////////////////////////////
int main(int argc, char** argv){

  initializeQueues();
  char* test = "Hello World";

  enqueue(test, &read);
  enqueue(test, &charReplaced);
  enqueue(test, &uppered);

  printQueue(&read);
  printQueue(&charReplaced);
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
      printf("At index %d: %s\n", temp, dequeue(pq));
      temp++;
    }
  }
  return;
}
/*********************************************************************************/

///////////////////////// ADD DATA TO THE FRONT OF A QUEUE ///////////////////////
int enqueue(char* string, queue* Queue){

  if(Queue->back == maxQueueLength){            //provide queue wrapping
    if(Queue->data[0] == NULL){
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
    printf("No data available, Queue empty\n");
    return NULL;
  }else{
    return Queue->data[Queue->front];
  }
}
/*********************************************************************************/


///////////////////////// READ FROM FILE TO FIRST QUEUE //////////////////////////
void reader(char* fileName)
{

  return;
}
/*********************************************************************************/


////////////////// REPLACE EACH SPACE WITH SPECIFIED CHAR ////////////////////////
void character(char newChar)
{

  return;
}
/*********************************************************************************/


////////////////////// MAKE EACH CHARACTER UPPERCASE /////////////////////////////
void toUpper()
{

  return;
}
/*********************************************************************************/


////////////////// WRITE FINISHED LINES TO OUTPUT FILE ///////////////////////////
void writer()
{

  return;
}
/*********************************************************************************/
