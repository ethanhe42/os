#include<stdlib.h>
#include<iostream>
#include<time.h>
#include<stdio.h>
using namespace std;
#define  total_instruction  100    /*ָ������*/
#define  M 100               /*ʵ��ҳ��*/

#define N 5    //����ҳ����
struct Pro
{
	int num, time;
};
int a[total_instruction];

int page[N];

void Input(Pro p[total_instruction])
{
	int m, i, m1, m2;
	srand((unsigned int)time(NULL));
	m = rand() % 400;  //
	for (i = 0; i<total_instruction;) /*����ָ�����*/
	{
		if (m<0 || m>399)
		{
			printf("When i==%d,Error,m==%d\n", i, m);
			exit(0);
		}
		a[i] = m;                            /*��ѡһָ����ʵ�m*/
		a[i + 1] = a[i] + 1;
		a[i + 2] = a[i] + 2;     /*˳��ִ������ָ��*/
		int m1 = rand() % m; /*ִ��ǰ��ַָ��m1 */
		a[i + 3] = m1;
		a[i + 4] = m1 + 1;
		a[i + 5] = m1 + 2;/*˳��ִ������ָ��*/

						  // s=(158-a[i+5])*rand( )/32767/32767/2+a[i+5]+2;
		m2 = rand() % (157 - m1) + m1 + 3;
		a[i + 6] = m2;
		if ((m2 + 2) > 159)
		{
			a[i + 7] = m2 + 1;
			i += 8;
		}
		else
		{
			a[i + 7] = m2 + 1;
			a[i + 8] = m2 + 2;
			i = i + 9;
		}
		m = rand() % m2;

	}
	for (i = 0; i<total_instruction; i++) /*��ָ�����б任��ҳ��ַ��*/
	{
		p[i].num = a[i] / 10;
		p[i].time = 0;
	}
}
void print(Pro *page1)//��ӡ��ǰ��ҳ��
{
	Pro *page = new Pro[N];
	page = page1;
	for (int i = 0; i<N; i++)
		printf("%-4d", page[i].num);
	//cout<<page[i].num<<"  ";
	cout << endl;
	//free(page);
}


int  Search(int e, Pro *page1)
{
	Pro *page = new Pro[N];
	page = page1;
	for (int i = 0; i<N; i++)if (e == page[i].num)return i;
	return -1;
}

int  Searchtime(int e, Pro *page1)
{
	Pro *page = new Pro[N];
	page = page1;
	for (int i = 0; i<N; i++)if (e == page[i].time)return i;
	return -1;
}

int Max(Pro *page1)
{
	Pro *page = new Pro[N];
	page = page1;
	int e = page[0].time, i = 0;
	while (i<N)//�ҳ�������ʱ�����ҳ��
	{
		if (e<page[i].time)e = page[i].time;
		i++;
	}
	for (i = 0; i<N; i++)if (e == page[i].time)return i;

	return -1;
}




int Compfu(Pro *page1, int i, int t, Pro p[M])
{
	Pro *page = new Pro[N];
	page = page1;

	int count = 0;
	for (int j = i; j<M; j++)
	{
		if (page[t].num == p[j].num)break;
		else count++;
	}
	return count;

}

int main()
{
	Pro p[total_instruction];
	Pro *page = new Pro[N];
	char c;
	int t = 0, i;
	float n = 0;
	Input(p);

	do {
		for (i = 0; i<N; i++)//���Ի�ҳ��������
		{
			page[i].num = -1;
			page[i].time = 2 - i;
		}
		printf("ϵͳ�����������Ϊ��");
		for (int e = 0; e<M; e++)
			cout << p[e].num << " ";
		cout << endl;
		i = 0;
		cout << "f:FIFOҳ���û�" << endl;
		cout << "l:LRUҳ���û�" << endl;
		cout << "o:OPTҳ���û�" << endl;
		cout << "n:NURҳ���û�" << endl;
		cout << "������������" << endl;
		cin >> c;

		if (c == 'f')//FIFOҳ���û�
		{
			n = 0;
			cout << "ҳ���û����:   " << endl;
			while (i< total_instruction)
			{
				if (Search(p[i].num, page) >= 0)
					i++;//�ҵ���ͬ��ҳ��
				else
				{
					if (t == N)t = 0;
					else
					{
						n++;//
						page[t].num = p[i].num;
						print(page);
						t++;
					}
				}
			}
			cout << "ȱҳ������" << n << "    �����ʣ�" << 1 - n / total_instruction << endl;
		}
		if (c == 'n')//NURҳ���û�
		{
			n = 0;
			cout << "��CLEAR_PERIODΪ5" << endl;
			cout << "ҳ���û����:   " << endl;
			int period = 0;
			int time_set;
			while (i < total_instruction) 
			{
				if (period % 10 == 0) {
					for (int q = 0; q < N; q++)
						page[q].time = 0;
				}
				t = Search(p[i].num, page);
				if (t >= 0) {
					page[t].time = 1;
				}
				else {
					time_set = Searchtime(0, page);
					if (time_set == -1){
						page[0].num = p[i].num;
						n++;
					}
					else
					{
						page[time_set].num = p[i].num;
						page[time_set].time = 1;
						n++;
					}
				}
				print(page);
				i++;
				period++;
			}
			cout << "ȱҳ������" << n << "    �����ʣ�" << 1 - n / total_instruction << endl;
		}

		if (c == 'l')//LRUҳ���û�
		{
			n = 0;
			cout << "ҳ���û����:   " << endl;
			while (i<total_instruction)
			{
				int k;
				k = t = Search(p[i].num, page);
				if (t >= 0)
					page[t].time = 0;
				else
				{
					n++;
					t = Max(page);
					page[t].num = p[i].num;
					page[t].time = 0;
				}
				for (int j = 0; j<N; j++)
				{
					if (j != t)page[j].time++;
				}
				/*if(t==0){page[t+1].time++;page[t+2].time++;}
				if(t==1){page[2].time++;page[0].time++;}
				if(t==2){page[1].time++;page[0].time++;}*/
				if (k == -1)   print(page);
				i++;
			}
			cout << "ȱҳ������" << n << "    �����ʣ�" << 1 - n / total_instruction << endl;
		}
		if (c == 'o')//OPTҳ���û�
		{
			n = 0;
			while (i<total_instruction)
			{
				if (Search(p[i].num, page) >= 0)i++;
				else
				{
					if (page[N - 1].num == -1)
					{
						for (int g = 0; g<N; g++)
							if (page[g].num == -1)
							{
								page[g].num = p[i].num;
								i++;
								n++;
								print(page);
								break;
							}
					}
					else {
						int temp = -1, cn;
						for (t = 0; t<N; t++)
						{
							if (temp<Compfu(page, i, t, p))
							{
								temp = Compfu(page, i, t, p);
								cn = t;
							}
						}
						page[cn] = p[i];
						n++;
						print(page);
						i++;
					}
				}
			}
			cout << "ȱҳ������" << n << "    �����ʣ�" << 1 - n / total_instruction << endl;
		}
	} while (c == 'f' || c == 'l' || c == 'o'|| c == 'n');
	return 0;
}
