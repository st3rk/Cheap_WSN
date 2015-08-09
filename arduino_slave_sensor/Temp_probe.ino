#include <LowPower.h>
#include <DHT.h>


#define DHTPIN 2
#define DHTTYPE DHT11
#define DHTALIM 3
#define TRANSCEIVERALIM 4

DHT dht(DHTPIN, DHTTYPE);

int sleep_index=0;
int mote_addr = 69;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(DHTALIM, OUTPUT);
  pinMode(TRANSCEIVERALIM, OUTPUT);
}

int sendData(int temp, int hum)
{
	int send_temp, cs_temp, cs_hum, redund, cs_redund;
  int ack = 0;
	int dummy_byte1=169;
	int dummy_byte2=244;
	int dummy_byte3=190;

	send_temp = temp + 25;
	cs_temp = 255 - (temp + 25);
	cs_hum = 255 - hum;
	redund = hum + temp + 25;
	cs_redund = 255 - (hum + temp + 25);

	byte packet[] = {dummy_byte1, dummy_byte2, dummy_byte3, mote_addr, hum, cs_hum, send_temp, cs_temp, redund, cs_redund, dummy_byte3, dummy_byte2, dummy_byte1};
  Serial.write(packet, 13);

  delay(200);
  if (Serial.available() > 0) {
    ack=Serial.read();
  }
  else {
    ack=0;
  }
  return ack;
}

void loop() {
  if (sleep_index >= 0)
  {
    
    Serial.println("UP!");

    digitalWrite(DHTALIM, HIGH);
    digitalWrite(TRANSCEIVERALIM, HIGH);
    delay(3000);
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    byte h_b;
    byte t_b;
    h_b = (byte) h;
    t_b = (byte) t;
  
    digitalWrite(DHTALIM, LOW);
  
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      while (sendData(t_b, h_b) != mote_addr)
      {
        delay(50);
      }
    }
    digitalWrite(TRANSCEIVERALIM, LOW);

    sleep_index = 0;
    
    /* Re-enter sleep mode. */
    Serial.println("go back to sleep");
    delay(50);
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); 
  }
  else
  {
    sleep_index++;
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }
}
