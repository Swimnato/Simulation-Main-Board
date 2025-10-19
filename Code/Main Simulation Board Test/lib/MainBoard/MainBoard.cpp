#include "MainBoard.h"
#ifndef MAIN_BOARD_CPP
#define MAIN_BOARD_CPP
#include <TCA9555.h>
#include <SdFat.h>

#define VERSION_MAIN_BOARD_CPP "REV: X0"

static bool I2CMuxInit = false;

SPIClass mainBoardSpi = SPIClass(HSPI);

HardwareSerial MainBoardSerialAlt(1);

SdFs mainBoardSD;

static TCA9535* IO_Expanders[NUM_DIGITAL_IO_EXPANDERS];

static bool mainBoardInitI2CMux();

SPIClass* getMainBoardSPI(){
    return &mainBoardSpi;
}

int MainBoardStart(bool initSD){;
    pinMode(MAIN_BOARD_WS2812_PIN, OUTPUT);
    pinMode(MAIN_BOARD_SPKR, OUTPUT);
    pinMode(MAIN_BOARD_ANALOG_MUX_IN, INPUT);
    Serial.begin(115200);
    Serial.println("Initializing");
    MainBoardSerialAlt.begin(MAIN_BOARD_UART_1_BAUD_RATE, SERIAL_8N1, MAIN_BOARD_UART_1_RX, MAIN_BOARD_UART_1_TX);

    mainBoardSpi.begin(MAIN_BOARD_SCK, MAIN_BOARD_MISO, MAIN_BOARD_MOSI); // CLK, MISO, MOSI
    mainBoardSpi.setFrequency(MAIN_BOARD_SPI_FREQ);
    mainBoardSpi.begin();

    if (initSD && !mainBoardSD.begin(SdSpiConfig(MAIN_BOARD_SD_CS, SHARED_SPI, MAIN_BOARD_SD_SPEED, &mainBoardSpi))) { 
        Serial.println(F(
            "\nSD initialization failed.\n"
            "Do not reformat the card!\n"
            "Is the card correctly inserted?\n"
            "Is there a wiring/soldering problem?\n"));
        if (isSpi(SdSpiConfig(MAIN_BOARD_SD_CS, SHARED_SPI, MAIN_BOARD_SD_SPEED))) {
        Serial.println(F(
            "Is SD_CS_PIN set to the correct value?\n"
            "Does another SPI device need to be disabled?\n"));
        }
    }


    Wire.setPins(MAIN_BOARD_I2C_SDA, MAIN_BOARD_I2C_SCL);
    Wire.begin();

    I2CMuxInit = mainBoardInitI2CMux();

    for(int i = 0; i < NUM_DIGITAL_IO_EXPANDERS; i++){
        IO_Expanders[i] = new TCA9535(MAIN_BOARD_DIGITAL_IO_EXPANDER_BASE_ADDR + i);
        if(IO_Expanders[i]->begin()){
            IO_Expanders[i]->pinMode16(0xFFFF);
        } else{
            Serial.print("Failed to initialize digital IO expander ");
            Serial.println(i+1);
        }
    }
    
    return 0;
}

bool mainBoardSetI2CBus(uint8_t bus){
    if(bus >= 8 || !I2CMuxInit){
        return false;
    }
    Wire.beginTransmission(MAIN_BOARD_I2C_MUX_ADDR);
    Wire.write(0x01 << bus);
    return !Wire.endTransmission();
}

static bool mainBoardInitI2CMux(){
    Wire.beginTransmission(MAIN_BOARD_I2C_MUX_ADDR);
    return !Wire.endTransmission();
}

bool mainBoardDigitalPinMode(uint8_t pin, uint8_t mode){
    return IO_Expanders[pin/16]->pinMode1(pin % 16, mode);
}


bool mainBoard16DigitalPinMode(uint8_t chip, uint16_t mode){
    return IO_Expanders[chip]->pinMode16(mode);
}


//NOT RECOMMENDED UNLESS YOU ONLY NEED TO READ ONE PIN
bool mainBoardGetDigitalInput(uint8_t pin){
    return IO_Expanders[pin/16]->read1(pin % 16);
}

uint16_t mainBoardGet16DigitalInput(uint8_t chip){
    return IO_Expanders[chip]->read16();
}

//NOT RECOMMENDED UNLESS YOU ONLY NEED TO WRITE TO ONE PIN
bool mainBoardWriteDigitalOutput(uint8_t pin, uint8_t val){
    return IO_Expanders[pin/16]->write1(pin % 16, val);
}

bool mainBoardWrite16DigitalOutput(uint8_t chip, uint16_t val){
    return IO_Expanders[chip]->write16(val);
}

uint32_t mainBoardGetAnalogMux(uint8_t address){
    mainBoardSpi.write(address);
    return analogRead(MAIN_BOARD_ANALOG_MUX_IN);
}
#endif