#include "MainBoard.h"

int MainBoardStart(){
    main_board_spi = SPIClass(HSPI);
    main_board_spi.begin(MAIN_BOARD_SCK, MAIN_BOARD_MISO, MAIN_BOARD_MOSI); // CLK, MISO, MOSI
    main_board_spi.setFrequency(MAIN_BOARD_SPI_FREQ);
    pinMode(MAIN_BOARD_WS2812_PIN, OUTPUT);
    pinMode(MAIN_BOARD_SPKR, OUTPUT);

    Wire.setPins(MAIN_BOARD_I2C_SDA, MAIN_BOARD_I2C_SCL);
    Wire.begin();
    
    return 0;
}