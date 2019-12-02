#include <EEPROM.h>
#include <FastSerial.h>

FastSerialPort2(Serial);        // FTDI/console

int ledPin =  13;    // LED connected to digital pin 13
#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
const char HELP_MSG[] = "Press :\r\n" \
						" 0 display configuration\r\n" \
						" 1 set configuration to 0x801F000 / 0x801F800 / 0x400 (RB MCU)\r\n" \
						" 2 set configuration to 0x801F000 / 0x801F800 / 0x800 (ZE/RE MCU)\r\n" \
						" 3 write/read variable\r\n" \
						" 4 increment address\r\n" \
						" 5 display pages\r\n" \
						" 6 initialize EEPROM\r\n" \
						" 7 format EEPROM\r\n" \
						" 8 Erase EEPROM\r\n";
#elif EEPROM_TYPE_ENABLE == EEPROM_I2C
const char HELP_MSG[] = "Press :\r\n" \
						" 0 NULL\r\n" \
						" 1 NULL\r\n" \
						" 2 Dump EEPROM\r\n" \
						" 3 write/read variable\r\n" \
						" 4 increment address\r\n" \
						" 5 display EEPROM\r\n" \
						" 6 Read EEPROM block\r\n" \
						" 7 format EEPROM\r\n" \
						" 8 Write EEPROM block\r\n";
#endif

uint16 DataWrite = 0;
uint16 AddressWrite = 0x10;

struct EEPROM_header
{
	uint16_t    magic;
	uint8_t     revision;
	uint8_t     spare;
};

static const uint16_t	k_EEPROM_magic		= 0x5041;	///< "AP"
static const uint16_t	k_EEPROM_revision	= 2;		///< current format revision

void setup()
{
	Serial.begin(115200, 128, 128);
#ifdef EEPROM_DEBUG_ENABLE
	eeprom_set_serial_for_debug(&Serial);
#endif

#if EEPROM_TYPE_ENABLE == EEPROM_I2C
	i2c_master_enable(I2C2, 0);
#endif

	// initialize the digital pin as an output:
	pinMode(ledPin, OUTPUT);
	Serial.print(HELP_MSG);
}

void loop()
{
	uint16 Status;
	uint16 Data;

	while (Serial.available())
	{
		char cmd = (char)Serial.read();
		Serial.println(cmd);
		if (cmd == '0')
		{
#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
			DisplayConfig();
#elif EEPROM_TYPE_ENABLE == EEPROM_I2C
			Serial.print(HELP_MSG);
#endif
		}
		else if (cmd == '1')
		{
#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
			EEPROM.PageBase0 = 0x801F000;
			EEPROM.PageBase1 = 0x801F800;
			EEPROM.PageSize  = 0x400;
			DisplayConfig();
#elif EEPROM_TYPE_ENABLE == EEPROM_I2C
			Serial.print(HELP_MSG);
#endif
		}
		else if (cmd == '2')
		{
#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
			EEPROM.PageBase0 = 0x801F000;
			EEPROM.PageBase1 = 0x801F800;
			EEPROM.PageSize  = 0x800;
			DisplayConfig();
#elif EEPROM_TYPE_ENABLE == EEPROM_I2C
		Serial.println();
		for (int i = 0; i < 0x10000; i += 32)
		{
			uint8_t buff[32];
			eeprom_read_block(buff, (const void *)i, sizeof(buff));
			for (int j = 0; j < sizeof(buff); j++)
				Serial.printf_P(PSTR("%02x"), buff[j]);
		}
		Serial.println();
#endif
		}
		else if (cmd == '3')
		{
			Status = EEPROM.write(AddressWrite, DataWrite);
			Serial.print("EEPROM.write(0x");
			Serial.print(AddressWrite, HEX);
			Serial.print(", 0x");
			Serial.print(DataWrite, HEX);
			Serial.print(") : Status : ");
			Serial.println(Status, HEX);
			Status = EEPROM.read(AddressWrite, &Data);
			Serial.print("EEPROM.read(0x");
			Serial.print(AddressWrite, HEX);
			Serial.print(", &..) = 0x");
			Serial.print(Data, HEX);
			Serial.print(" : Status : ");
			Serial.println(Status, HEX);
			++DataWrite;
		}
		else if (cmd == '4')
		{
			++AddressWrite;
		}
		else if (cmd == '5')
		{
			DisplayPages(0x10000);
			// DisplayPages(0x20);
			//DisplayPagesEnd(0x20);
		}
		else if (cmd == '6')
		{
#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
			Status = EEPROM.init();
			Serial.print("EEPROM.init() : ");
			Serial.println(Status, HEX);
			Serial.println();
#elif EEPROM_TYPE_ENABLE == EEPROM_I2C
			struct EEPROM_header	ee_header;
			uint16_t				eeprom_address;

			eeprom_address = 0;
			eeprom_read_block(&ee_header, (void *)eeprom_address, sizeof(ee_header));
			if ((ee_header.magic != k_EEPROM_magic) || (ee_header.revision != k_EEPROM_revision))
			{
				Serial.printf_P(PSTR("no header, magic 0x%02x revision 0x%02x"), ee_header.magic, ee_header.revision);
			}
			else
			{
				Serial.printf_P(PSTR("header ok, magic 0x%02x revision 0x%02x"), ee_header.magic, ee_header.revision);
			}
#endif
		}
		else if (cmd == '7')
		{
			Status = EEPROM.format();
			Serial.print("EEPROM.format() : ");
			Serial.println(Status, HEX);
		}
		else if (cmd == '8')
		{
#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
			byte b = 0;
			Serial.printf_P(PSTR("\nErasing EEPROM\n"));
			uint16_t res = 0;
			for (int i = 0; i < 2 * EEPROM_PAGE_SIZE; i++)
			{
				Serial.printf_P(PSTR("ADDR: %d... "), i);
				res = eeprom_write_byte((uint8_t *) i, b);
				Serial.printf_P(PSTR("%d\n"), res);
			}
			Serial.printf_P(PSTR("\ndone\n"));
#elif EEPROM_TYPE_ENABLE == EEPROM_I2C
			uint8_t buff[32];
			memset(buff, 0xFF, sizeof(buff));
			Serial.println("Write start...");
			for (int i = 0; i < 0x10000; i += 32)
			{
				Serial.printf_P(PSTR("%04x - %04x - "), i, i + 32 - 1);
				for (int j = 0; j < sizeof(buff); j++)
					Serial.printf_P(PSTR("%02x"), buff[j]);
				Serial.println();
				eeprom_write_block(buff, (void *)i, sizeof(buff));
			}
			Serial.println("Write end!");
#endif
		}
		else
			Serial.print(HELP_MSG);
	}
	digitalWrite(ledPin, HIGH);
	delay(500);
	digitalWrite(ledPin, LOW);
	delay(500);
}

