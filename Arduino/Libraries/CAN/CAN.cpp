#include <SPI.h>
#include "Arduino.h"
#include <CAN.h>

CANClass CAN;//create can object

void CANClass::begin()//constructor for initializing can module.
{
	// set the slaveSelectPin as an output 
	pinMode (SCK,OUTPUT);
	pinMode (MISO,INPUT);
	pinMode (MOSI, OUTPUT);
	pinMode (SS, OUTPUT);
	//pinMode(RESET,OUTPUT);

	// initialize SPI:
	SPI.begin(); 
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(SPI_CLOCK_DIV4);
	SPI.setBitOrder(MSBFIRST);

    //digitalWrite(RESET,LOW); /* RESET CAN CONTROLLER*/
    //delay(10);
    //digitalWrite(RESET,HIGH);
    //delay(100);
    
    reset();
}

void CANClass::baudConfig(int bitRate)//sets bitrate for CAN node
{
	byte config0, config1, config2;

	switch (bitRate)
	{
case 10:
		config0 = 0x31;
		config1 = 0xB8;
		config2 = 0x05;
		break;

case 20:
		config0 = 0x18;
		config1 = 0xB8;
		config2 = 0x05;
		break;

case 50:
		config0 = 0x09;
		config1 = 0xB8;
		config2 = 0x05;
		break;

case 100:
		config0 = 0x04;
		config1 = 0xB8;
		config2 = 0x05;
		break;

case 125:
		config0 = 0x03;
		config1 = 0xB8;
		config2 = 0x05;
		break;

case 250:
		config0 = 0x01;
		config1 = 0xB8;
		config2 = 0x05;
		break;

case 500:
		config0 = 0x00;
		config1 = 0xB8;
		config2 = 0x05;
		break;
case 1000:
	//1 megabit mode added by Patrick Cruce(pcruce_at_igpp.ucla.edu)
	//Faster communications enabled by shortening bit timing phases(3 Tq. PS1 & 3 Tq. PS2) Note that this may exacerbate errors due to synchronization or arbitration.
	config0 = 0x80;
	config1 = 0x90;
	config2 = 0x02;
	}
	digitalWrite(SS, LOW);
	delay(10);
	SPI.transfer(WRITE);
	SPI.transfer(CNF0);
	SPI.transfer(config0);
	delay(10);
	digitalWrite(SS, HIGH);
	delay(10);

	digitalWrite(SS, LOW);
	delay(10);
	SPI.transfer(WRITE);
	SPI.transfer(CNF1);
	SPI.transfer(config1);
	delay(10);
	digitalWrite(SS, HIGH);
	delay(10);

	digitalWrite(SS, LOW);
	delay(10);
	SPI.transfer(WRITE);
	SPI.transfer(CNF2);
	SPI.transfer(config2);
	delay(10);
	digitalWrite(SS, HIGH);
	delay(10);
}

//Method added to enable testing in loopback mode.(pcruce_at_igpp.ucla.edu)
void CANClass::setMode(CANMode mode) { //put CAN controller in one of five modes

	byte writeVal,mask,readVal;

	switch(mode) {
  	case CAN_MODE_CONFIGURATION:
			writeVal = 0x80;
			break;
  	case CAN_MODE_NORMAL:
		  writeVal = 0x00;
			break;
  	case CAN_MODE_SLEEP:
			writeVal = 0x20;
	  	break;
    case CAN_MODE_LISTEN:
			writeVal = 0x60;
	  	break;
  	case CAN_MODE_LOOPBACK:
			writeVal = 0x40;
	  	break;
   }

	mask = 0xE0;

	digitalWrite(SS, LOW);
	SPI.transfer(BIT_MODIFY);
	SPI.transfer(CANCTRL);
	SPI.transfer(mask);
	SPI.transfer(writeVal);
	digitalWrite(SS, HIGH);

}


void CANClass::send_0()//transmits buffer 0
{

	//delays removed from SEND command(pcruce_at_igpp.ucla.edu)
	//In testing we found that any lost data was from PC<->Serial Delays,
	//Not CAN Controller/AVR delays.  Thus removing the delays at this level
	//allows maximum flexibility and performance.
	digitalWrite(SS, LOW);
	SPI.transfer(SEND_TX_BUF_0);
	digitalWrite(SS, HIGH);
}

void CANClass::send_1()//transmits buffer 1
{
	digitalWrite(SS, LOW);
	SPI.transfer(SEND_TX_BUF_1);
	digitalWrite(SS, HIGH);
}

