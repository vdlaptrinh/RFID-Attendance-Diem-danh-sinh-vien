//*******************************libraries********************************
//RFID-----------------------------
//#include <SPI.h>
//#include <MFRC522.h>
#include <SoftwareSerial.h>
//NodeMCU--------------------------
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
SoftwareSerial RFID(D6, D5); // D6 - TX (RFID), D5 - RX (RFID)
//************************************************************************
//#define SS_PIN  D2  //D2
//#define RST_PIN D1  //D1
//************************************************************************
//MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
//************************************************************************
/* Set these to your desired credentials. */
const char *ssid = "1041 lau 2";
const char *password = "hoilamgi";
const char* device_token  = "8ceb36c810343326";
//************************************************************************
String URL = "http://192.168.20.113:80//rfidattendance/getdata.php"; //computer IP or the server domain
String getData, Link;
String OldCardID = "";
String CardID ="";
unsigned long previousMillis = 0;

int flag=0,RX_Flag=0;
char Code[14];
long Num=0; 
int BUZZ = D1;
int LED1 = D3;
int LED2 = D7;
int redLed = D0;
int blueLed = D4;
int i;
long temp=0, last=0;
//************************************************************************
void setup() {
  //delay(1000);
  Serial.begin(115200);
  delay(100);
  RFID.begin(9600);
  pinMode(BUZZ, OUTPUT); 
  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT);
  pinMode(blueLed, OUTPUT); 
  pinMode(redLed, OUTPUT);
  //SPI.begin();  // Init SPI bus
  //mfrc522.PCD_Init(); // Init MFRC522 card
  //---------------------------------------------
  connectToWiFi();
}
//************************************************************************
void loop() {
  //check if there's a connection to Wi-Fi or not
  if(!WiFi.isConnected()){
    connectToWiFi();    //Retry to connect to Wi-Fi
  }
  //---------------------------------------------
  if (millis() - previousMillis >= 15000) {
    previousMillis = millis();
    OldCardID="";
  }
  delay(50);
  //---------------------------------------------
  //look for new card
  
  ChuongTrinhDocThe();
  if(Num) {
    CardID = String(Num);
    Num=0;
    //---------------------------------------------
    if( CardID == OldCardID ){
      return;
    }
    else{
      OldCardID = CardID;
    }
    //---------------------------------------------
    //  Serial.println(CardID);
    SendCardID(CardID);
    delay(1000);
  }
}
void ChuongTrinhDocThe()
{
    Read_ID();
    if(RX_Flag==1)
    {
      digitalWrite(BUZZ, HIGH); 
      delay(200);
      digitalWrite(BUZZ, LOW);
      for(i=5;i<11;i++)//
      {
        Num<<=4;
        if(Code[i]>64)  Num+=((Code[i])-55);
        else Num+=((Code[i])-48);    
        //Serial.println(Num);
      } 
      Serial.println(Num);
      //SendCardID( String(Num));
      while(RFID.read()>0);//
      RX_Flag=0;//
      //Num=0; 
    }
}
void Read_ID(void)
{
    int i=0;
    char temp;
    for(i=0;(RFID.available()>0);i++)//
    //while(RFID.available()>0)
    {
      temp=RFID.read();
      //Serial.print(temp);
      delay(2);
      //Serial.println(i);
      if(temp==0X02)  //recieve the ssrt bit
        {
         flag=1;i=0;RX_Flag=0;//
        }
      if(flag==1)//detect the start bit and recieve data
      {
        if(temp==0X03)//detect the end code,
        {
         flag=0;  //zero clearing
         if(i==13) RX_Flag=1;//
         else RX_Flag=0;
         break;
        }
        Code[i]=temp;
      } 
    }       
     flag=0;//
}
//************send the Card UID to the website*************
void SendCardID( String Card_uid ){
  Serial.println("Sending the Card ID");
  if(WiFi.isConnected()){
    WiFiClient client;
    HTTPClient http;    //Declare object of class HTTPClient
    //GET Data
    getData = "?card_uid=" + String(Card_uid) + "&device_token=" + String(device_token); // Add the Card ID to the GET array in order to send it
    //GET methode
    Link = URL + getData;
    http.begin(client,Link); //initiate HTTP request   //Specify content-type header
    
    int httpCode = http.GET();   //Send the request
    String payload = http.getString();    //Get the response payload

//    Serial.println(Link);   //Print HTTP return code
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(Card_uid);     //Print Card ID
    Serial.println(payload);    //Print request response payload

    if (httpCode == 200) {
      if (payload.substring(0, 5) == "login") {
        String user_name = payload.substring(5);
    //  Serial.println(user_name);

      }
      else if (payload.substring(0, 6) == "logout") {
        String user_name = payload.substring(6);
    //  Serial.println(user_name);
        
      }
      else if (payload == "succesful") {

      }
      else if (payload == "available") {

      }
      delay(100);
      http.end();  //Close connection
    }
  }
}
//********************connect to the WiFi******************
void connectToWiFi(){
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected");
  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
    delay(1000);
}
//=======================================================================
