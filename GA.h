#pragma once

const int Numgene = 48; //����λ��
const int Numpop = 200;  //��Ⱥ����
const double Pc = 0.6;
const double Pm = 0.1;
const int Elite = 10;
const int Iter = 2000;

struct individual
{
	int cities[Numgene + 1] = { 0 };
	double fitness = 0;
};

class GA{

public:
    /*��ȡ���ݼ����ҳ�ʼ����Ⱥ*/
    void init();

    /*ѡ����һ��*/
    void selection();

    /*����*/
	void cycleCrossover();

    /*����*/
	void mutationForNorepeAll();

	/*����Ⱥ���оֲ�����*/
	void GA::localSearchAll();

	/*��ӡ���*/
	void resultPrint();

private:

    /*������Ⱥ��Ӧ��*/
    void fitness(individual *indi);

	/*��Ӣѡ��*/
	void eliteChoose();

	/*���̶�*/
	void rouletteChoose(individual *populations, int sizeofPop, int *pool, int sizeofSelect);

	/*�ֲ�����*/
	void localSearchIndiPro(individual *indi);

	/*���ظ�ʵ��������췽��*/
	void mutationForNoRepe(individual *indi);

private:
	individual populations[Numpop + 1]; //��Ⱥ����
	individual offspring[4 * Numpop + 1]; //�Ӵ����и���
	int numOfOffspring = 0; //�Ӵ�������Ŀ
	individual optimalIndividual;//���Ÿ���
	
	int gr48[50][50] = { 0 }; //gr48���ݼ�
};

