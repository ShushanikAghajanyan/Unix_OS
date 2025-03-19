#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>

#define MESSAGE_SIZE 100

typedef struct {
    char message[MESSAGE_SIZE];
    sem_t sem_parent;  
    sem_t sem_child;   
} SharedMemory;

SharedMemory* create_shared_memory() {
    SharedMemory *sharedmem = mmap(NULL, MESSAGE_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sharedmem == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    sem_init(&sharedmem->sem_parent, 1, 1); 
    sem_init(&sharedmem->sem_child, 1, 0);   

    return sharedmem;
}

void destroy_shared_memory(SharedMemory *sharedmem) {
    sem_destroy(&sharedmem->sem_parent);
    sem_destroy(&sharedmem->sem_child);
    munmap(sharedmem, MESSAGE_SIZE);
}

void child_process(SharedMemory *sharedmem) {
    while (1) {
        sem_wait(&sharedmem->sem_child);  
        if (strcmp(sharedmem->message, "exit") == 0) break;

        printf("Child received: %s\n", sharedmem->message);


        printf("Child: ");
        fgets(sharedmem->message, MESSAGE_SIZE, stdin);
        sharedmem->message[strcspn(sharedmem->message, "\n")] = 0;

        sem_post(&sharedmem->sem_parent); 
    }
}

void parent_process(SharedMemory *sharedmem) {
    while (1) {
        sem_wait(&sharedmem->sem_parent); 

    
        printf("Parent: ");
        fgets(sharedmem->message, MESSAGE_SIZE, stdin);
        sharedmem->message[strcspn(sharedmem->message, "\n")] = 0;

        sem_post(&sharedmem->sem_child);  
        if (strcmp(sharedmem->message, "exit") == 0) break;

   
        sem_wait(&sharedmem->sem_parent);
        printf("Parent received: %s\n", sharedmem->message);
    }

    sem_post(&sharedmem->sem_child);  
    wait(NULL);  
    destroy_shared_memory(sharedmem);
}

int main() {
    SharedMemory *sharedmem = create_shared_memory();

    if (fork() == 0) { 
        child_process(sharedmem); 
    } else { 
        parent_process(sharedmem);
    }

    return 0;
}
