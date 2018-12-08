
#include <WiFi.h>
#include <HTTPClient.h>
#include <string.h>

#define DEBUG 0

//Setting up all of our global variables. This will alleviate 
//any additional memory use on the stack of the processor by keeping them
//as globals. 

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

//Didn't end up using this section but we used it as a stub.
//Switches:
#define RED_SWITCH 21
#define ORA_SWITCH 17
#define GRE_SWITCH 16
#define BLU_SWITCH 19
#define YEL_SWITCH 18
#define WHI_SWITCH 5

//This will delete the session from the app when we are done with it. 
void deleteSession(int sessionIdentifier) {
  //adding local variables for this function.
  String url = siteaddress; 
  HTTPClient client;
  
  //wifi start up section
  client.begin(url);
  client.addHeader("Content-Type", "text/plain");
  
  int resp = client.POST("delete=1");
  //checking for correct response from server. 
  if (resp == 200) {
    Serial.println("deleteSession to " + String(score, DEC) + " is good");
  }  else {
    Serial.println("deleteSession failed");
  }
  client.end();
}

//This section will post the high score to the server after every successful run
void postScore(String username, int score) {
  //setting up local variables for this function
  HTTPClient client;
  Serial.println("postScore(): " + username);
  
  //wifi setup for posting score. 
  client.begin(root + "score/" + username + "/?score=" + String(score));
  client.addHeader("Content-Type", "text/json");
  
  int resp = client.POST("score=" + String(score));
  //checking for correct response from server.
  if (resp == 200) {
    Serial.println("postScore to " + String(score) + " is good");
  }  else {
    Serial.println("postScore failed: " + client.getString());
  }
  client.end();
}

String getUserSequence(String username, String sequence) {
  //setting up local variables for this function
  HTTPClient client;
  String response = "";
  String userentered = "";
  
  //wifi setup for getting responses for usernames
  client.begin(root + "/user/" + username + "/userpress/");
  client.addHeader("Content-Type", "text/plain");
  
  //this is the section where we are waiting for a user before the game starts
  while (userentered == "") {
    client.GET();
    response = client.getString();
    int firstQuote = response.indexOf('"');
    int secondQuote = response.lastIndexOf('"');
    userentered = response.substring(firstQuote + 1, secondQuote);
  }
  
  delay(10000);
  return userentered;
}

//this will light up the led in accordance with our sequence
void lightLED(int LED) {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}

//a case statement for lighting leds
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
	//this is our error light up section
    default:
      lightLED(RED_LED);
      lightLED(WHI_LED);
      lightLED(ORA_LED);
      lightLED(YEL_LED);
      break;
  } 
}

//our led sequence lighting function. 
void showSessionID(int sessionID) {
  //we are using each digit to send to the lighting function
  unsigned int firstDigit = (sessionID/1000U) % 10;
  sequenceLED(firstDigit);
  unsigned int secondDigit = (sessionID/100U) % 10;
  sequenceLED(secondDigit);
  unsigned int thirdDigit = (sessionID/10U) % 10;
  sequenceLED(thirdDigit);
  unsigned int fourthDigit = (sessionID/1U) % 10;
  sequenceLED(fourthDigit);
}

//this will create our randomized sequence id knowing that we 
//can only access numbers 1-4 in each place in the sequence. 
void getSessionID () {
  int firstDigit = random(1,5)*1000;
  int secondDigit = random(1,5)*100;
  int thirdDigit = random(1,5)*10;
  int fourthDigit = random(1,5);

  //debugging sequence for troubleshooting.
  #if DEBUG
  int sessionID = 4444;
  #else
  int sessionID = firstDigit + secondDigit + thirdDigit + fourthDigit;
  #endif

  String sessionTemp = String(sessionID, DEC);

  sessionIdentifier = sessionID;
  showSessionID(sessionID);
}

//adding a new entry into a sequence for testing the users memory
//the sequence will be entered into a string instead of an array 
//so for easier checking and also smaller memory placement
bool addSimonentry() {

//setting up local variables
  int entry = random(1,5);
  sequence = sequence + String(entry, DEC);
  
  //creating and adding to the string
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
  
  //now sending the sequence to the led light up sequencing and 
  //then getting back user entry from the application for checking against 
  //the two strings
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

//This section will set up our connection to the server and allow for continuous 
//communication between the hardware, the app, and the server. 
int setupWIFI() {
  if(WiFi.status()== WL_CONNECTED){   //Check WiFi connection status
 
    HTTPClient http;   
	//Specify destination for HTTP request
    http.begin(siteaddress + sessionIdentifier + "/");  
	//Specify content-type header
    http.addHeader("Content-Type", "text/plain");             
	
	//Send the actual POST request
    int httpResponseCode = http.POST("ESP32 Wooooo");   
 
    //checking our response code and then cleaning the response for use by the hardware
    if(httpResponseCode>0){
      String response = http.getString();  //Get the response to the request
      int firstQuote = response.indexOf('"');
      int secondQuote = response.lastIndexOf('"');
      username =  response.substring(firstQuote + 1, secondQuote);
      //Send to game setup and running code

      Serial.println(username);           
   }
   //if our response code is no good!
   else{
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
   }
 
   http.end();  //Free resources
 }
 else{
    Serial.println("Error in WiFi connection");   
 }
  //send the request every 10 seconds
  delay(10000);  
}

void setup() {
  //Setting up game variables
  int game_type;
  
  //Start wifi setup
  Serial.begin(115200);
  delay(4000);   //Delay needed before calling the WiFi.begin

  //set up pins for output
  pinMode(RED_LED, OUTPUT);
  pinMode(ORA_LED, OUTPUT);
  pinMode(GRE_LED, OUTPUT);
  pinMode(BLU_LED, OUTPUT);
  pinMode(YEL_LED, OUTPUT);
  pinMode(WHI_LED, OUTPUT);

  //setting up the wifi connection (can't use WPA or WEP for this protocol)
  WiFi.begin(ssid, password); 
 
  //constantly try to connect every second
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  //wooo finally we are connected
  Serial.println("Connected to the WiFi network");
  getSessionID();
  
  //this will loop until we get a username
  while (username == "None") {
    setupWIFI();
    showSessionID(sessionIdentifier);
    Serial.println(sessionIdentifier);
  }
}
 
//our looping section will only be used as long as the user is correct in the 
//sequencing. 
void loop() {

  bool outcome = addSimonentry();
  if (outcome == true) {
    score = sequence.length();
    delay(1000);
  }
  else {
    postScore(username, score); 
  }
}




