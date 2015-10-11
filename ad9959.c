#include "ad9959.h"

#define n 5

void AD9959_Init(void)
{
	//uint8_t RegisterData[4] = {0,0,0,0};
		
	AD9959_RST = 0;
	Delay_us(n);
	AD9959_RST = 1;
	Delay_us(n);
	AD9959_RST = 0;
	HAL_Delay(100);
	//��ʼ��	
	AD9959ChannelEnable(CH0);//ʹ��ͨ��CH1
	AD9959ClockConfig();		 //ʱ������
	//
	
};

void delay(uint16_t i)
{
	i*=600;
	for(;i>0;i--);
};

void AD9959_Write_SPI(uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
	uint8_t ControlValue = 0;
	uint8_t ValueToWrite = 0;
	signed char RegisterIndex = 0;
	uint8_t i=0,j=0;
	
	//Create the 8-bit header
	ControlValue = RegisterAddress;
	
	AD9959_CLK = 0;
	Delay_us(n);
	AD9959_CS  = 1;
	Delay_us(n);
	AD9959_CS  = 0;
	Delay_us(n);
	
	SDA_OUT ;
	//write out the control word
	for(i=0;i<8;i++)
	{
		AD9959_CLK = 0;
		if((ControlValue & 0x80) == 0x80)
			SDIO0_OUT = 1;
		else 
			SDIO0_OUT = 0;
		Delay_us(n);
		AD9959_CLK = 1;
		Delay_us(n);
		ControlValue<<=1;
	}
	AD9959_CLK = 0;

	Delay_us(n+2);
	//And then the data
	for(RegisterIndex = NumberofRegisters; RegisterIndex>0; RegisterIndex--)
	{
		ValueToWrite = *(RegisterData + RegisterIndex - 1 );
		for(j=0; j<8; j++)
		{
			AD9959_CLK = 0;
			if((ValueToWrite & 0x80) == 0x80)
				SDIO0_OUT = 1;
			else 
				SDIO0_OUT = 0;
		Delay_us(n);
		AD9959_CLK = 1;
		Delay_us(n);
		ValueToWrite <<= 1;
		}
	}
	AD9959_CLK = 0;
	Delay_us(n);
	AD9959_CS = 1;
};

void AD9959_Read_SPI (uint8_t RegisterAddress, uint8_t NumberofRegisters, uint8_t *RegisterData)
{
	unsigned	char	ControlValue = 0;
	signed		char	RegisterIndex = 0;
	unsigned	char	ReceiveData = 0;
	unsigned	char	i = 0;
	unsigned	int		iTemp = 0;

	//Create the 8-bit header
	ControlValue = RegisterAddress;

	AD9959_CLK = 1;
	Delay_us(n);
	AD9959_CS  = 1;
	Delay_us(n);
	AD9959_CS  = 0;
	Delay_us(n);
	
	SDA_OUT ;	//Make SDIO an output

	//Write out the control word
	for(i=0; i<8; i++)
	{
		AD9959_CLK = 0;
		if(0x80 == (ControlValue & 0x80))
			SDIO0_OUT = 1;	  //Send one to SDIO pin
		else
			SDIO0_OUT = 0;
		Delay_us(n);
		AD9959_CLK = 1;
		Delay_us(n);
		ControlValue <<= 1;	//Rotate data
	}

	SDA_IN ;	//Make SDA an input
	Delay_us(n+2);
	//Read data in
	for (RegisterIndex=NumberofRegisters; RegisterIndex>0; RegisterIndex--)
	{
		for(i=0; i<8; i++)
		{
			AD9959_CLK = 0;
			ReceiveData <<= 1;		//Rotate data
			iTemp = SDIO0_IN;			//Read SDIO of AD9959
			if(0x00000001 == (iTemp & 0x00000001))
			{
				ReceiveData |= 1;	
			}
			Delay_us(n);
			AD9959_CLK = 1;
			Delay_us(n);
		}
		*(RegisterData + RegisterIndex - 1) = ReceiveData;
	}
	Delay_us(n);
	AD9959_CS = 1;	//bring CS high again
} 

void IO_Update(void)
{
	AD9959_IOUP = 0;
	Delay_us(n+1);
	AD9959_IOUP = 1;
	Delay_us(n+7);
	AD9959_IOUP = 0;
	Delay_us(n+1);	
}

