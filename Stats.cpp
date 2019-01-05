#include "Stats.h"
#include "GD2.h"
#include "digit_util.h"
#include "colors.h"

void StatsClass::init(int type_) {
   type = type_;
   for (int pos=0; pos<nrOfTrendPoints;pos++) { 
     value[pos] = undefinedValue;
   }
}
void StatsClass::addSample(float rawValue_) {
      rawValue = rawValue_; 
      prelimBuffer[prelimSamplesCounter++] = rawValue_;
      
      if (prelimSamplesCounter < maxSamplesBeforeStore) {
        return;
      } else {
        valueExt[endPtr][0] = prelimBuffer[0];
        valueExt[endPtr][1] = prelimBuffer[0];
        float tot=prelimBuffer[0];
        for (int i=1; i< maxSamplesBeforeStore ; i++) {
          tot = tot + prelimBuffer[i];
          if (prelimBuffer[i] <= valueExt[endPtr][0]) {
            valueExt[endPtr][0] = prelimBuffer[i];   // stor as min value
           }
          if (prelimBuffer[i] > valueExt[endPtr][1]) {
            valueExt[endPtr][1] = prelimBuffer[i];   // stor as max value
          }
          
        }
        value[endPtr] = tot/maxSamplesBeforeStore; // set value as mean value of prelim samples

        prelimSamplesCounter = 0;
        updateMinMax();
        span = maximum - minimum;

      }
      
      endPtr ++;
      if (endPtr > nrOfTrendPoints - 1) {
        endPtr = 0;
      } 

}


void StatsClass::updateMinMax() {      
    minimum = undefinedValue;
    maximum = -minimum;
    for (int i=0;i<nrOfTrendPoints;i++) {
      if (valueExt[i][0]<=minimum) {
        minimum = valueExt[i][0];
      }
      if (valueExt[i][1]>=maximum) {
        maximum = valueExt[i][1];
      }
    }
}