void CANClass::send_2()//transmits buffer 2
{
	digitalWrite(SS, LOW);
	SPI.transfer(SEND_TX_BUF_2);
	digitalWrite(SS, HIGH);
}

//char CANClass::readID_0()//reads ID in recieve buffer 0
//{
//	char retVal;
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(READ_RX_BUF_0_ID);
//	retVal = SPI.transfer(0xFF);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//	return retVal;
//}
//
//char CANClass::readID_1()//reads ID in reciever buffer 1
//{
//	char retVal;
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(READ_RX_BUF_1_ID);
//	retVal = SPI.transfer(0xFF);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//	return retVal;
//}
//
//char CANClass::readDATA_0()//reads DATA in recieve buffer 0
//{
//	char retVal;
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer( READ_RX_BUF_0_DATA);
//	retVal = SPI.transfer(0xFF);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//	return retVal;
//}
//
//char CANClass::readDATA_1()//reads data in recieve buffer 1
//{
//	char retVal;
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer( READ_RX_BUF_1_DATA);
//	retVal = SPI.transfer(0xFF);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//	return retVal;
//}

	//extending CAN data read to full frames(pcruce_at_igpp.ucla.edu)
	//It is the responsibility of the user to allocate memory for output.
	//If you don't know what length the bus frames will be, data_out should be 8-bytes
void CANClass::readDATA_ff_0(byte* length_out,byte *data_out,unsigned short *id_out){

	byte len,i;
	unsigned short id_h,id_l;

	digitalWrite(SS, LOW);
	SPI.transfer(READ_RX_BUF_0_ID);
	id_h = (unsigned short) SPI.transfer(0xFF); //id high
	id_l = (unsigned short) SPI.transfer(0xFF); //id low
	SPI.transfer(0xFF); //extended id high(unused)
	SPI.transfer(0xFF); //extended id low(unused)
	len = (SPI.transfer(0xFF) & 0x0F); //data length code
	for (i = 0;i<len;i++) {
		data_out[i] = SPI.transfer(0xFF);
	}
	digitalWrite(SS, HIGH);
	(*length_out) = len;
	(*id_out) = ((id_h << 3) + ((id_l & 0xE0) >> 5)); //repack identifier
	
}

void CANClass::readDATA_ff_1(byte* length_out,byte *data_out,unsigned short *id_out){

	byte id_h,id_l,len,i;

	digitalWrite(SS, LOW);
	SPI.transfer(READ_RX_BUF_1_ID);
	id_h = SPI.transfer(0xFF); //id high
	id_l = SPI.transfer(0xFF); //id low
	SPI.transfer(0xFF); //extended id high(unused)
	SPI.transfer(0xFF); //extended id low(unused)
	len = (SPI.transfer(0xFF) & 0x0F); //data length code
	for (i = 0;i<len;i++) {
		data_out[i] = SPI.transfer(0xFF);
	}
	digitalWrite(SS, HIGH);

	(*length_out) = len;
	(*id_out) = ((((unsigned short) id_h) << 3) + ((id_l & 0xE0) >> 5)); //repack identifier
}

	//Adding method to read status register
	//can be used to determine whether a frame was received.
	//(readStatus() & 0x80) == 0x80 means frame in buffer 0
	//(readStatus() & 0x40) == 0x40 means frame in buffer 1
byte CANClass::readStatus() 
{
	byte retVal;
	digitalWrite(SS, LOW);
	SPI.transfer(READ_STATUS);
	retVal = SPI.transfer(0xFF);
	digitalWrite(SS, HIGH);
	return retVal;

}

//void CANClass::load_0(byte identifier, byte data)//loads ID and DATA into transmit buffer 0
//{
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(LOAD_TX_BUF_0_ID);
//	SPI.transfer(identifier);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(LOAD_TX_BUF_0_DATA);
//	SPI.transfer(data);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//}
//
//void CANClass::load_1(byte identifier, byte data)//loads ID and DATA into transmit buffer 1
//{
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(LOAD_TX_BUF_1_ID);
//	SPI.transfer(identifier);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(LOAD_TX_BUF_1_DATA);
//	SPI.transfer(data);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//}
//
//void CANClass::load_2(byte identifier, byte data)//loads ID and DATA into transmit buffer 2
//{
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(LOAD_TX_BUF_2_ID);
//	SPI.transfer(identifier);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//
//	digitalWrite(SS, LOW);
//	delay(10);
//	SPI.transfer(LOAD_TX_BUF_2_DATA);
//	SPI.transfer(data);
//	delay(10);
//	digitalWrite(SS, HIGH);
//	delay(10);
//}

