#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

#define BOTtoken "BOT TOKEN"
#define CHAT_ID "CHAT ID"

const int LED1_PIN = 2;
const int LED2_PIN = 4;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
   }

    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String control = "Selamat Datang, " + from_name + ".\n";
      control += "/led1on - Nyalakan LED 1\n";
      control += "/led1off - Matikan LED 1\n";
      control += "/led2on - Nyalakan LED 2\n";
      control += "/led2off - Matikan LED 2\n";
      bot.sendMessage(chat_id, control, "");
    } else if (text == "/led1on") {
      digitalWrite(LED1_PIN, HIGH);
      bot.sendMessage(chat_id, "LED 1 Menyala", "");
    }  else if (text == "/led1off") {
      digitalWrite(LED1_PIN, LOW);
      bot.sendMessage(chat_id, "LED 1 Mati", "");
  }  else if (text == "/led2on") {
      digitalWrite(LED2_PIN, HIGH);
      bot.sendMessage(chat_id, "LED 2 Menyala", "");
  } else if (text == "/led2off") {
      digitalWrite(LED2_PIN, LOW);
      bot.sendMessage(chat_id, "LED 2 Mati", "");
   } 
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
