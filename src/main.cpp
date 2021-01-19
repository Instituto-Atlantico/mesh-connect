#include <Arduino.h>
#include <WiFi.h>

#include <lwip/sockets.h>
#include <lwip/pbuf.h>

int socket_desc;
const int num_socket_connections = 1;
struct sockaddr_in client;

const char *ssid = "Moni's Hotspot";
const char *password = "moni2wifi";
int variabel;
int InitializeServer(int domain, int type, int protocol, int num_connections);

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  socket_desc = InitializeServer(AF_INET, SOCK_STREAM, 0, num_socket_connections);
}

void loop()
{
  Serial.println("Waiting for incoming connections...");
  int new_socket, c = sizeof(struct sockaddr_in);

  new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);

  if (new_socket < 0)
  {
    Serial.println("accept failed");
  }
  else
  {
    const char *message = "Hello Client , I have received your connection\n";
    write(new_socket, message, strlen(message));
  }

  bool shouldLoop = true;
  int total =	10*1024;
  int sizeUsed = 0;
  char *data = (char*)malloc(total);
  memset(data, 0, sizeof(char));

  while (shouldLoop)
  {
    char strOut[256];
    ssize_t sizeRead = recv(new_socket, data + sizeUsed, total - sizeUsed, 0);
    if (sizeRead < 0)
    {
      sprintf_P(strOut, (PGM_P)F("recv: %d %s"), sizeRead, strerror(errno));
      shouldLoop = false;
    }
    if (sizeRead == 0)
    {
      break;
    }
    sizeUsed += sizeRead;
    Serial.println(data);
  }
}

int InitializeServer(int domain, int type, int protocol, int num_connections)
{
  int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc == -1)
  {
    Serial.println("Could not create socket");
    return -1;
  }
  struct sockaddr_in server;
  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8888);

  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
  {
    Serial.print("bind failed");
    return -1;
  }
  else
    Serial.println("bind done");

  listen(socket_desc, num_connections);
  return socket_desc;
}