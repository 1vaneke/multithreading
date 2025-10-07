# 🧵 Multithreading Exercises in C++

Набор учебных и экспериментальных программ по многопоточному программированию на **C++17/20**.  
Проекты охватывают темы потоков, синхронизации, работы с пулами потоков, HTTP-сервисами и атомарными счётчиками.

---

## 📂 Содержание

| Файл | Описание | Основные концепции |
|------|-----------|--------------------|
| **counter.cpp** | Потокобезопасный счётчик (на `std::atomic`) | `std::atomic`, многопоточность |
| **counter_mutex.cpp** | Счётчик с защитой `std::mutex` | `std::lock_guard`, `std::mutex` |
| **producer-consumer.cpp** | Классическая модель Producer–Consumer | `std::queue`, `std::condition_variable` |
| **ping-pong.cpp** | Два потока, чередующийся вывод `ping` / `pong` | `std::condition_variable`, блокировки |
| **a-b-c.cpp** | Три потока A–B–C, синхронный Round-Robin вывод | Очередность, условные переменные |
| **parallelsum.cpp** | Параллельная сумма массива | `std::async`, `std::future`, `std::reduce` |
| **promisefuture.cpp** | Конвейер на `promise` / `future` | Передача данных между потоками |
| **threadpool.cpp** | Минимальный пул потоков | Очередь задач, `std::function<void()>`, `std::condition_variable` |
| **threadpool2.cpp** | Улучшенный пул потоков (enqueue возвращает `std::future<T>`) | `std::packaged_task`, шаблоны, `std::future` |
| **slidingcounter.cpp** | Sliding window counter | Очистка устаревших событий, `std::deque` |
| **interview.cpp** | Пример мини-задачи по строкам | Подсчёт последовательностей символов |
| **viewscounter.cpp** | Простой HTTP-счётчик на WinSock2 | `std::atomic`, `std::thread`, сокеты |
| **viewscountersimulation.cpp** | Имитация key–value сервиса | `std::mutex`, `std::unordered_map`, потоки |
| **viewcounterdrogon.cpp** | HTTP-сервис на **Drogon Framework** | REST API, `std::mutex`, `std::atomic` |
| **viewcounterdrogoncpr.cpp** | Сервер + нагрузочный тест (Drogon + CPR) | Веб-сервер, клиентские запросы, барьер синхронизации |
