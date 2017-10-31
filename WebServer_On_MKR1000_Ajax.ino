/*
  An Arduino code to provide a generic basic web server that produces a 
  webpage that displays values of some 3 exemplary variables called X,Y,Z. 
  It allows AJAX CORS. To use it:
  1- Enter the SSID and PWD below for your network.
  2- Upload to the MKR1000.
  3- Find the IP of this MKR1000 via the Serial Monitor. If the
     WiFi connection is successful the IP should be printed in 
     the Serial Monitor in few seconds.
  4- Go to a browser on a computer on the same network, and enter any of these
     URLs (REST style):
     http://MKR1000IPAddress/XYZ 
     http://MKR1000IPAddress/X
     http://MKR1000IPAddress/Y
     http://MKR1000IPAddress/Z    
  One can change the server to accept different URLs and serve other variables
  and different web pages. The variables would be measurements from the Arduino
  sensors.
  S.M.
 */
#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "";//  your network SSID (name)
char pass[] = "";// your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
             
int status = WL_IDLE_STATUS;
WiFiServer server(80);          
String HTTPResponseHeaders = "";//Assuming all responses will use the same headers 

void setup() {
  Serial.begin(9600);      // initialize serial communication
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);       // don't continue
  }

  // attempt to connect to WiFi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();                           // start the web server on port 80
  printWiFiStatus();                        // you're connected now, so print out the 
  
  /* HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
     and a content-type so the client knows what's coming, then a blank line: */
  HTTPResponseHeaders += "HTTP/1.1 200 OK\n";
  HTTPResponseHeaders += "Access-Control-Allow-Origin: *\n";    //This is needed for AJAX CORS
  HTTPResponseHeaders += "Access-Control-Allow-Methods: GET\n"; //This is needed for AJAX CORS
  HTTPResponseHeaders += "Content-type:text/html\n";
  HTTPResponseHeaders += "\n";
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine  = "";                // make a String to hold incoming data from the client
    String HTTPRequest  = "";
    String HTTPResponse = HTTPResponseHeaders + "";
    
    boolean XYZRequested = false;
    
    /*This loop is meant to read the full HTTP request. Note the end of a request is marked by two \n */
    /* client.connected() is true as long as the client is connected OR disconnected but still 
       some data is in the stream WiFiClient inherits from Stream */
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        HTTPRequest += c;
        //Serial.write(c);                    // print it out the serial monitor
        /* Check if we hit the end of the request */
        if(HTTPRequest.endsWith("\r\n\r\n")) {//This is the end of HTTPRequests
          break; //We have received the whole request
        }      
      }      
    }

    /* Now process the request and send the corresponding response
       Structure of the request  GET /XYZ HTTP/1.1
                                 Host: 172.20.10.11 ...
    */
    int slashidx   = HTTPRequest.indexOf("/"); //Locate the /... 
    int HTTPidx    = HTTPRequest.indexOf("HTTP/"); //Locate HTTP
    String command = HTTPRequest.substring(slashidx,HTTPidx); //GET
    command.trim();//Remove extra spaces
    
    Serial.println("**************** Recvd HTTP Request ********************");
    Serial.println(HTTPRequest);
    Serial.println("**************** Resource requested ********************");
    Serial.println(command);

    HTTPResponse += ("<h2 style=\"color:green\">" + String("Command:") + command + "</h2> </br>\n");
    
    if(command == "/XYZ") {
       HTTPResponse += "X=" + String(random(0,10)) + " ";
       HTTPResponse += "Y=" + String(random(0,10)) + " ";
       HTTPResponse += "Z=" + String(random(0,10)) + " "; 
    } else if (command == "/X"){ 
       HTTPResponse += "X=" + String(random(0,10)) + " ";
    } else if (command == "/Y") {
       HTTPResponse += "Y=" + String(random(0,10)) + " ";
    } else if (command == "/Z") {
       HTTPResponse += "Z=" + String(random(0,10)) + " ";
    } else {
       HTTPResponse +=   "<h1 style=\"color:red\">" + String("Wrong request. Please use </br>" ) + 
                          "http://MKR1000IPAddress/XYZ </br> or </br>" + 
                          String("http://MKR1000IPAddress/X </br> or </br>")  + " ... </h1>";
    }
    //Send the HTTPResponse to the client    
    client.println(HTTPResponse);
    //Request-Response transaction done, close the connection:
    client.stop();
    Serial.println("client disonnected");
    Serial.println("*********************************************************");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
