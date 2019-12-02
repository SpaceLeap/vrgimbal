#include "HardwareI2C.h"
#include "wirish_time.h"

#define I2CDELAY 50
//#define DELAYI2C

HardwareI2C::HardwareI2C()
{
}

HardwareI2C::HardwareI2C(uint32_t i2c_num)
{
    switch (i2c_num) {
    case 1:
        this->i2c_d = _I2C1;
        //this->begin();
        break;
    case 2:
        this->i2c_d = _I2C2;
        //this->begin();
        break;
    default:
        assert_param(0);
        break;
    }
}

////////////// Public Methods ////////////////////////////////////////

void HardwareI2C::begin()
{
	/* set as master */
    if (this->i2c_d == _I2C1) afio_remap(AFIO_MAPR_I2C1_REMAP);
    i2cInit(this->i2c_d, I2C_400KHz_SPEED);
	delay(I2CDELAY);
}

void HardwareI2C::end()
{
}

void HardwareI2C::setSpeed(bool _fast)
{

}

/* WRITE ******************************************/
uint8_t HardwareI2C::write(uint8_t address, uint8_t registerAddress, uint8_t databyte)
{
	uint8_t rbuff[1];
	rbuff[0] = registerAddress;
	uint8_t dbuff[1];
	dbuff[0] = databyte;

	return i2cWriteReg(this->i2c_d, address, rbuff, sizeof(rbuff), dbuff, sizeof(dbuff));
}

uint8_t HardwareI2C::write(uint8_t address, uint16_t registerAddress, uint8_t databyte)
{
	uint8_t rbuff[2];
	rbuff[0] = (uint8_t)(registerAddress >> 8);
	rbuff[1] = (uint8_t)(registerAddress & 0xFF);
	uint8_t dbuff[1];
	dbuff[0] = databyte;

	return i2cWriteReg(this->i2c_d, address, rbuff, sizeof(rbuff), dbuff, sizeof(dbuff));
}

uint8_t HardwareI2C::write(uint8_t address, uint8_t registerAddress, uint8_t numberBytes, uint8_t *dataBuffer)
{
	uint8_t rbuff[1];
	rbuff[0] = registerAddress;

	return i2cWriteReg(this->i2c_d, address, rbuff, sizeof(rbuff), dataBuffer, numberBytes);
}

uint8_t HardwareI2C::write(uint8_t address, uint16_t registerAddress, uint8_t numberBytes, uint8_t *dataBuffer)
{
	uint8_t rbuff[2];
	rbuff[0] = (uint8_t)(registerAddress >> 8);
	rbuff[1] = (uint8_t)(registerAddress & 0xFF);

	return i2cWriteReg(this->i2c_d, address, rbuff, sizeof(rbuff), dataBuffer, numberBytes);
}

/* READ *******************************************/
uint8_t HardwareI2C::read(uint8_t address, uint8_t registerAddress, uint8_t numberBytes, uint8_t* dataBuffer)
{
	uint8_t rbuff[1];
	rbuff[0] = registerAddress;

	return i2cReadReg(this->i2c_d, address, rbuff, sizeof(rbuff), dataBuffer, numberBytes);
}

uint8_t HardwareI2C::read(uint8_t address, uint16_t registerAddress, uint8_t numberBytes, uint8_t *dataBuffer)
{
	uint8_t rbuff[2];
	rbuff[0] = (uint8_t)(registerAddress >> 8);
	rbuff[1] = (uint8_t)(registerAddress & 0xFF);

	return i2cReadReg(this->i2c_d, address, rbuff, sizeof(rbuff), dataBuffer, numberBytes);
}

int8_t HardwareI2C::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
{
    uint8_t b;
    uint8_t ret = readByte(devAddr, regAddr, &b); //, timeout);
    *data = b & (1 << bitNum);
    return ret;
}
//int8_t HardwareI2C::readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
//{
//	return 0;
//}
int8_t HardwareI2C::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
{
    // 01101001 read byte
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    //    010   masked
    //   -> 010 shifted
    uint8_t ret = 0;
    uint8_t b = 0;
    if ((ret = readByte(devAddr, regAddr, &b)) == 0) {

        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        b &= mask;
        b >>= (bitStart - length + 1);
        *data = b;
    }
    return ret; // count;
}
//int8_t HardwareI2C::readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
//{
//	return 0;
//}
int8_t HardwareI2C::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
{
	return read(devAddr, regAddr, 1, data);
}
//int8_t HardwareI2C::readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
//{
//	return 0;
//}
//int8_t HardwareI2C::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
//{
//	return 0;
//}
//int8_t HardwareI2C::readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data/* , uint16_t timeout=I2Cdev::readTimeout */)
//{
//	return 0;
//}

bool HardwareI2C::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data)
{
    uint8_t b;
    readByte(devAddr, regAddr, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    return writeByte(devAddr, regAddr, b);
}
//bool HardwareI2C::writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data)
//{
//	return false;
//}
bool HardwareI2C::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data)
{
    //      010 value to write
    // 76543210 bit numbers
    //    xxx   args: bitStart=4, length=3
    // 00011100 mask byte
    // 10101111 original value (sample)
    // 10100011 original & ~mask
    // 10101011 masked | value
    uint8_t b;
    if (readByte(devAddr, regAddr, &b) == 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
        return writeByte(devAddr, regAddr, b);
    } else {
        return false;
    }
}
//bool HardwareI2C::writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data)
//{
//	return false;
//}
bool HardwareI2C::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data)
{
	uint8_t ret = write(devAddr, regAddr, data);
	return (ret == 0);
}
bool HardwareI2C::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data)
{
	bool ret = writeByte(devAddr, regAddr, (data>>8)&0xFF);
	ret = ret & writeByte(devAddr, regAddr + 1, data&0xFF);

	return ret;
}
//bool HardwareI2C::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data)
//{
//	return false;
//}
//bool HardwareI2C::writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data)
//{
//	return false;
//}

