#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

// W5500 ピン設定
#define W5500_CS_PIN 5
#define W5500_INT_PIN 14

// UDP設定
#define UDP_SEND_PORT 22222 // 送り先のポート番号
#define UDP_RESV_PORT 22224 // このESP32のポート番号

// ネットワーク設定（適宜変更してください）
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 90, 2);     // ESP32のIP
IPAddress server(192, 168, 90, 1); // MacのIP（適宜変更）
IPAddress gateway(192, 168, 90, 1);
IPAddress subnet(255, 255, 255, 0);

// UDP関連
EthernetUDP udp;
char packetBuffer[255];
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 2000; // 2秒間隔

// 送信メッセージ（変更可能）
const char *sendMessage = "Hello, This is ESP32. Are you PC?";

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(100);
  }

  Serial.println("Starting ESP32 UDP Client with W5500");

  // SPI設定
  SPI.begin();

  // Ethernet開始
  Ethernet.init(W5500_CS_PIN);
  Ethernet.begin(mac, ip, gateway, subnet);

  // 接続確認
  if (Ethernet.hardwareStatus() == EthernetNoHardware)
  {
    Serial.println("Ethernet shield was not found.");
    while (true)
    {
      delay(1);
    }
  }

  // ネットワーク情報表示
  Serial.println("=== Network Information ===");
  Serial.print("ESP32 IP Address: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("Subnet Mask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("DNS Server: ");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println("============================");

  Serial.print("Target Mac IP: ");
  Serial.println(server);

  // UDP開始
  udp.begin(UDP_RESV_PORT);
  Serial.print("UDP listening on port: ");
  Serial.println(UDP_RESV_PORT);
  Serial.print("UDP sending to port: ");
  Serial.println(UDP_SEND_PORT);

  Serial.println("Setup complete!");
}

void loop()
{
  // UDP受信チェック
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    Serial.println("=== UDP Packet Received ===");
    Serial.print("Packet size: ");
    Serial.println(packetSize);
    Serial.print("From IP: ");
    IPAddress remote = udp.remoteIP();
    Serial.print(remote);
    Serial.print(", Port: ");
    Serial.println(udp.remotePort());

    // パケットをテキストとして読み取り
    memset(packetBuffer, 0, sizeof(packetBuffer)); // バッファクリア
    int len = udp.read(packetBuffer, sizeof(packetBuffer) - 1);
    if (len > 0)
    {
      packetBuffer[len] = '\0'; // 文字列終端
      Serial.print("Received Text: ");
      Serial.println(packetBuffer);
    }
    else
    {
      Serial.println("No data received");
    }
    Serial.println("===========================");
  }

  // 2秒ごとにメッセージ送信
  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= sendInterval)
  {
    Serial.println("Sending UDP packet to Mac...");

    udp.beginPacket(server, UDP_SEND_PORT);
    udp.write(sendMessage);
    udp.endPacket();

    Serial.print("Sent: ");
    Serial.println(sendMessage);

    lastSendTime = currentTime;
  }

  delay(10);
}