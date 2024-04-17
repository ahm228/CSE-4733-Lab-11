#include <iostream>
#include <thread>
#include <queue>
#include <random>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <semaphore.h>

const int QUEUE_SIZE_LIMIT = 10;
const int NUM_RANDOM_NUMBERS = 100;

std::queue<int> sharedQueue;
std::atomic<bool> shouldContinue(true);
std::mutex mutex;
std::condition_variable cv;
sem_t semaphore;

void producerThread()
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(1, 50);

    for (int i = 0; i < NUM_RANDOM_NUMBERS; ++i) {
        sem_wait(&semaphore);  //  Wait on a semaphore to allow consumer to empty the queue
        {
            std::lock_guard<std::mutex> guard(mutex);  // Create a std::lock_guard using the mutex to enter the critical section
            int num = distribution(generator); // Generate a random integer
            sharedQueue.push(num); // Push number to the shared queue
            std::cout << "Produced: " << num << std::endl;
        }
        cv.notify_all();  // Notify all threads waiting on the semaphore
    }
    shouldContinue = false;
    cv.notify_all();  // Notify all threads waiting on the semaphore

}

/**
 * @brief Determine if the calling thread should wait.
 *
 * IF the variable sharedQueue is NOT EMPTY OR IF the variable shouldContinue is FALSE
 *   return TRUE
 * ELSE
 *   return FALSE
 * @return
 */
bool shouldWait()
{
    return !sharedQueue.empty() || !shouldContinue;
}

void consumerThread()
{
    while (true)
    {

        std::unique_lock<std::mutex> lock(mutex);  // Prepare to wait
        cv.wait(lock, []{ return shouldWait(); });  // Wait until there's an item or stopping condition

        if (!sharedQueue.empty()) {
            int num = sharedQueue.front();
            sharedQueue.pop();
            std::cout << "Consumed: " << num << std::endl;
            lock.unlock();  // Unlock before signaling semaphore
            sem_post(&semaphore);  // Signal space available in the queue
        } else if (!shouldContinue) {
            break;  // Exit loop if no items and shouldContinue is false
        }

	// TODO (part 2):
	//
        // 1. Create a std::unique_lock
        // 2. Wait on the condition variable
        //    a. Call shouldWait will return TRUE if this thread should wait on the conditional
        // 3. IF the sharedQueue is NOT EMPTY THEN
        //    a. Get an item from the queue
        //    b. Delete item from the queue
        //    c. Write the consumed integer to console, "Consumed: <num>"
        //    d. Signal that a slot is available in the queue by calling sem_post
        // 4. IF shouldContinue is false and the sharedQueue is empty THEN
        //    Terminate loop

	// End TODO part 2

    }
}

int main()
{
    // Initialize the semaphore
    sem_init(&semaphore, 0, QUEUE_SIZE_LIMIT);

    // Create and launch the producer and consumer threads
    std::thread producer(producerThread);
    std::thread consumer(consumerThread);

    // Wait for the producer thread to finish and join with the consumer thread
    producer.join();
    consumer.join();

    // Destroy the semaphore
    sem_destroy(&semaphore);

    return 0;
}





