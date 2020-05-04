#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt.h"
#include "http_parser.h"
#include "secret_config.h"

#define RMT_TX_CHANNEL RMT_CHANNEL_4
#define RMT_TX_GPIO_NUM GPIO_NUM_33

#define RMT_RX_CHANNEL RMT_CHANNEL_0
#define RMT_RX_GPIO_NUM GPIO_NUM_35

#define RMT_CLK_DIV 100
#define RMT_TICK_10_US (80000000 / RMT_CLK_DIV / 100000)
#define rmt_item32_TIMEOUT_US 10000

#define MAX_SIGNAL_LEN 1024

uint32_t turnOff_data[] = {0x054b8af1, 0x01318180, 0x012e8188, 0x03ed818a, 0x03ed8183, 0x01308182, 0x03ee8189, 0x012f8182, 0x012f8189, 0x012f8189, 0x03ee8189, 0x012f8182, 0x012d818a, 0x03ee818b, 0x012f8181, 0x03ee818a, 0x012c8183, 0x03ed818b, 0x012e8183, 0x012d818b, 0x03ed818a, 0x012c8184, 0x012c818c, 0x012c818c, 0x012d818c, 0x03ee818a, 0x03ec8183, 0x03ec8184, 0x03ed8184, 0x012c8183, 0x03ec818c, 0x012d8184, 0x012c818b, 0x012b818d, 0x03ed818c, 0x03ed8183, 0x012c8183, 0x012b818d, 0x03ec818d, 0x012c8184, 0x012b818c, 0x0000818d, 0x25a525a5, 0x25a525a5, 0x25a525a5, 0x25a525a5, 0x25a525a5, 0x25a525a5, 0x25a525a5, 0x5ead3eef, 0x3aadd678, 0xbffb3601, 0xbffb3600, 0x800080f1, 0x80008000, 0xbffb3538, 0xbffb3538, 0x80008001, 0xbffb3560, 0x806f8018, 0x80018000, 0x80008004, 0x80008000, 0xbffcfd24, 0x80008000, 0xbffb3544, 0x4008f095, 0xbffb3572, 0x80008000, 0x8000b00c, 0x1d008227, 0x863c9400, 0x81308000, 0x80008008, 0x800f9896, 0xbffb7738, 0x80008800, 0xcac4bd34, 0x4fa4f0ee, 0xecf20312, 0x8908bd34, 0xbd348000, 0xf0eecac4, 0x03124fa4, 0xbd34ecf2, 0xf0eecac4, 0x2aaa8040, 0x80008003, 0x83010e88, 0x8302df00, 0x8000800a, 0x80008000, 0x800a8000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0xad008000, 0x154c33b9, 0x868d6e43, 0x8b72f454, 0x80622442, 0x2dbe6f00, 0x2d56f8de, 0x800081ba, 0xbffb3d49, 0xbffb5394, 0x6f008002, 0xf8de2dbe, 0x80ba2d56, 0xbffb3d49, 0xbffb5394, 0x8408ff00, 0x80008000, 0x5d408000, 0x72508031, 0x804a9004, 0x5ead3eef, 0x3aadd678, 0xbffb3d49, 0xbffb5394, 0x1fe591cf, 0x51f2ad96, 0x9081895f, 0x8301803c, 0x8600c910, 0x5ead3eef, 0x3aadd678, 0xbffb3d49, 0xbffb5394, 0x80008044, 0x80008000, 0xbffb3682, 0x802a802a, 0x80008000, 0x80008000, 0x80008000, 0x7fff8000, 0x7fff7fff, 0x03124fa4, 0x8608ecf2, 0x80088100, 0x81008406, 0x03124fa4, 0x28c0ecf2, 0x8000c201, 0x80008000, 0xc20128c0, 0x3aadd678};
uint32_t turnOn_data[] = {0x054c8aee, 0x012a817f, 0x0128818e, 0x03f28190, 0x03f1817e, 0x0129817f, 0x03f18190, 0x012b817d, 0x012a818e, 0x0127818f, 0x03f18190, 0x01288180, 0x01288190, 0x03f2818f, 0x0129817e, 0x03f2818f, 0x012a817f, 0x03f2818e, 0x0128817e, 0x01288190, 0x03f28190, 0x012b817d, 0x0127818f, 0x01278191, 0x01278191, 0x03f18191, 0x012a817e, 0x03f0818f, 0x03f2817f, 0x012a817e, 0x03f0818f, 0x01288180, 0x01278190, 0x01298191, 0x0128818f, 0x03f08190, 0x01278180, 0x01278191, 0x03f08191, 0x01268181, 0x01268192, 0x00008191, 0x4fa4f0ee, 0xecf20312, 0x8908bd34, 0xbd348000, 0xf0eecac4, 0x03124fa4, 0xbd34ecf2, 0xf0eecac4, 0x2aaa8040, 0x80008003, 0x83010e88, 0x8302df00, 0x8000800a, 0x80008000, 0x800a8000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0x80008000, 0xad008000, 0x154c33b9, 0x868d6e43, 0x8b72f454, 0x80622442, 0x2dbe6f00, 0x2d56f8de, 0x800081ba, 0xbffb3d49, 0xbffb5394, 0x6f008002, 0xf8de2dbe, 0x80ba2d56, 0xbffb3d49, 0xbffb5394, 0x8408ff00, 0x80008000, 0x5d408000, 0x72508031, 0x804a9004, 0x5ead3eef, 0x3aadd678, 0xbffb3d49, 0xbffb5394, 0x1fe591cf, 0x51f2ad96, 0x9081895f, 0x8301803c, 0x8600c910, 0x5ead3eef, 0x3aadd678, 0xbffb3d49, 0xbffb5394, 0x80008044, 0x80008000, 0xbffb3682, 0x802a802a, 0x80008000, 0x80008000, 0x80008000, 0x7fff8000, 0x7fff7fff, 0x03124fa4, 0x8608ecf2, 0x80088100, 0x81008406, 0x03124fa4, 0x28c0ecf2, 0x8000c201, 0x80008000, 0xc20128c0, 0x3aadd678, 0xbffb376d, 0xbffb376c, 0x800080aa, 0x80008000, 0xbffb36ec, 0xbffb36ec, 0x80008001, 0xbffb3714, 0x8038801a, 0x80018000, 0x80008001, 0x80008000, 0xbffcfd24, 0x80008000, 0xbffb36f8, 0x4005a04e, 0xbffb371c, 0x80008000, 0x8000b009, 0x1d008117, 0x84949300, 0x81308306, 0x80008000, 0x800f78a7, 0xbffb7738, 0x80008000, 0x80008000, 0x8000c988, 0xcac4bd34, 0x4fa4f0ee, 0xecf20312, 0x7fff7fff, 0x80107fff, 0x80028006, 0x8000a000, 0x2aaa8000, 0x80008003, 0x81008608, 0x84068008, 0x4fa48100, 0xecf20312, 0xc20128c0, 0x80008000, 0x28c08000};
void process();

