#ifndef _MAIN_CPP_
#define _MAIN_CPP_

#include <node.h>

// Define OLED display macros
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64

// Declare the MeshConnectNode
MeshConnectNode *node;

void handleException(const char *msg)
{
  Serial.println("Exception raised. Stopping...");
  Serial.println(msg);
  for (;;)
    ;
}

void setup()
{
  Serial.begin(9600);

  try
  {
    node = new MeshConnectNode(
        new Display(OLED_SDA, OLED_SCL, OLED_RST,
                    OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT));
  }
  catch (const char *msg)
  {
    handleException(msg);
  }
}

void loop()
{
  try
  {
    node->doWork();
    sleep(node->getSleepTime());
  }
  catch (const char *msg)
  {
    handleException(msg);
  }
}

#endif
