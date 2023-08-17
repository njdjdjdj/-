

#include "FLSH.h"


void Write_flsh_byte(uint32_t EEPROM_ADD, uint32_t EEPROM_Data)
{
	//1������FLASH
	HAL_FLASH_Unlock();
	//2������FLASH
	//��ʼ��FLASH_EraseInitTypeDef
	FLASH_EraseInitTypeDef f;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.PageAddress = EEPROM_ADD;
	f.NbPages = 1;
	//����PageError
	uint32_t PageError = 0;
	//���ò�������
	HAL_FLASHEx_Erase(&f, &PageError);
	//3����FLASH��д
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, EEPROM_ADD, EEPROM_Data);
	//4����סFLASH
	HAL_FLASH_Lock();
}

//FLASH��ȡ���ݲ���
uint32_t readFlash(uint32_t EEPROM_ADD)
{
	uint32_t temp = *(__IO uint32_t*)(EEPROM_ADD);
	return temp;
}