bool blynk_conected = false;
WiFiServer server(80);

void setup()
{
 Serial.begin(115200);
 Blynk.begin(BLYNK_TOKEN, SSID, PASSWORD);
 WiFi.begin(SSID, PASSWORD);
 while (WiFi.status() != WL_CONNECTED)
 {
  Serial.print(".");
  delay(500);
 }
 Serial.println("");

 Serial.print("IP: ");
 Serial.println(WiFi.localIP());

 server.begin();
 init_tx();
 init_rx();
}

void loop()
{
 Blynk.run();
 process();
}

/* ir */
bool ir_use = false;
size_t received = 0;
rmt_item32_t signals[MAX_SIGNAL_LEN];

void init_tx()
{
 rmt_config_t rmt_tx;
 rmt_tx.rmt_mode = RMT_MODE_TX;
 rmt_tx.channel = RMT_TX_CHANNEL;
 rmt_tx.gpio_num = RMT_TX_GPIO_NUM;
 rmt_tx.mem_block_num = 4;
 rmt_tx.clk_div = RMT_CLK_DIV;
 rmt_tx.tx_config.loop_en = false;
 rmt_tx.tx_config.carrier_duty_percent = 50;
 rmt_tx.tx_config.carrier_freq_hz = 38000;
 rmt_tx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
 rmt_tx.tx_config.carrier_en = 1;
 rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
 rmt_tx.tx_config.idle_output_en = true;
 rmt_config(&rmt_tx);
 rmt_driver_install(rmt_tx.channel, 0, 0);
}

