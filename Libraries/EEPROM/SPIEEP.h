/* Copyright (C) 2011 by Eric Brundick <spirilis@linux.com>
 * Distributed under the terms of the GNU Lesser General Public License
 * A copy of this license may be obtained at:
 * http://www.gnu.org/copyleft/lesser.html
 */


/* SPIEEP - A library to interface with Microchip 25LC512 SPI-accessible EEPROM chips.
 * Generalized to allow different address widths (multiple of 8 bits) along with different
 *   page sizes and capacities, so long as the instruction set works the same way.
 *
 * Utilizes Arduino's SPI library for its communication and works within the Arduino
 * framework (utilizing digitalWrite(), along with the 'byte' datatype)
 */

/* Example pinout for the 25LC512 chip:
                                           ---v---
             Arduino digital pin 10 <-  CS |     | Vcc (+2.5-5.5V)
  Arduino digital pin 12 (MISO) <-  SerOUT |     | HOLD -> Connected to Vcc (not used)
        Connected to Vcc (not used) <-  WP |     | SCK -> Arduino digital pin 13 (SCK)
                                 Vss (GND) |     | SerIN -> Arduino digital pin 11 (MOSI)
                                           -------

 * Note: WP is used for write protection, see the datasheet for details:
 *   http://ww1.microchip.com/downloads/en/DeviceDoc/22065C.pdf
 * HOLD is used to pause I/O mid-stream without losing place in the command sequence
 *   in case you have a higher-priority interrupt to handle. Not something I see myself using.
 */

#ifndef _SPIEEP_H_INCLUDED
#define _SPIEEP_H_INCLUDED

#include <wirish.h>


//#define AT25010		//Defines the EEPROM AT25010
//#define AT25020		//Defines the EEPROM AT25020
#define AT25040		//Defines the EEPROM AT25040


#if defined (AT25010)
	#define AT250xx
	#define SPIEEPROM_PAGE_SIZE		(int) 128
	#define SPIEEPROM_TOTAL_SIZE		(int) 128
	#define SPIEEPROM_ADDR_WIDTH		(int) 8
#elif defined (AT25020)
	#define AT250xx
	#define SPIEEPROM_PAGE_SIZE		(int) 256
	#define SPIEEPROM_TOTAL_SIZE		(int) 256
	#define SPIEEPROM_ADDR_WIDTH		(int) 8
#elif defined (AT25040)
	#define AT250xx
	#define SPIEEPROM_PAGE_SIZE		(int) 512
	#define SPIEEPROM_TOTAL_SIZE		(int) 512
	#define SPIEEPROM_ADDR_WIDTH		(int) 9
#else
#error	"No SPIEEPROM type specified."
#endif

#if ((defined (AT25010)) || (defined (AT25020)) || (defined (AT25040)))
#define SPIEEPROM_START_ADDRESS	0
#else
#error	"No SPIEEPROM type specified.."
#endif

class SPIEEP {
  private:

	HardwareSPI * _SPIx;
	bool _csActiveLow;
    uint8_t _cspin;
    uint16_t _pagesize;
    uint16_t _numpages;
    uint32_t _highestaddr;
    uint8_t _addrwidth;
    boolean _is_25LC040;
    boolean _write_validation();
    void _write_address(uint32_t);
    inline byte _compose_instruction_addrbit(uint32_t, byte);

    void _csEnable(bool enable);

  public:
    SPIEEP();
    SPIEEP(HardwareSPI * spi_dev,
    	   int cspin, bool csActiveLow = true,
    	   int addrwidth = SPIEEPROM_ADDR_WIDTH,
    	   int pagesize = SPIEEPROM_PAGE_SIZE,
    	   int totalsize = SPIEEPROM_TOTAL_SIZE);
	virtual ~SPIEEP();

    void init(HardwareSPI * spi_dev,
    			int cspin, bool csActiveLow = true,
    			int addrwidth = SPIEEPROM_ADDR_WIDTH,
    			int pagesize = SPIEEPROM_PAGE_SIZE,
    			int totalsize = SPIEEPROM_TOTAL_SIZE);
    //void begin(int cspin);
    //void begin_spi(int cspin);
    //void end();
    boolean verify_enabled();
    int pagesize();