void RST(void)
{
	AD9959_RST = 0;
	Delay_us(n+4);
	AD9959_RST = 1;
	Delay_us(n+9);
	AD9959_RST = 0;
	Delay_us(n+4);
};
/*----------------------------------------------------------------------------*/
//��������
//���ڽ��������õ�
/*----------------------------------------------------------------------------*/
//AD9959ChannelChoice:ѡ��ͨ���������
//Channel���������:����һ����8λ����ÿһλ����һ��ͨ���ֱ�Ϊ
//Channel��һλ--CH0
//Channel�ڶ�λ--CH1
//Channel����λ--CH2
//Channel����λ--CH3
//���ڲ����������ֻ��һ�������������Բ���IO_Update
/*----------------------------------------------------------------------------*/
void AD9959ChannelEnable(uint8_t Channel)
{
	uint8_t RegisterData[4] = {0,0,0,0};
	uint8_t temp=0;
	//
	if((Channel&0x01)==0x01)
		temp += 0x10;
	if((Channel&0x02)==0x02)
		temp += 0x20;
	if((Channel&0x04)==0x04)
		temp += 0x40;
	if((Channel&0x08))
			temp += 0x80;
	//
	RegisterData[0] = temp;		//Channel 1 enable
	AD9959_Write_SPI(0x00,1,RegisterData);
};
/*----------------------------------------------------------------------------*/
//AD9959ClockConfig:����ʱ��
//REFCLOCK:50MHz -- Multiplication factor = 10
//VCO gain control = 1, system clock = 500MHz
/*----------------------------------------------------------------------------*/
void AD9959ClockConfig(void)
{
	uint8_t RegisterData[4] = {0,0,0,0};
	//
	RegisterData[2] = 0xD0;		//REF CLK = 50MHz, Multiplication factor = 20--D0
	RegisterData[1] = 0x00;
	RegisterData[0] = 0x00;
	AD9959_Write_SPI(0x01,3,RegisterData);//VCO gain control = 1, system clock = 500MHz
	RegisterData[2] = 0x00;		
	RegisterData[1] = 0x03;
	RegisterData[0] = 0x00;
	AD9959_Write_SPI(0x03,3,RegisterData);
};
/*----------------------------------------------------------------------------*/
//AD9959Test:���һ��5MHz�����Ҳ�
/*----------------------------------------------------------------------------*/
void AD9959Test(void)
{
	AD9959SendConfirmed(5000000);
	
	//IO_Update();
};
/*----------------------------------------------------------------------------*/
//AD9959SendConfirmed:������Ҫ����Ĺ̶�Ƶ�ʵ����Ҳ���Ƶ��ֵ����
//frequence��Ϊʮ���Ƶ�Ƶ��ֵҪ��Ӧ�Ļ���
/*----------------------------------------------------------------------------*/
void AD9959SendConfirmed(uint64_t frequence)
{
	uint8_t RegisterData[4] = {0,0,0,0};
	//�м����
	double temp;
	//����Ƶ�ʵĶ�Ӧ�ļĴ���ֵ
	//���ֵΪ4294967296--250MHz
	//85899345----5MHz-5-1e-b8-51
	//66-66-66-54
	//28-f5-c2-88:40Mhz
	//���㲿��--4294967296/250000000=17.179869184
	temp =(frequence * FRE_REF);
	//���뵽�Ĵ���
	RegisterData[3] = ((uint32_t)(temp)&0xff000000)>>24;
	RegisterData[2] = ((uint32_t)(temp)&0x00ff0000)>>16;
	RegisterData[1] = ((uint32_t)(temp)&0x0000ff00)>>8;
	RegisterData[0] = ((uint32_t)(temp)&0x000000ff);
	AD9959_Write_SPI(0x04,4,RegisterData);//Output frequency = 10MHz 
		
	IO_Update();
};
/*----------------------------------------------------------------------------*/
//AD9959FrequenceSweepConfig:����ɨƵ����
//frequence��Ϊʮ���Ƶ�Ƶ��ֵҪ��Ӧ�Ļ���
/*----------------------------------------------------------------------------*/
void AD9959FrequenceSweepConfig(uint8_t Channel,uint32_t FreS,uint32_t FreE,double FTstep,double RTstep,uint32_t FFstep,uint32_t RFstep,uint8_t DWELL)
{
	uint8_t RegisterData[4] = {0,0,0,0};
	uint32_t temp;
	//����ͨ��
	AD9959ChannelEnable(Channel);
	//���÷���
	RegisterData[2] = 0x00;
	RegisterData[1] = 0x03;
	RegisterData[0] = 0xff;
	AD9959_Write_SPI(0x06,3,RegisterData);
	//����ʱ��
	AD9959ClockConfig();
	//����ѭ��
	if(DWELL)
	{
		RegisterData[2] = 0x80;
		RegisterData[1] = 0xc3;
		RegisterData[0] = 0x00;
		AD9959_Write_SPI(0x03,3,RegisterData);
	}else
	{
		RegisterData[2] = 0x80;
		RegisterData[1] = 0x43;
		RegisterData[0] = 0x14;
		AD9959_Write_SPI(0x03,3,RegisterData);
	}
	//������ʼƵ��
	temp =(FreS * FRE_REF);
	RegisterData[3] = ((uint32_t)(temp)&0xff000000)>>24;
	RegisterData[2] = ((uint32_t)(temp)&0x00ff0000)>>16;
	RegisterData[1] = ((uint32_t)(temp)&0x0000ff00)>>8;
	RegisterData[0] = ((uint32_t)(temp)&0x000000ff);
	AD9959_Write_SPI(0x04,4,RegisterData);
	//���ý�ֹƵ��
	temp =(FreE * FRE_REF);
	RegisterData[3] = ((uint32_t)(temp)&0xff000000)>>24;
	RegisterData[2] = ((uint32_t)(temp)&0x00ff0000)>>16;
	RegisterData[1] = ((uint32_t)(temp)&0x0000ff00)>>8;
	RegisterData[0] = ((uint32_t)(temp)&0x000000ff);
	AD9959_Write_SPI(0x0A,4,RegisterData);
	//����ʱ����������
	temp =(RFstep * FRE_REF);
	RegisterData[3] = ((uint32_t)(temp)&0xff000000)>>24;
	RegisterData[2] = ((uint32_t)(temp)&0x00ff0000)>>16;
	RegisterData[1] = ((uint32_t)(temp)&0x0000ff00)>>8;
	RegisterData[0] = ((uint32_t)(temp)&0x000000ff);
	AD9959_Write_SPI(0x08,4,RegisterData);
	//����Ƶ����������
	temp =(FFstep * FRE_REF);
	RegisterData[3] = ((uint32_t)(temp)&0xff000000)>>24;
	RegisterData[2] = ((uint32_t)(temp)&0x00ff0000)>>16;
	RegisterData[1] = ((uint32_t)(temp)&0x0000ff00)>>8;
	RegisterData[0] = ((uint32_t)(temp)&0x000000ff);
	AD9959_Write_SPI(0x09,4,RegisterData);
	//�����������½��ٶ�
	temp = ((((uint16_t)(FTstep*0.4)<<8))|((uint16_t)(RTstep*0.4)));
	RegisterData[1] = ((uint32_t)(temp)&0x0000ff00)>>8;
	RegisterData[0] = ((uint32_t)(temp)&0x000000ff);
	AD9959_Write_SPI(0x07,2,RegisterData);
	
	IO_Update();
};

void AD9959_Modulation_Off(uint8_t Channel)
{
	uint8_t RegisterData[4] = {0,0,0,0};
	//����ͨ��
	AD9959ChannelEnable(Channel);
	//�ر�����ͨ���ĵ���ģʽ
	RegisterData[2] = 0x00;
	RegisterData[1] = 0x03;
	RegisterData[0] = 0x14;
	AD9959_Write_SPI(0x03,3,RegisterData);
	
	IO_Update();
}

void AD9959_Sweep_Trigger(uint8_t Channel, uint32_t delay)
{
	if((Channel & CH0)==CH0) P0 = 1;
	if((Channel & CH1)==CH1) P1 = 1;
	if((Channel & CH2)==CH2) P2 = 1;
	if((Channel & CH3)==CH3) P3 = 1;
	Delay_us(delay);
	if((Channel & CH0)==CH0) P0 = 0;
	if((Channel & CH1)==CH1) P1 = 0;
	if((Channel & CH2)==CH2) P2 = 0;
	if((Channel & CH3)==CH3) P3 = 0;
	Delay_us(delay);
}