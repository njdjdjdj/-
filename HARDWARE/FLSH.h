
#ifndef __FLSH__H
#define __FLSH__H	 
 #include "stm32f1xx_hal.h"
  
// STM32F103C8T6     һ�����Ǵ������ַ��ʼ 0x0800F000    Ȼ�������

void Write_flsh_byte(uint32_t EEPROM_ADD, uint32_t EEPROM_Data);
uint32_t readFlash(uint32_t EEPROM_ADD);

#endif




