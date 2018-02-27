/************************************************
 *  Poor mans smu
 *  
 *  more to come...
 *  
 *  
 *  
 *  External library:
 *  
 *  FT81x graphics driver is initially copied (2016) from 
 *  jamesbowman / gd2-lib
 *  (https://github.com/jamesbowman/gd2-lib.git)
 *  -compile om 8266
 *  -support 800x480 pixels for all graphics
 * 
 ************************************************/

#include <SPI.h>
#include "GD2.h"
#include "colors.h"
#include "volt_display.h"
#include "current_display.h"
#include "Dac.h"

float DACVout;  // TODO: Dont use global



#include "dial.h"

void setup()
{
  //wdt_disable();
  Serial.begin(9600);
  DAC.init();
  Serial.println("Initializing WeatherNG graphics controller FT81x...");
  GD.begin(0);
  Serial.println("Done!");
  
}

void voltagePanel(int x, int y) {
  GD.ColorA(70);
  GD.Begin(LINE_STRIP);
  GD.LineWidth(32);
  GD.ColorRGB(0,255,150);
  GD.Vertex2ii(x+10, y+30); 
  GD.Vertex2ii(x+790, y+30);
  GD.Vertex2ii(x+790, y+210);
  GD.Vertex2ii(x+10, y+210);
  GD.Vertex2ii(x+10, y+30);

  // clear area behind heading
  GD.ColorA(255);
  GD.Begin(RECTS);
  GD.ColorRGB(00,00,00);
  GD.Vertex2ii(x+56, y+20);
  GD.Vertex2ii(x+260, y+50);

  // heading
  GD.ColorRGB(200,255,200);
  GD.cmd_text(x+56, y+16 ,   29, 0, "SOURCE VOLTAGE");

  //float rawMv = 10501.0 +  random(0, 199) / 1000.0;
  float rawMv = DACVout; // TODO: Dont use global
  VOLT_DISPLAY.renderMeasured(x + 17,y , rawMv);
  VOLT_DISPLAY.renderSet(x + 120, y+150, DIAL.getMv());

  // various other values, dummies for now...
  renderVariousDummyFields(x,y);
}

void renderVariousDummyFields(int x, int y) {
  GD.ColorRGB(200,255,200);
  GD.cmd_text(x+486, y+147, 27, 0, "Average");
  GD.ColorRGB(COLOR_VOLT);
  GD.cmd_text(x+486, y+163, 30, 0, "0.500 075V");

  GD.ColorRGB(200,255,200);
  GD.cmd_text(x+667, y+147, 27, 0, "Deviation");
  GD.ColorRGB(COLOR_VOLT);
  GD.cmd_text(x+667, y+163, 30, 0, "0.005%");
  
  GD.ColorRGB(200,255,200);
  
  GD.Begin(LINE_STRIP);
  GD.LineWidth(10);
  GD.Vertex2ii(x+620, y+70); 
  GD.Vertex2ii(x+780, y+70); 
  GD.Begin(LINE_STRIP);
  GD.LineWidth(10);
  GD.Vertex2ii(x+620, y+125); 
  GD.Vertex2ii(x+780, y+125);
}

void currentPanel(int x, int y) {
  GD.Begin(LINE_STRIP);
  GD.LineWidth(32);
  GD.ColorRGB(50,50,0); // yellow
  GD.ColorA(230);
  GD.Vertex2ii(x+10, y+20); 
  GD.Vertex2ii(x+790, y+20);
  GD.Vertex2ii(x+790, y+200);
  GD.Vertex2ii(x+10, y+200);
  GD.Vertex2ii(x+10, y+20);

  GD.Begin(RECTS);
  GD.ColorA(255);
  GD.ColorRGB(00,00,00);
  GD.Vertex2ii(x+56, y);
  GD.Vertex2ii(x+280, y+30);

  // heading
  GD.ColorRGB(232,202,158);
  GD.cmd_text(x+56, y+5, 29, 0, "MEASURE CURRENT");

  float rawMa = 56.0 +  random(0, 199) / 1000.0;
  CURRENT_DISPLAY.renderMeasured(x + 17, y, rawMa);
  CURRENT_DISPLAY.renderSet(x+120, y+135, 200.0);
}

void drawBall(int x, int y, bool set) {
  GD.PointSize(16 * 7); 
  GD.ColorRGB(255,255,255);
  GD.Begin(POINTS);
  GD.Vertex2ii(x, y);
  GD.PointSize(16 * 6);  
  if (set == true) {
    GD.ColorRGB(255,255,255); 
  } else {
    GD.ColorRGB(0,0,0); 
  }
  GD.Vertex2ii(x, y);
}

void scrollIndication(int x, int y) {
  drawBall(x,y,false);
  drawBall(x+30,y,false);
  drawBall(x+60,y,true);
  drawBall(x+90,y,false);
  drawBall(x+120,y,false);
}

