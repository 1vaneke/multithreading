#include <drogon/drogon.h>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <cpr/cpr.h>
#include <barrier>
#include <iostream>

std::unordered_map<std::string, std::atomic<int>> counters;
std::mutex countersMutex;

void startServer() {
    using namespace drogon;

    // POST /view?item_id=...
    app().registerHandler("/view",
        [](const HttpRequestPtr& req,
            std::function<void(const HttpResponsePtr&)>&& callback) {

                auto id = req->getParameter("item_id");
                if (id.empty()) {
                    auto resp = HttpResponse::newHttpResponse();
                    resp->setStatusCode(k400BadRequest);
                    resp->setBody("Missing item_id");
                    callback(resp);
                    return;
                }

                {
                    std::lock_guard<std::mutex> lock(countersMutex);
                    ++counters[id];
                }

                auto resp = HttpResponse::newHttpResponse();
                resp->setBody("OK");
                callback(resp);
        },
        { Post }
    );

    // GET /views?item_id=...
    app().registerHandler("/views",
        [](const HttpRequestPtr& req,
            std::function<void(const HttpResponsePtr&)>&& callback) {

                auto id = req->getParameter("item_id");
                int value = 0;

                {
                    std::lock_guard<std::mutex> lock(countersMutex);
                    if (counters.find(id) != counters.end()) {
                        value = counters[id].load();
                    }
                }

                auto resp = HttpResponse::newHttpResponse();
                resp->setBody(std::to_string(value));
                callback(resp);
        },
        { Get }
    );

    app().addListener("0.0.0.0", 8080).run();
}

void runTests() {
    const int clients = 20;
    const std::string item = "123";
    const std::string url_post = "http://localhost:8080/view?item_id=" + item;
    const std::string url_get = "http://localhost:8080/views?item_id=" + item;

    std::barrier sync_point(clients);
    std::vector<std::thread> threads;

    for (int i = 0; i < clients; i++) {
        threads.emplace_back([&, i]() {
            std::cout << "Thread " << i << " ready\n";
            sync_point.arrive_and_wait(); // одновременный старт
            auto r = cpr::Post(cpr::Url{ url_post });
            std::cout << "Thread " << i << " started!\n";
            if (r.status_code != 200) {
                std::cerr << "Client " << i << " failed: " << r.text << "\n";
            }
            });
    }

    for (auto& t : threads) t.join();

    // GET запрос - проверим результат
    auto r = cpr::Get(cpr::Url{ url_get });
    std::cout << "Final counter: " << r.text << " (expected " << clients << ")\n";
}

int main() {
    // Запускаем сервер в отдельном потоке
    std::thread serverThread([]() { startServer(); });

    // Даём серверу стартануть
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Запускаем тесты
    runTests();

    // Завершаем (для примера просто убиваем процесс)
    std::exit(0);
}
