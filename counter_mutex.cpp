//Задача 1: Потокобезопасный счётчик
//
//Есть глобальная переменная - счётчик, которую увеличивают несколько потоков.Нужно, чтобы в итоге в счётчике оказалось ровно N* M, где :
//
//	N — количество потоков,
//
//	M — сколько раз каждый поток увеличивает счётчик.
//
//	Условие
//
//	Напиши функцию, которая запускает N потоков.
//
//	Каждый поток делает M инкрементов.
//
//	В конце нужно вывести итоговое значение счётчика и проверить, что оно равно N* M.
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

int main()
{
	int n, m;
	std::cin >> n >> m;
	int counter = 0;
	std::mutex counter_mutex;
	std::vector<std::thread> threads(n);
	for (auto& t : threads)
	{
		t = std::thread([&counter, &counter_mutex, m]()
			{
				for (int j = 0; j < m; ++j)
				{
					std::lock_guard<std::mutex> lock(counter_mutex);
					++counter;
				}
			});
	}

	for (auto& t : threads)
	{
		t.join();
	}

	std::cout << n * m << " = " << counter << "\n";
}
