#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "extmem.h"

#include <iostream>
#include <vector>
#include <map>
using namespace std;

vector<vector<int>> R;
vector<vector<int>> S;

const int BufferSize = 520;
const int BlockSize = 64;
const int tuples = 7;

const int Rnums = 112;
const int Raddr = 0;
const int RDataSize = 8;


const int Snums = 224;
const int Saddr = 10000;
const int SDataSize = 8;


int Random(int low, int high);
void WriteData(unsigned int * blk, Buffer *buf);
void ReadData(unsigned int * blk, Buffer *buf);


vector<vector<int>> Sort(vector<vector<int>> Table, int index);
void LineSearch(Buffer *buf);
void BinarySearch(Buffer *buf);
void HashSearch(Buffer *buf);

void Projection(unsigned int * blk, Buffer *buf);

void NestLoopJoin(unsigned int * blk, Buffer *buf);
void SortMergeJoin(unsigned int * blk, Buffer *buf);
void HashJoin(unsigned int * blk, Buffer *buf);

void Union(unsigned int * blk, Buffer *buf);
void Intersection(unsigned int * blk, Buffer *buf);

void DiffRtoS(unsigned int * blk, Buffer *buf);
void DiffStoR(unsigned int * blk, Buffer *buf);

int main(int argc, char **argv)
{
	srand((unsigned)time(NULL));
	Buffer buf; /* A buffer */
	//    unsigned char *blk; /* A pointer to a block */
	unsigned int *blk = NULL;
	int i = 0, j;

	/* Initialize the buffer */
	if (!initBuffer(BufferSize, BlockSize, &buf))
	{
		perror("Buffer Initialization Failed!\n");
		return -1;
	}


	WriteData(blk, &buf);
	ReadData(blk, &buf);
	cout << "Result:" << endl;
	system("pause");
	LineSearch(&buf);
	system("pause");
	BinarySearch(&buf);
	system("pause");
	HashSearch(&buf);
	system("pause");
	Projection(blk, &buf);
	system("pause");
	NestLoopJoin(blk, &buf);
	system("pause");
	SortMergeJoin(blk, &buf);
	system("pause");
	HashJoin(blk, &buf);
	system("pause");
	Intersection(blk, &buf);
	system("pause");
	Union(blk, &buf);
	//cout << "Result:" << endl;
	system("pause");
	DiffRtoS(blk, &buf);
	system("pause");
	DiffStoR(blk, &buf);
	system("pause");
	return 0;
}


int Random(int low, int high)  //随机数生成
{
	return rand() % (high - low) + low + 1;
}

void WriteData(unsigned int * blk, Buffer *buf)
{
	unsigned int a, b, c, d;
	int Ri, Rj, Si, Sj;

	int Rblocks = Rnums / tuples;
	int Sblocks = Snums / tuples;

	for (Ri = 0; Ri<Rblocks; Ri++)
	{
		blk = (unsigned int *)getNewBlockInBuffer(buf);
		for (Rj = 0; Rj<tuples; Rj++)
		{
			a = Random(1, 40);
			b = Random(1, 1000);
			*(blk + 2 * Rj) = a;
			*(blk + 2 * Rj + 1) = b;
		}
		*(blk + 2 * Rj + 1) = Ri + 1;
		if (Ri == 15)
		{
			*(blk + 2 * Rj + 1) = 0;
		}
		if (writeBlockToDisk((unsigned char *)blk, Ri + Raddr, buf) != 0)
		{
			perror("Writing Block Failed!\n");
			return;
		}
	}

	for (Si = 0; Si<Sblocks; Si++)
	{
		blk = (unsigned int *)getNewBlockInBuffer(buf);
		for (Sj = 0; Sj<7; Sj++)
		{
			c = Random(20, 60);
			d = Random(1, 1000);
			*(blk + 2 * Sj) = c;
			*(blk + 2 * Sj + 1) = d;
		}
		*(blk + 2 * Sj + 1) = Si + Saddr + 1; //存下一个的块号
		if (Si == 31)
		{
			*(blk + 2 * Sj + 1) = 0;
		}
		if (writeBlockToDisk((unsigned char *)blk, Si + Saddr, buf) != 0)
		{
			perror("Writing Block Failed!\n");
			return;
		}
	}
}

