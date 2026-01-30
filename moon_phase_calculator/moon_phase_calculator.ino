/*
  Title   : Moon Phase Calculator
  Author  : TorLab
  github  : https://github.com/faytor
  Created : 13-08-2021


  --------------------------------------------------------------------------------------------
                                 DISCLAIMER
  --------------------------------------------------------------------------------------------

  This code is directly inspired from :
  
  https://skyandtelescope.org/wp-content/plugins/observing-tools/moonphase/moon.html

  The main function is moonPhases(int year, int month, int day) where 'year' is a four-digit number.
  Although I tested the code for different dates, I provide it without warranty.

 list of fonts : https://github.com/olikraus/u8g2/wiki/fntlist8#5-pixel-height
 
*/

#include "moon_images.h"
// #include <U8g2lib.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
// #include <WiFi.h>
// #include "time.h"


// I2C Pin definition
// #define CLOCK_PIN 15
// #define DATA_PIN  4
// #define RESET_PIN 16


// u8g2 contructor
//U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, CLOCK_PIN, DATA_PIN, RESET_PIN);

GxEPD2_BW<GxEPD2_420_GDEY042T81, GxEPD2_420_GDEY042T81::HEIGHT> display(GxEPD2_420_GDEY042T81(/*CS=5*/ 5, /*DC=*/ 4, /*RES=*/ 2, /*BUSY=*/ 15)); // 400x300, SSD1683



//--------------------------------------------------------------------------------------------
//                                 INPUTS
//--------------------------------------------------------------------------------------------
int timeZone = 2;
int day = 6;
int month = 1;
int year = 2026;
int moonNum = -1;
char* moonPhaseStr;

// List of the bitmap Moon phase images 
const unsigned char* moonPhaseImages[29] = {moon1_bmp, moon2_bmp, moon3_bmp, moon4_bmp,
                                            moon5_bmp, moon6_bmp, moon7_bmp, moon8_bmp,
                                            moon9_bmp, moon10_bmp, moon11_bmp, moon12_bmp,
                                            moon13_bmp, moon14_bmp, moon15_bmp, moon16_bmp,
                                            moon17_bmp, moon18_bmp, moon19_bmp, moon20_bmp,
                                            moon21_bmp, moon22_bmp, moon23_bmp, moon24_bmp,
                                            moon25_bmp, moon26_bmp, moon27_bmp, moon28_bmp,
                                            moon29_bmp
                                           };

//--------------------------------------------------------------------------------------------
//                                 VOID SETUP
//--------------------------------------------------------------------------------------------
void setup() 
{
    // u8g2.begin();
    Serial.begin(115200);
    display.init(115200, true, 50, false);
    display.setRotation(1);
    Serial.print("Expected bytes for 240x240: ");
    Serial.println(240 * 240 / 8);  // Should be 7200
    Serial.print("Array size: ");
    Serial.println(sizeof(moon17_bmp));
}


//--------------------------------------------------------------------------------------------
//                                 VOID LOOP
//--------------------------------------------------------------------------------------------
// void loop() 
// {
//     // Calculate the moon number
//     moonNum = moonPhases(year, month, day);
    
   
//     u8g2.firstPage();
//     do 
//     {
//       // Set the font for the moon phase text
//       u8g2.setFont(u8g2_font_5x7_tr); //u8g2_font_ncenB14_tr, u8g2_font_5x7_tr, u8g2_font_4x6_tf
      
//       // Draw the moon phase image
//       u8g2.drawXBMP(34, 0, moon_width, moon_height, moonPhaseImages[moonNum]);
  
//       // Write the moon phase text
//       u8g2.drawStr(25,64,"Moon day is : ");
//       u8g2.setCursor(100, 64);
//       u8g2.print(moonNum);
  
//     } while (u8g2.nextPage());
  
//     delay(500);

// }

void loop() {
    moonNum = moonPhases(year, month, day);
    
    display.fillScreen(GxEPD_BLACK);
    
    // Draw moon image (your working XBM version)
    drawMoonScaledXBM(50, 100, moonPhaseImages[moonNum], 60, 60, 4, GxEPD_WHITE);
    
    // Get moon phase text
    char moonText[64];
    getMoonPhaseText(moonNum, moonText);
    
    // Display the text
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(25, 64);
    display.print(moonText);
    
    display.display(false);
    delay(3600000UL);
}



// Use your WORKING 60x60 image data
// This function scales it 4x to display at 240x240

