#include <WiFi.h> 

#include <WiFiClientSecure.h> 

#include <UniversalTelegramBot.h> 

#include <DHT.h> 

const char* ssid = "Guntur";

const char* password = "burunggagak";

#define BOTtoken "7832024760:AAEaz4JIav0KNea601wHgklvy71mjAjYvtk"

#define CHAT_ID "7543457749"

#define DHTPIN 2 

#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE); 

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

            control += "/suhu - Untuk mendapatkan data suhu\n"; 

            control += "/kelembapan - Untuk mendapatkan data kelembapan\n"; 

            bot.sendMessage(chat_id, control, ""); 

            Serial.println("Mengirim pesan sambutan...");

        } else if (text == "/suhu") {

            float suhu = dht.readTemperature(); 

            if (isnan(suhu)) {

                bot.sendMessage(chat_id, "Gagal membaca suhu dari sensor", ""); 

                Serial.println("Gagal membaca suhu dari sensor DHT11");

            } else {

                bot.sendMessage(chat_id, "Suhu: " + String(suhu) + "°C", ""); 

                Serial.println("Suhu yang dikirim: " + String(suhu) + " Â°C");

            }

        } else if (text == "/kelembapan") {

            float kelembapan = dht.readHumidity(); 

            if (isnan(kelembapan)) {

                bot.sendMessage(chat_id, "Gagal membaca kelembapan dari sensor", ""); 

                Serial.println("Gagal membaca kelembapan dari sensor DHT11");

            } else {

                bot.sendMessage(chat_id, "Kelembapan: " + String(kelembapan) + " %", ""); 

                Serial.println("Kelembapan yang dikirim: " + String(kelembapan) + " %");

            }

        }

    }

}

void setup() {

    Serial.begin(115200); 

    dht.begin(); 

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