void ReadData(unsigned int * blk, Buffer *buf)
{
	int Rblocks = Rnums / tuples;
	int Sblocks = Snums / tuples;
	unsigned int a, b, c, d;
	int i, j;
	cout << "\nR:" << endl;
	for (i = 0; i < Rblocks; i++)
	{
		//	printf("i:%d\n", i);
		blk = (unsigned int *)readBlockFromDisk(Raddr + i, buf);
		for (j = 0; j < tuples; j++)
		{
			a = *(blk + 2 * j);
			b = *(blk + 2 * j + 1);
			vector<int>tmp;
			tmp.push_back(a);
			tmp.push_back(b);
			R.push_back(tmp);
			cout << "(" << R[i*tuples + j][0] << "," << R[i*tuples + j][1] << ") ";
			/*printf("j:%d\n", j);
			printf("%d-%d\n", a, b);*/
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	cout << "\nS:" << endl;

	for (i = 0; i < Sblocks; i++)
	{
		//	printf("i:%d\n", i);
		blk = (unsigned int *)readBlockFromDisk(i + Saddr, buf);
		for (j = 0; j < tuples; j++)
		{
			c = *(blk + 2 * j);
			d = *(blk + 2 * j + 1);
			vector<int>tmp;
			tmp.push_back(c);
			tmp.push_back(d);
			S.push_back(tmp);
			cout << "(" << S[i*tuples + j][0] << "," << S[i*tuples + j][1] << ") ";
			//printf("j:%d\n", j);
			//printf("%d-%d\n", c, d);
		}
		freeBlockInBuffer((unsigned char *)blk, buf);
	}
	cout << "\n\n\n\n";
}


void Projection(unsigned int * blk, Buffer *buf)
{
	int addr = 500, blkcount=0;
	vector<vector<int>> sortedR = Sort(R, 0);
	for (int i = 0; i < sortedR.size(); i++)
	{
		if (blkcount == 0)
		{
			blk = (unsigned int *)getNewBlockInBuffer(buf);
		}
		//去重
		if (i != 0 && sortedR[i][0] == sortedR[i - 1][0])
		{
			continue;
		}
		*(blk + blkcount) = sortedR[i][0];
		cout << sortedR[i][0] << endl;
		blkcount++;
		if (blkcount==15)
		{
			*(blk + blkcount) = addr + 1;
			writeBlockToDisk((unsigned char *)blk, addr++, buf);
			blkcount = 0;
		}
	}
}

vector<vector<int>> Sort(vector<vector<int>> Table, int index)
{
	for (int i = 0; i < Table.size(); i++)
	{
		for (int j = i; j < Table.size(); j++)
		{
			if (Table[i][index] > Table[j][index] )
			{
				swap(Table[i], Table[j]);
			}
		}
	}
	return Table;
}

void BinarySearch(Buffer *buf)
{
	int i;
	unsigned int *disk_blk;
	int addr = 200;

	vector<vector<int>> sortedR = Sort(R, 0);
	int index = 0, target = 40;
	int low = 0, high = 112;
	while (low <= high) {
		int mid = (high + low) / 2;
		if (sortedR[mid][index] == target)
		{
			cout << mid << sortedR[mid][index] << endl;
			int dl = mid, ul = mid;
			while (dl>-1 && sortedR[dl][index] == target)
			{
				dl--;
			}
			while (ul<sortedR.size() && sortedR[ul][index] == target)
			{
				ul++;
			}
			for (int i = dl + 1; i < ul; i++)
			{
				disk_blk = (unsigned int *)getNewBlockInBuffer(buf);
				*disk_blk = sortedR[i][0];
				*(disk_blk + 1) = sortedR[i][1];
				cout << "(" << sortedR[i][0] << "," << sortedR[i][1] << ")\t";
				writeBlockToDisk((unsigned char *)disk_blk, addr++, buf);
			}
			break;
		}
		else if (sortedR[mid][index]<target)
		{
			low = mid + 1;
		}
		else
		{
			high = mid - 1;
		}
	}

	vector<vector<int>> sortedS = Sort(S, 0);
	index = 0; target = 60;
	low = 0; high = sortedS.size();
	while (low <= high) {
		int mid = (high + low) / 2;
		if (sortedS[mid][index] == target)
		{
			cout << mid << sortedS[mid][index] << endl;
			int dl = mid, ul = mid;
			while (dl>-1 && sortedS[dl][index] == target)
			{
				dl--;
			}
			while (ul<sortedS.size() && sortedS[ul][index] == target)
			{
				ul++;
			}
			for (int i = dl + 1; i < ul; i++)
			{
				disk_blk = (unsigned int *)getNewBlockInBuffer(buf);
				*disk_blk = sortedS[i][0];
				*(disk_blk + 1) = sortedS[i][1];
				cout << "(" << sortedS[i][0] << "," << sortedS[i][1] << ")\t";
				writeBlockToDisk((unsigned char *)disk_blk, addr++, buf);
			}
			break;
		}
		else if (sortedS[mid][index]<target)
		{
			low = mid + 1;
		}
		else
		{
			high = mid - 1;
		}
	}
	return;
}

void LineSearch(Buffer *buf)
{
	int i;
	unsigned int *disk_blk;
	int addr = 100;
	for (i = 0; i < R.size(); i++)
	{
		if (R[i][0] == 40)
		{
			disk_blk = (unsigned int *)getNewBlockInBuffer(buf);
			*disk_blk = R[i][0];
			*(disk_blk + 1) = R[i][1];
			cout << "(" << R[i][0] << "," << R[i][1] << ")\t";
			writeBlockToDisk((unsigned char *)disk_blk, addr++, buf);
		}
	}

	for (i = 0; i < S.size(); i++)
	{
		if (S[i][0] == 60)
		{
			disk_blk = (unsigned int *)getNewBlockInBuffer(buf);
			*disk_blk = S[i][0];
			*(disk_blk + 1) = S[i][1];
			cout << "(" << S[i][0] << "," << S[i][1] << ")\t";
			writeBlockToDisk((unsigned char *)disk_blk, addr++, buf);
		}
	}
	cout << endl;
}

void HashSearch(Buffer *buf)
{
	map<int, vector<int>> HR;
	map<int, vector<int>> HS;
	int i = 0, j = 0, count = 0, flag = 0, addr = 300, w_count = 0;
	unsigned int * wrblk = (unsigned int *)getNewBlockInBuffer(buf); //存结果
	for (int i = 0; i < R.size(); i++)
	{
		HR[R[i][0]].push_back(R[i][1]);
	}
	for (int i = 0; i < S.size(); i++)
	{
		HS[S[i][0]].push_back(S[i][1]);
	}
	HR[40];
	HS[60];
	for (int i = 0; i < HR[40].size(); i++)
	{
		cout << "(" << 40 << "," << HR[40][i] << ")" << endl;

		*(wrblk + w_count) = 40;
		*(wrblk + w_count + 1) = HR[40][i];
		w_count += 2;
		if (w_count == 15)
		{
			*(wrblk + w_count) = addr + 1;
			writeBlockToDisk((unsigned char *)wrblk, addr++, buf);
			w_count = 0;
		}
	}
	for (int i = 0; i < HS[60].size(); i++)
	{
		cout << "(" << 60 << "," << HS[60][i] << ")" << endl;
		*(wrblk + w_count) = 60;
		*(wrblk + w_count + 1) = HS[60][i];
		w_count += 2;
		if (w_count == 15)
		{
			*(wrblk + w_count) = addr + 1;
			writeBlockToDisk((unsigned char *)wrblk, addr++, buf);
			w_count = 0;
		}
	}
	if (w_count != 15)
	{
		writeBlockToDisk((unsigned char *)wrblk, addr++, buf);
	}
}


void NestLoopJoin(unsigned int * blk, Buffer *buf)
{
	unsigned int *wrblk = NULL;
	unsigned int a, b, c, d;
	int addr = 700;
	int count = 0, blkcount = 0;
	for (int i = 0; i < R.size(); i++)
	{
		for (int j = 0; j < S.size(); j++)
		{
			if (R[i][0] == S[j][0])
			{
				a = R[i][0];
				b = R[i][1];
				d = S[j][1];
				cout << "(" << R[i][0] << "," << R[i][1] << ") ";
				cout << "(" << S[j][0] << "," << S[j][1] << ") \t";
				if (blkcount == 0)
				{
					wrblk = (unsigned int *)getNewBlockInBuffer(buf);
				}
				*(wrblk + blkcount) = a;
				*(wrblk + blkcount + 1) = b;
				*(wrblk + blkcount + 2) = d;
				count++;
				blkcount += 3;
				if (blkcount == 15)
				{
					*(wrblk + blkcount) = addr + 1;
					writeBlockToDisk((unsigned char *)wrblk, addr++, buf);
					blkcount = 0;
				}
			}
		}
	}
	if (blkcount != 0)
	{
		writeBlockToDisk((unsigned char *)wrblk, addr++, buf);
	}

	cout << "\n一共：" << count << "对" << endl;
}

void SortMergeJoin(unsigned int * blk, Buffer *buf)
{
	int i = 0, j = 0, count = 0, flag = 0, addr = 1000, w_count = 0;
	blk = (unsigned int *)getNewBlockInBuffer(buf); //存结果
	vector<vector<int>> sortedR = Sort(R, 0);
	vector<vector<int>> sortedS = Sort(S, 0);
	while (i < Rnums && j < Snums)
	{
		if (sortedR[i][0] == sortedS[j][0])
		{
			printf("(%d,%d,%d) ", sortedS[j][0], sortedR[i][1], sortedS[j][1]);
			*(blk + w_count) = sortedS[j][0];
			*(blk + w_count + 1) = sortedR[i][1];
			*(blk + w_count + 2) = sortedS[j][1];
			w_count += 3;
			if (sortedR[i-1][0] < sortedR[i][0] || i == 0) flag = i;
			count++;
			if (count % 6 == 0) printf("\n");
			if (i == 111) //边缘特殊处理，因找不到比i最后一个还大的,j不能自增
			{
				i = flag;
				j++;
			}
			else i++;
		}
		else if (sortedR[i][0] < sortedS[j][0]) i++;
		else
		{
			j++;
			if (sortedS[j][0] == sortedS[j-1][0])  i = flag;
		}
		if (w_count == 15) //够一块存储
		{
			*(blk + w_count) = addr + 1;
			writeBlockToDisk((unsigned char *)blk, addr++, buf);
			w_count = 0;
		}
	}
	if (w_count != 0) //最后不满一块存储
	{
		*(blk + w_count) = addr + 1;
		writeBlockToDisk((unsigned char *)blk, addr++, buf);
	}
	cout << "\n一共：" << count << "对" << endl;
	return;
}

void HashJoin(unsigned int * blk, Buffer *buf)
{
	map<int, vector<int>> HR;
	map<int, vector<int>> HS;
	int i = 0, j = 0, count = 0, flag = 0, addr = 1300, w_count = 0;
	blk = (unsigned int *)getNewBlockInBuffer(buf); //存结果
	for (int i = 0; i < R.size(); i++)
	{
		HR[R[i][0]].push_back(R[i][1]);
	}
	for (int i = 0; i < S.size(); i++)
	{
		HS[S[i][0]].push_back(S[i][1]);
	}
	for (int i = 20; i <= 40 ; i++)
	{
		HS[i];
		HR[i];
		for (int si = 0; si < HS[i].size(); si++)
		{
			for (int ri = 0; ri < HR[i].size(); ri++)
			{
				printf("(%d,%d,%d) ", i,HR[i][ri],HS[i][si]);
				count++;
				*(blk + w_count) = i;
				*(blk + w_count + 1) = HR[i][ri];
				*(blk + w_count + 2) = HS[i][si];
				w_count += 3;
				if (w_count == 15) //够一块存储
				{
					*(blk + w_count) = addr + 1;
					writeBlockToDisk((unsigned char *)blk, addr++, buf);
					w_count = 0;
				}
			}
		}
	}
	if (w_count != 0) //最后不满一块存储
	{
		*(blk + w_count) = addr + 1;
		writeBlockToDisk((unsigned char *)blk, addr++, buf);
	}
	cout << "\n一共：" << count << "对" << endl;
	return;
}


void Union(unsigned int * blk, Buffer *buf)
{
	int addr = 2500, blkcount = 0, count = 0;
	vector<vector<int>> sortedR = Sort(R, 0);
	vector<vector<int>> sortedS = Sort(S, 0);
	blk = (unsigned int *)getNewBlockInBuffer(buf); //存结果
	for (int i = 0; i < sortedR.size(); i++)
	{
		for (int j = 0; j < sortedS.size(); j++)
		{
			if (sortedR[i][0] == sortedS[j][0] && sortedR[i][1] == sortedS[j][1])
			{
				break;
			}
			if (j == sortedS.size() - 1)
			{
				if (blkcount == 0)
				{
					blk = (unsigned int *)getNewBlockInBuffer(buf);
				}
				//cout << i <<"\t"<< j << endl;;
				cout << "(" << sortedR[i][0] << "," << sortedR[i][1] << ") ";
				blkcount++;
				count++;
				if (blkcount == 15)
				{
					*(blk + blkcount) = addr + 1;
					writeBlockToDisk((unsigned char *)blk, addr++, buf);
					blkcount = 0;
				}
			}
		}
	}
	for (int j = 0; j < sortedS.size(); j++)
	{
		if (blkcount == 0)
		{
			blk = (unsigned int *)getNewBlockInBuffer(buf);
		}
		//cout << i <<"\t"<< j << endl;;
		cout << "(" << sortedS[j][0] << "," << sortedS[j][1] << ") ";
		blkcount++;
		count++;
		if (blkcount == 15)
		{
			*(blk + blkcount) = addr + 1;
			writeBlockToDisk((unsigned char *)blk, addr++, buf);
			blkcount = 0;
		}
	}
	if (blkcount != 15)
	{
		writeBlockToDisk((unsigned char *)blk, addr++, buf);
	}
	cout << "\n一共：" << count << "对" << endl;

}

void Intersection(unsigned int * blk, Buffer *buf)
{
	int addr = 2200, blkcount = 0, count = 0;
	vector<vector<int>> sortedR = Sort(R, 0);
	vector<vector<int>> sortedS = Sort(S, 0);	
	blk = (unsigned int *)getNewBlockInBuffer(buf); //存结果
	for (int i = 0; i < sortedR.size(); i++)
	{
		for (int j = 0; j < sortedS.size(); j++)
		{
			if (sortedR[i][0] == sortedS[j][0] && sortedR[i][1] == sortedS[j][1])
			{
				if (blkcount == 0)
				{
					blk = (unsigned int *)getNewBlockInBuffer(buf);
				}
				//cout << i <<"\t"<< j << endl;;
				cout << "(" << sortedR[i][0] << "," << sortedR[i][1] << ") ";
				blkcount++;
				count++;
				if (blkcount == 15)
				{
					*(blk + blkcount) = addr + 1;
					writeBlockToDisk((unsigned char *)blk, addr++, buf);
					blkcount = 0;
				}
			}
		}
	}
	if (blkcount != 15)
	{
		writeBlockToDisk((unsigned char *)blk, addr++, buf);
	}
	cout << "\n一共：" << count << "对" << endl;
	return;
}

void DiffRtoS(unsigned int * blk, Buffer *buf)
{
	int addr = 2400, blkcount = 0, count=0;
	vector<vector<int>> sortedR = Sort(R, 0);
	vector<vector<int>> sortedS = Sort(S, 0);
	blk = (unsigned int *)getNewBlockInBuffer(buf); //存结果
	for (int i = 0; i < sortedR.size(); i++)
	{
		for (int j = 0; j < sortedS.size(); j++)
		{
			if (sortedR[i][0] == sortedS[j][0] && sortedR[i][1] == sortedS[j][1])
			{
				break;
			}
			if (j == sortedS.size()-1)
			{
				if (blkcount == 0)
				{
					blk = (unsigned int *)getNewBlockInBuffer(buf);
				}
				//cout << i <<"\t"<< j << endl;;
				cout << "(" << sortedR[i][0] << "," << sortedR[i][1] << ") ";
				blkcount++;
				count++;
				if (blkcount == 15)
				{
					*(blk + blkcount) = addr + 1;
					writeBlockToDisk((unsigned char *)blk, addr++, buf);
					blkcount = 0;
				}
			}
		}
	}
	if (blkcount != 15)
	{
		writeBlockToDisk((unsigned char *)blk, addr++, buf);
	}
	cout << "\n一共：" << count << "对" << endl;
}

void DiffStoR(unsigned int * blk, Buffer *buf)
{
	int addr = 2400, blkcount = 0, count = 0;
	vector<vector<int>> sortedR = Sort(R, 0);
	vector<vector<int>> sortedS = Sort(S, 0);
	blk = (unsigned int *)getNewBlockInBuffer(buf); //存结果
	for (int i = 0; i <sortedS.size(); i++)
	{
		for (int j = 0; j < sortedR.size(); j++)
		{
			if (sortedR[j][0] == sortedS[i][0] && sortedR[j][1] == sortedS[i][1])
			{
				break;
			}
			if (j == sortedR.size() - 1)
			{
				if (blkcount == 0)
				{
					blk = (unsigned int *)getNewBlockInBuffer(buf);
				}
				//cout << i <<"\t"<< j << endl;;
				cout << "(" << sortedS[i][0] << "," << sortedS[i][1] << ") ";
				blkcount++;
				count++;
				if (blkcount == 15)
				{
					*(blk + blkcount) = addr + 1;
					writeBlockToDisk((unsigned char *)blk, addr++, buf);
					blkcount = 0;
				}
			}
		}
	}
	if (blkcount != 15)
	{
		writeBlockToDisk((unsigned char *)blk, addr++, buf);
	}
	cout << "\n一共：" << count << "对" << endl;
}