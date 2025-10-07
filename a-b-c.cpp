//Задача 4: Три потока A–B–C(Round - Robin)
//Условие
//
//Есть три потока :
//
//A → печатает "A"
//
//B → печатает "B"
//
//C → печатает "C"
//
//Нужно, чтобы они печатали строго по очереди :
//
//A
//B
//C
//A
//B
//C
//...
//
//
//Количество итераций(N) известно заранее(например, N = 5 → получится ABCABCABCABCABC).

#include <iostream>
#include <mutex>
int main()
{
	int n = 0;
	std::cin >> n;

	int turn = 0;
	std::mutex turn_mutex;
	std::condition_variable cv;

	auto print_task = [&](char letter, int my_turn)
		{
			for (int i = 0; i < n; ++i)
			{
				std::unique_lock<std::mutex> lock(turn_mutex);
				cv.wait(lock, [&] { return turn == my_turn; });
				std::cout << letter;
				turn = (turn + 1) % 3;
				cv.notify_all();
			}
		};

	std::thread a(print_task, 'A', 0);
	std::thread b(print_task, 'B', 1);
	std::thread c(print_task, 'C', 2);

	a.join();
	b.join();
	c.join();

	return 0;
}