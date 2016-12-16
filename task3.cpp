#include<stdlib.h>
#include<iostream>
#include<time.h>
#include<stdio.h>

using namespace std;

#define  total_instruction  100    /*指令流长*/
#define  M 100               /*实际页数*/

#define N 5    //可用页面数
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
	for (i = 0; i<total_instruction;) /*产生指令队列*/
	{
		if (m<0 || m>399)
		{
			printf("When i==%d,Error,m==%d\n", i, m);
			exit(0);
		}
		a[i] = m;                            /*任选一指令访问点m*/
		a[i + 1] = a[i] + 1;
		a[i + 2] = a[i] + 2;     /*顺序执行两条指令*/
		int m1 = rand() % m; /*执行前地址指令m1 */
		a[i + 3] = m1;
		a[i + 4] = m1 + 1;
		a[i + 5] = m1 + 2;/*顺序执行两条指令*/

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
	for (i = 0; i<total_instruction; i++) /*将指令序列变换成页地址流*/
	{
		p[i].num = a[i] / 10;
		p[i].time = 0;
	}
}
void print(Pro *page1)//打印当前的页面
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
	while (i<N)//找出离现在时间最长的页面
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
		for (i = 0; i<N; i++)//初试化页面基本情况
		{
			page[i].num = -1;
			page[i].time = 2 - i;
		}
		printf("系统产生的随机数为：");
		for (int e = 0; e<M; e++)
			cout << p[e].num << " ";
		cout << endl;
		i = 0;
		cout << "f:FIFO页面置换" << endl;
		cout << "l:LRU页面置换" << endl;
		cout << "o:OPT页面置换" << endl;
		cout << "n:NUR页面置换" << endl;
		cout << "按其它键结束" << endl;
		cin >> c;

		if (c == 'f')//FIFO页面置换
		{
			n = 0;
			cout << "页面置换情况:   " << endl;
			while (i< total_instruction)
			{
				if (Search(p[i].num, page) >= 0)
					i++;//找到相同的页面
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
			cout << "缺页次数：" << n << "    命中率：" << 1 - n / total_instruction << endl;
		}
		if (c == 'n')//NUR页面置换
		{
			n = 0;
			cout << "设CLEAR_PERIOD为5" << endl;
			cout << "页面置换情况:   " << endl;
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
			cout << "缺页次数：" << n << "    命中率：" << 1 - n / total_instruction << endl;
		}

		if (c == 'l')//LRU页面置换
		{
			n = 0;
			cout << "页面置换情况:   " << endl;
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
			cout << "缺页次数：" << n << "    命中率：" << 1 - n / total_instruction << endl;
		}
		if (c == 'o')//OPT页面置换
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
			cout << "缺页次数：" << n << "    命中率：" << 1 - n / total_instruction << endl;
		}
	} while (c == 'f' || c == 'l' || c == 'o'|| c == 'n');
	return 0;
}
