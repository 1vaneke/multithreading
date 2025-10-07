#include <drogon/drogon.h>
#include <atomic>
#include <unordered_map>
#include <mutex>

std::unordered_map<std::string, std::atomic<int>> counters;
std::mutex countersMutex;

int main() {
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


 //iwr "http://localhost:8080/view?item_id=123" -Method POST
 //iwr "http://localhost:8080/views?item_id=123"
