//Задаче 3: Ping–Pong.
//
//🔹 Условие
//
//Есть два потока :
//
//Ping → печатает "ping".
//
//Pong → печатает "pong".
//
//Нужно, чтобы вывод шёл чётко чередуясь :
//
//ping
//pong
//ping
//pong
//...
//
//
//Количество итераций заранее известно(N).
//
//🔹 Подсказка
//
//Использовать std::mutex + std::condition_variable.
//
//Нужен общий флаг(bool ping_turn = true;), который указывает, чей сейчас ход.
//
//Каждый поток должен :
//
//Ждать, пока его очередь.
//
//Напечатать строку.
//
//Переключить флаг.
//
//Вызвать notify_one().

#include <mutex>
#include <iostream>
int main() {
	int n = 0;
	std::cin >> n;

	bool bPing = true;
	std::mutex ping_mutex;
	std::condition_variable cv;

	std::thread ping([&]()
		{
			for (int i = 0; i < n; ++i)
			{
				std::unique_lock<std::mutex> lock(ping_mutex);
				cv.wait(lock, [&] { return bPing; });
				std::cout << "ping\n";
				bPing = false;
				cv.notify_one();
			}
		});

	std::thread pong([&]()
		{
			for (int i = 0; i < n; ++i)
			{
				std::unique_lock<std::mutex> lock(ping_mutex);
				cv.wait(lock, [&] { return !bPing; });
				std::cout << "pong\n";
				bPing = true;
				cv.notify_one();
			}
		});

	ping.join();
	pong.join();

	return 0;
}