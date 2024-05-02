#include <LiquidCrystal_I2C.h>     

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Custom characters
byte gaugeEmpty[8] =  {B11111, B00000, B00000, B00000, B00000, B00000, B00000, B11111};    
byte gaugeFill[5][8] = {
  {B11111, B10000, B10000, B10000, B10000, B10000, B10000, B11111},
  {B11111, B11000, B11000, B11000, B11000, B11000, B11000, B11111},
  {B11111, B11100, B11100, B11100, B11100, B11100, B11100, B11111},
  {B11111, B11110, B11110, B11110, B11110, B11110, B11110, B11111},
  {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}
};    

byte gaugeLeft[8] =   {B11111, B10000, B10000, B10000, B10000, B10000, B10000, B11111};    
byte gaugeRight[8] =  {B11111, B00001, B00001, B00001, B00001, B00001, B00001, B11111};    

byte gaugeMaskLeft[8] = {B01111, B11111, B11111, B11111, B11111, B11111, B11111, B01111};  
byte gaugeMaskRight[8] = {B11110, B11111, B11111, B11111, B11111, B11111, B11111, B11110}; 

byte warningIcon[8] = {B00100, B00100, B01110, B01010, B11011, B11111, B11011, B11111};     

byte gaugeLeftDynamic[8];   
byte gaugeRightDynamic[8];  

int cpuGauge = 0;       
char buffer[10];         
int moveOffset = 0;     

const int gaugeSizeChars = 16;       
char gaugeString[gaugeSizeChars + 1]; 

void setup() {
  lcd.init();                      
  lcd.createChar(7, gaugeEmpty);   
  for (int i = 0; i < 5; i++) {
    lcd.createChar(i + 1, gaugeFill[i]);
  }
  lcd.createChar(0, warningIcon);   
  lcd.backlight();                   
}

void loop() {
  float unitsPerPixel = (gaugeSizeChars * 5.0) / 100.0;        
  int valueInPixels = round(cpuGauge * unitsPerPixel);    
  
  int tipPosition = 0;      
  if (valueInPixels < 5) {
    tipPosition = 1;                            
  } else if (valueInPixels > gaugeSizeChars * 5.0 - 5) {
    tipPosition = 3;  
  } else {
    tipPosition = 2;
  }                                                

  moveOffset = 4 - ((valueInPixels - 1) % 5);      
  updateDynamicGauges(tipPosition, moveOffset);
  updateGaugeString(valueInPixels);

  displayContent();
  
  cpuGauge = (cpuGauge + 1) % 101;
  delay(100); 
}

void updateDynamicGauges(int tipPosition, int offset) {
  for (int i = 0; i < 8; i++) {
    if (tipPosition == 1) {
      gaugeLeftDynamic[i] = (gaugeFill[4][i] << offset) | gaugeLeft[i];  
    } else {
      gaugeLeftDynamic[i] = gaugeFill[4][i];                                                     
    }
    gaugeLeftDynamic[i] &= gaugeMaskLeft[i];                                 
  }
  for (int i = 0; i < 8; i++) {
    if (tipPosition == 3) {
      gaugeRightDynamic[i] = (gaugeFill[4][i] << offset) | gaugeRight[i];  
    } else {
      gaugeRightDynamic[i] = gaugeRight[i];                                                       
    }
    gaugeRightDynamic[i] &= gaugeMaskRight[i];                                
  }  
}

void updateGaugeString(int valueInPixels) {
  for (int i = 0; i < gaugeSizeChars; i++) {
    if (i == 0) {
      gaugeString[i] = byte(5);
    } else if (i == gaugeSizeChars - 1) {
      gaugeString[i] = byte(6);
    } else {
      if (valueInPixels <= i * 5) {
        gaugeString[i] = byte(7);
      } else if (valueInPixels > i * 5 && valueInPixels < (i + 1) * 5) {
        gaugeString[i] = byte(5 - moveOffset);
      } else {
        gaugeString[i] = byte(255);
      }
    }
  }
}

void displayContent() {
  lcd.setCursor(0, 0);                         
  sprintf(buffer, "CPU:%3d%% ", cpuGauge);    
  lcd.print(buffer);                           
  lcd.write(byte(0));                         
  lcd.setCursor(0, 1);              
  lcd.print(gaugeString);         
}
