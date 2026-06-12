#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFER_SIZE 4096
#define NUM_THREADS 2
#define BYTE_RANGE 256



typedef struct {
    int* count;
    unsigned char* buffer;
    int* bytes_in_buffer;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond_full;
    pthread_cond_t* cond_empty;
    int* done_reading;
    sem_t *empty;
    sem_t *full;
    char* filename;
} ThreadArgs;
/*

thread_A starts with reading the file, checking the buffersize, then writing to the struct.
after writing to the struct it signals the b thread to do it's thing.

*/
void* thread_A(void* arg) {

   ThreadArgs* data = (ThreadArgs*)arg;

   printf("Thread A:opening file...\n");
   FILE* fp = fopen(data->filename, "rb");
   if (!fp) {
      perror("Failed to open file");
      exit(EXIT_FAILURE);
   }
    printf("Thread A: file open, starting read loop\n");
    
   while (1) {
      printf("Thread A: locking mutex\n");
      //pthread_mutex_lock(data->mutex);
      sem_wait(data->empty);
      printf("Thread A; checking buffer fullness\n");
      while(*data->bytes_in_buffer == BUFFER_SIZE){
         printf("Thread A: buffer full, signaling Thread B, waiting \n");
         pthread_cond_wait(data->cond_empty, data->mutex);
      }
      printf("Thread A:passed buffer check \n");
      printf("Trying to read %d bytes from file...\n", BUFFER_SIZE - *data->bytes_in_buffer);
      int read_bytes = fread(data->buffer + *data->bytes_in_buffer, 1, BUFFER_SIZE - *data->bytes_in_buffer, fp);
      if(ferror(fp)) perror("fread failed");
      printf("Thread A: read %d bytes\n", read_bytes);
      *data->bytes_in_buffer += read_bytes;

      if (read_bytes < BUFFER_SIZE - *data->bytes_in_buffer) {
         /*pthread_mutex_unlock(data->mutex);*/
         /*break;*/
         *data->done_reading = 1;
      }
      //pthread_mutex_unlock(data->mutex);
      //pthread_cond_signal(data->cond_full);
      sem_post(data->full);
      
      if(read_bytes == 0){
        break;
      }
   }
   fclose(fp);
   pthread_exit(NULL);
}

void* thread_B(void* arg) {
    
   ThreadArgs* data = (ThreadArgs*)arg;

   memset(data->count, 0, BYTE_RANGE * sizeof(int));
   printf("Thread B: starting, locking mutex\n");
   while (1) {
      //pthread_mutex_lock(data->mutex);
      sem_wait(data->full);
      printf("Thread B: got mutex \n");
     /* while (*(data->bytes_in_buffer) == 0){
        printf("Thread B: buffer empty waiting \n");
        pthread_cond_wait(data->cond_full, data->mutex);
        printf("Thread B: woke up fra wait \n");
      } */
      if(*(data->done_reading) && *(data->bytes_in_buffer) == 0){
       // pthread_mutex_unlock(data->mutex);
       sem_post(data->empty);
        printf("Thread B: Done reading, exiting loop \n");
        break;
      }
      /*
      while(*(data->bytes_in_buffer) == 0){
        if(*(data->done_reading)){
            pthread_mutex_unlock(data->mutex);
            printf("Thread B done reading, exiting loop\n");
            pthread_exit(NULL);
        }
        printf("Thread B: buffer empty waiting \n");
        pthread_cond_wait(data->cond_full, data->mutex);
        printf("Thread B woke up from wait\n");
      }
        */
      for (int i = 0; i < *(data->bytes_in_buffer); i++)
         data->count[data->buffer[i]]++;

      *(data->bytes_in_buffer) = 0;
     // pthread_cond_signal(data->cond_empty);
     // pthread_mutex_unlock(data->mutex);
        sem_post(data->empty);
     /*if (*(data->done_reading) && *(data->bytes_in_buffer) == 0)
         break; */
   }
   printf("Thread B: Printing results:\n");
   for (int i = 0; i < BYTE_RANGE; i++)
      printf("%d: %d\n", i, data->count[i]);
   pthread_exit(NULL);
}

/*
In main we first initialize the variables from the struct ThreadArgs and then initialize a struct
and then initialize the struct with the variables mentioned before. the struct is then sent to each of the threads. The idea is to have the struct to act as an intermediate which both threads can access, update and read from.
*/

int main(int argc, char *argv[]) {

    if(argc < 2){
        fprintf(stderr,"Use %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
 
    int count[BYTE_RANGE];
    unsigned char buffer[BUFFER_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t cond_full, cond_empty;
    int bytes_in_buffer = 0;
    int done_reading = 0;
    sem_t empty;
    sem_t full;
    
    sem_init(&empty, 0, 1);
    sem_init(&full, 0, 0);

    pthread_t threadA, threadB;
    ThreadArgs args;
    args.count = count;
    args.buffer = buffer;
    args.bytes_in_buffer = &bytes_in_buffer;
    args.mutex = &mutex;
    args.cond_full = &cond_full;
    args.cond_empty = &cond_empty;
    args.done_reading = &done_reading;
    args.empty = &empty;
    args.full = &full;
    args.filename = argv[1];

  // void* memory_buffer = malloc(BUFFER_SIZE);
   
   if (pthread_create(&threadA, NULL, thread_A, (void*)&args) != 0) {
      perror("Could not create thread A");
      exit(1);
   }

   if (pthread_create(&threadB, NULL, thread_B, (void*)&args) != 0) {
      perror("Could not create thread B");
      exit(1);
   }
    
   if (pthread_join(threadA, NULL) != 0) {
      perror("Could not join thread A");
      exit(1);
   }
   if (pthread_join(threadB, NULL) != 0) {
      perror("Could not join thread B");
      exit(1);
   }

   //free(memory_buffer);
   sem_destroy(&empty);
   sem_destroy(&full);

   return 0;
}










