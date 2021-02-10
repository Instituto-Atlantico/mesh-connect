#include <Arduino.h>
#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>

#undef LL2_DEBUG

#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26

#define LORA_FREQ 915E6
#define LED 25
#define TX_POWER 20
#define DATAGRAM_HEADER 5

char MAC[9] = "c0d3f00d";
uint8_t LOCAL_ADDRESS[ADDR_LENGTH] = {0xc0, 0xd3, 0xf0, 0x0d};
uint8_t RECEIVER[ADDR_LENGTH] = {0xc0, 0xd3, 0xca, 0xfe};

Layer1Class* Layer1;
LL2Class* LL2;

int startTime = 0; 
int endTime = 0;
int numEnvios = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("* Initializing LoRa...");
  Serial.println("LoRa Sender");

  Layer1 = new Layer1Class();
  Layer1->setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Layer1->setTxPower(TX_POWER);
  Layer1->setLoRaFrequency(LORA_FREQ);
  if (Layer1->init()) {
    Serial.println(" --> Layer1 initialized");
    LL2 = new LL2Class(Layer1);  // initialize Layer2
    LL2->setLocalAddress(MAC);  // this should either be randomized or set using
                                // the wifi mac address
    LL2->setInterval(10000);    // set to zero to disable routing packets
    if (LL2->init() == 0) {
      Serial.println(" --> LoRaLayer2 initialized");
    } else {
      Serial.println(" --> Failed to initialize LoRaLayer2");
    }
  } else {
    Serial.println(" --> Failed to initialize LoRa");
  }
  //lastTransmit = Layer1Class::getTime();
}

void loop() 
{
  int msgLength = 0;
  int datagramsize = 0;
  struct Datagram datagram;
  numEnvios++;
  Serial.print("Envio numero #");
  Serial.println(numEnvios);
  memcpy(datagram.destination, RECEIVER,ADDR_LENGTH); 
  datagram.type = 's';  // can be anything, but 's' for 'sensor'
  for (int i = 1; i <= 230; i+=2)
  {
    
     msgLength = i;
    char datapayload[msgLength];
    memset(datapayload,'a',sizeof(datapayload));
    //msgLength = sprintf((char*)datagram.message, "%s", dataPayload);
    strncpy((char*)datagram.message,datapayload, msgLength);
    datagramsize = msgLength + DATAGRAM_HEADER;
    startTime = micros();
    LL2->daemon(); 
    endTime = micros();   
    LL2->writeData(datagram, datagramsize);
    endTime = micros();
    Serial.print("Tempo de envio de ");
    Serial.print(msgLength);
    Serial.print(" bytes: ");
    Serial.println(endTime - startTime); 
  }
  delay(5000);
}

