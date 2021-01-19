#include <Arduino.h>
#include <WiFi.h>

#include <lwip/api.h>
#include <lwip/pbuf.h>

const char *ssid = "Moni's Hotspot";
const char *password = "moni2wifi";

struct netconn *server, *client;
err_t err;

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server = netconn_new(NETCONN_TCP);

  if (server != NULL) 
  {  
    // bind connection to any ip on port 8888
    err = netconn_bind(server, NULL, 8888);

    if(err == ERR_OK)
    {
      Serial.println("Bound!");

      netconn_listen(server);

      bool should_loop = true;
      err = netconn_accept(server, &client);
      while(should_loop)
      {
        if(err == ERR_OK)
        {
          Serial.println("Got connection");

          struct netbuf *packet;
          void *data;
          u16_t len;

          err = netconn_recv(client, &packet);
          if(err == ERR_OK)
          {
            netbuf_data(packet, &data, &len);
            Serial.print(len);

            /* TODO:
            Parse packet data using its pointers:
              - packet->p->payload
              - packet->ptr
            */
          }
          else
          {
            should_loop = false;
            Serial.print("receive failed... error: ");
            Serial.println(err);
          }
        }
        else
        {
          should_loop = false;
          Serial.print("client connection failed... error: ");
          Serial.println(err);
        }
      }
    }
    else
    {
      Serial.print("Cannot bind... error: ");
      Serial.println(err);
    }
  }
}

void loop()
{
  Serial.println("something went wrong :(");
  while(1);
}