void init_rx()
{
 rmt_config_t rmt_rx;
 rmt_rx.rmt_mode = RMT_MODE_RX;
 rmt_rx.channel = RMT_RX_CHANNEL;
 rmt_rx.clk_div = RMT_CLK_DIV;
 rmt_rx.gpio_num = RMT_RX_GPIO_NUM;
 rmt_rx.mem_block_num = 4;
 rmt_rx.rx_config.filter_en = true;
 rmt_rx.rx_config.filter_ticks_thresh = 100;
 rmt_rx.rx_config.idle_threshold = rmt_item32_TIMEOUT_US / 10 * (RMT_TICK_10_US);
 rmt_config(&rmt_rx);
 rmt_driver_install(rmt_rx.channel, 1000, 0);
}

void rmt_tx_task(void *)
{
 Serial.println("send...");
 rmt_write_items(RMT_TX_CHANNEL, signals, received, true);
 rmt_wait_tx_done(RMT_TX_CHANNEL, portMAX_DELAY);

 Serial.println("send done");

 ir_use = false;
 vTaskDelete(NULL);
}

void rmt_rx_task(void *)
{
 RingbufHandle_t rb = NULL;
 rmt_get_ringbuf_handle(RMT_RX_CHANNEL, &rb);
 rmt_rx_start(RMT_RX_CHANNEL, 1);

 size_t rx_size = 0;
 Serial.println("wait ir signal...");
 rmt_item32_t *item = (rmt_item32_t *)xRingbufferReceive(rb, &rx_size, 3000);
 rmt_rx_stop(RMT_RX_CHANNEL);
 if (!item)
 {
  Serial.println("no data received");
  ir_use = false;
  vTaskDelete(NULL);
  return;
 }
 Serial.print("received items: ");
 Serial.println(rx_size);

 memcpy(signals, item, sizeof(rmt_item32_t) * rx_size);
 for (int i = 0; i < rx_size; ++i)
 {
  signals[i].level0 = ~signals[i].level0;
  signals[i].level1 = ~signals[i].level1;
 }
 received = rx_size;
 vRingbufferReturnItem(rb, (void *)item);

 Serial.println("recv done");

 rmt_rx_stop(RMT_RX_CHANNEL);
 ir_use = false;
 vTaskDelete(NULL);
}

/* http */
void send_202(WiFiClient &client)
{
 client.print("HTTP/1.1 202 Accepted\r\n");
 client.print("Connection: close\r\n");
 client.print("Content-Length: 0\r\n\r\n");
}

void send_400(WiFiClient &client)
{
 client.print("HTTP/1.1 400 Bad Request\r\n");
 client.print("Connection: close\r\n");
 client.print("Content-Length: 0\r\n\r\n");
}

void send_404(WiFiClient &client)
{
 client.print("HTTP/1.1 404 Not Found\r\n");
 client.print("Connection: close\r\n");
 client.print("Content-Length: 0\r\n\r\n");
}

void send_409(WiFiClient &client)
{
 client.print("HTTP/1.1 409 Conflict\r\n");
 client.print("Connection: close\r\n");
 client.print("Content-Length: 0\r\n\r\n");
}

char url[128];
int on_url(http_parser *http_parser, const char *buf, size_t len)
{
 if (sizeof(url) <= strlen(url) + len)
 {
  Serial.println("URL too long");
  return 1;
 }
 strncat(url, buf, len);

 return 0;
}

char body[sizeof(signals)];
size_t bodylen = 0;
int on_body(http_parser *http_parser, const char *buf, size_t len)
{
 if (sizeof(body) < bodylen + len)
 {
  Serial.println("Body too long");
  return 1;
 }
 memcpy(body + bodylen, buf, len);
 bodylen += len;
 return 0;
}

bool request_end = false;
int on_message_complete(http_parser *http_parser)
{
 request_end = true;
}

int on_chunk_complete(http_parser *http_parser)
{
 request_end = true;
}

int turnOff()
{
 received = sizeof(turnOff_data) / sizeof(rmt_item32_t);
 memcpy(signals, turnOff_data, sizeof(turnOff_data));
 if (ir_use)
 {
  return 409;
 }
 else
 {
  ir_use = true;
  xTaskCreate(rmt_tx_task, "rmt_tx_task", 2048, NULL, 10, NULL);
  return 202;
 }
}

