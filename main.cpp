#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <random>

const int NUM_PHILOSOPHERS = 5;

// М'ютекси для палиць
std::mutex chopsticks[NUM_PHILOSOPHERS];

// Генератор випадкових чисел
std::random_device rd;
std::mt19937 rng(rd());
std::uniform_int_distribution<int> dist(0, 1000);

// Функція, яка моделює дії філософа
void philosopher(int index) {
    while (true) {
        // Філософ думає
        std::cout << "Philosopher " << index << " is thinking.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));

        // Філософ намагається їсти
        int left = index;
        int right = (index + 1) % NUM_PHILOSOPHERS;

        // Впровадження асиметрії для уникнення мертвого блокування
        if (index % 2 == 0) {
            std::lock(chopsticks[left], chopsticks[right]);
            std::lock_guard<std::mutex> leftLock(chopsticks[left], std::adopt_lock);
            std::lock_guard<std::mutex> rightLock(chopsticks[right], std::adopt_lock);
        } else {
            std::lock(chopsticks[right], chopsticks[left]);
            std::lock_guard<std::mutex> rightLock(chopsticks[right], std::adopt_lock);
            std::lock_guard<std::mutex> leftLock(chopsticks[left], std::adopt_lock);
        }

        std::cout << "Philosopher " << index << " is eating.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
        // М'ютекси автоматично розблоковуються після завершення scope
    }
}

int main() {
    // Створюємо потоки для філософів
    std::vector<std::thread> philosophers;
    for (int i = 0; i < NUM_PHILOSOPHERS; ++i) {
        philosophers.emplace_back(philosopher, i);
    }

    // Очікуємо завершення всіх потоків (у цій програмі вони працюють нескінченно)
    for (auto& thread : philosophers) {
        thread.join();
    }

    return 0;
}
