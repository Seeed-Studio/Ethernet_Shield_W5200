/********************************************************

    This example shows how to connect a simple web server read that shows
    the value of the analog input pins. And then write the data to and read
    from an SD card file--"test.txt".

    The W5200 circuit:
    W5200 on Ethernet Shield V2.0 by seeed attached to SPI bus as follows:
 ** ----------------
 **| W5200 - Arduino|
 **| MOSI  - MOSI   |
 **| MISO  - MISO   |
 **| CLK   - SCK    |
 **| CS    - pin 10 |
 ** ----------------

    The other Circuit:
    SD card attached to SPI bus as follows:
 ** ----------------
 **| SD    - Arduino|
 **| MOSI  - MOSI   |
 **| MISO  - MISO   |
 **| CLK   - SCK    |
 **| CS    - pin 4  |
 ** ----------------

    Analog inputs attached to pins A0 through A5 (optional).

    Author: Frankie.Chu at Seeed Studio.
    Date:   2012-11-20

 *******************************************************/

#include <SD.h>
#include <SPI.h>
#include <EthernetV2_0.h>

#define W5200_CS  10
#define SDCARD_CS 4
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

File myFile;

void setup() {
    Serial.begin(9600);
    Serial.print("Initializing SD card...");
    // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
    // Note that even if it's not used as the CS pin, the hardware SS pin
    // (10 on most Arduino boards, 53 on the Mega) must be left as an output
    // or the SD library functions will not work.
    pinMode(W5200_CS, OUTPUT);
    //disconnect the W5200
    digitalWrite(W5200_CS, HIGH);

    pinMode(SDCARD_CS, OUTPUT);
    if (!SD.begin(SDCARD_CS)) {
        Serial.println("initialization failed!");
        return;
    }
    Serial.println("initialization done.");

    // open the file. note that only one file can be opened at one time.
    myFile = SD.open("test.txt", FILE_WRITE);
    //write to the file after it's successfully opened or created:
    if (myFile) {
        Serial.print("Writing to test.txt...");
        myFile.println("testing 1, 2, 3.");
        // close the file
        myFile.close();
        Serial.println("done.");
    } else {
        //failed.
        Serial.println("error opening test.txt");
    }

    // re-open the file for reading:
    myFile = SD.open("test.txt");
    if (myFile) {
        Serial.println("test.txt:");

        // read all data from the file:
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
    } else {
        // failed
        Serial.println("error opening test.txt");
    }
    // disconnect the SD card
    digitalWrite(SDCARD_CS, HIGH);

    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
}

int buff[6];

void loop() {
    //listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
        // an http request ends with a blank line
        Serial.println("new client");
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connnection: close");
                    client.println();
                    client.println("<!DOCTYPE HTML>");
                    client.println("<html>");
                    // add a meta refresh tag, so the browser pulls again every 5 seconds:
                    client.println("<meta http-equiv=\"refresh\" content=\"5\">");

                    // output the value of each analog input pin
                    for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
                        client.print("analog input ");
                        client.print(analogChannel);
                        client.print(" is ");
                        buff[analogChannel] = analogRead(analogChannel);
                        client.print(buff[analogChannel]);
                        client.println("<br />");
                    }
                    break;
                }
                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                } else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
        // give the web browser time to receive the data
        delay(1);
        // close the connection:
        client.stop();
        //disconnect the W5200
        digitalWrite(W5200_CS, HIGH);

        //write the content of buffer to test.txt
        myFile = SD.open("test.txt", FILE_WRITE);
        if (myFile) {

            for (int i = 0; i < 6; i++) {
                myFile.print("analog input ");
                myFile.print(i);
                myFile.print(" is ");
                myFile.println(buff[i]);
            }
            // write all content of buffer to the file
            myFile.close();
        } else {
            //failed
            Serial.println("error opening test.txt");
        }

        //read all data from test.txt
        myFile = SD.open("test.txt");
        if (myFile) {
            Serial.println("test.txt opened:");
            // read all data from the file
            while (myFile.available()) {
                //send the content onto the serial monitor
                Serial.write(myFile.read());
            }
            // close the file
            myFile.close();
        } else {
            //failed
            Serial.println("error opening test.txt");
        }
        //disconnect the SD card
        digitalWrite(SDCARD_CS, HIGH);
    }
}

