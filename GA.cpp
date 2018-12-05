#include <memory.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>
#include "GA.h"
#include "tools.h"

using namespace std;

/*��ȡ���ݼ����ҳ�ʼ����Ⱥ*/
void GA::init()
{
	/*���ļ�*/
	int d[2000];
	int dk = 0;
	ifstream infile("gr48.txt", ios::in);
	int k = 0;
	while (!infile.eof())
		infile >> d[k++];

	infile.close();             //�ر��ļ������� 

	for (int j = 1; j <= Numgene; j++)
	{
		for (int i = 1; i <= j; i++)
		{
			gr48[j][i] = d[dk++];
		}
	}

	/*��ʼ���Ӵ���Ⱥ����*/
	numOfOffspring = 0;

	/*���������������*/
	int sample[Numgene + 1] = { 0 };

	for (int i = 1; i <= Numgene; i++)
		sample[i] = i;

	for (int i = 1; i <= Numpop; i++)
	{
		random_shuffle(sample + 1, sample + Numgene +1);
		memcpy(populations[i].cities + 1, sample + 1, Numgene * sizeof(int));
		fitness(populations + i);
	}
}

/*ѡ����һ��*/
void GA::selection()
{
	memcpy(offspring + numOfOffspring + 1, populations + 1, Numpop * sizeof(individual));//��������
	numOfOffspring = numOfOffspring + Numpop;
	/*��Ӣ����*/
	eliteChoose();

	/*���̶�ѡ��*/
	int pool[Numpop - Elite + 1] = { 0 };
	rouletteChoose(offspring, numOfOffspring, pool, Numpop - Elite);

	for (int i = Elite + 1; i <= Numpop; i++)
		memcpy(populations + i, offspring + pool[i - Elite], sizeof(individual));//�������̶�ѡ��ĸ���
	
	//memset(offspring, 0, numOfOffspring * sizeof(individual));
	numOfOffspring = 0;
}

/*ѭ������*/
void GA::cycleCrossover()
{
	int pool[Numpop + 1] = { 0 };
	int cycleSize = 0;
	rouletteChoose(populations, Numpop, pool, Numpop);//���̶�ѡ������������н���
	
	for (int i = 1; i <= Numpop; i = i + 2)
	{
		if (random0To1(999) < Pc)
		{
			int mutation1 = numOfOffspring + 1;
			int mutation2 = numOfOffspring + 2;
			int p1 = pool[i];
			int p2 = pool[i + 1];

			int cycleIndex[Numgene + 2] = { 0 };
			cycleIndex[1] = randomAtInterval(1, Numgene);
			for (int index = 2; index <= Numgene + 1; index++)
			{
				cycleIndex[index] = getIndex(populations[p1].cities, Numgene, populations[p2].cities[cycleIndex[index - 1]]);
				if (populations[p2].cities[cycleIndex[index]] == populations[p1].cities[cycleIndex[1]])
				{
					cycleSize = index;
					break;
				}
			}
			memcpy(offspring + mutation1, populations + p2, sizeof(individual));
			memcpy(offspring + mutation2, populations + p1, sizeof(individual));
			for (int j = 1; j <= cycleSize; j++)
			{
				offspring[mutation1].cities[cycleIndex[j]] = populations[p1].cities[cycleIndex[j]];
				offspring[mutation2].cities[cycleIndex[j]] = populations[p2].cities[cycleIndex[j]];
			}

			fitness(offspring + mutation1);
			fitness(offspring + mutation2);

			numOfOffspring = numOfOffspring + 2;

		}
	}
}

/*���ظ�ʵ��������췽��*/
void GA::mutationForNoRepe(individual *indi)
{
	int mutationStart = 0, mutationEnd = 0,mutationLength = 0;
	int mutationResult[Numgene + 1], mutationFragment[Numgene + 1];

	mutationStart = randomAtInterval(2, Numgene - 1);
	mutationEnd = randomAtInterval(2, Numgene - 1);
	while (mutationStart == mutationEnd)
		mutationEnd = randomAtInterval(2, Numgene - 1);
	makeBigSmall(mutationEnd,mutationStart);

	mutationLength = mutationEnd - mutationStart + 1;

	for (int j = 1; j <= mutationLength; ++j)
		mutationFragment[j] = indi->cities[j + mutationStart - 1];

	for (int j = 1; j <= Numgene - mutationLength; ++j)
	{
		if (j < mutationStart)
			mutationResult[j] = indi->cities[j];
		else mutationResult[j] = indi->cities[j + mutationLength];
	}

	for (int j = Numgene - mutationLength; j <= Numgene; ++j)
		mutationResult[j + 1] = mutationFragment[j - Numgene + mutationLength + 1];

	memcpy(indi->cities, mutationResult,sizeof(individual));
}