void drawMoonScaledXBM(int16_t x, int16_t y, const uint8_t *bitmap, 
                       int16_t srcW, int16_t srcH, int16_t scale, uint16_t color) {
    
    int16_t bytesPerRow = (srcW + 7) / 8;
    
    for (int16_t srcRow = 0; srcRow < srcH; srcRow++) {
        for (int16_t srcCol = 0; srcCol < srcW; srcCol++) {
            
            uint16_t byteIdx = srcRow * bytesPerRow + (srcCol >> 3);
            uint8_t bitIdx = srcCol & 7;  // LSB first for XBM
            
            uint8_t byte = pgm_read_byte(&bitmap[byteIdx]);
            
            if (byte & (1 << bitIdx)) {
                for (int16_t dy = 0; dy < scale; dy++) {
                    for (int16_t dx = 0; dx < scale; dx++) {
                        display.drawPixel(x + srcCol * scale + dx, 
                                         y + srcRow * scale + dy, color);
                    }
                }
            }
        }
    }
}




//----------------------------------------------------------------------------------------
//                            MOON PHASE CALCULATION
//----------------------------------------------------------------------------------------
int moonPhases(int year, int month, int day)
{
    double jd = julianDat(year, month, day);  // calculate Julian Date
  
    float dr = PI / 180.0;
  
    float rd = 1 / dr;
  
    unsigned long meeDT = pow(jd - 2382148, 2) / 41048480 / 86400;
  
    double meeT = (jd + meeDT - 2451545.0) / 36525;
  
    unsigned long meeT2 = pow(meeT, 2);
  
    unsigned long meeT3 = pow(meeT, 3);
  
    double meeD = 297.85 + (445267.1115 * meeT) - (0.0016300 * meeT2) + (meeT3 / 545868);
  
    meeD = proper_ang(meeD) * dr;
  
    double meeM1 = 134.96 + (477198.8676 * meeT) + (0.0089970 * meeT2) + (meeT3 / 69699);
  
    meeM1 = proper_ang(meeM1) * dr;
  
    double meeM = 357.53 + (35999.0503 * meeT);
  
    meeM = proper_ang(meeM) * dr;
  
    double elong = meeD * rd + 6.29 * sin(meeM1);
  
    elong = elong - 2.10 * sin(meeM);
  
    elong = elong + 1.27 * sin(2 * meeD - meeM1);
  
    elong = elong + 0.66 * sin(2 * meeD);
  
    elong = proper_ang(elong);
  
    elong = round(elong);
  
    delay(1000);
  
    double moonNum = ((elong + 6.43) / 360) * 28;
  
    moonNum = floor(moonNum);
  
    if (moonNum == 28)
    {
      moonNum = 0;
    }
  
    return moonNum;
}

//--------------------------------------------------------------------------------------------
//                                 PROPER ANG
//--------------------------------------------------------------------------------------------
double proper_ang(double big)
{
    double tmp = 0;
  
    if (big > 0)
    {
      tmp = big / 360.0;
      tmp = (tmp - floor(tmp)) * 360.0;
    }
    else
    {
      tmp = ceil(abs(big / 360.0));
      tmp = big + tmp * 360.0;
    }
  
    return tmp;
}


//----------------------------------------------------------------------------------------
//                            CALCULATE JULIAN DATE
//----------------------------------------------------------------------------------------
double julianDat(int year, int month, int day)
{
  double  zone = -(timeZone * 60 / 1440.0);

  if (month <= 2)
  {
    year -= 1;
    month += 12;
  }
  
  double day2 = day + zone + 0.5;
  double A = floor(year / 100.0);
  double B = 2 - A + floor(A / 4.0);
  double JD = floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day2 + B - 1524.5;

  return JD;
}



void getMoonPhaseText(int moonNum, char* outputStr) {
    const char* moonPhase;
    
    // Determine the base phase
    if (moonNum <= 3 || moonNum >= 25) {
        moonPhase = "New Moon";
    } else if (moonNum >= 4 && moonNum <= 10) {
        moonPhase = "First Quarter";
    } else if (moonNum >= 11 && moonNum <= 17) {
        moonPhase = "Full Moon";
    } else {
        moonPhase = "Last Quarter";
    }
    
    // Calculate days relative to phase
    int daysFromPhase = moonNum % 7;
    
    if (daysFromPhase == 0) {
        sprintf(outputStr, "%s", moonPhase);
    } else if (daysFromPhase <= 3) {
        sprintf(outputStr, "%d day%s past %s", daysFromPhase, 
                daysFromPhase == 1 ? "" : "s", moonPhase);
    } else {
        int daysBefore = 7 - daysFromPhase;
        sprintf(outputStr, "%d day%s before %s", daysBefore,
                daysBefore == 1 ? "" : "s", moonPhase);
    }
}






 
