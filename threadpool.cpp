//Минимальный Thread Pool
//Условие
//
//Реализовать простейший пул потоков :
//
//В пуле есть N воркеров(рабочих потоков).
//
//Есть очередь задач(std::function<void()>).
//
//Функция enqueue(task) кладёт новую задачу в очередь.
//
//Потоки - воркеры берут задачи из очереди и выполняют.
//
//При завершении работы пул корректно останавливается.
//
//🔹 Подсказка
//
//Тебе понадобятся :
//
//std::queue<std::function<void()>> tasks;
//
//std::mutex tasks_mutex;
//
//std::condition_variable cv;
//
//Флаг stop, чтобы корректно завершить воркеры.

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <iostream>
#include <sstream>

class ThreadPool
{
public:
	ThreadPool(size_t n)
	{
		for (size_t i = 0; i < n; ++i)
		{
			m_workers.emplace_back([this]
				{
					while (true)
					{
						std::function<void()> task;
						
						{
							std::unique_lock<std::mutex> lock(m_tasks_mutex);
							m_cv.wait(lock, [this]
								{
									return m_stop || !m_tasks.empty();
								});
							if (m_stop && m_tasks.empty())
								return;

							task = std::move(m_tasks.front());
							m_tasks.pop();
						}

						task();
					}
				});
		}
	};

	~ThreadPool()
	{
		{
			std::lock_guard<std::mutex> lock(m_tasks_mutex);
			m_stop = true;
		}
		m_cv.notify_all();
		for (auto& worker : m_workers)
		{
			worker.join();
		}
	};

	void enqueue(std::function<void()> task)
	{
		std::lock_guard<std::mutex> lock(m_tasks_mutex);
		m_tasks.push(task);
		m_cv.notify_one();
	};

private:
	std::vector<std::thread> m_workers;

	std::mutex m_tasks_mutex;
	std::queue<std::function<void()>> m_tasks;
	std::condition_variable m_cv;
	bool m_stop = false;
};

int main()
{
	int n;
	std::cin >> n;
	ThreadPool pool(n);

	int m;
	std::cin >> m;

	for (int i = 0; i < m; ++i)
	{
		pool.enqueue([i]
			{
				std::ostringstream oss;
				oss << "task " << i << " in thread " << std::this_thread::get_id() << std::endl;
				std::cout << oss.str();
			});
	}

	return 0;
}