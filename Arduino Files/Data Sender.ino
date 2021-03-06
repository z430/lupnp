#include <Arduino.h>
#include <dht.h>
#include <XBee.h>

#define DHT11_PIN 5
#define PIR_PIN 6

dht DHT;
XBee xbee = XBee();

uint8_t payload[] = {0,0};

// SH + SL Address of receiving XBee
XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00000000); //send to coordinator
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

ZBTxStatusResponse txStatus = ZBTxStatusResponse();

int hum;
int pir;

union u_tag {
	uint8_t b[4];
	float fval;
} u;

void setup() {
	//pirCallibration();
	Serial.begin(9600);
	xbee.setSerial(Serial);
}

void loop() {

	// check humidity
	hum = humCheck();
	pir = pirCheck();

	payload[0] = hum;
	payload[1] = pir;
/*	
	Serial.print(payload[0]);
	Serial.print(" ");
	Serial.println(payload[1]);


	u.fval = hum;
	for (int i = 0; i < 4; i++){
		payload[i] = u.b[1];
	}

	u.fval = pir;
	for (int i = 0; i < 4; i++){
		payload[i+4] = u.b[i];
	}
	*/
	xbee.send(zbTx);
	delay(100);


}

int humCheck(){
	int chk = DHT.read11(DHT11_PIN);
	return DHT.humidity;
}

int pirCheck(){

	return random(0, 12);

	//return digitalRead(PIR_PIN);
}

void pirCallibration(){
	int callibrationTime = 30;
	long unsigned int lowIn;
	long unsigned int pause = 5000;

	for(int i = 0;i < callibrationTime; i++){
		delay(1000);
	}
}