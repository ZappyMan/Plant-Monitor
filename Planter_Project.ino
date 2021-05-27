#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define SCREEN_WIDTH 128                                // OLED display width, in pixels
#define SCREEN_HEIGHT 64                                // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET  4                     // Reset pin # (basically useless)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int analogUV = 15; // Analog PIN in for UV sensor
const int analogM = 2;  // Analog PIN in for moister sensor

IPAddress server_addr(128,193,85,19);  // IP of the MySQL *server* here
char user[] = "chimiene-db";              // MySQL user login username
char password[] = "zPLoIouQtjsrUzD5";        // MySQL user login password

const char* ssid = "Pizanos Pizzeria";    //For wifi connection
const char* pass = "12383vceba";

// Sample query
char INSERT_SQL[] = "INSERT INTO chimiene-db.Sensor (light,water)";

WiFiClient client;                 // Use this for WiFi instead of EthernetClient
MySQL_Connection conn(&client);
MySQL_Cursor* cursor;

float light;  //UV sensor
float water;  //Moister sensor

void display_data(int light, int water){  //Input moister and UV sensor data to be displayed on OLED
  display.clearDisplay();
  display.setTextSize(2); //Draw text at 2X scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.print(F("UV: ")); //Print text
  display.print(map(light,0,4096,0,100));   // Print %
  display.println(F("%")); //Print text
  display.print(F("Soil: ")); //Print text
  display.print(map(water,3450,0,0,100));   // Print %
  display.println(F("%")); //Print text
  display.display();    // Print data to screen
}

void setup() {
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {    //IF SCREEN IS NOT DETECTED//
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  WiFi.begin(ssid, pass);                                  //Connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

  Serial.print("Connecting to SQL...  ");
  if (conn.connect(server_addr, 3306, user, password))
    Serial.println("OK.");
  else
    Serial.println("FAILED.");
  
  // create MySQL cursor object
  cursor = new MySQL_Cursor(&conn);
  
  display.clearDisplay();    // Clear the buffer
  display.display();        //Turn on Display
  
}

void loop() {
  display.clearDisplay();
  light = analogRead(analogUV); //UV SENSOR in window hit maximum of 25. However when moving the sensor it hit 45.
  water = analogRead(analogM);  //Moister Sensor  309 == ALL WATER  742 == DRY AIR
  Serial.print("Light = ");
  Serial.println(light);
  Serial.print("Moisture = ");
  Serial.println(water);

  if (conn.connected())
    cursor->execute(INSERT_SQL);
  
  display_data(light,water);
  delay(5000);
}
