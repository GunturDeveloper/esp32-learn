#include <WiFi.h>

#include <WiFiClientSecure.h>

#include <UniversalTelegramBot.h>

#include <ESP32Servo.h>

const char* ssid = "";

const char* password = "";

#define BOTtoken ""

#define CHAT_ID ""

Servo myServo;

const int SERVO_PIN = 2;

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

    Serial.println("Request diterima dari: " + from_name);

    Serial.println("Pesan: " + text);

    if (text == "/start") {

      String control = "Selamat Datang, " + from_name + ".\n";

      control += "Kirimkan sudut untuk menggerakkan servo (0-180 derajat).\n";

      bot.sendMessage(chat_id, control, "");

      Serial.println("Mengirim pesan sambutan...");

    } else {

      int angle = text.toInt();

      if (angle >= 0 && angle <= 180) {

        myServo.write(angle);

        bot.sendMessage(chat_id, "Servo bergerak ke sudut " + String(angle) + " derajat", "");

        Serial.println("Servo bergerak ke sudut: " + String(angle) + " derajat");

      } else {

        bot.sendMessage(chat_id, "Masukkan sudut yang valid antara 0 dan 180 derajat.", "");

        Serial.println("Sudut tidak valid: " + String(angle));

      }

    }

  }

}

void setup() {

  Serial.begin(115200);

  ESP32PWM::allocateTimer(0);

  ESP32PWM::allocateTimer(1);

  myServo.attach(SERVO_PIN);

  myServo.write(90);

  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  #ifdef ESP32

    client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  #endif

  while (WiFi.status() != WL_CONNECTED) {

    delay(1000);

    Serial.println("Connecting to WiFi..");

  }

  Serial.println("Terhubung ke WiFi, IP Address: " + WiFi.localIP().toString());

}

void loop() {

  if (millis() > lastTimeBotRan + botRequestDelay) {

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {

      handleNewMessages(numNewMessages);

      numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    }

    lastTimeBotRan = millis();

  }

}
