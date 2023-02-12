#include "color.h"
// Meter colour schemes
#define RED2RED     0
#define GREEN2GREEN 1
#define BLUE2BLUE   2
#define BLUE2RED    3
#define GREEN2RED   4
#define RED2GREEN   5
#define RAINBOW     6

#define xviolet        10
#define xblue          11
#define xlight_blue    12
#define xgreen         13
#define xyellow_green  14
#define xyellow        15
#define xorange        16
#define xred           17
#define xir            18
#define xClear         19

#include <TFT_eSPI.h>
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>

String labels[] = {"415", "455", "480", "515", "555", "590", "630", "680", "990" , "Clear"};

const char * ssid = "00000000";
const char * password = "00000000";

int bands[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


int vol = 34;
int hold = 38;
int d1, d2, d3, d4, d5, d6, d7, d8, d9, d10;
int xd1, xd2, xd3, xd4, xd5, xd6, xd7, xd8, xd9, xd10;
int yd1, yd2, yd3, yd4, yd5, yd6, yd7, yd8, yd9, yd10;
String zd1, zd2, zd3, zd4, zd5, zd6, zd7, zd8, zd9, zd10;
int AVG_d = 0;
int reading = 0;
unsigned long period = 20000;
unsigned long last_time = 0;

//=======web========
WebServer server(80);

// Adding a websocket to the server
WebSocketsServer webSocket = WebSocketsServer(81);

// Serving a web page (from flash memory)
char webpage[] PROGMEM = R"=====(
<html>
<!-- Adding a data chart using Chart.js -->
<head>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js'></script>
</head>
<body onload="javascript:init()">
<h2> Bilirubinometer Ver 2.3 </h2>
<div>
  <canvas id="chart" width="600" height="400"></canvas>
</div>
<!-- Adding a websocket to the client (webpage) -->
<script>
  var webSocket, dataPlot;
  var maxDataPoints = 20;
  const maxValue = 5000;
  const maxLow = maxValue * 0.5;
  const maxMedium = maxValue * 0.2;
  const maxHigh = maxValue * 0.3;

  function init() {
    webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
    dataPlot = new Chart(document.getElementById("chart"), {
      type: 'bar',
      data: {
        labels: [],
        datasets: [{
          data: [],
          label: "Low",
          backgroundColor: "#D6E9C6"
        },
        {
          data: [],
          label: "Moderate",
          backgroundColor: "#FAEBCC"
        },
        {
          data: [],
          label: "High",
          backgroundColor: "#EBCCD1"
        },
        ]
      }, 
      options: {
          responsive: false,
          animation: false,
          scales: {
              xAxes: [{ stacked: true }],
              yAxes: [{
                  display: true,
                  stacked: true,
                  ticks: {
                    beginAtZero: true,
                    steps: 1000,
                    stepValue: 500,
                    max: maxValue
                  }
              }]
           }
       }
    });
    webSocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
      dataPlot.data.labels = [];
      dataPlot.data.datasets[0].data = [];
      dataPlot.data.datasets[1].data = [];
      dataPlot.data.datasets[2].data = [];
      
      data.forEach(function(element) {
        dataPlot.data.labels.push(element.bin);
        var lowValue = Math.min(maxLow, element.value);
        dataPlot.data.datasets[0].data.push(lowValue);
        
        var mediumValue = Math.min(Math.max(0, element.value - lowValue), maxMedium);
        dataPlot.data.datasets[1].data.push(mediumValue);
        
        var highValue = Math.max(0, element.value - lowValue - mediumValue);
        dataPlot.data.datasets[2].data.push(highValue);

      });
      dataPlot.update();
    }
  }

</script>
</body>
</html>
)=====";



void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  // Do something with the data from the client
  if(type == WStype_TEXT){

  }
}
//===============WEB=================

void getData() {
  String json = "[";
  for (int i = 0; i < 10; i++) {
    if (i > 0) {
      json +=", ";
    }
    json += "{\"bin\":";
    json += "\"" + labels[i] + "\"";
    json += ", \"value\":";
    json += String(bands[i]);
    json += "}"; 
  }
  json += "]";
  webSocket.broadcastTXT(json.c_str(), json.length());
}