#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
void DisplayConfig(void)
{
	Serial.print  ("EEPROM.PageBase0 : 0x");
	Serial.println((int32)  EEPROM.PageBase0, HEX);
	Serial.print  ("EEPROM.PageBase1 : 0x");
	Serial.println((int32) EEPROM.PageBase1, HEX);
	Serial.print  ("EEPROM.PageSize  : 0x");
	Serial.print  ((int32) EEPROM.PageSize, HEX);
	Serial.print  (" (");
	Serial.print  ((int32) EEPROM.PageSize, DEC);
	Serial.println(")");
}
#endif

void DisplayHex(uint16 value)
{
	if (value <= 0xF)
		Serial.print("000");
	else if (value <= 0xFF)
		Serial.print("00");
	else if (value <= 0xFFF)
		Serial.print("0");
	Serial.print(value, HEX);
}

void DisplayPages(uint32 endIndex)
{
#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
	Serial.println("Page 0     Top         Page 1");

	for (uint32 idx = 0; idx < endIndex; idx += 4)
	{
		Serial.print  ((int32) (EEPROM.PageBase0 + idx), HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		Serial.print  ("    ");
		Serial.print  ((int32) (EEPROM.PageBase1 + idx), HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		Serial.println();
	}
#elif EEPROM_TYPE_ENABLE == EEPROM_I2C
	for (int i = 0; i < endIndex; i += 32)
	{
		uint8_t buff[32];
		eeprom_read_block(buff, (const void *)i, sizeof(buff));
		Serial.printf_P(PSTR("%04x - %04x - "), i, i + 32 - 1);
		for (int j = 0; j < sizeof(buff); j++)
			Serial.printf_P(PSTR("%02x"), buff[j]);
		Serial.println();
	}
#endif
}

#if EEPROM_TYPE_ENABLE == EEPROM_FLASH
void DisplayPagesEnd(uint32 endIndex)
{
	Serial.println("Page 0     Bottom      Page 1");

	for (uint32 idx = EEPROM.PageSize - endIndex; idx < EEPROM.PageSize; idx += 4)
	{
		Serial.print  ((int32) (EEPROM.PageBase0 + idx), HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase0 + idx + 2));
		Serial.print  ("    ");
		Serial.print  ((int32) (EEPROM.PageBase1 + idx), HEX);
		Serial.print  (" : ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx));
		Serial.print  (" ");
		DisplayHex(*(uint16*)(EEPROM.PageBase1 + idx + 2));
		Serial.println();
	}
}
#endif
