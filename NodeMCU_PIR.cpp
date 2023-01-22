#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
const char* ssid = "Wi-Fi Name"; 
const char* password = "Password";
const char *host = "maker.ifttt.com";
const char *privateKey = "Private Key";
ESP8266WebServer server(80); //Server on port 80
void send_event(const char *event);
int buzzer = 14;   //Buzzer  alarm  connected to GPIO-14 or D5 of nodemcu
int PIRsensor = 5; //PIR sensor output connected to GPIO-5 or D1 of nodemcu 
String Message;
const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
  <title>Data Logger</title>
  <h1 style="text-align:center; color:red;">Iot Design Pro</h1>
  <h3 style="text-align:center;">Motion Detector</h3>
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }
  /* Data Table Styling*/ 
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
    text-align: center;
  }
  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }
  #dataTable tr:nth-child(even){background-color: #f2f2f2;}
  #dataTable tr:hover {background-color: #ddd;}
  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: center;
    background-color: #050505;
    color: white;
  }
  </style>
</head>
<body>   
<div>
  <table id="dataTable">
    <tr><th>Time</th><th>Activity</th></tr>
  </table>
</div>
<br>
<br>  
<script>
var Avalues = [];
//var timeStamp = [];
var dateStamp = [];
setInterval(function() {
  // Call a function repetatively with 5 Second interval
  getData();
}, 3000); //5000mSeconds update rate
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
 // var time = new Date().toLocaleTimeString();
  var date = new Date();
  var txt = this.responseText;
  var obj = JSON.parse(txt); 
      Avalues.push(obj.Activity);
     // timeStamp.push(time);
      dateStamp.push(date);
  //Update Data Table
    var table = document.getElementById("dataTable");
    var row = table.insertRow(1); //Add after headings
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    cell1.innerHTML = date;
    //cell2.innerHTML = time;
    cell2.innerHTML = obj.Activity;
    }
  };
  xhttp.open("GET", "readData", true); //Handle readData server on ESP8266
  xhttp.send();
}    
</script>
</body>
</html>

)====="; 
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
void readData() {
  int state = digitalRead(PIRsensor); //Continuously check the state of PIR sensor
  delay(500);                         //Check state of PIR after every half second
  Serial.print(state);
    if(state == HIGH){ 
    digitalWrite (buzzer, HIGH);    //If intrusion detected ring the buzzer
    delay(1000);
    digitalWrite (buzzer, LOW);
    Message = "Motion Deteced";
    String data = "{\"Activity\":\""+ String(Message) +"\"}";
    server.send(200, "text/plane", data); //Send ADC value, temperature and humidity JSON to client ajax request
    send_event("motion_event");               
    Serial.println("Motion detected!");
    }
}
void setup() {
 Serial.begin(9600);
 Serial.print("Connecting to Wifi Network");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println("");
 Serial.println("Successfully connected to WiFi.");
 Serial.println("IP address is : ");
 Serial.println(WiFi.localIP());
 server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
 server.on("/readData", readData); //This page is called by java Script AJAX
 server.begin();                  //Start server
 Serial.println("HTTP server started");
 pinMode(PIRsensor, INPUT); // PIR sensor as input  
 pinMode(buzzer, OUTPUT);   // Buzzer alaram as output
 digitalWrite (buzzer, LOW);// Initially buzzer off
}
void loop(){
  server.handleClient();          //Handle client requests 
}
void send_event(const char *event)
{
  Serial.print("Connecting to "); 
  Serial.println(host); 
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  } 
  // We now create a URI for the request
  String url = "/trigger/";
  url += event;
  url += "/with/key/";
  url += privateKey; 
  Serial.print("Requesting URL: ");
  Serial.println(url);  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  while(client.connected())
  {
    if(client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    } else {
      // No data yet, wait a bit
      delay(50);
    };
  }  
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}
