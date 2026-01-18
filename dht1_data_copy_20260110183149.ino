#include <WiFiS3.h>
#include <ThingSpeak.h>
#include <DHT.h>

// -------- WiFi Credentials --------
char ssid[] = "VodafoneF2CB76";
char pass[] = "chdeg2XhyfPkNyct";

// -------- ThingSpeak Details ------
unsigned long channelID = 3220897;
const char *apiKey = "24X4WNYJAMURTAFY";

// -------- Sensor Pins -------------
#define DHTPIN 2
#define DHTTYPE DHT11
#define MQ2_PIN A0
#define RAIN_PIN A1

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  ThingSpeak.begin(client);
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gasValue = analogRead(MQ2_PIN);
  int rainValue = analogRead(RAIN_PIN);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read DHT sensor!");
    return;
  }

  // -------- Rain Status ----------
  String rainStatus;
  int rainStatusCode;
  if (rainValue < 500) {
    rainStatus = "YES";
    rainStatusCode = 1;
  } else {
    rainStatus = "NO";
    rainStatusCode = 0;
  }

  // -------- Gas Status -----------
  String gasStatus;
  int gasStatusCode;
  if (gasValue < 300) {
    gasStatus = "GOOD";
    gasStatusCode = 1;
  } else if (gasValue < 600) {
    gasStatus = "MODERATE";
    gasStatusCode = 2;
  } else {
    gasStatus = "BAD";
    gasStatusCode = 3;
  }

  // -------- Serial Monitor Output --------
  Serial.println("------ Smart Weather Monitoring ------");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Gas Value: "); Serial.print(gasValue);
  Serial.print(" → Status: "); Serial.println(gasStatus);
  Serial.print("Rain Value: "); Serial.print(rainValue);
  Serial.print(" → Rain: "); Serial.println(rainStatus);
  Serial.println("-------------------------------------");

  // -------- ThingSpeak Upload --------
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, gasValue);
  ThingSpeak.setField(4, rainValue);
  ThingSpeak.setField(5, rainStatusCode);  // 0 = NO, 1 = YES
  ThingSpeak.setField(6, gasStatusCode);   // 1 = GOOD, 2 = MODERATE, 3 = BAD

  int response = ThingSpeak.writeFields(channelID, apiKey);

  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("Error sending data to ThingSpeak!");
  }

  delay(20000);
}



