
#include "SPIFFS.h"

// JPEG decoder library
#include <JPEGDecoder.h>
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 

#define SCLK_PIN 18
#define MOSI_PIN 23
#define DC_PIN   17
#define CS_PIN   5
#define RST_PIN  4


// Option 1: use any pins but a little slower
Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);  

// Option 2: must use the hardware SPI pins 
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be 
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
//Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);



int imageIndex = 0;

void setup(void) {
  //Serial.begin(115200);
  Serial.print("hello!");
  
  
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  
  tft.begin();
  tft.setRotation(3);
  
  
  Serial.println("init");

   if (!SPIFFS.begin()) {
      Serial.println("SPIFFS initialisation failed!");
      while (1) yield(); // Stay here twiddling thumbs waiting
    }

  listDir("/");
}

void listDir(char * dir){
 
  File root = SPIFFS.open(dir);
  File file = root.openNextFile();
  while(file){
 
      Serial.print("FILE: ");
      Serial.println(file.name());
      file = root.openNextFile();
  }
}


void loop() {
  
    if(imageIndex > 9){
      imageIndex = 0;
    }

    String fileIndex = "/" + String(imageIndex) +  ".jpg";
    

    
    Serial.println("fileIndex : " + fileIndex);
    drawFSJpeg(fileIndex.c_str(), 0, 0);
    
    imageIndex++;

    
    //testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", WHITE);
}
/*====================================================================================
  This sketch contains support functions to render the Jpeg images.

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/

// Return the minimum of two values a and b
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

//====================================================================================
//   This function opens the Filing System Jpeg image file and primes the decoder
//====================================================================================
void drawFSJpeg(const char *filename, int xpos, int ypos) {

  Serial.println("=====================================");
  Serial.print("Drawing file: "); Serial.println(filename);
  Serial.println("=====================================");

  // Open the file (the Jpeg decoder library will close it)
  fs::File jpgFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpgFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  if ( !jpgFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  // To initialise the decoder and provide the file, we can use one of the three following methods:
  //boolean decoded = JpegDec.decodeFsFile(jpgFile); // We can pass the SPIFFS file handle to the decoder,
  //boolean decoded = JpegDec.decodeSdFile(jpgFile); // or we can pass the SD file handle to the decoder,
  boolean decoded = JpegDec.decodeFsFile(filename);  // or we can pass the filename (leading / distinguishes SPIFFS files)
                                                     // The filename can be a String or character array
  if (decoded) {
    // print information about the image to the serial port
    jpegInfo();

    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

//====================================================================================
//   Decode and paint onto the TFT screen
//====================================================================================
void jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      for (int h = 1; h < win_h-1; h++)
      {
        memcpy(pImg + h * win_w, pImg + (h + 1) * mcu_w, win_w << 1);
      }
    }


    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height())
    {
      tft.drawRGBBitmap(mcu_x, mcu_y, pImg, win_w, win_h);
    }

    // Stop drawing blocks if the bottom of the screen has been reached,
    // the abort function will close the file
    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();

  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime;

  // print the results to the serial port
  Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
  Serial.println("=====================================");

}

//====================================================================================
//   Send time taken to Serial port
//====================================================================================
void jpegInfo() {
  Serial.println(F("==============="));
  Serial.println(F("JPEG image info"));
  Serial.println(F("==============="));
  Serial.print(F(  "Width      :")); Serial.println(JpegDec.width);
  Serial.print(F(  "Height     :")); Serial.println(JpegDec.height);
  Serial.print(F(  "Components :")); Serial.println(JpegDec.comps);
  Serial.print(F(  "MCU / row  :")); Serial.println(JpegDec.MCUSPerRow);
  Serial.print(F(  "MCU / col  :")); Serial.println(JpegDec.MCUSPerCol);
  Serial.print(F(  "Scan type  :")); Serial.println(JpegDec.scanType);
  Serial.print(F(  "MCU width  :")); Serial.println(JpegDec.MCUWidth);
  Serial.print(F(  "MCU height :")); Serial.println(JpegDec.MCUHeight);
  Serial.println(F("==============="));
}

//====================================================================================
//   Open a Jpeg file and dump it to the Serial port as a C array
//====================================================================================
void createArray(const char *filename) {

  fs::File jpgFile;  // File handle reference for SPIFFS
  //  File jpgFile;  // File handle reference For SD library
  
  if ( !( jpgFile = SPIFFS.open( filename, "r"))) {
    Serial.println(F("JPEG file not found"));
    return;
  }

  uint8_t data;
  byte line_len = 0;
  Serial.println("// Generated by a JPEGDecoder library example sketch:");
  Serial.println("// https://github.com/Bodmer/JPEGDecoder");
  Serial.println("");
  Serial.println("#if defined(__AVR__)");
  Serial.println("  #include <avr/pgmspace.h>");
  Serial.println("#endif");
  Serial.println("");
  Serial.print("const uint8_t ");
  while (*filename != '.') Serial.print(*filename++);
  Serial.println("[] PROGMEM = {"); // PROGMEM added for AVR processors, it is ignored by Due

  while ( jpgFile.available()) {

    data = jpgFile.read();
    Serial.print("0x"); if (abs(data) < 16) Serial.print("0");
    Serial.print(data, HEX); Serial.print(",");// Add value and comma
    line_len++;
    if ( line_len >= 32) {
      line_len = 0;
      Serial.println();
    }

  }

  Serial.println("};\r\n");
  // jpgFile.seek( 0, SeekEnd);
  jpgFile.close();
}
//====================================================================================//====================================================================================