//--------socket------------
void setup(void) {

  
  pinMode(vol, INPUT);
  pinMode(hold, INPUT_PULLUP);

  Serial.begin(115200);

  tft.begin();

  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);
  tft.setCursor(8, 215);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.print (" Vio  Lid  Blu  Cya  Gre  Yel  Ora  Red  NIR  clr");
  tft.setCursor(8, 225);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.print (" 415  445  480  515  555  590  630  680  910  nm.");

  tft.setCursor(10, 10);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.print ("Bilirubinometer Ver 2.3");

  WiFi.begin(ssid, password);
  Serial.begin(115200);
  tft.setCursor(10, 10);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });

   tft.setCursor(200, 10);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.print ("IP:"+IpAddress2String(WiFi.localIP()));

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
}

int vo = 0;
int MA = 0;
float offset = 1.75;

void loop() {
   webSocket.loop();
   server.handleClient();
  
  //LED Range is 4mA to 258mA [ rotary_encode ]
  if (digitalRead(hold) == 0) {
  } else {
    MA = random(0, 128);
    if (MA < 4) {
      MA = 0;
    }
    else if (MA > 128) {
      MA = 120;
    }
    else {}

    //----------------------------------------------
    if ( millis() - last_time > period + last_time) {
      last_time = millis();
      tft.fillScreen(TFT_BLACK);
      //ESP.restart();
      tft.setRotation(1);
      tft.setCursor(8, 215);
      tft.setTextSize (0);
      tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
      tft.print (" Vio  Lid  Blu  Cya  Gre  Yel  Ora  Red  NIR  clr");
      tft.setCursor(8, 225);
      tft.setTextSize (0);
      tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
      tft.print (" 415  445  480  515  555  590  630  680  910  nm.");
      tft.setCursor(10, 10);
      tft.setTextSize (0);
      tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
      tft.print ("Bilirubinometer Ver 2.3 ");
      tft.setCursor(200, 10);
      tft.setTextSize (0);
      tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
      tft.print ("IP:"+IpAddress2String(WiFi.localIP()));
    }



    read_X();

    linearMeter(map(d1, 0, AVG_d * offset, 0, 20), 20,       10, 5, 25, 3, 20, xviolet);
    linearMeter(map(d2, 0, AVG_d * offset, 0, 20), 20,  10 + 30, 5, 25, 3, 20, xblue);
    linearMeter(map(d3, 0, AVG_d * offset, 0, 20), 20,  40 + 30, 5, 25, 3, 20, xlight_blue);
    linearMeter(map(d4, 0, AVG_d * offset, 0, 20), 20,  70 + 30, 5, 25, 3, 20, xgreen);
    linearMeter(map(d5, 0, AVG_d * offset, 0, 20), 20, 100 + 30, 5, 25, 3, 20, xyellow_green);
    linearMeter(map(d6, 0, AVG_d * offset, 0, 20), 20, 130 + 30, 5, 25, 3, 20, xyellow);
    linearMeter(map(d7, 0, AVG_d * offset, 0, 20), 20, 160 + 30, 5, 25, 3, 20, xorange);
    linearMeter(map(d8, 0, AVG_d * offset, 0, 20), 20, 190 + 30, 5, 25, 3, 20, xred);
    linearMeter(map(d9, 0, AVG_d * offset, 0, 20), 20, 220 + 30, 5, 25, 3, 20, xir);
    linearMeter(map(d10, 0, AVG_d * offset , 0, 20), 20, 250 + 30, 5, 25, 3, 20, RAINBOW);
    getData();

  }
}