int turnOn()
{
 received = sizeof(turnOn_data) / sizeof(rmt_item32_t);
 memcpy(signals, turnOn_data, sizeof(turnOn_data));
 if (ir_use)
 {
  return 409;
 }
 else
 {
  ir_use = true;
  xTaskCreate(rmt_tx_task, "rmt_tx_task", 2048, NULL, 10, NULL);
  return 202;
 }
}

void process()
{
 char buf[1024];
 http_parser parser;
 http_parser_settings settings;
 WiFiClient client = server.available();
 bool error = false;

 memset(url, 0, sizeof(url));
 request_end = false;
 bodylen = 0;
 http_parser_init(&parser, HTTP_REQUEST);
 http_parser_settings_init(&settings);
 settings.on_url = on_url;
 settings.on_body = on_body;
 settings.on_message_complete = on_message_complete;
 settings.on_chunk_complete = on_chunk_complete;

 if (!client)
 {
  return;
 }

 while (client.connected())
 {
  if (client.available())
  {
   size_t nread = client.readBytes(buf, sizeof(buf));
   size_t nparsed = http_parser_execute(&parser, &settings, buf, nread);
   if (nread != nparsed || parser.http_errno != HPE_OK)
   {
    error = true;
    break;
   }
   if (request_end)
   {
    break;
   }
  }
 }

 if (!request_end || error)
 {
  send_400(client);
 }
 else if (strcmp(url, "/dump") == 0)
 {
  client.print("HTTP/1.1 200 OK\r\nContent-Type: octet-stream\r\nContent-Length: ");
  client.print(sizeof(rmt_item32_t) * received);
  client.print("\r\n\r\n");
  client.write((uint8_t *)signals, sizeof(rmt_item32_t) * received);
 }
 else if (strcmp(url, "/send") == 0)
 {
  received = bodylen / sizeof(rmt_item32_t);
  memcpy(signals, body, bodylen);
  if (ir_use)
  {
   send_409(client);
  }
  else
  {
   ir_use = true;
   xTaskCreate(rmt_tx_task, "rmt_tx_task", 2048, NULL, 10, NULL);
   send_202(client);
  }
 }
 else if (strcmp(url, "/off") == 0)
 {
  int res = turnOff();

  if (res == 409)
  {
   send_409(client);
  }
  else
  {
   send_202(client);
  }
 }
 else if (strcmp(url, "/on") == 0)
 {
  int res = turnOn();

  if (res == 409)
  {
   send_409(client);
  }
  else
  {
   send_202(client);
  }
 }
 else if (strcmp(url, "/recv") == 0)
 {
  if (ir_use)
  {
   send_409(client);
  }
  else
  {
   ir_use = true;
   xTaskCreate(rmt_rx_task, "rmt_rx_task", 2048, NULL, 10, NULL);
   send_202(client);
  }
 }
 else if (strcmp(url, "/health") == 0)
 {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
  client.printf("<!doctype html><head><title>myLight Monitor</title><meta charset=\"utf-8\"></head><body><h1>Health Monitor</h1><p>Hello from myLight. Now I'm awake!<br> Blynk Status: %s </p></body>", (blynk_conected
                                                                                                                                                                                                              ? "<span style=\"color:green\">Conneected</span>"
                                                                                                                                                                                                              : "<span style=\"color:red\">Disconnected</span>"));
 }
 else
 {
  send_404(client);
 }

 client.flush();
 client.stop();
}

BLYNK_WRITE(V1)
{ //Blynk Virtual Pin
 Serial.println("-------Blynk Request--------");
 const char *word = param[0].asString();

 Serial.printf("Said: '%s'\r\n", word);

 if (strncmp(word, "off", 3) == 0 || strncmp(word, " off", 4) == 0)
 {
  turnOff();
  Serial.println("Handled : turn OFF");
 }
 else if (strncmp(word, "on", 2) == 0 || strncmp(word, " on", 3) == 0)
 {
  turnOn();
  Serial.println("Handled : turn ON");
 }
}

BLYNK_CONNECTED()
{
 blynk_conected = true;
 Serial.println("Connected!");
}

BLYNK_DISCONNECTED()
{
 blynk_conected = false;
 Serial.println("Disconnected!");
}
