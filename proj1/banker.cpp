/*  Bank service:
 *  @Brief: In real life, we have all been waiting in lines. Why is waiting
 *      in line so important? I don't know. But certainly, there is good
 *      reason why we should be doing this homework.
 *
 *  @Input: A input txt that defines the behavior and event
 *  @Ouput: No output format is required.
 *  @Author: Tingwu Wang, THU, written 29/11/2015 */


#include <iostream>
#include <pthread.h>
#include <fstream>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <vector>

static const int NUM_COUNTER = 7;
static const float startTime = (float)clock()/CLOCKS_PER_SEC;

class customer;
class banker;

void* customerBehavior(void* ptr);
void* bankerBehavior(void* ptr);

class queueingLine {
    public:
        queueingLine() {
            size = 0; 
            servingTime.clear();
            sem_init(&queueWriting, 0, 0);
            sem_init(&accessQueue, 0, 1);
        }
        ~queueingLine() {
            servingTime.clear();
            sem_destroy(&queueWriting);
        }
        void serve();
        void add(customer);
    private:
        std::vector<int> servingTime;
        int size;
        sem_t queueWriting;
        sem_t accessQueue;
};

class customer{
    public:
        customer(int serveTime, int serveStartTime) {
            servingTime = serveTime;
            startTime = serveStartTime;
        }
        customer();
        bool inline isActivate() {
            return (float)clock()/CLOCKS_PER_SEC > startTime;
        }
    private:
        int servingTime;
        int startTime;
};

class banker{
    public:
        banker() { servingTime = 0; startTime = 0; }
        bool inline isActivate() {
            return (float)clock()/CLOCKS_PER_SEC >
                startTime + servingTime;
        }
    private:
        int servingTime;
        int startTime;
};

int main(int argc, char** argv){
    // in this part, read the data
    int numCtm = 10;

    pthread_t* customerThread = new pthread_t[numCtm];
    pthread_t* counterThread = new pthread_t[NUM_COUNTER];
    banker* counter = new banker[NUM_COUNTER];
    customer* buyers = new customer[numCtm]();

    for (int i_thread = 0; i_thread < numCtm; i_thread ++) {
        pthread_create(
                &(customerThread[i_thread]),
                NULL, customerBehavior, 
                (void*)(&buyers[i_thread])
                );
    }

    for (int i_thread = 0; i_thread < NUM_COUNTER; i_thread ++) {
        pthread_create(
                &(counterThread[i_thread]),
                NULL, bankerBehavior, 
                (void*)(&counter[i_thread])
                );
    }

    sem_t sem_id;
    sem_init(&sem_id, 0, 7);
    //sem_getvalue(&sem_id, &temp);
    //std::cout<<temp<<std::endl;
    return 0;
}

void* customerBehavior(void* ptr) {

};

void* bankerBehavior(void* ptr) {

};