void read_X() {
  tft.setRotation(1);
  tft.setCursor(10, 30);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.print ("LED : " + (String)MA + " mA. ");
  d1 = random(0, 5000);
  d2 = random(0, 5000);
  d3 = random(0, 5000);
  d4 = random(0, 5000);
  d5 = random(0, 5000);
  d6 = random(0, 5000);
  d7 = random(0, 5000);
  d8 = random(0, 5000);
  d10 = random(0,5000);
  d9 = random(0, 5000);

  bands[0] = d1;
  bands[1] = d2;
  bands[2] = d3;
  bands[3] = d4;
  bands[4] = d5;
  bands[5] = d6;
  bands[6] = d7;
  bands[7] = d8;
  bands[8] = d9;
  bands[9] = d10;
  
  delay(1000);
  
  AVG_d = (d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8 + d9 + 10) / 10;
  if (d1 > 1000) {
    xd1 = d1 / 1000;
    yd1 = d1 % 1000;
    yd1 = yd1 / 100;
    zd1 = (String)xd1 + "." + (String)yd1 + "k ";
  } else {
    zd1 = (String)d1 + " ";
  }
  if (d2 > 1000) {
    xd2 = d2 / 1000;
    yd2 = d2 % 1000;
    yd2 = yd2 / 100;
    zd2 = (String)xd2 + "." + (String)yd2 + "k ";
  } else {
    zd2 = (String)d2 + " ";
  }
  if (d3 > 1000) {
    xd3 = d3 / 1000;
    yd3 = d3 % 1000;
    yd3 = yd3 / 100;
    zd3 = (String)xd3 + "." + (String)yd3 + "k ";
  } else {
    zd3 = (String)d3 + " ";
  }
  if (d4 > 1000) {
    xd4 = d4 / 1000;
    yd4 = d4 % 1000;
    yd4 = yd4 / 100;
    zd4 = (String)xd4 + "." + (String)yd4 + "k ";
  } else {
    zd4 = (String)d4 + " ";
  }
  if (d5 > 1000) {
    xd5 = d5 / 1000;
    yd5 = d5 % 1000;
    yd5 = yd5 / 100;
    zd5 = (String)xd5 + "." + (String)yd5 + "k ";
  } else {
    zd5 = (String)d5 + " ";
  }
  if (d6 > 1000) {
    xd6 = d6 / 1000;
    yd6 = d6 % 1000;
    yd6 = yd6 / 100;
    zd6 = (String)xd6 + "." + (String)yd6 + "k ";
  } else {
    zd6 = (String)d6 + " ";
  }
  if (d7 > 1000) {
    xd7 = d7 / 1000;
    yd7 = d7 % 1000;
    yd7 = yd7 / 100;
    zd7 = (String)xd7 + "." + (String)yd7 + "k ";
  } else {
    zd7 = (String)d7 + " ";
  }
  if (d8 > 1000) {
    xd8 = d8 / 1000;
    yd8 = d8 % 1000;
    yd8 = yd8 / 100;
    zd8 = (String)xd8 + "." + (String)yd8 + "k ";
  } else {
    zd8 = (String)d8 + " ";
  }
  if (d9 > 1000) {
    xd9 = d9 / 1000;
    yd9 = d9 % 1000;
    yd9 = yd9 / 100;
    zd9 = (String)xd9 + "." + (String)yd9 + "k ";
  } else {
    zd9 = (String)d9 + " ";
  }
  if (d10 > 1000) {
    xd10 = d10 / 1000;
    yd10 = d10 % 1000;
    yd10 = yd10 / 100;
    zd10 = (String)xd10 + "." + (String)yd10 + "k ";
  } else {
    zd10 = (String)d10 + " ";
  }

  tft.setRotation(1);
  tft.setTextSize (0);
  tft.setTextColor ( TFT_YELLOW , TFT_BLACK);
  tft.setCursor(10, 42); tft.print (zd1 );
  tft.setCursor(10 + 30, 42); tft.print (zd2 );
  tft.setCursor(10 + 60, 42); tft.print (zd3 );
  tft.setCursor(10 + 90, 42); tft.print (zd4 );
  tft.setCursor(10 + 120, 42); tft.print (zd5);
  tft.setCursor(10 + 150, 42); tft.print (zd6);
  tft.setCursor(10 + 180, 42); tft.print (zd7);
  tft.setCursor(10 + 210, 42); tft.print (zd8);
  tft.setCursor(10 + 240, 42); tft.print (zd9);
  tft.setCursor(10 + 270, 42); tft.print (zd10);

  // Print out the stored values for each channel
  Serial.print("415nm:");
  Serial.print(d1);
  Serial.print("\t445nm:");
  Serial.print(d2);
  Serial.print("\t480nm:");
  Serial.print(d3);
  Serial.print("\t515nm:");
  Serial.print(d4);
  Serial.print("\t555nm:");
  Serial.print(d5);
  Serial.print("\t590nm:");
  Serial.print(d6);
  Serial.print("\t630nm:");
  Serial.print(d7);
  Serial.print("\t680nm:");
  Serial.print(d8);
  Serial.print("\tClear:");
  Serial.print(d9);
  Serial.print("\tIR:");
  Serial.println(d10);

  getData();
}