void StatsClass::renderTrend(int x, int y, bool limitDetails) {     
    GD.ColorRGB(type==DigitUtilClass::typeVoltage?COLOR_VOLT:COLOR_CURRENT);

    GD.cmd_text(x+56, y, 29, 0, type==DigitUtilClass::typeVoltage?"VOLTAGE TREND":"CURRENT TREND");
    
    GD.ColorRGB(0xffffff); 

    y=y+25;
    int lines = 11;
    int height = 150;
    float visibleSpan = visibleMax - visibleMin;
    float distanceBetweenLines = visibleSpan/(lines-1);

    // show values for the horisontal lines
    GD.ColorRGB(0xffffff);
    for (int i=0;i<lines;i++) {
      if (i==0 || i ==lines-1) {
        GD.ColorRGB(type==DigitUtilClass::typeVoltage?COLOR_VOLT:COLOR_CURRENT);
      } else {
        GD.ColorRGB(0xffffff);
      }
      DIGIT_UTIL.renderValue(x, 15 + y + i*height/(lines-1), visibleMax - distanceBetweenLines*i, 0, type);
    }


    // draw horisontal lines
    int farRight = x + 780;
    if (farRight > 780) {
      farRight = 780;
    }
    GD.LineWidth(8);
    for (int i=0;i<lines;i++) {
      int yaxis = 23 + y + i*height/(lines - 1);
      GD.Begin(LINE_STRIP);
            if (i == 0 || i==5 || i==10) {
               GD.ColorRGB(0xffffff);
            } else {
               GD.ColorRGB(0x444444);
            }

      GD.Vertex2ii(x+90+(i==5?100:0), yaxis); 
      GD.Vertex2ii(farRight, yaxis); 
    }
   
    x = x + 190;
    y = y + 23;
  
    int viewHeight;

    // Add by 1000 to reduce decimal roudning problems.... should have stored value in uV and uA instead of mV and mA....
    maximum = (int)(maximum *1000.0);
    minimum = (int)(minimum *1000.0);

    // initially, let's say highest value in graph corresponds to highest value in sample set
    visibleMax = maximum;
    // and lowest graph value corresponds to lowest value in sample set
    visibleMin = minimum;

    // Try to introduce slow transition of graph highest value when highest sample value suddenly drops 
    if (maximum >= visibleMaxSlow) {
       visibleMaxSlow = maximum;
    } else {
       visibleMaxSlow = visibleMaxSlow - (visibleMaxSlow - maximum)/3.0;
      if (visibleMaxSlow >= maximum) {
         visibleMax = visibleMaxSlow;
      }
    }

     // Try to introduce slow transition of graph lowest value when lowest value suddenly increases
    if (minimum <= visibleMinSlow) {
       visibleMinSlow = minimum;
    } else {
       visibleMinSlow = visibleMinSlow + (minimum - visibleMinSlow)/3.0;   
       if (visibleMinSlow < minimum) {
          visibleMin = visibleMinSlow;
       }         
    }  

    // set a minimum full span view so very small value changes will not appear too noisy
    float minimumFullpan = 100.0; // 100u 
    float span = maximum - minimum;
    if (span < minimumFullpan) {
      float adjust= (minimumFullpan-span)/4.0;
      visibleMax = visibleMax + adjust;
      visibleMin = visibleMin - adjust;
    }    
  
    //  Add by 1000 to reduce decimal roudning problems.... should have stored value in uV and uA instead of mV and mA....
    maximum = maximum/1000.0;
    minimum = minimum/1000.0;
  
    // round the max and min to get more "round" values for top and bottom in graph
    visibleMax = ((int)(visibleMax/10.0) + 1) * 10;     
    visibleMax = visibleMax/1000.0;
    visibleMin = ((int)(visibleMin/10.0)) * 10;     
    visibleMin = visibleMin/1000.0;


    int pixelsPrSample = 6;
    
    viewHeight = 150;

    GD.Begin(LINE_STRIP);
    GD.LineWidth(30);
    GD.ColorRGB(type==DigitUtilClass::typeVoltage?COLOR_VOLT:COLOR_CURRENT);
 
    GD.ColorA(255);

    int i = endPtr;

    uispan = visibleMax - visibleMin;

    // main graph
    for (int pos=0; pos<nrOfTrendPoints;pos++) { 
      float height = viewHeight * ( (visibleMax - value[i]) / uispan);
      int xpos = x + pos*(1+pixelsPrSample);
      if (xpos>800) {
        return;
      }
      if (value[i] < undefinedValue){
        GD.Vertex2ii(xpos, y + height); 
      }
      i=i+1;
      if (i>nrOfTrendPoints - 1) {
        i=0;
      }  
    }

    i = endPtr;

    if (!limitDetails) {
      
      // max min graphing
      GD.LineWidth(40);
      GD.ColorA(150);

      for (int pos=0; pos<nrOfTrendPoints;pos++) { 
        float min = viewHeight * ( (visibleMax - valueExt[i][0]) / uispan);
        float max = viewHeight * ( (visibleMax - valueExt[i][1]) / uispan);
        int xpos = x + pos*(1+pixelsPrSample);
        if (xpos>800) {
          return;
        }
        if (value[i] < undefinedValue){
          GD.Begin(LINE_STRIP);
          // red color if difference between mean value and extremes are > 1%
          if (valueExt[i][0] < value[i]*0.99 || valueExt[i][1] > value[i]*1.01){
             GD.ColorRGB(0xff0000); // red
          } else {
             GD.ColorRGB(type==DigitUtilClass::typeVoltage?COLOR_VOLT:COLOR_CURRENT);
          }
  
          GD.Vertex2ii(xpos, y + min); 
          GD.Vertex2ii(xpos, y + max); 
        }
        i=i+1;
        if (i>nrOfTrendPoints - 1) {
          i=0;
        }  
      }
    }
    
    x=x-5;
    // show the span info with top and bottom
    float top = viewHeight * ( (visibleMax - maximum) / uispan);
    float bottom = viewHeight * ( (visibleMax - minimum) / uispan);
    GD.ColorRGB(0xdddddd);
    GD.ColorA(255);
    DIGIT_UTIL.renderValue(x-10, y + top - 18, maximum, 0, type); 
    GD.Begin(LINE_STRIP);
    GD.LineWidth(20);
    GD.Vertex2ii(x+5, y + top); 
    GD.Vertex2ii(x, y + top + 2); 
    //GD.Vertex2ii(x, y + (viewHeight)/2 - 5); 
    //GD.Vertex2ii(x-5, y + (viewHeight)/2 ); 
    //GD.Vertex2ii(x, y + (viewHeight)/2 + 5); 
    GD.Vertex2ii(x, y + bottom - 2);
    GD.Vertex2ii(x+5, y + bottom);
    // if close to bottom, put it at the bottom, trying to avoid "flicker" when minimum close to the bottom
    if (viewHeight - bottom > 5) {
      DIGIT_UTIL.renderValue(x-10, y + bottom + 5, minimum, 0, type); 
    } else {
       DIGIT_UTIL.renderValue(x-10,  y + viewHeight + 5 , minimum, 0, type); 
    }
    float actualSpan = maximum - minimum;
    GD.cmd_text(x-70+6, y + top + (bottom-top)/2 -15 , 26, 0, "Span");
    DIGIT_UTIL.renderValue(x-98,  y + top + (bottom-top)/2 , actualSpan, 0 , type); 
}


