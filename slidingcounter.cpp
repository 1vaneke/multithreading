#include <iostream>
#include <unordered_map>
#include <deque>
#include <string>

struct Event {
    long long ts;       // timestamp
    std::string user;
    std::string item;
};

class SlidingCounter {
public:
    explicit SlidingCounter(long long windowSec = 300) : window(windowSec) {}

    void add_event(long long ts, const std::string& user, const std::string& item) {
        auto& dq = events[item];
        dq.push_back({ ts, user, item });
        cleanup(dq, ts); // подчистим старые
    }

    int get_stats(const std::string& item, long long now) {
        auto it = events.find(item);
        if (it == events.end()) return 0;

        auto& dq = it->second;
        cleanup(dq, now);
        return (int)dq.size();
    }

private:
    long long window;
    std::unordered_map<std::string, std::deque<Event>> events;

    void cleanup(std::deque<Event>& dq, long long now) {
        while (!dq.empty() && dq.front().ts <= now - window) {
            dq.pop_front();
        }
    }
};

int main() {
    SlidingCounter counter(300); // окно 5 мин = 300 сек

    counter.add_event(1000, "u1", "itemA");
    counter.add_event(1100, "u2", "itemA");
    counter.add_event(1400, "u3", "itemA");

    std::cout << counter.get_stats("itemA", 1400) << "\n"; // 3
    std::cout << counter.get_stats("itemA", 1600) << "\n"; // 2 (1000 уже выпал)
    std::cout << counter.get_stats("itemA", 2000) << "\n"; // 0 (все устарели)
    std::cout << counter.get_stats("itemB", 1500) << "\n"; // 0 (пустое окно)
}
