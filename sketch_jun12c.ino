#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SimpleDHT.h>

#define DIGITAL_PIN 35
#define ANALOG_PIN 34
#define DHT11_PIN 15 
#define motor1A 13
#define motor2A 14
// Initialize Telegram BOT
#define BOTtoken "6940073116:AAFnvoHv21zR-XFF6Ytebg6XkHl4ndoBM2M"
// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "439932411"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

uint16_t gasVal;
boolean isgas = false;
String gas;
float temperature=0.0;
float humidity=0.0;

SimpleDHT11 dht11(DHT11_PIN);

const char* ssid = "Vodafone-C01530140";
const char* password = "JXxfp7PnhndMPR7Y";
/*
{
  //Your Domain name with URL path or IP address with path
}
*/
const char* serverName = "http://192.168.1.15/IoTProject/dataGathering.php";
/*
{
  // the following variables are unsigned longs because the time, measured in
  // milliseconds, will quickly become a bigger number than can be stored in an int.
}
*/
unsigned long lastTime = 0;
/*
{
  // Timer set to 10 minutes (600000)
  //unsigned long timerDelay = 600000;
  // Set timer to 5 seconds (5000)
}
*/
unsigned long timerDelay = 0;

void setup() {
  Serial.begin(115200);
  // initialize digital pin as an output.
  pinMode(motor1A, OUTPUT);
  pinMode(motor2A, OUTPUT); 
  digitalWrite(motor1A, LOW);     
  digitalWrite(motor2A, LOW); 
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  bot.sendMessage(CHAT_ID, "Bot started up", "");
  Serial.println("The sensor is warming up...");
  delay(30000);
  pinMode(DIGITAL_PIN, INPUT);
}

void loop() {
  getGasVal();
  getIsGas();
  getTempAndHum();
  //printTempAndHum();
  //printGas();
  sendData();
  if((temperature>=55.0)||(gasVal>100.0)){
    isgas=true;
    activateFan();
    sendToMartin();
  }else{
  isgas=false;
  getIsGas();
  digitalWrite(motor1A, LOW);     
  digitalWrite(motor2A, LOW);
  }
  Serial.println("Temperatura " + String(temperature,1) + "\nUmidità " + String(humidity,1) +
                 "\nValore gas " + String(gasVal) + "\nLanciare allarme? " + gas);

  delay(120000);
}

void activateFan(){
  // Rotate
  digitalWrite(motor1A, HIGH);     
  digitalWrite(motor2A, LOW);   
  delay(2000);
}


void sendData(){
    //Send an HTTP POST request every 10 minutes
    if ((millis() - lastTime) > timerDelay) {
      //Check WiFi connection status
      if(WiFi.status()== WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;
      
        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName);
        
        // If you need Node-RED/server authentication, insert user and password below
        http.setAuthorization("root", "");
        
        // Specify content-type header
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        // Data to send with HTTP POST
        String httpRequestData = "api_key=tPmAT5Ab3j7F9&temperature="+ String(temperature,1) +"&humidity=" + 
                                  String(humidity,1) + "&gasVal=" + String(gasVal) + "&alarm=" + gas;           
        // Send HTTP POST request
        int httpResponseCode = http.POST(httpRequestData);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        http.end();
      }
      else {
        Serial.println("WiFi Disconnected");
        while(WiFi.status()== WL_CONNECTED){
          WiFi.begin(ssid, password);
          delay(500);
        }
      }
      lastTime = millis();
    }
}

void getGasVal(){
  gasVal = analogRead(ANALOG_PIN);
  gasVal = map(gasVal, 0, 1023, 0, 100);
}

void getIsGas(){
  isgas=digitalRead(DIGITAL_PIN);
  if (gasVal>100) {
      gas = "Yes";
 }
 else {
      gas = "No";
 }
}
void printGas(){
    Serial.print("Rilevazione gas: ");
    Serial.println(gas);
    Serial.print("Valore gas: ");
    Serial.print(gasVal);
    Serial.println("\n");
    //Serial.println("Sample DHT11... ");
}

void getTempAndHum(){
  int err = SimpleDHTErrSuccess;
  if((err=dht11.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess){
    Serial.print("Lettura DHT11 fallita, err=");
    Serial.print(err);
    return;
  }
}

void printTempAndHum(){
   Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.println(" *C ");
    Serial.print("Umidità: ");
   Serial.print(humidity);
   Serial.print(" RH% \n");
}

void sendToMartin(){
  bot.sendMessage(CHAT_ID, "Warning! Temperature or gas levels above the norm! Activating ventilation...", "");
}
