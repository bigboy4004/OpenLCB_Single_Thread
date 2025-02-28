#ifndef MCPCAN_H
#define MCPCAN_H

//#pragma message("!!! compiling MCPcan.h ")

#include "OlcbCan.h"

#define SUPPORT_MCP2517 1

#include <ACAN2517.h>
#include <SPI.h>

class Can : public OlcbCan {
  public:
    void init();                    // initialization
    uint8_t avail();                // read rxbuffer available
    uint8_t read();                 // read a buffer
    uint8_t txReady();              // write txbuffer available
    uint8_t write(long timeout);    // write, 0= immediately or fail; 0< if timeout occurs fail
    uint8_t write();                // write(0), ie write immediately
    void setL(uint16_t l);
};

#endif // MCPCAN_H
