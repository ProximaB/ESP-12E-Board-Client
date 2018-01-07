/*
* WebSocketClient.ino
*
*  Created on: 24.05.2015
*
*/

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//#define USE_SERIAL Serial1

const char* ssid = "TP-LINK_A1A5A5";
const char* password = "28387756";

const char* host = "192.168.0.106";
const char* port = "57493";

const int led = 2;
const int _switch = 13;
const int button = 14;
const int button2 = 4;

String receive_id, state;
String payloadStr;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch (type) {
	case WStype_DISCONNECTED:
		//USE_SERIAL.printf("[WSc] Disconnected!\n");
		Serial.print("[WSc] Disconnected!\n");
		break;
	case WStype_CONNECTED: {
		//USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
		Serial.print("[WSc] Connected to url : ");
		Serial.print((char*)payload);
		// send message to server when Connected
		//webSocket.sendTXT("Connected");
	}
						   break;
	case WStype_TEXT:
		//USE_SERIAL.printf("[WSc] get text: %s\n", payload);
		Serial.print("[WSc] get text: \n");
		Serial.print((char*)payload);
		Serial.print("\n");

		payloadStr = (char*)payload;
		
		for (int i = 0; i < payloadStr.length(); i++) {
			if (payloadStr[i] == ':') {
				receive_id = payloadStr.substring(0, i);
				state = payloadStr.substring(i + 1, i + 3);
				break;
			}
		}
		Serial.print("payloadStr: " + payloadStr + "\n");

		Serial.print("state " + state + "\n");
		Serial.print("Receive_id: " + receive_id + "\n");
		

		if (receive_id == "8") {
			if (state[0] == 'O' && state[1] == 'N')
		{
			Serial.print("payload==ON");
			digitalWrite(led, 0);
			digitalWrite(_switch, 1);
			
		}
		else
		{
			Serial.print("payload==OFF");
			digitalWrite(led, 1);
			digitalWrite(_switch, 0);
		}
		// send message to server
		//webSocket.sendTXT("message here");
		}
		break;
	case WStype_BIN:
		//USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
		Serial.print("[WSc] get binary length: ");
		Serial.print(length);
		Serial.print("\n");

		hexdump(payload, length);

		// send data to server
		webSocket.sendBIN(payload, length);
		break;
	}

}

void setup() {
	// USE_SERIAL.begin(921600);
	//USE_SERIAL.begin(115200);
	Serial.begin(115200);

	pinMode(led, OUTPUT);
	pinMode(_switch, OUTPUT);
	pinMode(button, INPUT);
	pinMode(button2, INPUT);
	digitalWrite(_switch, 1);
	digitalWrite(led, 1);

	//Serial.setDebugOutput(true);
	//USE_SERIAL.setDebugOutput(true);
	//USE_SERIAL.println();
	//USE_SERIAL.println();
	//USE_SERIAL.println();

	for (uint8_t t = 4; t > 0; t--) {
		//USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		Serial.print("[SETUP] BOOT WAIT");
		Serial.print(t);
		Serial.print("\n");
		//USE_SERIAL.flush();
		delay(1000);
	}

	WiFiMulti.addAP("TP-LINK_A1A5A5", "28387756");

	//WiFi.disconnect();
	while (WiFiMulti.run() != WL_CONNECTED) {
		delay(100);	
	}
	Serial.print("Trying connect to server.\n");
	//USE_SERIAL.printf("Trying connect to server.\n");
	// server address, port and URL
	webSocket.begin("192.168.0.106", 57493, "/notifications");

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	Serial.print("Zakonczono!!\n");
	webSocket.setReconnectInterval(5000);

}

int _button, _button2;

void loop() {
	webSocket.loop();

	_button = digitalRead(button);
	_button2 = digitalRead(button);
	delay(100);

	if (digitalRead(button) == 1 && _button == 0) {
		digitalWrite(led, 0);
		digitalWrite(_switch, 1);
		webSocket.sendTXT("8:ON");
		Serial.print("Button1 ON");
	}
	else if(digitalRead(button2) == 1 && _button2 == 0)
	{
		digitalWrite(led, 1);
		digitalWrite(_switch, 0);
		webSocket.sendTXT("8:OFF");
		Serial.print("Button2 OFF");
	}
	
	
}