// #########################################################################
//  Draw the linear meter
// #########################################################################
// val =  reading to show (range is 0 to n)
// x, y = position of top left corner
// w, h = width and height of a single bar
// g    = pixel gap to next bar (can be 0)
// n    = number of segments
// s    = colour scheme
void linearMeter(int val, int x, int y, int w, int h, int g, int n, byte s)
{
  tft.setRotation(0);
  // Variable to save "value" text colour from scheme and set default
  int colour = TFT_BLUE;
  // Draw n colour blocks
  for (int b = 1; b <= n; b++) {
    if (val > 0 && b <= val) { // Fill in coloured blocks
      switch (s) {
        case 0: colour = TFT_RED; break; // Fixed colour
        case 1: colour = TFT_GREEN; break; // Fixed colour
        case 2: colour = TFT_BLUE; break; // Fixed colour
        case 3: colour = rainbowColor(map(b, 0, n, 127,   0)); break; // Blue to red
        case 4: colour = rainbowColor(map(b, 0, n,  63,   0)); break; // Green to red
        case 5: colour = rainbowColor(map(b, 0, n,   0,  63)); break; // Red to green
        case 6: colour = rainbowColor(map(b, 0, n,   0, 159)); break; // Rainbow (red to violet)
        //==================================================================
        case 10: colour = TFT_MAGENTA; break;
        case 11: colour = TFT_BLUE; break;
        case 12: colour = TFT_CYAN; break;
        case 13: colour = TFT_GREEN; break;
        case 14: colour = TFT_YELLOWGREEN; break;
        case 15: colour = TFT_YELLOW; break;
        case 16: colour = TFT_DARKORANGE ; break;
        case 17: colour = TFT_RED; break;
        case 18: colour = TFT_BROWN; break;
        case 19: colour = TFT_GREENYELLOW; break;
      }
      tft.fillRect(x + b * (w + g), y, w, h, colour);
    }
    else // Fill in blank segments
    {
      tft.fillRect(x + b * (w + g), y, w, h, TFT_DARKGREY);
    }
  }
}

/***************************************************************************************
** Function name:           rainbowColor
** Description:             Return a 16 bit rainbow colour
***************************************************************************************/
// If 'spectrum' is in the range 0-159 it is converted to a spectrum colour
// from 0 = red through to 127 = blue to 159 = violet
// Extending the range to 0-191 adds a further violet to red band

uint16_t rainbowColor(uint8_t spectrum)
{
  spectrum = spectrum % 192;

  uint8_t red   = 0; // Red is the top 5 bits of a 16 bit colour spectrum
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  uint8_t sector = spectrum >> 5;
  uint8_t amplit = spectrum & 0x1F;

  switch (sector)
  {
    case 0:
      red   = 0x1F;
      green = amplit; // Green ramps up
      blue  = 0;
      break;
    case 1:
      red   = 0x1F - amplit; // Red ramps down
      green = 0x1F;
      blue  = 0;
      break;
    case 2:
      red   = 0;
      green = 0x1F;
      blue  = amplit; // Blue ramps up
      break;
    case 3:
      red   = 0;
      green = 0x1F - amplit; // Green ramps down
      blue  = 0x1F;
      break;
    case 4:
      red   = amplit; // Red ramps up
      green = 0;
      blue  = 0x1F;
      break;
    case 5:
      red   = 0x1F;
      green = 0;
      blue  = 0x1F - amplit; // Blue ramps down
      break;
  }

  return red << 11 | green << 6 | blue;
}

String IpAddress2String(const IPAddress& ipAddress)
{
    
    String x = String(ipAddress[0]) + "." +
           String(ipAddress[1]) + "." +
           String(ipAddress[2]) + "." +
           String(ipAddress[3]);
           return x;
}
