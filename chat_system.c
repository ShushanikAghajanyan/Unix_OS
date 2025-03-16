#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MESSAGE_SIZE 100

typedef struct {
    char *message;
} SharedMemory;

SharedMemory create_shared_memory() {
    SharedMemory sharedmem;
    sharedmem.message = mmap(NULL, MESSAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sharedmem.message == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    return sharedmem;
}

void destroy_shared_memory(SharedMemory *sharedmem) {
    munmap(sharedmem->message, MESSAGE_SIZE);
}

void child_process(SharedMemory *sharedmem) {
    while (1) {
        sleep(1); 
        if (strcmp(sharedmem->message, "exit") == 0) break;
        printf("Child received: %s\n", sharedmem->message);
    }
}

void parent_process(SharedMemory *sharedmem) {
    while (1) {
        printf("Parent: ");
        fgets(sharedmem->message, MESSAGE_SIZE, stdin);
        sharedmem->message[strcspn(sharedmem->message, "\n")] = 0;
        if (strcmp(sharedmem->message, "exit") == 0) break;
    }
    destroy_shared_memory(sharedmem);
}

int main() {
    SharedMemory sharedmem = create_shared_memory();

    if (fork() == 0) { 
        child_process(&sharedmem); 
    } else { 
        parent_process(&sharedmem);
    }

    return 0;
}
