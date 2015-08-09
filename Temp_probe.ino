#include <DHT.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define DHTALIM 3
#define TRANSCEIVERALIM 4

DHT dht(DHTPIN, DHTTYPE);

int f_wdt=1;
int sleep_count=0;
int ack=0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(DHTALIM, OUTPUT);
  pinMode(TRANSCEIVERALIM, OUTPUT);
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}

void loop() {
  if((f_wdt == 1) && (sleep_count >= 0))
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

    /* Don't forget to clear the flag. */
    f_wdt = 0;
    
    sleep_count = 0;
    
    /* Re-enter sleep mode. */
    Serial.println("go back to sleep");
    delay(50);
    enterSleep();
  }
  else if(f_wdt == 1)
  {
    sleep_count++;
    f_wdt = 0;
    enterSleep();
  }
  else
  {
    /* Do nothing. */
  }
}

ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
}

void enterSleep(void)
{
  Serial.println("zzzzzz");
  delay(50);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
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
