//Идея
//
//У нас есть несколько Reader - ов, каждый читает число и кладёт результат в свой std::promise<int>.
//
//Processor ждёт от каждого future, считает квадрат, и кладёт во второй promise<int>.
//
//Printer ждёт все результаты и печатает их.

#include <iostream>
#include <thread>
#include <future>
#include <vector>

int main() {
    const int readers_count = 3;

    // Промисы/фьючерсы для Readers -> Processor
    std::vector<std::promise<int>> p_readers(readers_count);
    std::vector<std::future<int>> f_readers;
    for (auto& p : p_readers) {
        f_readers.push_back(p.get_future());
    }

    // Промисы/фьючерсы для Processor -> Printer
    std::vector<std::promise<int>> p_proc(readers_count);
    std::vector<std::future<int>> f_proc;
    for (auto& p : p_proc) {
        f_proc.push_back(p.get_future());
    }

    // Readers
    std::vector<std::thread> readers;
    for (int i = 0; i < readers_count; i++) {
        readers.emplace_back([i, &p_readers]() {
            int x = (i + 1) * 10; // имитация чтения
            std::cout << "Reader " << i << " read: " << x << "\n";
            p_readers[i].set_value(x);
            });
    }

    // Processor
    std::thread processor([&]() {
        for (int i = 0; i < readers_count; i++) {
            int x = f_readers[i].get();
            int y = x * x;
            std::cout << "Processor got " << x << ", computed " << y << "\n";
            p_proc[i].set_value(y);
        }
        });

    // Printer
    std::thread printer([&]() {
        for (int i = 0; i < readers_count; i++) {
            int result = f_proc[i].get();
            std::cout << "Printer result: " << result << "\n";
        }
        });

    // join
    for (auto& r : readers) r.join();
    processor.join();
    printer.join();

    return 0;
}
