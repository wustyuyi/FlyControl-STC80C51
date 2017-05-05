
#include <intrins.h>
#include "STC15W4K60S4.H"
#include "EEPROM.h"
#include "NRF24L01.H"
#include "MPU6050.H"

#include "CONSTANT.H"

#define CMD_IDLE    0
#define CMD_READ    1
#define CMD_PROGRAM 2
#define CMD_ERASE   3

#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
extern int   xdata g_x, g_y, g_z;		//�����ǽ�������
extern float xdata a_x, a_y;			//�ǶȽ�������
extern float data Angle, Angley;

u8	xdata	tmp[20];

//========================================================================
// ����: void	DisableEEPROM(void)
// ����: ��ֹ����ISP/IAP.
// ����: non.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void	DisableEEPROM(void)
{
	IAP_CONTR = 0;			//��ֹISP/IAP����
	IAP_CMD   = 0;			//ȥ��ISP/IAP����
	IAP_TRIG  = 0;			//��ֹISP/IAP�����󴥷�
	IAP_ADDRH = 0xff;		//��0��ַ���ֽ�
	IAP_ADDRL = 0xff;		//��0��ַ���ֽڣ�ָ���EEPROM������ֹ�����
}

//========================================================================
// ����: void EEPROM_read_n(u16 EE_address,u8 *DataAddress,u16 number)
// ����: ��ָ��EEPROM�׵�ַ����n���ֽڷ�ָ���Ļ���.
// ����: EE_address:  ����EEPROM���׵�ַ.
//       DataAddress: �������ݷŻ�����׵�ַ.
//       number:      �������ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================

void EEPROM_read_n(u16 EE_address, u8 *DataAddress, u16 number)
{
	F0 = EA;
	EA = 0;		//��ֹ�ж�
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD   = CMD_READ;
	do
	{
		IAP_ADDRH = EE_address / 256;		//�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
		IAP_ADDRL = EE_address % 256;		//�͵�ַ���ֽ�
		IAP_TRIG  = 0x5a;	IAP_TRIG  = 0xa5;
		_nop_();
		*DataAddress = IAP_DATA;			//��������������
		EE_address++;
		DataAddress++;
	} while (--number);

	DisableEEPROM();
	EA = F0;		//���������ж�
}


/******************** ������������ *****************/
//========================================================================
// ����: void EEPROM_SectorErase(u16 EE_address)
// ����: ��ָ����ַ��EEPROM��������.
// ����: EE_address:  Ҫ����������EEPROM�ĵ�ַ.
// ����: non.
// �汾: V1.0, 2013-5-10
//========================================================================
void EEPROM_SectorErase(u16 EE_address)
{
	F0 = EA;
	EA = 0;		//��ֹ�ж�
	IAP_ADDRH = EE_address / 256;			//��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
	IAP_ADDRL = EE_address % 256;			//��������ַ���ֽ�
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD   = CMD_ERASE;
	IAP_TRIG  = 0x5a;	IAP_TRIG  = 0xa5;
	_nop_();
	DisableEEPROM();
	EA = F0;		//���������ж�
}

//========================================================================
// ����: void EEPROM_write_n(u16 EE_address,u8 *DataAddress,u16 number)
// ����: �ѻ����n���ֽ�д��ָ���׵�ַ��EEPROM.
// ����: EE_address:  д��EEPROM���׵�ַ.
//       DataAddress: д��Դ���ݵĻ�����׵�ַ.
//       number:      д����ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void EEPROM_write_n(u16 EE_address, u8 *DataAddress, u16 number)
{
	F0 = EA;
	EA = 0;		//��ֹ�ж�
	IAP_CONTR = ENABLE_IAP;
	IAP_CMD   = CMD_PROGRAM;
	do
	{
		IAP_ADDRH = EE_address / 256;		//�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
		IAP_ADDRL = EE_address % 256;		//�͵�ַ���ֽ�
		IAP_DATA  = *DataAddress;			//�����ݵ�IAP_DATA��ֻ�����ݸı�ʱ����������
		IAP_TRIG  = 0x5a;	IAP_TRIG  = 0xa5;
		_nop_();
		EE_address++;
		DataAddress++;
	} while (--number);

	DisableEEPROM();
	EA = F0;		//���������ж�
}

extern	u8	xdata	tp[];

void IAP_Gyro(void)   //������У׼
{
	u8	i, j;
	g_x = ((int *)&tp)[4];  //��ȡ����������
	g_y = ((int *)&tp)[5];
	g_z = ((int *)&tp)[6];

	((int *)&tmp)[0] = g_x;
	((int *)&tmp)[1] = g_y;
	((int *)&tmp)[2] = g_z;
	((int *)&tmp)[3] = 0x55aa;

	((float *)&tmp)[2] = Angle;
	((float *)&tmp)[3] = Angley;
	a_x = Angle;
	a_y = Angley;


	for (i = 0, j = 0; i < 16; i++)	j += tmp[i];
	j = ~j + 1;
	tmp[i] = j;

	EEPROM_SectorErase(_EF_PROM); //��������
	EEPROM_write_n(_EF_PROM, tmp, 17);
}

void IAPRead(void)		//��ȡ�����Ǿ���
{
	u8	i, j;

	EEPROM_read_n(_EF_PROM, tmp, 17);
	for (i = 0, j = 0; i < 17; i++)	j += tmp[i];

	if ((j == 0) && (((int *)&tmp)[3] == 0x55aa))		//�ж������Ƿ��Ѿ����¹������¹��Ŷ�ȡ����
	{
		g_x = ((int *)&tmp)[0];
		g_y = ((int *)&tmp)[1];
		g_z = ((int *)&tmp)[2];
		a_x = ((float *)&tmp)[2];
		a_y = ((float *)&tmp)[3];
	}
	else
	{
		g_x = 0;
		g_y = 0;
		g_z = 0;
		a_x = 0;
		a_y = 0;
	}

}
