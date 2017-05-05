
#include <intrins.h>

#include "STC15W4K60S4.H"
#include "NRF24L01.H"
#include "CONSTANT.H"

//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5   	// 5 uints TX address width
#define RX_ADR_WIDTH    5   	// 5 uints RX address width
#define TX_PLOAD_WIDTH  _TX_LENGTH  	// 20 uints TX payload
#define RX_PLOAD_WIDTH  _RX_LENGTH  	// 20 uints TX payload
u8 code TX_ADDRESS[TX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};	//���ص�ַ
u8 code RX_ADDRESS[RX_ADR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};	//���յ�ַ

//***************************************NRF24L01�Ĵ���ָ��*******************************************************
#define READ_REG        0x00  	// ���Ĵ���ָ��
#define WRITE_REG       0x20 	// д�Ĵ���ָ��
#define RD_RX_PLOAD     0x61  	// ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0  	// д��������ָ��
#define FLUSH_TX        0xE1 	// ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2  	// ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3  	// �����ظ�װ������ָ��
#define NOP             0xFF  	// ����
//*************************************SPI(nRF24L01)�Ĵ�����ַ****************************************************
#define CONFIG2         0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ��������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//*****************************************����ʱ*****************************************
void Delay(unsigned int s)
{
	volatile unsigned int i;
	for (i = 0; i < s; i++);
	for (i = 0; i < s; i++);
}
//******************************************************************************************
u8 		bdata sta;   //״̬��־
sbit	RX_DR	= sta ^ 6;
sbit	TX_DS	= sta ^ 5;
sbit	MAX_RT	= sta ^ 4;
sbit    TX_FULL = sta ^ 0;
/******************************************************************************************
/*��ʱ����
/******************************************************************************************/
void inerDelay_us(u8 n)
{
	for (; n > 0; n--);
}
//****************************************************************************************
/*NRF24L01��ʼ��
//***************************************************************************************/
void init_NRF24L01(void)
{
	CE = 0;  // chip enable
	CSN = 1; // Spi  disable
	SCK = 0; //
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      //  Ƶ��0�Զ�	ACKӦ������
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  //  �������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21
	SPI_RW_Reg(WRITE_REG + RF_CH, 0x6e);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x27);   		//���÷�������Ϊ1MB/S�����书��Ϊ���ֵ+7dB��������X2401L���ţ�ʵ��+21dbm���
}
/****************************************************************************************************
/*������u8 SPI_RW(u8 uchar)
/*���ܣ�NRF24L01��SPIдʱ��
/****************************************************************************************************/
u8 SPI_RW(u8 dat)
{
	B = dat;
	P_24L01_MOSI = B7;	_nop_();	P_24L01_SCK = 1;	B7 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = B6;	_nop_();	P_24L01_SCK = 1;	B6 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = B5;	_nop_();	P_24L01_SCK = 1;	B5 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = B4;	_nop_();	P_24L01_SCK = 1;	B4 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = B3;	_nop_();	P_24L01_SCK = 1;	B3 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = B2;	_nop_();	P_24L01_SCK = 1;	B2 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = B1;	_nop_();	P_24L01_SCK = 1;	B1 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = B0;	_nop_();	P_24L01_SCK = 1;	B0 = P_24L01_MISO;	P_24L01_SCK = 0;
	P_24L01_MOSI = 0;
	return (B);           		 // return read byte
}

/****************************************************************************************************
/*������uchar SPI_Read(uchar reg)
/*���ܣ�NRF24L01��SPIʱ��
/****************************************************************************************************/

u8 SPI_Read(u8 reg)
{
	u8 reg_val;

	CSN = 0;                // CSN low, initialize SPI communication...
	inerDelay_us(6);
	SPI_RW(reg);            // Select register to read from..
	inerDelay_us(6);
	reg_val = SPI_RW(0);    // ..then read registervalue
	inerDelay_us(6);
	CSN = 1;                // CSN high, terminate SPI communication

	return (reg_val);       // return register value
}


/****************************************************************************************************/
/*���ܣ�NRF24L01��д�Ĵ�������
/****************************************************************************************************/
u8 SPI_RW_Reg(u8 reg, u8 value)
{
	u8 status;

	CSN = 0;                   // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register
	SPI_RW(value);             // ..and write value to it..
	CSN = 1;                   // CSN high again

	return (status);           // return nRF24L01 status
}
/****************************************************************************************************/
/*������u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
/*����: ���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���
/****************************************************************************************************/
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status, uchar_ctr;

	CSN = 0;                    		// Set CSN low, init SPI tranaction
	status = SPI_RW(reg);       		// Select register to write to and read status

	for (uchar_ctr = 0; uchar_ctr < uchars; uchar_ctr++)
		pBuf[uchar_ctr] = SPI_RW(0);    //

	CSN = 1;

	return (status);                   // return nRF24L01 status
}


