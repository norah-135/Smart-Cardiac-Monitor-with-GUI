

#define WIFI_SSID "Lama's iPhone (2)"
#define WIFI_PASSWORD "28062005"


#define BLYNK_PRINT Serial
#include <WiFiS3.h>
#include <BlynkSimpleWifi.h>
char auth[] = "AASEHCJHqLbwy-NXXlC2d8yesmP9vgXr";  

WidgetLED led_normal(V1);
WidgetLED led_alarm(V2);   


#define samp_siz 4
#define rise_threshold 4
#define read_interval 100

int sensorPin = A0;

unsigned long previousMillis = 0;
float print_value;

unsigned long send_delay = 0;

void setup() {
  Serial.begin(9600);

  Serial.println("Connecting to Wifi...");

  Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD, IPAddress(161, 8, 88, 105), 8080);

  Serial.println("Connected :)");
}

void loop() {
  Blynk.run();
  read_heart_rate();

  if (millis() - send_delay > 5000) {

    
    Blynk.virtualWrite(V0, print_value);

    if (print_value >= 30 && print_value <= 100) {
      led_normal.on();
      led_alarm.off();
    } else {
      led_normal.off();
      led_alarm.on();
    }


    send_delay = millis();
  }
}


void read_heart_rate() {

  static float reads[samp_siz] = { 0 }; 
  static float sum = 0;                  
  static long int index = 0;               
  static float before = 0;   // it is the previous smoothed signal           

  float last, reader, start;   //last is the avg last reading
  float first = 0, second = 0, third = 0;
  static bool rising = false;     
  static int rise_count = 0;      
  static long int last_beat = 0;  

  unsigned long currentMillis = millis();

  
  if (currentMillis - previousMillis >= read_interval) {
    previousMillis = currentMillis;  

    int n = 0;
    reader = 0;
    start = millis();

    do {
      reader += analogRead(sensorPin); 
      n++;
    } while (millis() < start + 20);  

    reader /= n;  

    Serial.print("Raw Value: ");
    Serial.println(reader);  

    sum -= reads[index];
    sum += reader;
    reads[index] = reader;
    last = sum / samp_siz;  

    // Check for a rising edge (heartbeat detection)
    if (last > before) { // if this signal is larger than the previous
      rise_count++;  
      if (!rising && rise_count > rise_threshold) {
        rising = true;    // Heartbeat detected 

        first = millis() - last_beat;  
        last_beat = millis(); 

        print_value = 60000. / (0.4 * first + 0.3 * second + 0.3 * third);

        Serial.print("Heart Rate: ");
        Serial.println(print_value);

        third = second;
        second = first;
      }
    } else {
      rising = false;  
      rise_count = 0; 
    }

    before = last;

    index++;
    index %= samp_siz;  
  }
}