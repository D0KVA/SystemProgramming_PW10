#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Stepper.h>
#include <Servo.h>

#define WIFI_SSID "52"
#define WIFI_PASSWORD "Gg13371488"
const byte Servo_PIN = D2;

#define BOT_TOKEN "7638002533:AAG5vhpXtae_eE4-bGEeOvKeH1QujnInWCc"      
Servo servo;


const unsigned long BOT_MTBS = 1000; 

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; 

const int ledPin = D1;
int ledStatus = 0;
const byte STEPS = 32;
Stepper stepper(STEPS, D6, D7, D8, D9);
bool flag = false;

void handleNewMessages(int numNewMessages)
{
  Serial.print("Обработка нового сообщения ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/ledon")
    {
      digitalWrite(ledPin, HIGH);
      ledStatus = 1;
      bot.sendMessage(chat_id, "Светодиод включен - ON", "");
    }

    if (text == "/ledoff")
    {
      ledStatus = 0;
      digitalWrite(ledPin, LOW);
      bot.sendMessage(chat_id, "Светодиод выключен - OFF", "");
    }

    if (text == "/Open")
    {
      servo.write(180);
      bot.sendMessage(chat_id, "Сервопривод - 180", "");
    }

    if (text == "/Close")
    {
      servo.write(0);
      bot.sendMessage(chat_id, "Сервопривод - 0", "");
    }

    if (text == "/ledChangePower")
    {
      analogWrite(ledPin, 128); 
      bot.sendMessage(chat_id, "Жоска поменял яркость на 128", "");
    }

    if (text == "/StopMotor")
    {
      flag = false;
      bot.sendMessage(chat_id, "Моторчик выключен", "");
    }

    if (text == "/StartMotor")
    {
      flag = true;
      bot.sendMessage(chat_id, "52", "");
    }

    if (text == "/status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "Светодиод включен - ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Светодиод выключен - OFF", "");
      }
    }

    if (text == "/start")
    {
      String welcome = "Привет, я Телеграм Бот. Я умею преключать светодиод на модуле WeMos D2 R1 " + from_name + ".\n";
      welcome += "А это перечень команд, которые я пока знаю.\n\n";
      welcome += "/ledon : Переключает светодиод в состояние ON\n";
      welcome += "/ledoff : Переключает светодиод в состояние OFF\n";
      welcome += "/Open : Устанавливает угол сервопривода на 180\n";
      welcome += "/Close : Устанавливает угол сервопривода на 0\n";
      welcome += "/status : Возвращает текущее состояние светодиода\n";
      welcome += "/ledChangePower : Меняет яркость светодиода на 128\n";
      welcome += "/StartMotor : Включает мотор\n";
      welcome += "/StopMotor : Выключает мотор\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  servo.attach(Servo_PIN);
  pinMode(ledPin, OUTPUT); // Настраиваем пин ledPin на выход
  delay(10);
  digitalWrite(ledPin, HIGH); // По умолчанию светодиод выключен

  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Подключение к сети Wifi SSID: ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi подключен. IP адрес: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Время подключения: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("Получен ответ");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
  if (flag == false){
      stepper.setSpeed(0);
      stepper.step(0);
  }
  if (flag == true){
    stepper.setSpeed(500);
    stepper.step(1);
  }
}