void CANClass::load_ff_0(byte length,unsigned short identifier,byte *data)
{
	
	byte i,id_high,id_low;

	//generate id bytes before SPI write
	id_high = (byte) (identifier >> 3);
	id_low = (byte) ((identifier << 5) & 0x00E0);

	digitalWrite(SS, LOW);
	SPI.transfer(LOAD_TX_BUF_0_ID);
	SPI.transfer(id_high); //identifier high bits
	SPI.transfer(id_low); //identifier low bits
	SPI.transfer(0x00); //extended identifier registers(unused)
	SPI.transfer(0x00);
	SPI.transfer(length);
	for (i=0;i<length;i++) { //load data buffer
		SPI.transfer(data[i]);
	}

	digitalWrite(SS, HIGH);

}

void CANClass::load_ff_1(byte length,unsigned short identifier,byte *data)
{
	
	byte i,id_high,id_low;

	//generate id bytes before SPI write
	id_high = (byte) (identifier >> 3);
	id_low = (byte) ((identifier << 5) & 0x00E0);

	digitalWrite(SS, LOW);
	SPI.transfer(LOAD_TX_BUF_1_ID);
	SPI.transfer(id_high); //identifier high bits
	SPI.transfer(id_low); //identifier low bits
	SPI.transfer(0x00); //extended identifier registers(unused)
	SPI.transfer(0x00);
	SPI.transfer(length);
	for (i=0;i<length;i++) { //load data buffer
		SPI.transfer(data[i]);
	}

	digitalWrite(SS, HIGH);


}

void CANClass::load_ff_2(byte length,unsigned short identifier,byte *data)
{
	
	byte i,id_high,id_low;

	//generate id bytes before SPI write
	id_high = (byte) (identifier >> 3);
	id_low = (byte) ((identifier << 5) & 0x00E0);

	digitalWrite(SS, LOW);

	SPI.transfer(LOAD_TX_BUF_2_ID);
	SPI.transfer(id_high); //identifier high bits
	SPI.transfer(id_low); //identifier low bits
	SPI.transfer(0x00); //extended identifier registers(unused)
	SPI.transfer(0x00);
	SPI.transfer(length); //data length code
	for (i=0;i<length;i++) { //load data buffer
		SPI.transfer(data[i]);
	}

	digitalWrite(SS, HIGH);

}

//------------------------------------------------------------------------------
//Added for ram
void CANClass::writeRegister(byte address, byte data)
{
    digitalWrite(SS, LOW);
    delay(10);
    SPI.transfer(WRITE);
	SPI.transfer(address);
    SPI.transfer(data);
    delay(10);
	digitalWrite(SS, HIGH);
    delay(10);
}
void CANClass::readRegister(byte address, byte *data_out)
{
	digitalWrite(SS, LOW);
	delay(10);
	SPI.transfer(READ);
    SPI.transfer(address);
	*data_out = SPI.transfer(0xFF);
	delay(10);
	digitalWrite(SS, HIGH);
	delay(10);
}

void CANClass::reset()
{
    digitalWrite(SS, LOW);
    delay(10);
	SPI.transfer(RESET_REG);
    delay(10);
	digitalWrite(SS, HIGH);
    delay(10);
}

byte CANClass::readRXStatus()
{
    byte retVal;
	digitalWrite(SS, LOW);
	SPI.transfer(RX_STATUS);
	retVal = SPI.transfer(0xFF);
	digitalWrite(SS, HIGH);
	return retVal;
}

void CANClass::bitModify(byte address, byte mask, byte data)
{
    digitalWrite(SS, LOW);
    SPI.transfer(BIT_MODIFY);
    SPI.transfer(address);
    SPI.transfer(mask);
    SPI.transfer(data);
    digitalWrite(SS, HIGH);
}

void CANClass::setMask(unsigned short identifier)
{
    setMask_0(identifier);
    setMask_1(identifier);
}

void CANClass::setMask_0(unsigned short identifier)
{
    writeRegister(RXM0SIDH, (byte)(identifier>>3));
    writeRegister(RXM0SIDL, (byte)(identifier<<5));
}

void CANClass::setMask_1(unsigned short identifier)
{
    writeRegister(RXM1SIDH, (byte)(identifier>>3));
    writeRegister(RXM1SIDL, (byte)(identifier<<5));
}

