///////////////////////////////////////////////////////////////////////////
//
// file name: main.c
// language: C
//
// author: Shion Mizuguchi <shion.mizuguchi@cooper.edu>
// org: The Cooper Union
// date: 2022-08-03
//
///////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

// global variables
int numAgents = 10; 
int numTickets = 150;

/* defines the pthread_attr structure, consisting of the thread number,
a pointer to numTickets, and a pointer to the semaphore. Allows 
multiple attributes to be passed into each thread. */
struct pthread_attr{
    int agentID; // thread number
    int *numTicketsP; // pointer to numTickets
    sem_t *lockP; // pointer to lock
};

/* function is the start routine of the thread, in which each thread
gets its attributes passed in by the *data parameter. */
void *sellTickets(void *data){
    int agentID = ((struct pthread_attr*)data)->agentID;
    int *numTicketsP = ((struct pthread_attr*)data)->numTicketsP;
    sem_t *lockP = ((struct pthread_attr*)data)->lockP;

    // tracks number of tickets sold by each thread
    int numTicketsSold = 0; 

	while(*numTicketsP > 0){
        sem_wait(lockP);
        // critical region start
        if(*numTicketsP == 0) { break; }; 
        (*numTicketsP)--;
        printf("Agent # %d sold a ticket: %d tickets left\n", agentID, *numTicketsP);
        numTicketsSold++;   
        // critical region end
        sem_post(lockP);
        
        // randomly wait for 1 to 3 seconds so that the OS passes
        // control to another thread 
        sleep(rand()%3+1); 
    }
    sem_post(lockP);
    printf("-----Agent # %d sold a total of %d tickets\n", agentID, numTicketsSold);
    return NULL;
}

int main(){
     // makes randomnization different every time the program is run
    srand(time(NULL)); 

    sem_t lock;
    sem_init(&lock, 0, 1); // initializes semaphore to 1
    
    // initialize thread attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // create pthread and struct (pthread_attr) arrays
    pthread_t t[numAgents];
    struct pthread_attr t_attr[numAgents];

    // loop to create threads and assign attributes to pthread_attr array
    for(int agent = 0; agent <= numAgents-1; agent++){
        t_attr[agent].agentID = agent;
        t_attr[agent].numTicketsP = &numTickets;
        t_attr[agent].lockP = &lock;

        pthread_create(&t[agent], &attr, sellTickets, &t_attr[agent]);
    }
    // loop to ensure the code doesn't exit until all threads are complete
    for(int agent = 0; agent <= numAgents-1; agent++){
        pthread_join(t[agent], NULL);
    }
  
    sem_destroy(&lock);
    return 0; 
}
