#include <layer2.h>
#include <lwip/def.h>

#define FRAME_CHECK_SEQUENCE_LEN 4

static inline ieee80211_frame_type_t getFrameType(void* buffer) {
  const ieee80211_frame_ctrl_t* frameControl = (ieee80211_frame_ctrl_t*)buffer;

  if ((frameControl->subtype >> 2 & 1) == 1)
    return WIFI_NULL_FRAME;

  bool isQoS = (frameControl->subtype >> 3 & 1) == 1;
  return isQoS ? WIFI_QOSDATA_FRAME : WIFI_DATA_FRAME;
}

static inline bool isRFC1042Compliant(ieee80211_llc_headers_t* llc) {
  return llc != nullptr && llc->ssap == 0xAA && llc->dsap == 0xAA &&
         llc->control == 0x3 && llc->oui[0] == 0 && llc->oui[1] == 0 &&
         llc->oui[2] == 0;
}

static inline bool hasValidFCS(void* buffer, size_t length) {
  return true;  // To be implemented
}

layer2_data_t getLayer2Data(void* buffer, uint16_t length) {
  layer2_data_t layer2data{.type = ETHER_TYPE_UNKNOWN};

  if (!hasValidFCS(buffer, length) ||
      length - FRAME_CHECK_SEQUENCE_LEN < IEEE80211_DATAFRAME_LEN)
    return layer2data;

  length -= FRAME_CHECK_SEQUENCE_LEN;
  ieee80211_mac_data_headers_t* mac = nullptr;
  ieee80211_llc_headers_t* llc = nullptr;

  switch (getFrameType(buffer)) {
    case WIFI_NULL_FRAME:
      break;
    case WIFI_DATA_FRAME: {
      ieee80211_dataframe_t* dataframe = (ieee80211_dataframe_t*)buffer;
      mac = &dataframe->mac;
      llc = &dataframe->llc;
      layer2data.payload = dataframe->payload;
      layer2data.length = length - IEEE80211_DATAFRAME_LEN;
      break;
    }
    case WIFI_QOSDATA_FRAME: {
      ieee80211_qosdataframe_t* qosdataframe =
          (ieee80211_qosdataframe_t*)buffer;
      mac = (ieee80211_mac_data_headers_t*)&qosdataframe->mac;
      llc = &qosdataframe->llc;
      layer2data.payload = qosdataframe->payload;
      layer2data.length = length - IEEE80211_QOSDATAFRAME_LEN;
      break;
    }
  }

  if (isRFC1042Compliant(llc)) {
    layer2data.type = (ether_type_t)ntohs(llc->type);
    for (int i = 0; i < 6; i++) {
      layer2data.source[i] = mac->addr2[i];
      layer2data.destination[i] = mac->addr3[i];
    }
  }

  return layer2data;
}
