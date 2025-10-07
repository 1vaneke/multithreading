//Задача 2: Producer–Consumer(классика)
//Условие
//
//Есть общая очередь std::queue<int>.
//
//Поток Producer кладёт туда числа от 1 до N.
//
//Поток Consumer забирает их и печатает.
//
//Нужно синхронизировать доступ к очереди :
//
//чтобы не было гонок данных;
//
//чтобы Consumer ждал, когда очередь пуста(а не крутился в бесконечном цикле).
//
//Подсказка
//
//Используй std::mutex для защиты очереди.
//
//Используй std::condition_variable, чтобы Consumer ждал, пока Producer что - то положит.

#include <queue>
#include <mutex>
#include <iostream>

int main() {
	int n;
	std::cin >> n;

	std::queue<int> queue;
	std::mutex queue_mutex;
	std::condition_variable cv;

	bool ready = false;

	std::thread producer([&]()
		{
			for (int i = 0; i < n; ++i)
			{
				std::lock_guard<std::mutex> lock(queue_mutex);
				queue.push(i + 1);
				cv.notify_one();
			}
			std::lock_guard<std::mutex> lock(queue_mutex);
			ready = true;
			cv.notify_one();
		});

	std::thread consumer([&]()
		{
			while(true)
			{
				std::unique_lock<std::mutex> lock(queue_mutex);
				cv.wait(lock, [&] { return ready || !queue.empty(); });
				while (!queue.empty())
				{
					std::cout << queue.front() << "\n";
					queue.pop();
				}

				if (ready && queue.empty())
					break;
			}
		});

	producer.join();
	consumer.join();

	return 0;
}