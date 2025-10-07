//������ 1: ���������������� �������
//
//���� ���������� ���������� - �������, ������� ����������� ��������� �������.�����, ����� � ����� � �������� ��������� ����� N* M, ��� :
//
//	N � ���������� �������,
//
//	M � ������� ��� ������ ����� ����������� �������.
//
//	�������
//
//	������ �������, ������� ��������� N �������.
//
//	������ ����� ������ M �����������.
//
//	� ����� ����� ������� �������� �������� �������� � ���������, ��� ��� ����� N* M.
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

std::atomic_int counter(0);

int main()
{
	int n, m;
	std::cin >> n >> m;
	std::vector<std::thread> threads(n);
	for (auto& t : threads)
	{
		t = std::thread([m]()
			{
				for (int j = 0; j < m; ++j)
				{
					++counter;
				}
			});
	}
	
	for (auto& t : threads)
	{
		t.join();
	}

	std::cout << n * m << " = " << counter.load() << "\n";
}
