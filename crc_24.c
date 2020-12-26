#include "stdio.h"
#include "stdint.h"


#define TABLE_SIZE 		256
#define CRC_POLY 		0x00065B

uint32_t CRC_TABLE[TABLE_SIZE] = {0};

/*
// 6C E4 5D
#define CRC_INIT		0x15A78D
#define DATA_SIZE 		11
uint8_t Serial_Data[DATA_SIZE]={0x0A, 0x09, 0x05, 0x00, 0x04, 0x00, 0x07, 0x0E, 0x00, 0x18, 0x00};
*/

// B7 2D 25
#define CRC_INIT		0x5A09FF
#define DATA_SIZE 		11
uint8_t Serial_Data[DATA_SIZE]={0x0A, 0x09, 0x05, 0x00, 0x04, 0x00, 0x01, 0x08, 0x0F, 0x00, 0x0A};

uint8_t bit_ref_8(uint8_t data)
{
	uint8_t ref = 0;
	uint8_t i = 0;

	for (i = 0; i < 8; i++)
	{
		ref = (ref << 1) | ((data >> i) & 0x01);
	}

	return ref;
}

uint32_t bit_ref_24(uint32_t data)
{
	uint32_t ref = 0;
	uint8_t i = 0;

	for (i = 0; i < 24; i++)
	{
		ref = (ref << 1) | ((data >> i) & 0x01);
	}

	return ref;
}

uint32_t CRC24_Direct(int len, uint8_t *ptr, uint32_t CRC_Init)
{
	uint32_t crc24;
	int i;
	crc24 = CRC_Init;

	while(len--!=0)
	{
		crc24 ^= ((bit_ref_8(*ptr++))<<16);

		for(i = 0; i < 8; i++)
		{
			if (crc24 & 0x800000)
			{
				crc24 <<= 1;
				crc24 ^= CRC_POLY;
			}
			else 
			{
				crc24 <<= 1;
			}
		}
	}

	return bit_ref_24(crc24 & 0xFFFFFF);
}

uint32_t CRC24_DriveTable(int len, uint8_t *ptr, uint32_t CRC_Init)
{
	uint32_t CRC_Res = CRC_Init;

	while(len--)
	{
		CRC_Res = ((CRC_Res << 8) & 0xFFFFFF) ^ CRC_TABLE[((CRC_Res>>16)^bit_ref_8(*ptr++))&0xff];
	}
	return bit_ref_24(CRC_Res);
 
}

void CRC24_Table_Init()
{
	uint32_t i32, j32;
	uint32_t nData32;
	uint32_t CRC_Reg;

	for ( i32 = 0; i32 < 256; i32++ )
	{
		nData32 = ( uint32_t )( i32 << 16 );
		CRC_Reg = 0;
		for ( j32 = 0; j32 < 8; j32++ )
		{
			if ( ( nData32 ^ CRC_Reg ) & 0x800000 )
				CRC_Reg = ( CRC_Reg << 1 ) ^ CRC_POLY;
			else
				CRC_Reg <<= 1;
			nData32 <<= 1;
		}
		CRC_TABLE[i32] = CRC_Reg;
	}
}

int main()
{
	CRC24_Table_Init();

	printf("CRC24 Direct = 0x%06x\r\n", CRC24_Direct(DATA_SIZE, Serial_Data, CRC_INIT) & 0xFFFFFF);
	printf("CRC24 DriveTable = 0x%06x\r\n", CRC24_DriveTable(DATA_SIZE, Serial_Data, CRC_INIT) & 0xFFFFFF);

	return 0;
}


