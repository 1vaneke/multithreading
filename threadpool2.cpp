// сделаем так, чтобы метод enqueue возвращал std::future<T> от результата задачи.

#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <iostream>
#include <sstream>
#include <future>

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

	template <class F, class... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
	{
		using return_type = decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
		std::future<return_type> res = task->get_future();
		
		{
			std::lock_guard<std::mutex> lock(m_tasks_mutex);
			if (m_stop)
			{
				throw std::runtime_error("enqueu on stopped threadpool");
			}
			m_tasks.emplace([task]
				{
					(*task)();
				});

		}

		m_cv.notify_one();
		return res;
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
	ThreadPool pool(3);

	// задача с void
	auto f1 = pool.enqueue([] { std::cout << "Hello from thread\n"; });

	// задача с возвратом значения
	auto f2 = pool.enqueue([](int a, int b) {
		return a + b;
		}, 5, 7);

	f1.get(); // дождались
	std::cout << "Sum = " << f2.get() << std::endl;

	return 0;
}