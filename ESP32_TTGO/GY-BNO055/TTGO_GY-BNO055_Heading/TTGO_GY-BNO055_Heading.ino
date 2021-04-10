/////////////////////////////////////////////////////////////////
/*
  GY-BNO055 | Ep.0: Connecting with ESP32 TTGO via I2C (ft. Heading Direction)
  Video Tutorial: https://youtu.be/dgCpOPEA6ZM
  Created by Eric Nam (ThatProject)
*/
/////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#define I2C_SDA 21
#define I2C_SCL 22

double xPos = 0, yPos = 0, headingVel = 0;
uint16_t BNO055_SAMPLERATE_DELAY_MS = 10; //how often to read data from the board
uint16_t PRINT_DELAY_MS = 500; // how often to print the data
uint16_t printCount = 0; //counter to avoid printing every 10MS sample

//velocity = accel*dt (dt in seconds)
//position = 0.5*accel*dt^2
double ACCEL_VEL_TRANSITION =  (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251; //trig functions require radians, BNO055 outputs degrees

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
TwoWire I2CBNO = TwoWire(0);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &I2CBNO);

TFT_eSPI tft = TFT_eSPI();

TFT_eSprite dial   = TFT_eSprite(&tft); // Sprite object for dial
TFT_eSprite needle = TFT_eSprite(&tft); // Sprite object for needle

uint32_t startMillis;
int16_t angle = 0;

// =======================================================================================
// Setup
// =======================================================================================

void setup()   {
  Serial.begin(115200);
  I2CBNO.begin(I2C_SDA, I2C_SCL);
  if (!bno.begin())
  {
    Serial.print("No BNO055 detected");
    while (1);
  }

  tft.begin();
  tft.setRotation(0);

  // Clear TFT screen
  tft.fillScreen(TFT_BLACK);

  // Create the dial Sprite and dial (this temporarily hijacks the use of the needle Sprite)
  createDialScale();   // create scale (start angle, end angle, increment angle)

  createNeedle();              

  dial.setTextDatum(TC_DATUM);  
  dial.drawString("BNO055", tft.width()/2, 4, 4);

  dial.setTextDatum(TL_DATUM);  
  dial.drawString("Position", 0, tft.height()/2 + 45, 2);
  dial.drawString("Speed", 0, tft.height()/2 + 85, 2);

  delay(1000);
}

// =======================================================================================
// Loop
// =======================================================================================

void loop() {
  unsigned long tStart = micros();
  sensors_event_t orientationData , linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  //  bno.getEvent(&angVelData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  xPos = xPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.x;
  yPos = yPos + ACCEL_POS_TRANSITION * linearAccelData.acceleration.y;

  // velocity of sensor in the direction it's facing
  headingVel = ACCEL_VEL_TRANSITION * linearAccelData.acceleration.x / cos(DEG_2_RAD * orientationData.orientation.x);

  if (printCount * BNO055_SAMPLERATE_DELAY_MS >= PRINT_DELAY_MS) {
    //enough iterations have passed that we can print the latest data
    Serial.print("Heading: ");
    Serial.println(orientationData.orientation.x);
    angle = (int)orientationData.orientation.x;
    plotDial(0, 0, angle, "Heading", angle);
    updateDialLabel();

    Serial.print("Position: ");
    Serial.print(xPos);
    Serial.print(" , ");
    Serial.println(yPos);
    Serial.print("Speed: ");
    Serial.println(headingVel);
    Serial.println("-------");

    printCount = 0;
  }
  else {
    printCount = printCount + 1;
  }

  while ((micros() - tStart) < (BNO055_SAMPLERATE_DELAY_MS * 1000))
  {
    //poll until the next sample is ready
  }
}


void updateDialLabel(){
  String _position = "";
  _position += xPos;
  _position += ", ";
  _position += yPos;

  dial.setTextDatum(TC_DATUM);  
  dial.drawString(_position, tft.width()/2, tft.height()/2 + 60, 4);
  dial.drawString((String)headingVel, tft.width()/2, tft.height()/2 + 100, 4);
}
// =======================================================================================
// Create the dial sprite, the dial outer and place scale markers
// =======================================================================================

void createDialScale()
{
  // Create the dial Sprite
  dial.setColorDepth(8);       // Size is odd (i.e. 91) so there is a centre pixel at 45,45
  dial.createSprite(tft.width(), tft.height());   // 8bpp requires 91 * 91 = 8281 bytes
  dial.setPivot(tft.width()/2, tft.height()/2 - 20);       // set pivot in middle of dial Sprite

  // Draw dial outline
  dial.fillSprite(TFT_TRANSPARENT);           // Fill with transparent colour
  dial.fillCircle(tft.width()/2, tft.height()/2 - 20, tft.width()/2, TFT_LIGHTGREY);  // Draw dial outer
}


// =======================================================================================
// Add the empty dial face with label and value
// =======================================================================================

void drawEmptyDial(String label, int16_t val)
{
  // Draw black face
  dial.fillCircle(tft.width()/2, tft.height()/2 - 20, tft.width()/2 - 4, TFT_BLACK);
  
  dial.setTextDatum(TC_DATUM);  
  dial.drawString(label, tft.width()/2, tft.height()/2 -50, 4);
  dial.drawNumber(val, tft.width()/2, tft.height()/2, 4);
}

// =======================================================================================
// Update the dial and plot to screen with needle at defined angle
// =======================================================================================

void plotDial(int16_t x, int16_t y, int16_t angle, String label, uint16_t val)
{
  // Draw the blank dial in the Sprite, add label and number
  drawEmptyDial(label, val);

  // Push a rotated needle Sprite to the dial Sprite, with black as transparent colour
  needle.pushRotated(&dial, angle, TFT_BLACK); // dial is the destination Sprite

  // Push the resultant dial Sprite to the screen, with transparent colour
  dial.pushSprite(x, y, TFT_TRANSPARENT);
}

// =======================================================================================
// Create the needle Sprite and the image of the needle
// =======================================================================================

void createNeedle(void)
{
  needle.setColorDepth(8);
  needle.createSprite(20, tft.width()/2); // create the needle Sprite 11 pixels wide by 49 high
  needle.fillSprite(TFT_BLACK);          // Fill with black

  // Define needle pivot point
  uint16_t piv_x = needle.width() / 2;   // x pivot of Sprite (middle)
  uint16_t piv_y = needle.height() - 10; // y pivot of Sprite (10 pixels from bottom)
  needle.setPivot(piv_x, piv_y);         // Set pivot point in this Sprite

  // Draw the red needle with a yellow tip
  // Keep needle tip 1 pixel inside dial circle to avoid leaving stray pixels
  needle.fillRect(piv_x - 1, 2, 3, piv_y + 8, TFT_RED);
  needle.fillCircle(piv_x - 1, 5, 5, TFT_YELLOW);
  // Draw needle centre boss
  needle.fillCircle(piv_x, piv_y, 5, TFT_MAROON);
  needle.drawPixel( piv_x, piv_y, TFT_WHITE);     // Mark needle pivot point with a white pixel
}

// =======================================================================================