/*********************************************************************************************************
/*������u8 SPI_Write_Buf(uchar reg, uchar *pBuf, uchar uchars)
/*����: ����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���
/*********************************************************************************************************/
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status, uchar_ctr;

	CSN = 0;            //SPIʹ��
	status = SPI_RW(reg);
	for (uchar_ctr = 0; uchar_ctr < uchars; uchar_ctr++) //
		SPI_RW(*pBuf++);
	CSN = 1;           //�ر�SPI
	return (status);   //
}

/*********************************************************************************************************
/*������int getTxDS()
/*����: �����ͳɹ���TX_DS������Ϊ1
/*********************************************************************************************************/
int getTxDS() {
	return TX_DS ;
}

/*********************************************************************************************************
/*������int getRxDR()
/*����: �����ճɹ���Rx_DR������Ϊ1
/*********************************************************************************************************/
int getRxDR() {
	return RX_DR ;
}

/*********************************************************************************************************
/*������void nrf_RxTx(uchar mod_nrf, uchar *buff)
/*����: ʵ��α˫����ͬʱ�շ�
/*********************************************************************************************************/
void nrf_RxTx(uchar mod_nrf, uchar *buff)
{
	static uchar mod_nrf_b;

	if (mod_nrf == 't')
	{
		if (mod_nrf_b != 't')
		{
			mod_nrf_b = 't';
			CE = 0;
			SPI_RW_Reg(WRITE_REG + STATUS, 0xff);
			SPI_RW_Reg(FLUSH_TX, 0x00);
			SPI_RW_Reg(WRITE_REG + CONFIG2, 0x5e);
			CE = 1;
			inerDelay_us(75);
		}

		CE = 0;
		SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);
		SPI_Write_Buf(WR_TX_PLOAD, buff, TX_PLOAD_WIDTH);
		CE = 1;
		inerDelay_us(75);

		//inerDelay_us(100);
		sta = SPI_Read(STATUS);
		SPI_RW_Reg(WRITE_REG + STATUS, sta);

		if (TX_DS == 1)
		{
			CE = 0;
			SPI_RW_Reg(FLUSH_TX, 0x00);
			CE = 1;
		}

	}
	else if (mod_nrf == 'r') //����ģʽ
	{
		if (mod_nrf_b != 'r')
		{
			mod_nrf_b = 'r';
			CE = 0;
			SPI_RW_Reg(WRITE_REG + STATUS, 0xff);
			SPI_RW_Reg(FLUSH_RX, 0x00);
			SPI_RW_Reg(WRITE_REG + CONFIG2, 0x0f);
			CE = 1;
			inerDelay_us(75);
		}
		inerDelay_us(250);
		sta = SPI_Read(STATUS);
		SPI_RW_Reg(WRITE_REG + STATUS, sta);
		if (RX_DR == 1)
		{
			CE = 0;
			SPI_Read_Buf(RD_RX_PLOAD, buff, RX_PLOAD_WIDTH);
			SPI_RW_Reg(FLUSH_RX, 0x00);
			CE = 1;
		}
	}
}

/****************************************************************************************************/
/*������void SetRX_Mode(void)
/*���ܣ����ݽ�������
/****************************************************************************************************/
void SetRX_Mode(void)
{
	CE = 0;
	SPI_RW_Reg(WRITE_REG + CONFIG2, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	CE = 1;
}
/******************************************************************************************************/
/*������u8 nRF24L01_RxPacket(u8* rx_buf)
/*���ܣ����ݶ�ȡ�����rx_buf���ջ�������
/******************************************************************************************************/

u8 nRF24L01_RxPacket(u8* rx_buf)
{
	u8 revale = 0;
	sta = SPI_Read(STATUS);	// ��ȡ״̬�Ĵ������ж����ݽ���״��
	if (RX_DR)				// �ж��Ƿ���յ�����
	{
		CE = 0; 			//SPIʹ��
		SPI_Read_Buf(RD_RX_PLOAD, rx_buf, TX_PLOAD_WIDTH); // read receive payload from RX_FIFO buffer
		revale = 1;			//��ȡ������ɱ�־
	}
	SPI_RW_Reg(WRITE_REG + STATUS, sta); //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
	CE = 1;
	return revale;
}
