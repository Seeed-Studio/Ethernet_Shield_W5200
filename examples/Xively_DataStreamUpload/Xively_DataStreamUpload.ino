#include <SPI.h>
#include <EthernetV2_0.h>
#include <HttpClient.h>
#include <Xively.h>

// MAC address for your Ethernet shield
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02  };

// Your Xively key to let you upload data
char xivelyKey[] = "niUYuSJkjqyzPFwnWqpApm7lLNv8fInUD6ijAoRrikqKFWbg";

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)
int sensorPin = A5;

// Define the strings for our datastream IDs
char sensorId[] = "Sound";
XivelyDatastream datastreams[] = {
    XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(571464242, datastreams, 1 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    Serial.println("Starting single datastream upload to Xively...");
    Serial.println();

    while (Ethernet.begin(mac) != 1) {
        Serial.println("Error getting IP address via DHCP, trying again...");
        delay(5000);
    }
    // print your local IP address:
    Serial.print("My IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
        // print the value of each byte of the IP address:
        Serial.print(Ethernet.localIP()[thisByte], DEC);
        Serial.print(".");
    }
    Serial.println();
}

void loop() {
    int sensorValue = analogRead(sensorPin);
    datastreams[0].setFloat(sensorValue);

    Serial.print("Read sensor value ");
    Serial.println(datastreams[0].getFloat());

    Serial.println("Uploading it to Xively");
    int ret = xivelyclient.put(feed, xivelyKey);
    Serial.print("xivelyclient.put returned ");
    Serial.println(ret);

    Serial.println();
    delay(15000);
}
