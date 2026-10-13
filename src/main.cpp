#include "WiFi.h"
#include "PubSubClient.h" //pio lib install "knolleary/PubSubClient"

#define SSID          "IB3"
#define PWD           "ingenieursbeleving3"

#define MQTT_SERVER   "192.168.0.190"
#define MQTT_PORT     1883

#define LED_PIN       2

#define mqtt_user "pswgenerator"
#define mqtt_password "pswofpswgenerator"


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

unsigned long long validTags[7] = {1241507010ULL, 1131320889430145, 1248637872444032, 1130122906113409, 1281768468399492, 1228337857, 1324203623003008};
int remainingTags = 7;

void callback(char *topic, byte *message, unsigned int length);

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi..");

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{

  Serial.begin(115200);

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  pinMode(LED_PIN, OUTPUT);
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "tag")
{
    unsigned long long receivedTag =
        strtoull(messageTemp.c_str(), NULL, 10);

    for (int i = 0; i < remainingTags; i++)
    {
        if (receivedTag == validTags[i])
        {
            Serial.println("Geldige tag gevonden");

            // verwijder tag uit array
            validTags[i] = validTags[remainingTags - 1];
            remainingTags--;

            Serial.print("Nog ");
            Serial.print(remainingTags);
            Serial.println(" tags te vinden");

            break;
        }
    }

    if (remainingTags == 0)
    {
        String password = "";

        for (int i = 0; i < 8; i++)
        {
            password += String(random(0, 10));
        }

        Serial.print("Wachtwoord: ");
        Serial.println("superGeheimWachtwoord123");

        client.publish("password", password.c_str());
    }
}

}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // creat unique client ID
    // in Mosquitto broker enable anom. access
    if (client.connect("ESP8266Client"), mqtt_user, mqtt_password)
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("tag");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000)
  {
    lastMsg = now;
  }
}