void renderDisplay() {
  
  GD.cmd_romfont(1, 34); // put FT81x font 34 in slot 1

  int x = 0;
  int y = 0;
  voltagePanel(x,y);
  currentPanel(x,y+260);
  
  scrollIndication(340,250);
    
  GD.cmd_fgcolor(0xaaaa90);  
  GD.Tag(BUTTON_VOLT_SET);
  GD.cmd_button(20,143,90,58,30,0,"SET");  
  GD.cmd_button(20,393,90,58,30,0,"LIM");

  GD.cmd_button(350,143,90,58,30,0,"AUTO");
  GD.cmd_button(350,393,90,58,30,0,"AUTO");
}

unsigned long previousMillis = 0; 
unsigned long previousMillisSlow = 0; 
const long interval = 50; 

  float avgVout; //TODO: fix global...
  int smoothingSamples = 4;//TODO: fix global...


  float minimum;
  float maximum;
  int endPtr = 0;
    const int nrOfTrendPoints = 75;

  float value[nrOfTrendPoints];
void loop()
{
  //GD.wr(REG_PWM_DUTY, 20);

  unsigned long currentMillis = millis();

// restore SPI
SPI.setDataMode(SPI_MODE0);
//SPI.setClockDivider(SPI_CLOCK_DIV2);
GD.resume();

  




  

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    GD.ClearColorRGB(0x000000); // black
    GD.Clear();
    renderDisplay();

  GD.ColorA(255);
    GD.ColorRGB(255,255,255);

  

    //int v, mv, uv;
    float span = maximum - minimum;
    //VOLT_DISPLAT.separate(&v, &mv, &uv, span);

    int minmV = minimum;
    int maxmV = maximum;
    if (minimum < 0.0f) {
      minmV = (int)(- minimum * 1000.0f);
    }
        if (maximum < 0.0f) {
      maxmV = (int)(- maximum * 1000.0f);
    }

    GD.cmd_text(680, 36, 26, 0, "mV");
    GD.cmd_number(620, 55, 26, 6, minmV);
    GD.cmd_number(620, 128, 26, 6, maxmV);

  //GD.cmd_number(x+153+6-17, y+36 ,   1, 3, ma);

    int i = endPtr;
        GD.Begin(LINE_STRIP);
        GD.LineWidth(10);
                  Serial.print("RENDER:");
    for (int pos=0; pos<nrOfTrendPoints;pos++) {
 
     
        float height = 60 * (value[i] / span);
        GD.Vertex2ii(pos*2+630, 90 - height); 

//        GD.PointSize(16 * 2); 
//  GD.ColorRGB(255,255,255);
//  GD.Begin(POINTS);
//  GD.Vertex2ii(pos*10+630, 120+value[i]*2000.0f);

//
//          Serial.print(i);
//          Serial.print("=");
//          Serial.print(value[i],3);
//          Serial.print(",");


      i=i+1;
      if (i>nrOfTrendPoints - 1) {
        i=0;
      }  
     

    }
          Serial.println("");

    
     

    GD.get_inputs();
    if (GD.inputs.tag == BUTTON_VOLT_SET) {
      DIAL.open();
    }
    if (DIAL.isDialogOpen()) {
      DIAL.handleKeypadDialog();
      DIAL.checkKeypress();
    }
    
    if (currentMillis - previousMillisSlow >= 10000) {
      previousMillisSlow = currentMillis;
    }
  
    float sum = DIAL.getMv();
    //Serial.printf( "Entered: %.6f mV", sum);
    //Serial.println("");
//    Serial.print("Entered:");
//    Serial.print(sum, 3);
//    Serial.println("mV");

    GD.swap();
    
   GD.__end();


// change SPI
SPI.setDataMode(SPI_MODE1);
    if(DAC.checkDataAvilable() == true) {
      float Vout = DAC.convertToMv();
     
      Serial.print("Vout in mV : ");  
      Serial.println(Vout, 3);

      DACVout = Vout;
      
      //avgVout = DAC.smoothing(avgVout, smoothingSamples, Vout);
      //Serial.print("AvgOut in mV : ");  
      //Serial.println(avgVout, 3);
      //DACVout = avgVout;



      value[endPtr] = DACVout;

  
      endPtr ++;
      if (endPtr > nrOfTrendPoints - 1) {
        endPtr = 0;
      } 
      minimum = value[0];
      maximum = value[0];

for (int i=0;i<nrOfTrendPoints;i++) {
  if (value[i]<minimum) {
    minimum = value[i];
  }
    if (value[i]>maximum) {
    maximum = value[i];
  }

       

  
}

  Serial.println(maximum,3);
         Serial.println(minimum,3);
         
//      for (int i=0;i<nrOfTrendPoints;i++) {
//        Serial.print(value[i],3);
//        Serial.print(",");
//      }
//      Serial.println(endPtr);


  

       delay(60);
    }


  }
}
