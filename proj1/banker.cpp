/*  Bank service:
 *  @Brief: In real life, we have all been waiting in lines. Why is waiting
 *      in line so important? I don't know. But certainly, there is good
 *      reason why we should be doing this homework.
 *
 *  @Input: A input txt that defines the behavior and event
 *  @Ouput: No output format is required.
 *  @Author: Tingwu Wang, THU, written 29/11/2015 */


#include <iostream>
#include <fstream>
#include <stdlib.h>     /* atoi */
#include <sstream>
#include <pthread.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <semaphore.h>
#include <vector>
#include <deque>
#include <time.h>
static int NUM_COUNTER = 2;
static clock_t sysStartTime = clock();

class customer;
class banker;

class queueingLine {
    public:
        queueingLine() {
            size = 0; 
            servingTime.clear();
            sem_init(&queueNum, 0, 0);
            sem_init(&accessQueue, 0, 1);
            count = 0;
            servedCount = 0;
        }
        ~queueingLine() {
            servingTime.clear();
            sem_destroy(&queueNum);
            sem_destroy(&accessQueue);
        }
        void serve(banker* Me);
        void add(customer Me);
        int getServingTime() {
            int time = servingTime[0];
            servingTime.pop_front();
            return time;
        }
        int getServedCount() { return servedCount; }
    private:
        std::deque<int> servingTime;
        int count;
        int servedCount;
        int size;
        sem_t queueNum;
        sem_t accessQueue;
};

class customer{
    public:
        customer(int serveTime, int serveStartTime) {
            servingTime = serveTime;
            startTime = serveStartTime;
        }
        bool inline isActivate() {
            return (float)clock()/CLOCKS_PER_SEC > startTime;
        }
        void customerSleep() {  
            /* the customer arrives after sleep */
            sleep((unsigned)startTime);
        }
        static void* customerBehavior(void* ptr);
        int getServingTime() { return servingTime; }
        int getStartTime() { return startTime; }
    private:
        int servingTime;
        int startTime;
};

class banker{
    public:
        banker(int Id) { 
            servingTime = 0;
            startTime = 0; 
            counterId = Id;
            myTime = 0;
        }
        bool inline isActivate() {
            return (float)clock()/CLOCKS_PER_SEC >
                startTime + servingTime;
        }
        static void* bankerBehavior(void* ptr);
        void serve(int Who) {
            std::cout << "**" << myTime <<
                "s** The customer " << Who <<
                " is served by counter " << counterId; 
            std::cout << ". He will takes " << servingTime 
                << " seconds" << std::endl;
            myTime = myTime + servingTime;
            sleep((unsigned)servingTime);
        }
        void setServTime(int newTime) { servingTime = newTime; }
    private:
        int myTime;
        int servingTime;
        int startTime;
        int counterId;
};

void queueingLine::add(customer Me) {
    sem_wait(&accessQueue);
    count = count + 1;  //
    servingTime.push_back(Me.getServingTime());
    std::cout << "**" << Me.getStartTime() << "s** A customer step in side, at time: " <<
        Me.getStartTime();
    std::cout << "  He is the : " << count <<
        " th in line" << std::endl;
    sem_post(&queueNum);  // add one to the line
    sem_post(&accessQueue);
};

void queueingLine::serve(banker* Me) {
    sem_wait(&queueNum);
    sem_wait(&accessQueue);
    int serveTime = getServingTime();
    int serveWho = servedCount;
    servedCount = servedCount + 1;
    sem_post(&accessQueue);

    Me->setServTime(serveTime);
    Me->serve(serveWho);
}


queueingLine bankLine;  // this is the banker line
static int numCtm = 0;

int main(int argc, char** argv){
    // in this part, read the data
    std::ifstream inputFile(argv[1]);
    NUM_COUNTER = atoi(argv[2]);
    std::string line;

    std::vector<banker> counter;
    std::vector<customer> buyers;

    while(std::getline(inputFile, line)) {
        std::istringstream iss(line);
        int tempId, tempArrive, tempServe;
        if (!(iss >> tempId >> tempArrive  >> tempServe)) { break; } // error
        numCtm ++;
        customer tempCustomer((tempServe), (tempArrive));
        buyers.push_back(tempCustomer);
    }

    for (int i_banker = 0; i_banker < NUM_COUNTER; i_banker ++) {
        banker tempBanker(i_banker);
        counter.push_back(tempBanker);
    }

    std::cout << "The thread initilizing!" << std::endl;
    sysStartTime = clock();

    pthread_t* customerThread = new pthread_t[numCtm];
    pthread_t* counterThread = new pthread_t[NUM_COUNTER];

    for (int i_thread = 0; i_thread < numCtm; i_thread ++) {
        pthread_create(
                &(customerThread[i_thread]),
                NULL, customer::customerBehavior, 
                (void*)(&buyers[i_thread])
                );
    }

    for (int i_thread = 0; i_thread < NUM_COUNTER; i_thread ++) {
        pthread_create(
                &(counterThread[i_thread]),
                NULL, banker::bankerBehavior, 
                (void*)(&counter[i_thread])
                );
    }

    std::cout << "  The thread initilized!" << std::endl;
    //sem_getvalue(&sem_id, &temp);
    //std::cout<<temp<<std::endl;
    for (int i_thread = 0; i_thread < NUM_COUNTER; i_thread ++) {
        pthread_join(counterThread[i_thread], NULL);
    }
    for (int i_thread = 0; i_thread < numCtm; i_thread ++) {
        pthread_join(customerThread[i_thread], NULL);
    }
    delete [] customerThread;
    delete [] counterThread;
    return 0;
}

void* customer::customerBehavior(void* ptr) {
    customer* Me = (customer*) ptr;
    Me->customerSleep();
    bankLine.add(*Me);
    return 0;
};

void* banker::bankerBehavior(void* ptr) {
    banker* Me = (banker*) ptr;
    for(;;) {  // bankers work forever
        bankLine.serve(Me);
        if (bankLine.getServedCount() == numCtm) break;
    }
    return 0;
};
