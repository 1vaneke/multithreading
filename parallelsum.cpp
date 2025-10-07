//Параллельная сумма массива
//Условие
//
//У нас есть вектор из N чисел.
//Нужно вычислить их сумму, распараллелив работу на несколько кусков с помощью std::async.
//
//Требования
//
//Разделить массив на k частей.
//
//Для каждой части запустить std::async и вернуть частичную сумму.
//
//Собрать все результаты через future.get() и сложить.

#include <iostream>
#include <vector>
#include <numeric>
#include <future>
#include <execution>
int main()
{
	int n;
	std::cin >> n;
	std::vector<int> data(n);
	std::iota(data.begin(), data.end(), 1);

	int k = std::thread::hardware_concurrency();
	std::cout << k << std::endl;

	if (k == 0) k = 2; // fallback (если не удалось определить)
	if (k > n) k = n;  // не делим на больше кусочков, чем элементов
	
	std::vector<std::future<long long>> futures(k);

	int chunk_size = n / k;

	for (int i = 0; i < k; ++i)
	{
		int start = i * chunk_size;
		int end = i == k - 1 ? n : (i + 1) * chunk_size;

		futures[i] = std::async(std::launch::async, [start, end, &data]()
			{
				return std::reduce(/*std::execution::par,*/ data.begin() + start, data.begin() + end, 0LL);
			});
	}

	long long sum = 0;
	for (auto& f : futures)
		sum += f.get();

	std::cout << sum;

	return 0;
}