    void wren();
    boolean is_wren();
    void wrdi();
    void dpd();
    byte rdid();
    void sleepmode();  // Alias of dpd()
    byte wakeup();     // Alias of rdid()
    byte readstatus();
    void writestatus(byte);
    boolean test_chip();  /* Run a bunch of tests on the chip
                           * to validate functionality
                           */

    /* byte-oriented functions (these are the core I/O functions) */
    byte read(uint32_t);
    boolean readn(uint32_t, byte *, uint16_t);
    boolean write(uint32_t, byte);
    boolean writen(uint32_t, byte *, uint16_t);
    boolean read_page(uint32_t, byte *);
    boolean write_page(uint32_t, byte *);

    /* char-casting versions of these functions */
    boolean readn(uint32_t, char *, uint16_t);
    boolean write(uint32_t, char);
    boolean writen(uint32_t, char *, uint16_t);
    boolean read_page(uint32_t, char *);
    boolean write_page(uint32_t, char *);

    /* Integer variants which address-translate */
    int read_int(uint32_t);
    boolean readn_int(uint32_t, int *, uint16_t);
    boolean write_int(uint32_t, int);
    boolean writen_int(uint32_t, int *, uint16_t);
    /* Offset variants - allow the use of an offset to adjust the origin address for
     *   all computed addresses (to facilitate partitioning of the memory among
     *   heterogeneous data types
     */
    int read_int_offset(uint32_t, uint32_t);
    boolean readn_int_offset(uint32_t, uint32_t, int *, uint16_t);
    boolean write_int_offset(uint32_t, uint32_t, int);
    boolean writen_int_offset(uint32_t, uint32_t, int *, uint16_t);

    /* Long-integer variants which address-translate */
    long read_long(uint32_t);
    boolean readn_long(uint32_t, long *, uint16_t);
    boolean write_long(uint32_t, long);
    boolean writen_long(uint32_t, long *, uint16_t);
    /* Offset variants */
    long read_long_offset(uint32_t, uint32_t);
    boolean readn_long_offset(uint32_t, uint32_t, long *, uint16_t);
    boolean write_long_offset(uint32_t, uint32_t, long);
    boolean writen_long_offset(uint32_t, uint32_t, long *, uint16_t);

    /* Float variants which address-translate */
    float read_float(uint32_t);
    boolean readn_float(uint32_t, float *, uint16_t);
    boolean write_float(uint32_t, float);
    boolean writen_float(uint32_t, float *, uint16_t);
    /* Offset variants */
    float read_float_offset(uint32_t, uint32_t);
    boolean readn_float_offset(uint32_t, uint32_t, float *, uint16_t);
    boolean write_float_offset(uint32_t, uint32_t, float);
    boolean writen_float_offset(uint32_t, uint32_t, float *, uint16_t);

    /* Double-wide float variants which address-translate */
    double read_double(uint32_t);
    boolean readn_double(uint32_t, double *, uint16_t);
    boolean write_double(uint32_t, double);
    boolean writen_double(uint32_t, double *, uint16_t);
    /* Offset variants */
    double read_double_offset(uint32_t, uint32_t);
    boolean readn_double_offset(uint32_t, uint32_t, double *, uint16_t);
    boolean write_double_offset(uint32_t, uint32_t, double);
    boolean writen_double_offset(uint32_t, uint32_t, double *, uint16_t);

    boolean chip_erase();
    boolean page_erase(uint32_t);

    /****************************************************/
    //Dichiarate per compatibilità

    virtual void eeprom_read_block(void *pointer_ram, const void *pointer_eeprom, size_t n) = 0;
    virtual void eeprom_write_block(const void *pointer_ram, void *pointer_eeprom, size_t n) = 0;

	virtual uint8_t eeprom_read_byte(const uint8_t *addr) = 0;
	virtual uint16_t eeprom_write_byte(uint8_t *addr, uint8_t value) = 0;

	virtual uint16_t eeprom_read_word(const uint16_t *addr) = 0;
	virtual void eeprom_write_word(uint16_t *addr, uint16_t value) = 0;

	virtual uint32_t eeprom_read_dword(const uint32_t *addr) = 0;
	virtual void eeprom_write_dword(uint32_t *addr, uint32_t value) = 0;
};

#endif
