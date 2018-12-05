#include <iostream>
#include <ctime>

#include "GA.h"

using namespace std;

int main() 
{
	int It = Iter;
	
    GA ga;

	srand((unsigned)time(NULL));
	
	clock_t clockStart = clock();
	for (int i = 1; i <= 15; i++) 
	{
		It = Iter;
		ga.init(); //��ȡ���ݼ��Լ���Ⱥ��ʼ��

		while (It--)
		{
			ga.cycleCrossover();//ѭ������

			ga.mutationForNorepeAll();//����

			ga.localSearchAll(); //�ֲ�����

			ga.selection();//ѡ����һ��
		}
	}
	clock_t clockEnd = clock();
	cout << "Run time is :" << (double)(clockEnd - clockStart) / CLOCKS_PER_SEC << endl;
    return 0;
}