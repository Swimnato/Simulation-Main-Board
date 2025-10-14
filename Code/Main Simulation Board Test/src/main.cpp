#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <MainBoard.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Ethernet3.h>
#include <SdFat.h>

#define TEST_ETHERNET false
#define TEST_SD false
#define TEST_SERVO false
#define TEST_WS2812 false
#define TEST_I2C false
#define TEST_I2C_MUX false
#define TEST_DIGITAL false
#define TEST_ANALOG false

#define LED_COUNT 1

Servo servo;

Adafruit_NeoPixel strip(LED_COUNT, MAIN_BOARD_WS2812_PIN, NEO_GRB + NEO_KHZ800);


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);
EthernetServer server(80);

SdFs sd;

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    strip.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}


void scanI2C(){
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void scanEthClients(){
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void setup() {
  MainBoardStart();

  #if TEST_SD
  if (!sd.cardBegin(SdSpiConfig(MAIN_BOARD_SD_CS, SHARED_SPI, SD_SCK_MHZ(133)))) {
    Serial.println(F(
        "\nSD initialization failed.\n"
        "Do not reformat the card!\n"
        "Is the card correctly inserted?\n"
        "Is there a wiring/soldering problem?\n"));
    if (isSpi(SdSpiConfig(MAIN_BOARD_SD_CS, SHARED_SPI, SD_SCK_MHZ(133)))) {
      Serial.println(F(
          "Is SD_CS_PIN set to the correct value?\n"
          "Does another SPI device need to be disabled?\n"));
    }
    return;
  } else{
    Serial.println("SD INITIALIZED CORRECTLY");
  }
  #endif

  #if TEST_ETHERNET
  Ethernet.setCsPin(MAIN_BOARD_ETHERNET_CS);
  Ethernet.setHostname("MainBoardTest");
  if(!Ethernet.begin(mac)){
    Serial.println("Ethernet Failed to initialize using DHCP!");
    Ethernet.begin(mac, ip);
  }
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  #endif
  #if TEST_SERVO
  servo.attach(MAIN_BOARD_SERVO_1_PIN);
  #endif
  #if TEST_WS2812
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  #endif
  Serial.print("Initialization Complete @");
  Serial.print(millis());
  Serial.println("ms");
}


void loop() {
  // Fill along the length of the strip in various colors...
  #if TEST_ANALOG
  uint8_t iterator = 0;
  while(iterator != 128){
    Serial.print("Getting analog reading from port ");
    Serial.print(iterator);
    Serial.print(" : ");
    Serial.println(mainBoardGetAnalogMux(iterator));
    iterator++;
    if(iterator == 128){
      iterator = 0;
      unsigned long startTime = micros();
      while(iterator != 128){
        mainBoardGetAnalogMux(iterator);
        iterator++;
      }
      unsigned long endTime = micros();
      Serial.print("Read all ports in ");
      Serial.print(endTime - startTime);
      Serial.println("us");
      delay(20000);
    }
  }
  
  #endif

  #if TEST_ETHERNET
  scanEthClients();
  #endif
  #if TEST_WS2812
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  // Do a theater marquee effect in various colors...
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(10);             // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
  #endif

  #if TEST_SERVO
  for(int i = 0; i < 180; i++){
    servo.write(i);
    delay(10);
  }
  delay(150);
  scanEthClients();
  for(int i = 180; i >= 0; i--){
    servo.write(i);
    delay(10);
  }
  #endif
  #if TEST_I2C & !TEST_DIGITAL
  scanI2C();
  #endif
  #if TEST_DIGITAL
  static bool init = false;
  if(!init){
    for(int i = 0; i < 64; i++){
      mainBoardDigitalPinMode(i, INPUT);
    }
    for(int i = 0; i < 32; i++){
      mainBoardDigitalPinMode(i < 16 ? i + 16 : i + 32, OUTPUT);
    }
    init = true;
  }
  for(int i = 0; i < 32; i++){
    int index = i < 16 ? i : i + 16;
    Serial.print("Pin ");
    Serial.print(index);
    Serial.print(" is ");
    Serial.println(mainBoardGetDigitalInput(index));
  }
  for(int chip = 0; chip < 4; chip++){
    Serial.print("Chip One Is ");
    Serial.println(mainBoardGet16DigitalInput(chip), BIN);
  }
  for(int i = 0; i < 32; i++){
    int index = i < 16 ? i + 16 : i + 32;
    mainBoardWriteDigitalOutput(index, HIGH);
  }
  for(int i = 0; i < 3; i++){
    delay(1000); 
    for(int chip = 0; chip < 4; chip++){
      if(chip == 1 || chip == 3){
        mainBoardWrite16DigitalOutput(chip, 0xFFFF * (i % 2));
      }
    }
  }
  delay(1000);
  #endif
  #if TEST_I2C_MUX
  for(int i = 0; i < 8; i++){
    Serial.print("Setting I2C Bus ");
    Serial.print(i);
    Serial.print(" returned: ");
    Serial.println(mainBoardSetI2CBus(i));
    scanI2C();
  }
  delay(1000);
  #endif
}

