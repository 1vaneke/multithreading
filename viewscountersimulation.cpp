#include <sstream>
#include <unordered_map>
#include <string>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>


std::unordered_map<std::string, uint64_t> store;
std::mutex store_mtx;

// "Сервисная" функция: обработка команд
std::string handle_command(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string op, key;
    iss >> op >> key;

    if (op == "SET") {
        uint64_t val;
        iss >> val;
        {
            std::lock_guard<std::mutex> lock(store_mtx);
            store[key] = val;
        }
        return "OK";
    }
    else if (op == "GET") {
        uint64_t val = 0;
        {
            std::lock_guard<std::mutex> lock(store_mtx);
            if (auto it = store.find(key); it != store.end())
                val = it->second;
        }
        return std::to_string(val);
    }
    else if (op == "INCR") {
        uint64_t val;
        {
            std::lock_guard<std::mutex> lock(store_mtx);
            val = ++store[key];
        }
        return std::to_string(val);
    }

    return "ERR";
}

// Поток: "клиент"
void client_simulation(int id) {
    for (int i = 0; i < 5; i++) {
        std::string key = "counter";
        std::string resp = handle_command("INCR " + key);
        std::ostringstream oss;
        oss << "Thread " << id << " -> " << resp << std::endl;
        std::cout << oss.str();
    }
}

int main() {
    std::vector<std::thread> threads;

    // Запускаем 4 "клиента"
    for (int i = 0; i < 4; i++) {
        threads.emplace_back(client_simulation, i);
    }

    for (auto& t : threads) t.join();

    std::cout << "Final counter = " << handle_command("GET counter") << std::endl;
}
