/**
 * @file proj2.c
 * @author Kristian Dobes (xdobes22)
 * @date 2023-04-30
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

// SEMAPHORES
#define SEM_MUTEX           "/xdobes22-ios2-mutex"
#define SEM_BMUTEX           "/xdobes22-ios2-bmutex"
#define SEM_CUSTOMER        "/xdobes22-ios2-customer"
#define SEM_WORKER          "/xdobes22-ios2-worker"
#define SEM_CUSTOMERDONE    "/xdobes22-ios2-customerdone"
#define SEM_WORKERDONE      "/xdobes22-ios2-workerdone"
#define SEM_PRINT           "/xdobes22-ios2-print"
#define SEM_CUSTOMERPROCESSFINISHED           "/xdobes22-ios2-customerprocessfinished"
#define SEM_BARRIER         "/xdobes22-ios2-barrier"
#define SEM_QUEUE1          "/xdobes22-ios2-queue1"
#define SEM_QUEUE2          "/xdobes22-ios2-queue2"
#define SEM_QUEUE3          "/xdobes22-ios2-queue3"

sem_t *mutex = NULL;
sem_t *bmutex = NULL;
sem_t *customer = NULL;
sem_t *worker = NULL;
sem_t *customerdone = NULL;
sem_t *workerdone = NULL;
sem_t *print = NULL;
sem_t *customerprocessfinished = NULL;
sem_t *barrier = NULL;
sem_t *queue1 = NULL;
sem_t *queue2 = NULL;
sem_t *queue3 = NULL;

// SHARED MEMORY
static int *printCounter;
static int *numOfCustomersInShop;
static bool *isClosed;
static int *customersForService1;
static int *customersForService2;
static int *customersForService3;
static int *finalNumberOfThreads;
static int *currentNumberOfThreads;

// FILE
FILE *file;

void cleanup(){
    // SEMAPHORE UNLINK
    sem_unlink(SEM_MUTEX);
    sem_close(mutex);
    
    sem_unlink(SEM_BMUTEX);
    sem_close(bmutex);

    sem_unlink(SEM_CUSTOMER);
    sem_close(customer);

    sem_unlink(SEM_WORKER);
    sem_close(worker);

    sem_unlink(SEM_CUSTOMERDONE);
    sem_close(customerdone);

    sem_unlink(SEM_WORKERDONE);
    sem_close(workerdone);

    sem_unlink(SEM_PRINT);
    sem_close(print);

    sem_unlink(SEM_CUSTOMERPROCESSFINISHED);
    sem_close(customerprocessfinished);

    sem_unlink(SEM_BARRIER);
    sem_close(barrier);

    sem_unlink(SEM_QUEUE1);
    sem_close(queue1);

    sem_unlink(SEM_QUEUE2);
    sem_close(queue2);

    sem_unlink(SEM_QUEUE3);
    sem_close(queue3);


    // SHARED MEMORY UNMAP
    munmap(printCounter, sizeof *printCounter);
    munmap(numOfCustomersInShop, sizeof *numOfCustomersInShop);
    munmap(isClosed, sizeof *isClosed);
    munmap(isClosed, sizeof *customersForService1);
    munmap(isClosed, sizeof *customersForService2);
    munmap(isClosed, sizeof *customersForService3);
    munmap(isClosed, sizeof *finalNumberOfThreads);
    munmap(isClosed, sizeof *currentNumberOfThreads);
}

void init(){
    // SEMAPHORE INIT
    mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [mutex] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    
    bmutex = sem_open(SEM_BMUTEX, O_CREAT | O_EXCL, 0666, 1);
    if (bmutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [bmutex] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    customer = sem_open(SEM_CUSTOMER, O_CREAT | O_EXCL, 0666, 0);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [customer] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    worker = sem_open(SEM_WORKER, O_CREAT | O_EXCL, 0666, 0);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [worker] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    customerdone = sem_open(SEM_CUSTOMERDONE, O_CREAT | O_EXCL, 0666, 0);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [customerdone] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    workerdone = sem_open(SEM_WORKERDONE, O_CREAT | O_EXCL, 0666, 0);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [workerdone] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    print = sem_open(SEM_PRINT, O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [print] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    customerprocessfinished = sem_open(SEM_CUSTOMERPROCESSFINISHED, O_CREAT | O_EXCL, 0666, 0);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [customerprocessfinished] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    barrier = sem_open(SEM_BARRIER, O_CREAT | O_EXCL, 0666, 0);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [barrier] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    queue1 = sem_open(SEM_QUEUE1, O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [queue1] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    queue2 = sem_open(SEM_QUEUE2, O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [queue2] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }

    queue3 = sem_open(SEM_QUEUE3, O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED) {
        fprintf(stderr, "ERROR > Semaphore [queue3] not initialized.\n");
        cleanup();
        fclose(file);
        exit(1);
    }


    // SHARED MEMORY INIT
    printCounter = mmap(NULL, sizeof *printCounter, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (printCounter == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [printCounter] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *printCounter = 1;  

    numOfCustomersInShop = mmap(NULL, sizeof *numOfCustomersInShop, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (numOfCustomersInShop == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [numOfCustomersInShop] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *numOfCustomersInShop = 0; 

    isClosed = mmap(NULL, sizeof *isClosed, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (isClosed == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [isClosed] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *isClosed = false; 

    customersForService1 = mmap(NULL, sizeof *customersForService1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (customersForService1 == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [customersForService1] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *customersForService1 = 0;

    customersForService2 = mmap(NULL, sizeof *customersForService2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (customersForService2 == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [customersForService2] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *customersForService2 = 0;

    customersForService3 = mmap(NULL, sizeof *customersForService3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (customersForService3 == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [customersForService3] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *customersForService3 = 0;

    finalNumberOfThreads = mmap(NULL, sizeof *finalNumberOfThreads, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (finalNumberOfThreads == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [finalNumberOfThreads] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *finalNumberOfThreads = 0;

    currentNumberOfThreads = mmap(NULL, sizeof *currentNumberOfThreads, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (currentNumberOfThreads == MAP_FAILED) {
        fprintf(stderr, "ERROR > Allocation of shared memory [currentNumberOfThreads] failed.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    *currentNumberOfThreads = 0;

}

void barrierFunc(){
    sem_wait(bmutex);
    (*currentNumberOfThreads)++;
    sem_post(bmutex);

    if( *currentNumberOfThreads == *finalNumberOfThreads ){
        sem_post(barrier);
    }

    sem_wait(barrier);
    sem_post(barrier);
}

void processCustomer(int NZ, int idZ, int TZ) {
    (void) NZ;
    
    barrierFunc();

    sem_wait(print);
    fprintf(file, "%d: Z %d: started\n", *printCounter, idZ);
    fflush(file);
    (*printCounter)++;
    sem_post(print);

    int sleeptime;
    if( TZ != 0){
        srand(time(NULL) * getpid());
        sleeptime = (rand() % TZ) * 1000;
        usleep(sleeptime);
    }

    sem_wait(mutex);
    if(*isClosed == true){
        // CUSTOMER IS LEAVING
        sem_wait(print);
        fprintf(file, "%d: Z %d: going home\n", *printCounter, idZ);
        fflush(file);
        (*printCounter)++;
        sem_post(print);

        sem_post(mutex);
        exit(0);
    }
    sem_post(mutex);

    sem_wait(mutex);
    (*numOfCustomersInShop)++;
    sem_post(mutex);

    int currentCustomerService;

    // SELECT RANDOM QUEUE
    srand(time(NULL) * getpid());
    currentCustomerService = 1 + (rand() % 3);

    sem_wait(mutex);
    if( currentCustomerService == 1 ){
        // service1++;
        (*customersForService1)++;
    }else if( currentCustomerService == 2 ){
        // service2++;
        (*customersForService2)++;
    }else if( currentCustomerService == 3 ){
        // service3++;
        (*customersForService3)++;
    }

    // ENTERING OFFICE FOR A SERVICE
    sem_wait(print);
    fprintf(file, "%d: Z %d: entering office for a service %d\n", *printCounter, idZ, currentCustomerService);
    fflush(file);
    (*printCounter)++;
    sem_post(print);
    
    sem_post(mutex);

    sem_post(customer);

    sem_wait(worker);

    // ZAKAZNIK JE OBSLUHOVAN
    sem_wait(print);
    fprintf(file, "%d: Z %d: called by office worker\n", *printCounter, idZ);
    fflush(file);
    (*printCounter)++;
    sem_post(print);

    srand(time(NULL) * getpid());
    sleeptime = (rand() % 10) * 1000;
    usleep(sleeptime);

    sem_post(customerdone);

    sem_wait(workerdone); 

    sem_wait(print);
    fprintf(file, "%d: Z %d: going home\n", *printCounter, idZ);
    fflush(file);
    (*printCounter)++;
    sem_post(print);

    //SEM_POST CUSTOMER PROCESS FINISHED
    sem_post(customerprocessfinished);
}

void processWorker(int NU, int idU, int TU) {
    (void) NU;

    barrierFunc();

    sem_wait(print);
    fprintf(file, "%d: U %d: started\n", *printCounter, idU);
    fflush(file);
    (*printCounter)++;
    sem_post(print);

    int serviceToServe;
    int sleeptime;

    for(;;){

        sem_wait(mutex);

        if( *numOfCustomersInShop <= 0 && *isClosed == true ){
            if( *numOfCustomersInShop < 0 ){
                *numOfCustomersInShop = 0;
            }

            sem_wait(print);
            fprintf(file, "%d: U %d: going home\n", *printCounter, idU);
            fflush(file);
            (*printCounter)++;
            sem_post(print);

            sem_post(mutex);
            exit(0);
        }

        sem_wait(print);    // NO MORE TAKING BREAK AFTER CLOSING
        if( *numOfCustomersInShop <= 0 && *isClosed == false ){
            if( *numOfCustomersInShop < 0 ){
                *numOfCustomersInShop = 0;
            }
            sem_post(mutex);
            sem_post(print);

            sem_wait(print);
            fprintf(file, "%d: U %d: taking break\n", *printCounter, idU);
            fflush(file);
            (*printCounter)++;
            sem_post(print);

            if( TU != 0 ){
                srand(time(NULL) * getpid());
                sleeptime = (rand() % TU) * 1000;
                usleep(sleeptime);
            }

            sem_wait(print);
            fprintf(file, "%d: U %d: break finished\n", *printCounter, idU);
            fflush(file);
            (*printCounter)++;
            sem_post(print);

            continue;
        }
        sem_post(print);

        sem_post(mutex);

        sem_wait(mutex);
        if( *customersForService1 == 0 && *customersForService2 == 0 && *customersForService3 == 0){
            
            sem_post(mutex);
            continue;
        }
        
        (*numOfCustomersInShop)--;
        sem_post(mutex);

        // SELECT RANDOM QUEUE
        srand(time(NULL) * getpid());
        serviceToServe = 1 + (rand() % 3);

        if( serviceToServe==1 && *customersForService1 != 0 ){  // SERVE QUEUE 1
            
            (*customersForService1)--;
            sem_wait(queue1);

        }else if( serviceToServe==2 && *customersForService2 != 0 ){ // SERVE QUEUE 2
            
            (*customersForService2)--;
            sem_wait(queue2);

        }else if( serviceToServe==3 && *customersForService3 != 0 ){ // SERVE QUEUE 3
            
            (*customersForService3)--;
            sem_wait(queue3);

        }else{  // INVALID QUEUE
            if( *customersForService1 != 0 ){
                serviceToServe = 1;
                (*customersForService1)--;

                sem_wait(queue1);
            }else if( *customersForService2 != 0 ){
                serviceToServe = 2;
                (*customersForService2)--;

                sem_wait(queue2);
            }else if( *customersForService3 != 0 ){
                serviceToServe = 3;
                (*customersForService3)--;

                sem_wait(queue3);
            }else{

                if(serviceToServe == 1){
                    sem_wait(queue1);
                }else if(serviceToServe == 2){
                    sem_wait(queue2);
                } else if(serviceToServe == 3){
                    sem_wait(queue3);
                }
            }
        }
        
        sem_wait(customer);

        sem_post(worker);

        sem_wait(customerdone);

        // SERVING A SERVICE OF TYPE X
        sem_wait(print);
        fprintf(file, "%d: U %d: serving a service of type %d\n", *printCounter, idU, serviceToServe);
        fflush(file);
        (*printCounter)++;
        sem_post(print);

        srand(time(NULL) * getpid());
        sleeptime = (rand() % 10) * 1000;
        usleep(sleeptime);

        sem_post(workerdone);

        // SERVICE FINISHED
        sem_wait(print);
        fprintf(file, "%d: U %d: service finished\n", *printCounter, idU);
        fflush(file);
        (*printCounter)++;
        sem_post(print);

        sem_wait(mutex);
        if( serviceToServe == 1 ){
            sem_post(queue1);
        }else if( serviceToServe == 2 ){
            sem_post(queue2);
        }else if( serviceToServe == 3 ){
            sem_post(queue3);
        }else{
            true;
        }
        sem_post(mutex);

        //SEM_WAIT END OF CUSTOMER PROCESS FINISHED
        sem_wait(customerprocessfinished);

        // sem_wait(print);
        // fprintf(file, "%d: U %d: test print\n", *printCounter, idU);
        // fflush(file);
        // (*printCounter)++;
        // sem_post(print);
    }
}

void killAll(){
    for(int i=0; i < *finalNumberOfThreads; i++){
        sem_post(mutex);
        sem_post(customer);
        sem_post(worker);
        sem_post(customerdone);
        sem_post(workerdone);
        sem_post(print);
        sem_post(customerprocessfinished);
        sem_post(barrier);
    }

    while(wait(NULL) > -1);
    fclose(file);
    cleanup();
}

int main(int argc, char **argv){

    // PARSING ARGUMENTS
    if(argc != 6){
        fprintf(stderr, "ERROR > Invalid number of arguments.\n");
        exit(1);
    }

    int NZ, NU, TZ, TU, F;

    // CHECK ARG TYPE
    char *temp;  // temp
    NZ = strtol(argv[1], &temp, 10);
    if (*temp != '\0' || temp == argv[1]) {
        printf("ERROR > '%s' is not a number.\n", argv[1]);
        exit(1);
    }
    NU = strtol(argv[2], &temp, 10);
    if (*temp != '\0' || temp == argv[2]) {
        printf("ERROR > '%s' is not a number.\n", argv[2]);
        exit(1);
    }
    TZ = strtol(argv[3], &temp, 10);
    if (*temp != '\0' || temp == argv[3]) {
        printf("ERROR > '%s' is not a number.\n", argv[3]);
        exit(1);
    }
    TU = strtol(argv[4], &temp, 10);
    if (*temp != '\0' || temp == argv[4]) {
        printf("ERROR > '%s' is not a number.\n", argv[4]);
        exit(1);
    }
    F = strtol(argv[5], &temp, 10);
    if (*temp != '\0' || temp == argv[5]) {
        printf("ERROR > '%s' is not a number.\n", argv[5]);
        exit(1);
    }

    // CHECK ARG RANGE
    if (NZ < 0) {
        fprintf(stderr, "ERROR > Value NZ out of range\n");
        exit(1);
    } 
    if (NU <= 0) {
        fprintf(stderr, "ERROR > Value NU out of range\n");
        exit(1);
    } 
    if (TZ < 0 || TZ > 10000) {
        fprintf(stderr, "ERROR > Value TZ out of range\n");
        exit(1);
    } 
    if (TU < 0 || TU > 100) {
        fprintf(stderr, "ERROR > Value TU out of range\n");
        exit(1);
    } 
    if (F < 0 || F > 10000) {
        fprintf(stderr, "ERROR > Value F out of range\n");
        exit(1);
    } 

    // SEMAPHORE AND SHARED MEMORY INIT
    init();

    // FILE INIT
    file = fopen("proj2.out", "w");
    if (file == NULL){
        fprintf(stderr, "ERROR > Opening file.\n");
        cleanup();
        fclose(file);
        exit(1);
    }
    setbuf(file, NULL);

    // VARIABLE FOR BARRIER
    *finalNumberOfThreads = NU + NZ;

    // GENERATE UREDNIK PROCESS
    for(int idU = 1; idU <= NU; idU++){
        pid_t generateWorkers = fork();

        if( generateWorkers == 0 ){
            processWorker(NU, idU, TU);
            exit(0);
        }else if( generateWorkers < 0 ){
            fprintf(stderr, "ERROR > Fork failed %d (generateWorkers).\n", getpid());
            killAll();
            exit(1);
        }
    }

    // GENERATE ZAKAZNIK PROCESS
    for(int idZ = 1; idZ <= NZ; idZ++){
        pid_t generateCustomers = fork();

        if( generateCustomers == 0 ){
            processCustomer(NZ, idZ, TZ);
            exit(0);
        }else if( generateCustomers < 0 ){
            fprintf(stderr, "ERROR > Fork failed %d (generateCustomers).\n", getpid());
            killAll();
            exit(1);
        }
    }

    if( F != 0 ){
        int closedSleeptime;

        srand(time(NULL) * getpid());
        closedSleeptime = ( (F/2) + (rand() % (F/2)) ) * 1000;
        usleep(closedSleeptime);
    }
    
    sem_wait(mutex);
    sem_wait(print);
    fprintf(file, "%d: closing\n", *printCounter);
    fflush(file);
    *isClosed = true;
    (*printCounter)++;
    sem_post(print);
    sem_post(mutex);

    while(wait(NULL) > 0);  //if all child processes killed ----> program finishes

    // CLEAN
    cleanup();
    fclose(file);
    return 0;
}