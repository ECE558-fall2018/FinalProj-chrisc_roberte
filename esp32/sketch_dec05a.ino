
#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>

#define DEBUG 0

char* ssid = "BESH";
char* password =  "mysupersecurepassword";
String siteaddress = "http://bobbyeshleman.pythonanywhere.com/api/v1/session/";
String root = "http://bobbyeshleman.pythonanywhere.com/api/v1/";
int sessionIdentifier;
int score; 
int count;
String username = "None";
String sequence;
String usersequence;
bool useranswers; 

//Setting up pin definitions for switches and led's
//LED's:
#define RED_LED 23
#define ORA_LED 22
#define GRE_LED 14
#define BLU_LED 32
#define YEL_LED 15
#define WHI_LED 33

//Switches
#define RED_SWITCH 21
#define ORA_SWITCH 17
#define GRE_SWITCH 16
#define BLU_SWITCH 19
#define YEL_SWITCH 18
#define WHI_SWITCH 5


void deleteSession(int sessionIdentifier) {
  String url = siteaddress; 
  HTTPClient client;
  client.begin(url);
  client.addHeader("Content-Type", "text/plain");
  int resp = client.POST("delete=1");
  if (resp == 200) {
    Serial.println("deleteSession to " + String(score, DEC) + " is good");
  }  else {
    Serial.println("deleteSession failed");
  }
  client.end();
}

void postScore(String username, int score) {
  HTTPClient client;
  Serial.println("postScore(): " + username);
  client.begin(root + "score/" + username + "/?score=" + String(score));
  client.addHeader("Content-Type", "text/json");
  int resp = client.POST("score=" + String(score));
  if (resp == 200) {
    Serial.println("postScore to " + String(score) + " is good");
  }  else {
    Serial.println("postScore failed: " + client.getString());
  }
  client.end();
}

String getUserSequence(String username, String sequence) {
  HTTPClient client;
  client.begin(root + "/user/" + username + "/userpress/");
  client.addHeader("Content-Type", "text/plain");
  String response = "";
  String userentered = "";
  while (userentered == "") {
    client.GET();

    response = client.getString();
    Serial.println(response);
    int firstQuote = response.indexOf('"');
    int secondQuote = response.lastIndexOf('"');
    userentered = response.substring(firstQuote + 1, secondQuote);
  }
  Serial.println("userentered: " + userentered);
  delay(10000);
  return userentered;
  
}

void lightLED(int LED) {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}

void sequenceLED(int digit) {
  switch(digit) {
    case 1:
      lightLED(RED_LED);
      break;
    case 2:
      lightLED(ORA_LED);
      break;
    case 3:
      lightLED(GRE_LED);
      break;
    case 4:
      lightLED(BLU_LED);
      break;
    case 5:
      lightLED(YEL_LED);
      break;
    case 6:
      lightLED(WHI_LED);
      break;
    default:
      lightLED(RED_LED);
      lightLED(WHI_LED);
      lightLED(ORA_LED);
      lightLED(YEL_LED);
      break;
  } 
}

void showSessionID(int sessionID) {
  //String sessionTemp = String(sessionID);
  unsigned int firstDigit = (sessionID/1000U) % 10;
  sequenceLED(firstDigit);
  unsigned int secondDigit = (sessionID/100U) % 10;
  sequenceLED(secondDigit);
  unsigned int thirdDigit = (sessionID/10U) % 10;
  sequenceLED(thirdDigit);
  unsigned int fourthDigit = (sessionID/1U) % 10;
  sequenceLED(fourthDigit);
}

void getSessionID () {
  int firstDigit = random(1,5)*1000;
  int secondDigit = random(1,5)*100;
  int thirdDigit = random(1,5)*10;
  int fourthDigit = random(1,5);

  #if DEBUG
  int sessionID = 4444;
  #else
  int sessionID = firstDigit + secondDigit + thirdDigit + fourthDigit;
  #endif

  String sessionTemp = String(sessionID, DEC);
  Serial.println(sessionTemp);

  sessionIdentifier = sessionID;
  
  showSessionID(sessionID);
}

bool addSimonentry() {
  int entry = random(1,5);
  sequence = sequence + String(entry, DEC);
  
  for (int i = 0; i < sequence.length(); i++) {
    char arr[sequence.length()] = {'\0'};
    Serial.println("sequence: " + sequence + ", ");
    sequence.toCharArray(arr, sequence.length());
    char c = sequence.charAt(i);
    long seqNum = atol(&c);
    Serial.println("seqNum(): " + String(seqNum));
    Serial.println("digit: " + String(seqNum));
    sequenceLED(seqNum);
    delay(1000);
  }
  
  String userresponse = getUserSequence(username, sequence);
  if (userresponse == sequence) {
    useranswers = true;
    return useranswers;
  }
  else {
    useranswers = false;
    return useranswers;
  }
}

int setupWIFI() {
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
    HTTPClient http;   
    http.begin(siteaddress + sessionIdentifier + "/");  //Specify destination for HTTP request
    http.addHeader("Content-Type", "text/plain");             //Specify content-type header
 
    int httpResponseCode = http.POST("POSTING from ESP32");   //Send the actual POST request
 
    if(httpResponseCode>0){
      String response = http.getString();  //Get the response to the request
      int firstQuote = response.indexOf('"');
      int secondQuote = response.lastIndexOf('"');
      username =  response.substring(firstQuote + 1, secondQuote);
      //Send to game setup and running code
      //Serial.println(httpResponseCode);   //Print return code
      Serial.println(username);           //Print request answer
   }
   else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
 
   http.end();  //Free resources
 }
 else{
    Serial.println("Error in WiFi connection");   
 }
  delay(10000);  //Send a request every 10 seconds
}

void setup() {
  //Setting up game variables
  int game_type;
  
  //Start wifi setup
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin

  pinMode(RED_LED, OUTPUT);
  pinMode(ORA_LED, OUTPUT);
  pinMode(GRE_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(YEL_LED, OUTPUT);
  pinMode(WHI_LED, OUTPUT);

  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
  getSessionID();
  while (username == "None") {
    setupWIFI();
    showSessionID(sessionIdentifier);
    Serial.println(sessionIdentifier);
  }
}
 
void loop() {

  bool outcome = addSimonentry();
  if (outcome == true) {
    score = sequence.length();
    delay(1000);
  }
  else {
    postScore(username, score);
    
//    username = "None";
//    sequence = "";
//    getSessionID();
//    while (username == "None") {
//      setupWIFI();
//      showSessionID(sessionIdentifier);
//      Serial.println(sessionIdentifier); }
  }
}




