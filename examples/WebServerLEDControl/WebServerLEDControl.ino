/**
    Web Server LED Control

    @author Miguel Alvarez

    @date    27-FEB-2015

    @details A sketch that displays an HTML page with three
            buttons to toggle three digital pins. You may
            connect an LED to pins 2,3, and 5 with a 1k
            resistor. Then visit the IP address (printed in the
            serial window) in a web browser. Tested on this
            version of the shield:
            http://www.seeedstudio.com/depot/W5200-Ethernet-Shield-p-1577.html

            For more detail instructions and example output see:
            http://www.seeedstudio.com/wiki/Ethernet_Shield_V2.4

    @circuit Stack the W5200 Ethernet Shield from Seeed Studio on
            the Arduino board. Connect Three leds and 1k
            resistors to pins 2, 3, and 5.
*/

#include <SPI.h>
#include <EthernetV2_0.h>

int pins[] = {2, 3, 5}; // the digital pins you want to control.
// NOTE: do not attempt to control the pins used by the SPI bus or 0,1, and 4 pins

// define the controller's MAC address
byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// set the server at port 80 for HTTP
int port = 80;
String ipAddress = "";
EthernetServer server(port);
// Ethernet controller SPI CS pin
#define W5200_CS  10
// SD card SPI CS pin
#define SDCARD_CS 4

void setup() {
    // the pins to be OUTPUTS and be LOW intially
    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
        pinMode(pins[i], OUTPUT);
        digitalWrite(pins[i], LOW);
    }

    Serial.begin(9600);
    // make sure that the SD card is not selected for the SPI port
    // by setting the CS pin to HIGH
    pinMode(SDCARD_CS, OUTPUT);
    digitalWrite(SDCARD_CS, HIGH);
    // loop until we get a connection to the access point
    while (Ethernet.begin(mac) == 0) {
        Serial.println("DHCP configuration failed. Trying again...");
    }
    // now that we have a good ethernet connection we can start the server
    server.begin();
    Serial.print("The server can be accessed at: ");
    IPAddress ip = Ethernet.localIP();
    ipAddress += ip[0];
    ipAddress += ".";
    ipAddress += ip[1];
    ipAddress += ".";
    ipAddress += ip[2];
    ipAddress += ".";
    ipAddress += ip[3];
    Serial.println(ipAddress);
}

void loop() {
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);

                // look for the string "pin=" in the browser/client request
                if (!client.find("pin=")) {
                    // the string "pin=" is not present in the browser/client HTTP request
                    // so display the buttons webpage.

                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connnection: close");
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    // Now the HTML page body follows.
                    client.print("<html>");
                    client.print("<head>");
                    client.print("<title>WiFi Shield Webpage</title>");
                    client.print("</head>");
                    client.print("<body>");
                    client.print("<h1>LED Toggle Webpage</h1>");

                    // Create the HTML button code e.g. <button id="xx" class="led">Toggle Pin xx</button> where xx is the pin number
                    // In the <button id="xx" class="led"> tags, the ID attribute is the value sent to the arduino via the "pin" GET parameter
                    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) {
                        String buttonHTML = "<button id=\"";
                        buttonHTML += pins[i];
                        buttonHTML += "\" class=\"led\">Toggle Pin ";
                        buttonHTML += pins[i];
                        buttonHTML += "</button><br />";
                        client.println(buttonHTML);
                    }

                    // Include the JQuery library. Note: the computer that loads this page will need an internet connection.
                    client.print("<script src=\"http://ajax.googleapis.com/ajax/libs/jquery/2.1.3/jquery.min.js\"></script>"); // include the JQuery library hosted by Google
                    // Javascript code that receives the button clicks and the correct pin back to the shield
                    client.print("<script type=\"text/javascript\">"); // beg javascript code
                    client.print("$(document).ready(function(){");
                    client.print("$(\".led\").click(function(){");
                    client.print("var p = $(this).attr('id');"); // get id value (i.e. pin13, pin12, or pin11)

                    // create the JQuery $.get function "$.get(\"http://ipaddress:port/\", {pin:p},function(data){alert(data)});"
                    // The .get functions sends and HTTP GET request to the IP address of the shield with the parameter "pin" and value "p", then executes the function alert
                    String get = "$.get(\"http://";
                    get += ipAddress;
                    get += ":";
                    get += port;
                    get += "/\", {pin:p},function(data){alert(data)});";
                    client.print(get);
                    client.print("});"); // close the click function
                    client.print("});"); // close the read function
                    client.print("</script>"); // end the javascript snippet
                    client.print("</body>"); // end HTML body
                    client.print("</html>"); // end HTML
                    break;
                } else {
                    // The string "pin=" is found in the browser/client rquest, this meanst that the user wants to toggle the LEDs
                    int pinNumber = (client.read() - 48); // get first number i.e. if the pin 13 then the 1st number is 1
                    int secondNumber = (client.read() - 48);
                    if (secondNumber >= 0 && secondNumber <= 9) {
                        pinNumber *= 10;
                        pinNumber +=
                            secondNumber; // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
                    }

                    digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin

                    // Build pinstate string. The Arduino replies to the browser with this string.
                    String pinState = "Pin ";
                    pinState += pinNumber;
                    pinState += " is ";
                    if (digitalRead(pinNumber)) { // check if the pin is ON or OFF
                        pinState += "ON"; // the pin is on
                    } else {
                        pinState += "OFF"; // the pin is off
                    }

                    // Send HTTP response back to browser/client
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connnection: close");
                    client.println();
                    client.println(pinState);
                    break;
                }

            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        Serial.println("client disonnected");
    }
}
