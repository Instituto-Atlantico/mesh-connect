#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include<bits/stdc++.h>

using namespace std;

#define SS 18
#define RST 14
#define DIO0 26

vector <int>tempo;

int startTime = 0; 
int endTime = 0;
int numEnvios = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender");
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(915E6)) 
  {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() 
{
  numEnvios++;
  Serial.print("Envio numero #");
  Serial.println(numEnvios);
  for (int i = 1; i <= 256; i*=2)
  {
    uint8_t* dataPayload = (uint8_t*)malloc(i);
    startTime = micros();
    LoRa.beginPacket();
    LoRa.write(dataPayload, i);
    LoRa.endPacket();
    endTime = micros();
    Serial.print("Tempo de envio de ");
    Serial.print(i);
    Serial.print(" bytes: ");
    Serial.println(endTime - startTime); //Insere o valor na nova posicao do vetor e guarda o valor da posicao
    free(dataPayload);
  }
  delay(5000);
}