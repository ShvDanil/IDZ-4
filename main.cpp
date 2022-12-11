#include "iostream"
#include "limits"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"

// Defines section.
// Amount of philosophers.
#define PHILOSOPHER_AMOUNT 5

// Available states for philosophers.
#define EATING 0
#define HUNGRY 1
#define THINKING 2

// Forks positions.
#define LEFT ((id + 4) % PHILOSOPHER_AMOUNT)
#define RIGHT ((id + 1) % PHILOSOPHER_AMOUNT)

// Global scope variables.
int state[PHILOSOPHER_AMOUNT];
int philosophers[PHILOSOPHER_AMOUNT] = {0, 1, 2, 3, 4 };

// Mutex & philosophers semaphores array.
sem_t mutex;
sem_t semaphores[PHILOSOPHER_AMOUNT];

// Amount of food iterations for philosophers.
int eatsIterations;

// Function which handles the raised by philosopher process.
void test(int id) {
    if (state[id] == HUNGRY
        && state[LEFT] != EATING
        && state[RIGHT] != EATING) {

        // State describing that philosopher is eating.
        state[id] = EATING;

        sleep(2);

        printf("Philosopher %d takes fork %d and %d!\n", id + 1, LEFT + 1, id + 1);
        printf("Philosopher %d is eating!\n", id + 1);

        // Sem_post(&semaphores[id]) has no effect during the taking of fork process.
        // It is used to wake up hungry philosophers during the putting the fork down process.
        sem_post(&semaphores[id]);
    }
}

// Function which handles the process of taking fork by philosopher.
void take_fork(int id) {
    sem_wait(&mutex);

    // State describing that philosopher is hungry.
    state[id] = HUNGRY;

    printf("Philosopher %d is hungry!\n", id + 1);

    // The process of eat if the neighbours of current philosopher are not eating.
    test(id);

    sem_post(&mutex);

    // If current philosopher is unable to eat, he waits to be informed.
    sem_wait(&semaphores[id]);

    sleep(2);
}

// Function which handles the process of putting fork down by philosopher.
void put_fork(int id) {
    sem_wait(&mutex);

    // State describing that philosopher is thinking.
    state[id] = THINKING;

    printf("Philosopher %d putting fork %d and %d down!\n", id + 1, LEFT + 1, id + 1);
    printf("Philosopher %d is thinking!\n", id + 1);

    test(LEFT);
    test(RIGHT);

    sem_post(&mutex);
}

// Function which executes the process of spaghetti eating by philosophers.
[[noreturn]] void* philosopher(void* num) {
    /*
    int curr_eat_iter = 0;
    while (curr_eat_iter < eatsIterations) {
        ++curr_eat_iter;

        int* i = static_cast<int*>(num);
        sleep(3);

        take_fork(*i);
        sleep(2);
        put_fork(*i);
    }
    */

    while (true) {
        int* i = static_cast<int*>(num);
        sleep(3);

        take_fork(*i);
        sleep(2);
        put_fork(*i);
    }

//    return nullptr;
}

// The code below is commented because there was done the restriction for eat iterations of philosophers,
// but afterwards in the chat was written better to leave the loop endless.

// Function which assigns the amount of eat iterations for philosophers.
/*
void getEatIterations() {
    do {
        // We ask the user how many times he wants the philosophers to eat spaghetti and make a check for
        // the interval from 1 to 100 including the borders.
        printf("Enter amount of eat iterations for philosophers [1; 100]: ");
        std::cin >> eatsIterations;

        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (eatsIterations <= 0 || eatsIterations > 100) {
            printf("Incorrect input! Try again!\n");
        } else {
            printf("\n");
            break;
        }

    } while (true);
}
*/

// The entry point of program.
int main() {
//    getEatIterations();

    int i;
    pthread_t thread_id[PHILOSOPHER_AMOUNT];

    // Initialization of semaphores.
    sem_init(&mutex, 0, 1);

    for (i = 0; i < PHILOSOPHER_AMOUNT; i++) {
        sem_init(&semaphores[i], 0, 0);
    }

    for (i = 0; i < PHILOSOPHER_AMOUNT; i++) {
        // Create philosopher processes.
        pthread_create(&thread_id[i], NULL, philosopher, &philosophers[i]);

        printf("Philosopher %d is thinking!\n", i + 1);
    }

    // Join the threads.
    for (i = 0; i < PHILOSOPHER_AMOUNT; i++) {
        pthread_join(thread_id[i], NULL);
    }

    // Will not be printed because the loop is endless.
//    printf("\nThis is the end of the program! There was no mutual exclusion, hooray!!!\n");
}