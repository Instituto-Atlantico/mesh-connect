#ifndef _LAYER2_CPP_
#define _LAYER2_CPP_

#include <layer2.h>
#include <lwip/def.h>

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

layer2_data_t getLayer2Data(void* buffer) {
  ieee80211_mac_data_headers_t* mac = nullptr;
  ieee80211_llc_headers_t* llc = nullptr;
  void* payload = nullptr;

  switch (getFrameType(buffer)) {
    case WIFI_NULL_FRAME:
      break;
    case WIFI_DATA_FRAME: {
      ieee80211_dataframe_t* dataframe = (ieee80211_dataframe_t*)buffer;
      mac = &dataframe->mac;
      llc = &dataframe->llc;
      payload = dataframe->payload;
      break;
    }
    case WIFI_QOSDATA_FRAME: {
      ieee80211_qosdataframe_t* qosdataframe =
          (ieee80211_qosdataframe_t*)buffer;
      mac = (ieee80211_mac_data_headers_t*)&qosdataframe->mac;
      llc = &qosdataframe->llc;
      payload = qosdataframe->payload;
      break;
    }
  }

  layer2_data_t layer2data{.type = ETHER_TYPE_UNKNOWN};

  if (isRFC1042Compliant(llc)) {
    layer2data.type = (ether_type_t)ntohs(llc->type);
    layer2data.payload = payload;
    for (int i = 0; i < 6; i++) {
      layer2data.source[i] = mac->addr2[i];
      layer2data.destination[i] = mac->addr3[i];
    }
  }

  return layer2data;
}

#endif
