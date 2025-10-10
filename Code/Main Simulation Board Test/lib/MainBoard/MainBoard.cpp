#include "MainBoard.h"

SPIClass mainBoardSpi = SPIClass(HSPI);

int MainBoardStart(){
    mainBoardSpi.begin(MAIN_BOARD_SCK, MAIN_BOARD_MISO, MAIN_BOARD_MOSI); // CLK, MISO, MOSI
    mainBoardSpi.setFrequency(MAIN_BOARD_SPI_FREQ);
    pinMode(MAIN_BOARD_WS2812_PIN, OUTPUT);
    pinMode(MAIN_BOARD_SPKR, OUTPUT);

    Wire.setPins(MAIN_BOARD_I2C_SDA, MAIN_BOARD_I2C_SCL);
    Wire.begin();
    
    return 0;
}