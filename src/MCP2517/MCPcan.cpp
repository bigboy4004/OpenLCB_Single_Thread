// OpenLCB for ACAN2517 library
// copyright GC 2022

#define __AVR_DA__		// for editing purposes

#if defined(__AVR_ATmega8__)  || defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || \
    defined(__AVR_ATmega168__) ||defined(__AVR_ATmega168P__) || \
    defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328P__) || \
    defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) || \
    defined(__AVR_ATmega128__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || \
    defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__) || \
	defined(__AVR_DA__) || defined(__AVR_DB__)

#include "Arduino.h"

#include "OlcbCan.h"
#include "MCPcan.h"

//class CanBus;
//CanBus* canbus;
//tCAN* canbus;

/*
#define	BITRATE_10_KBPS	0	// ungetestet
#define	BITRATE_20_KBPS	1	// ungetestet
#define	BITRATE_50_KBPS	2	// ungetestet
#define	BITRATE_100_KBPS	3	// ungetestet
#define	BITRATE_125_KBPS	4
#define	BITRATE_250_KBPS	5
#define	BITRATE_500_KBPS	6
#define	BITRATE_1_MBPS	7
*/

static const byte MCP2517_CS = 10; // CS input of MCP2517 
static const byte MCP2517_INT = 3; // INT output of MCP2517

//——————————————————————————————————————————————————————————————————————————————
//  ACAN2517 Driver object
//——————————————————————————————————————————————————————————————————————————————

ACAN2517 acan(MCP2517_CS, SPI, MCP2517_INT);

void Can::init()  {
	//----------------------------------- Begin SPI
	SPI.begin();
	//--- Configure ACAN2517
	ACAN2517Settings settings(ACAN2517Settings::OSC_4MHz10xPLL, 125UL * 1000UL);
  //--- Default values are too high for an Arduino Uno that contains 2048 bytes of RAM: reduce them
	settings.mDriverTransmitFIFOSize = 1;
	settings.mDriverReceiveFIFOSize = 1;
	//--- Begin
	const uint32_t errorCode = acan.begin(settings, [] { acan.isr(); });
	if (errorCode != 0) {
		Serial.print("2517 Configuration error 0x");
		Serial.println(errorCode, HEX);
	}
}

uint8_t Can::avail()  {
	bool r = acan.available();
    return r;
}

uint8_t Can::read()  {
	CANMessage m;
	if(avail()) {
		acan.receive(m);
        this->id = m.id;
	    this->length = m.len;
		memcpy(this->data, m.data, length);
        return true;
    }
    return false;
}

uint8_t Can::txReady()  {
                    //Serial.print("\nIn AT90Can::txReady(): ");
                    //Serial.print("check_free_buffer="); Serial.print(((CanBus*)this)->check_free_buffer());
    //return ((CanBus*)this)->check_free_buffer();
    //return acan.tryToSend();
	return true;
}
uint8_t Can::write(long timeout)  {
                    //Serial.print("\nIn MCPCan::write(): [");
	CANMessage m;
    m.id = this->id;
    m.len = this->length;
    m.ext = true;
    m.rtr = false;
	m.idx = 0;
    memcpy(m.data,this->data,length);
                    //Serial.print(m.id, HEX);
                    //Serial.print("]("); Serial.print(m.length);
                    //Serial.print(") ");
                    //for(int i=0;i<m.length;i++)
                    //    { Serial.print(m.data[i],HEX); Serial.print(" "); }
    if(timeout==0) {
        //return canbus->send_buffered_message(&m);
        return acan.tryToSend(m);
        active = true;
    }
    unsigned long to = millis() + timeout;
    while(millis()<to) {
        if(acan.tryToSend(m)) {
            active = true;
            return true;
        }
    }
    return false;
}

uint8_t Can::write() { return this->write(0); }

void Can::setL(uint16_t l) { length = l; }

#endif // AVR

