#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <dataqueue.h>
#include <lwip/inet_chksum.h>
#include <nat.h>

void setup() {
  Serial.begin(115200);

  LoRa.setPins(18, 14, 26);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  LoRa.setSignalBandwidth(250E3);
  LoRa.setCodingRate4(5);
  LoRa.setSpreadingFactor(7);
}

void queueExperiment() {
  size_t size = 255;
  ipv4_datagram_t datagram =
      ipv4_datagram_t{.payload = malloc(size), .size = size};

  DataQueue<message_t> queue(10);
  auto message = newIPv4DataMessage(datagram);

  auto now = micros();
  queue.push(&message);
  auto elapsed = micros() - now;

  now = micros();
  queue.poll();
  auto elapsed2 = micros() - now;

  Serial.printf("Push: %lu\tPoll: %lu\n", elapsed, elapsed2);
}

void natExperiment() {
  GatewayNAT nat;

  size_t size = 255;
  ipv4_datagram_t datagram =
      ipv4_datagram_t{.payload = malloc(size), .size = size};

  auto now = micros();
  nat.translate(&datagram, 124);
  auto elapsed = micros() - now;

  now = micros();
  try {
    nat.revert(&datagram);
  } catch (...) {
  }
  auto elapsed2 = micros() - now;

  now = micros();
  inet_chksum(datagram.payload, size);
  auto elapsed3 = micros() - now;

  Serial.printf("Translate: %lu\tRevert: %lu\tChecksum: %lu\n", elapsed,
                elapsed2, elapsed3);

  free(datagram.payload);
}

void loraTransmitExperiment() {
  LoRa.setSignalBandwidth(250E3);
  LoRa.setCodingRate4(5);

  uint64_t transmissionTimes[6][9];  // 6 SFs and 9 sizes
  memset(transmissionTimes, 0, sizeof(transmissionTimes));

  for (int size = 0; size < 9; size++) {
    while (LoRa.beginPacket() == false)
      ;
    auto now = millis();

    for (int sf = 7; sf <= 12; sf++) {
      LoRa.setSpreadingFactor(sf);
      int bytes = pow(2, size);
      for (int i = 0; i < bytes; i++) {
        LoRa.write(i);
      }
      LoRa.endPacket();

      while (LoRa.beginPacket() == false)
        ;
      transmissionTimes[sf - 7][size] = millis() - now;
      now = millis();
    }

    LoRa.endPacket();
  }

  for (int i = 0; i < 6; i++) {
    Serial.println("SF\tSIZE\tTIME");
    for (int j = 0; j < 9; j++) {
      Serial.printf("%d\t%d\t%" PRIu64 "\n", i + 7, (int)pow(2, j),
                    transmissionTimes[i][j]);
    }
  }
}

void memoryExperiment() {
  int mallocTimes[12];
  int copyTimes[12];
  int j = 0;

  void* dst = malloc(2048);

  for (int i = 1; i <= 2048; i *= 2) {
    auto start = micros();
    void* data = malloc(i);
    mallocTimes[j] = micros() - start;

    start = micros();
    memcpy(dst, data, i);
    copyTimes[j++] = micros() - start;

    free(data);
  }

  Serial.println("SIZE\tMALLOC\tCOPY");
  for (int i = 0; i < j; i++) {
    Serial.printf("%d\t%d\t%d\n", (int)pow(2, i), mallocTimes[i], copyTimes[i]);
  }

  free(dst);
}

void loraReceiveExperiment() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    int i = 0;
    auto now = micros();
    while (LoRa.available()) {
      LoRa.read();
      i++;
    }
    auto elapsed = micros() - now;

    Serial.printf("Receive time: %lu\tcount %d\tRSSI: %d\n", elapsed, i, LoRa.packetRssi());
  }
}

void loraSender() {
  while (LoRa.beginPacket() == false)
    ;
  for (int i = 0; i < 256; i++) {
    LoRa.write(i);
  }

  LoRa.endPacket();
}

void loop() {
  natExperiment();
  delay(2000);
}