/*����*/
void GA::mutationForNorepeAll()
{
	int crossNumOfOffspring = numOfOffspring;

	for (int i = 1; i <= crossNumOfOffspring; i++)
	{
		bool flag = false;
		
		if (random0To1(999) < Pm)
		{
			memcpy(offspring + numOfOffspring + 1, offspring + i, sizeof(individual));
			mutationForNoRepe(offspring + numOfOffspring + 1);
			flag = true;
		}

		if (flag)
		{
			fitness(offspring + numOfOffspring + 1);
			numOfOffspring++;
		}
	}
}

/*���ظ���������ľֲ�����*/
void GA::localSearchIndiPro(individual *indi)
{
	individual copy = *indi;

	int invertStart, invertEnd;

	invertStart = randomAtInterval(2, Numgene - 1);
	invertEnd = randomAtInterval(2, Numgene - 1);
	while (invertStart == invertEnd)
		invertEnd = randomAtInterval(2, Numgene - 1);
	makeBigSmall(invertEnd,invertStart);

	int flagA, flagB;
	for (flagA = invertStart, flagB = invertEnd; flagA < flagB; ++flagA, --flagB)
		swap(copy.cities[flagA], copy.cities[flagB]);

	fitness(&copy);

	if (copy.fitness > indi->fitness)
		*indi = copy;
}

/*����Ⱥ���оֲ�����*/
void GA::localSearchAll()
{
	for (int i = 1; i <= numOfOffspring; i = i++)
	{
		localSearchIndiPro(offspring + i);
	}
}

/*��ӡ���*/
void GA::resultPrint()
{
	cout << "Optimal city quen: ";
	for (int i = 1; i <= Numgene; i++) {
		cout << optimalIndividual.cities[i] << "->";
	}
	cout << endl;

	cout << "The min distance is:" << 1 / (optimalIndividual.fitness / 100000.0) << endl<<endl;

	clock_t clockEnd = clock();
}

/*������Ⱥ��Ӧ��*/
void GA::fitness(individual *indi)
{
	int big = 0, small = 0;

	indi->fitness = 0;
	for (int i = 1; i < Numgene; i++)
	{
		big = indi->cities[i];
		small = indi->cities[i + 1];
		makeBigSmall(big, small);
		indi->fitness += gr48[big][small];
	}
	big = indi->cities[1];
	small = indi->cities[Numgene];
	makeBigSmall(big, small);
	indi->fitness += gr48[big][small];

	indi->fitness = (1 / indi->fitness) * 100000.0;

	if (optimalIndividual.fitness < indi->fitness) {
		resultPrint();
		memcpy(&optimalIndividual, indi, sizeof(individual));
	}
}

/*��Ӣѡ��*/
void GA::eliteChoose()
{
	partial_sort(offspring + 1, offspring + Elite +1,offspring + numOfOffspring ,getElite);
	memcpy(populations + 1, offspring + 1, Elite * sizeof(individual));
}

/*���̶�*/
void GA::rouletteChoose(individual *pop,int sizeofPop,int *pool,int sizeofSelect)
{
	double fit[Numpop * 4 +1] = { 0.0 };
	double sumFit = 0;
	int x = 0;
	for (int i = 1; i <= sizeofPop; i++)
		sumFit += pop[i].fitness;

	for (int i = 1; i <= sizeofPop; i++)
	{
		fit[i] = pop[i].fitness / sumFit + fit[i - 1];
	}
	for (int i = 1; i <= sizeofSelect; i++)
	{
		double chooseOne = random0To1(999);
		for (int j = 1; j <= sizeofPop; j++)
		{
			if (chooseOne < fit[j])
			{
			    x = j;
				pool[i] = j;
				break;
			}
		}
	}
}

