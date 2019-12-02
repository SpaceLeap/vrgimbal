#include "EEPROM.h"

#if EEPROM_TYPE_ENABLE == EEPROM_I2C

HardwareI2C *EEPROMClass::_I2Cx;

EEPROMClass::EEPROMClass(void)
{

}

EEPROMClass::EEPROMClass(HardwareI2C *i2c_d)
{
	_I2Cx = i2c_d;
	this->init(_I2Cx);
}

uint16 EEPROMClass::init(HardwareI2C *i2c_d)
{
	_I2Cx = i2c_d;

	Status = 0x0000;
	return Status;
}


uint16 EEPROMClass::format(void)
{
	for (unsigned int i = 0; i < EEPROM_PAGE_SIZE; i++)
	{
		write(EEPROM_START_ADDRESS + i, 0xFF);
	}

	return 0x0000;
}


uint16 EEPROMClass::read(uint16 Address)
{
	uint16 data;
	read(Address, &data);
	return data;
}

uint16 EEPROMClass::read(uint16 Address, uint16 *Data)
{
	uint8_t rdata[10];

	int8_t xret = _I2Cx->read(EEPROM_ADDRESS, (uint16_t)Address, (uint8_t)1, (uint8_t *)rdata);
	*Data = (uint16)rdata[0];

	if (xret != 0)
		return 0x0001;
	else
		return 0x0000;

}

uint16 EEPROMClass::write(uint16 Address, uint16 Data)
{
	int8_t xret = _I2Cx->write(EEPROM_ADDRESS, (uint16_t)Address, (uint8_t)Data);
	delay(5);

	if (xret != 0)
		return 0x0001;
	else
		return 0x0000;
}

//static functions - access to utilities to emulate EEPROM
void EEPROMClass::eeprom_read_block (void *pointer_ram, const void *pointer_eeprom, size_t n)
{
	uint8_t * buff = (uint8_t *)pointer_ram;
	uint16 addr16 = (uint16)(uint32)pointer_eeprom;
	for (uint16 i = 0; i < (uint16)n; i++) 
	{
		buff[i] = (uint8_t)read(addr16 + i);
	}
}

void EEPROMClass::eeprom_write_block (const void *pointer_ram, void *pointer_eeprom, size_t n)
{
	uint8_t * buff = (uint8_t *)pointer_ram;
	uint16 addr16 = (uint16)(uint32)pointer_eeprom;

	for (uint16 i = 0; i < (uint16)n; i++) 
	{
		write(addr16 + i, (uint16) buff[i] );
	}
}

uint8_t EEPROMClass::eeprom_read_byte (const uint8_t *addr)
{
	uint16 addr16 = (uint16)(uint32)addr;
	return (uint8_t)read(addr16);
}

uint16_t EEPROMClass::eeprom_write_byte (uint8_t *addr, uint8_t value)
{
	uint16 addr16 = (uint16)(uint32)addr;
	return write(addr16, (uint16) value );
}

uint16_t EEPROMClass::eeprom_read_word (const uint16_t *addr)
{
	uint16_t val = 0;
	eeprom_read_block(&val, addr, sizeof(val));
	return val;
}

void EEPROMClass::eeprom_write_word (uint16_t *addr, uint16_t value)
{
	uint16_t val = value;
	eeprom_write_block(&val, addr, sizeof(val));
}

uint32_t EEPROMClass::eeprom_read_dword (const uint32_t *addr)
{
	uint32_t val = 0;
	eeprom_read_block(&val, addr, sizeof(val));
	return val;
}

void EEPROMClass::eeprom_write_dword (uint32_t *addr, uint32_t value)
{
	uint32_t val = value;
	eeprom_write_block(&val, addr, sizeof(val));
}

EEPROMClass EEPROM;

#elif EEPROM_TYPE_ENABLE == EEPROM_INT

#elif EEPROM_TYPE_ENABLE == EEPROM_FLH

#elif EEPROM_TYPE_ENABLE == EEPROM_SPI

EEPROMClass::EEPROMClass(void)
{

}

EEPROMClass::EEPROMClass(HardwareSPI *spi_d, int cspin) :
	SPIEEP(spi_d, cspin)
{
}

uint16 EEPROMClass::init(HardwareSPI *spi_d, int cspin)
{
	SPIEEP::init(spi_d, cspin);
	Status = 0x0000;
	return Status;
}

//static functions - access to utilities to emulate EEPROM
void EEPROMClass::eeprom_read_block (void *pointer_ram, const void *pointer_eeprom, size_t n)
{
	uint32 addr32 = (uint32)pointer_eeprom;
	readn(addr32, (byte *)pointer_ram, (uint16_t) n);
}

void EEPROMClass::eeprom_write_block (const void *pointer_ram, void *pointer_eeprom, size_t n)
{
	uint32 addr32 = (uint32)pointer_eeprom;
	writen(addr32, (byte *)pointer_ram, (uint16_t) n);
}

uint8_t EEPROMClass::eeprom_read_byte (const uint8_t *addr)
{
	uint32 addr32 = (uint32)addr;
	return (uint8_t)read(addr32);
}

uint16_t EEPROMClass::eeprom_write_byte (uint8_t *addr, uint8_t value)
{
	uint32 addr32 = (uint32)addr;
	if (write(addr32, (byte) value ))
		return 1;
	return 0;
}

uint16_t EEPROMClass::eeprom_read_word (const uint16_t *addr)
{
	uint16_t val = 0;
	eeprom_read_block(&val, addr, sizeof(val));
	return val;
}

void EEPROMClass::eeprom_write_word (uint16_t *addr, uint16_t value)
{
	uint16_t val = value;
	eeprom_write_block(&val, addr, sizeof(val));
}

uint32_t EEPROMClass::eeprom_read_dword (const uint32_t *addr)
{
	uint32_t val = 0;
	eeprom_read_block(&val, addr, sizeof(val));
	return val;
}

void EEPROMClass::eeprom_write_dword (uint32_t *addr, uint32_t value)
{
	uint32_t val = value;
	eeprom_write_block(&val, addr, sizeof(val));
}

EEPROMClass EEPROM;

#endif

