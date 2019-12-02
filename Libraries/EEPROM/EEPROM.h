#ifndef __EEPROM_H
#define __EEPROM_H

#include <stddef.h>
#include <stdint.h>
#include <wirish.h>

#define EEPROM_I2C   0					//Defines the type of External EEPROM via I2C
#define EEPROM_INT   1					//Defines the type of Internal EEPROM
#define EEPROM_FLH   2					//Defines the type of External FLASH
#define EEPROM_SPI   3					//Defines the type of External EEPROM via SPI

//#define EEPROM_TYPE_ENABLE EEPROM_INT	//Defines the type of EEPROM used
//#define EEPROM_TYPE_ENABLE EEPROM_I2C	//Defines the type of EEPROM used
//#define EEPROM_TYPE_ENABLE EEPROM_FLH	//Defines the type of EEPROM used
#define EEPROM_TYPE_ENABLE EEPROM_SPI	//Defines the type of EEPROM used

#if EEPROM_TYPE_ENABLE == EEPROM_I2C


	#define MC24C64		//Defines the EEPROM MC24C64
	#define EEPROM_ADDRESS	0x50

	#ifndef EEPROM_PAGE_SIZE
		#if defined (MC24C64)
			#define EEPROM_PAGE_SIZE	(uint32_t)0x10000
		#else
			#error	"No EEPROM type specified."
		#endif
	#endif

	#ifndef EEPROM_START_ADDRESS
		#if defined (MC24C64)
			#define EEPROM_START_ADDRESS	0x00
		#else
			#error	"No EEPROM type specified.."
		#endif
	#endif

	class EEPROMClass
	{
	public:
		uint16 Status;

		EEPROMClass(void);
		EEPROMClass(HardwareI2C *i2c_d);
		uint16 init(HardwareI2C *i2c_d);
		uint16 format(void);
		uint16 read(uint16 address);
		uint16 read(uint16 address, uint16 *data);
		uint16 write(uint16 address, uint16 data);

		void eeprom_read_block(void *pointer_ram, const void *pointer_eeprom, size_t n);
		void eeprom_write_block(const void *pointer_ram, void *pointer_eeprom, size_t n);

		uint8_t eeprom_read_byte(const uint8_t *addr);
		uint16_t eeprom_write_byte(uint8_t *addr, uint8_t value);

		uint16_t eeprom_read_word(const uint16_t *addr);
		void eeprom_write_word(uint16_t *addr, uint16_t value);

		uint32_t eeprom_read_dword(const uint32_t *addr);
		void eeprom_write_dword(uint32_t *addr, uint32_t value);

	private:
		static HardwareI2C *_I2Cx;
	};

#elif EEPROM_TYPE_ENABLE == EEPROM_INT

#elif EEPROM_TYPE_ENABLE == EEPROM_FLH

#elif EEPROM_TYPE_ENABLE == EEPROM_SPI

#include "SPIEEP.h"

#define EEPROM_PAGE_SIZE SPIEEPROM_PAGE_SIZE
#define EEPROM_START_ADDRESS SPIEEPROM_START_ADDRESS

	class EEPROMClass : public SPIEEP {
	public:
		uint16 Status;

		EEPROMClass(void);
		EEPROMClass(HardwareSPI *spi_d, int cspin);
		uint16 init(HardwareSPI *spi_d, int cspin);

		virtual void eeprom_read_block(void *pointer_ram, const void *pointer_eeprom, size_t n);
		virtual void eeprom_write_block(const void *pointer_ram, void *pointer_eeprom, size_t n);

		virtual uint8_t eeprom_read_byte(const uint8_t *addr);
		virtual uint16_t eeprom_write_byte(uint8_t *addr, uint8_t value);

		virtual uint16_t eeprom_read_word(const uint16_t *addr);
		virtual void eeprom_write_word(uint16_t *addr, uint16_t value);

		virtual uint32_t eeprom_read_dword(const uint32_t *addr);
		virtual void eeprom_write_dword(uint32_t *addr, uint32_t value);
	};

#endif

extern EEPROMClass EEPROM;

//static functions - access to utilities to emulate EEPROM

//extern void eeprom_read_block(void *pointer_ram, const void *pointer_eeprom, size_t n);
//extern void eeprom_write_block(const void *pointer_ram, void *pointer_eeprom, size_t n);
//
//extern uint8_t eeprom_read_byte(const uint8_t *addr);
//extern uint16_t eeprom_write_byte(uint8_t *addr, uint8_t value);
//
//extern uint16_t eeprom_read_word(const uint16_t *addr);
//extern void eeprom_write_word(uint16_t *addr, uint16_t value);
//
//extern uint32_t eeprom_read_dword(const uint32_t *addr);
//extern void eeprom_write_dword(uint32_t *addr, uint32_t value);

#endif	/* __EEPROM_H */
