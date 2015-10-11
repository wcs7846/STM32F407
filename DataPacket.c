#include "DataPacket.h"

#define n 8
//������ʵ�ֵĴ���������ֻ�Ǽ򵥴���û�ж����ݽ���ѹ��
//NeedPackΪ��Ҫ������������
//PackedΪ����õ�����
void DataPack(uint8_t* NeedPack, uint8_t* Packed)
{
	/*
		���ݽṹ���壺
		Packed[0]--70;
		Packed[1]--Second;
		Packed[2]--Miniture;
		Packed[3]--Hours;
	*/
	Packed[0]=70;
	//
	Packed[1]=NeedPack[0];
	Packed[2]=NeedPack[1];
	Packed[3]=NeedPack[2];
};

//�ȶ�������ݰ��������ݱ任�Ժ�Ž������ݵĽ��
void DataUnpack(uint8_t* Packed, uint8_t* PData)
{
	uint8_t temp[6];
	//���ݱ任
	DataConversion(Packed,temp);
	//
	if(temp[0]==70)
	{
		PData[0]=temp[1];
		PData[1]=temp[2];
		PData[2]=temp[3];
	}
};

//���ڽ��ܵ������ݰ��ĵ�һλ��һ����70��������Ҫȷ����һλ��λ��
//���ݰ�������Ϊ6
void DataConversion(uint8_t* Packed, uint8_t* temp)
{
	uint8_t i=0;	//�����ʼλ
	uint8_t j=0;
	//uint8_t temp[6];
	uint8_t Start=70;	//�Լ���������ݰ���ʼ
	
	//i��0��ʼ
	for(i=0;i<4;)
	{
		if(Start==Packed[i])
			break;
		i++;	//i�õ�����ֵΪ��ʼλ��λ��
	}
	//�����ݴ����ݴ������
	for(j=0;j<4;j++)
	{
		if((i+j)<4)
			temp[j]=Packed[i+j];
		else
			temp[j]=Packed[i+j-4];
	}
	//Packed=&temp[0];
};

//����������ȷ�������г��ִ���������
uint8_t JudgeData(uint8_t* NeedJudge)
{
	uint8_t i,j;
	uint8_t max;
	uint8_t times[n]={0};
	//����������һ����5��Ԫ��
	for(i=0;i<n-1;i++)
	{
		for(j=i+1;j<n;j++)
		{
			if(NeedJudge[i]==NeedJudge[j])
				times[i]++;
		}
	}
	//
	max=times[0];
	for(i=1;i<n;i++)
	{
		if(times[i]>max)
			max=times[i];
	}
	//
	for(i=0;i<n;i++)
	{
		if(times[i]==max)
			return NeedJudge[i];
	}		
};
	
	
