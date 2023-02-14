#include <SoftwareSerial.h>
#define RX 2
#define TX 3
#include <Adafruit_MLX90614.h>
#include <Wire.h>
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>     // Includes the PulseSensorPlayground Library. 
#include <LiquidCrystal_I2C.h> // Library modul I2C LCD
String AP = "Nisaaaa";       // WIFI NAME
String PASS = "00000000"; // WIFI PASSWORD
String API = "U3PFAJOQ5UEMAY2E";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
float temp_data1;
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED13 = 13;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 550;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value.
SoftwareSerial esp8266(RX,TX);
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3f, 16, 2);                                
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"
Adafruit_MLX90614 MLX_Sensor = Adafruit_MLX90614(); 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  //sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK"); upload password Wifi
  lcd.init();
  lcd.backlight();
  lcd.clear();  
  MLX_Sensor.begin();  
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);   
  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
}
}

void loop() {
 int myBPM = pulseSensor.getBeatsPerMinute();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BPM: ");                        // Print phrase "BPM: "
  lcd.setCursor(5,0); 
  lcd.print(myBPM);
  lcd.setCursor(0,1);
  lcd.print("SuhuOb: ");
  lcd.setCursor(8,1);     
  lcd.print(temp_data1);
  lcd.setCursor(14,1);
  lcd.print((char)223);
  lcd.setCursor(15,1);      
  lcd.print("C");
  delay(100);
 String getData = "GET /update?api_key="+ API +"&field1="+bpm()+"&field2="+suhu();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(10);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}

int getSensorData(){
  int myBPM = pulseSensor.getBeatsPerMinute();  // Calls function on our pulseSensor object that returns BPM as an "int".
                                               // "myBPM" hold this BPM value now. 
  //Display_Temperature('A'); //Get Abient temperature in Celsius
  //Display_Temperature('B'); //Get object temperature in Celsius
 temp_data1 = MLX_Sensor.readObjectTempC();
 Serial.print("BPM: ");                        // Print phrase "BPM: " 
 Serial.println(myBPM); //Menampilkan BPM diserial
 Serial.print("Suhu Ob: ");
 Serial.print(temp_data1); //Menampilkan Suhu di serial
 Serial.println("########");

}
String bpm(){
  int myBPM = pulseSensor.getBeatsPerMinute();
  Serial.print("BPM: ");// Print phrase "BPM: " 
  Serial.println(myBPM); //Menampilkan BPM diserial
  return String(myBPM);
}

String suhu(){
   temp_data1 = MLX_Sensor.readObjectTempC();
   Serial.print("Suhu Ob: ");
   Serial.print(temp_data1);
   Serial.print("°");      
   Serial.println("C");
   return String(temp_data1);
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
