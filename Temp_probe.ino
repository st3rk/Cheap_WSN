#include <LowPower.h>
#include <DHT.h>


#define DHTPIN 2
#define DHTTYPE DHT11
#define DHTALIM 3
#define TRANSCEIVERALIM 4

DHT dht(DHTPIN, DHTTYPE);

int sleep_index=0;
int ack=0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(DHTALIM, OUTPUT);
  pinMode(TRANSCEIVERALIM, OUTPUT);
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
      while (sendData(t_b, h_b) != 69)
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

int sendData(int temp, int hum)
{
  ack = 0;
  //dummy bytes
  Serial.write(169);
  Serial.write(244);
  Serial.write(190);
  //humidity
  Serial.write(hum);
  //255 - humidity
  Serial.write(255-hum);
  //temperature + 25
  //(temperature can be negative)
  Serial.write(temp+25);
  //230 - temperature
  Serial.write(255-temp);
  //for redunduncy, temperature + humidity
  Serial.write(hum+temp);
  //temperature + humidity - 255
  Serial.write(hum+temp);
  //dummy bytes
  Serial.write(190);
  Serial.write(244);
  Serial.write(169);
  delay(200);
  if (Serial.available() > 0) {
    ack=Serial.read();
  }
  else {
    ack=0;
  }
  return ack;
}
