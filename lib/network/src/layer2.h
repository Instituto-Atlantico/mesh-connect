#ifndef _LAYER2_H_
#define _LAYER2_H_

#include <inttypes.h>

typedef enum {
  WIFI_NULL_FRAME = 0,
  WIFI_DATA_FRAME,
  WIFI_QOSDATA_FRAME,
} ieee80211_frame_type_t;

typedef struct {
  uint8_t version : 2;
  uint8_t type : 2;
  uint8_t subtype : 4;
  bool toDS : 1;
  bool fromDS : 1;
  bool moreFragments : 1;
  bool retry : 1;
  bool powerMgmt : 1;
  bool moreData : 1;
  bool protect : 1;
  bool order : 1;
} ieee80211_frame_ctrl_t;  // 2 bytes
#define IEEE80211_FRAME_CTRL_LEN sizeof(ieee80211_frame_ctrl_t)

typedef struct {
  ieee80211_frame_ctrl_t frameCtrl;
  uint16_t duration : 16;
  uint8_t addr1[6];
  uint8_t addr2[6];
  uint8_t addr3[6];
  uint8_t fragmentNumber : 4;
  uint16_t sequenceNumber : 12;
} ieee80211_mac_data_headers_t;  // 24 bytes
#define IEEE80211_MAC_DATA_HEADERS_LEN sizeof(ieee80211_mac_data_headers_t)

typedef struct {
  ieee80211_frame_ctrl_t frameCtrl;
  uint16_t duration : 16;
  uint8_t addr1[6];
  uint8_t addr2[6];
  uint8_t addr3[6];
  uint8_t fragmentNumber : 4;
  uint16_t sequenceNumber : 12;
  uint16_t : 16;
} ieee80211_mac_qosdata_headers_t;  // 26 bytes
#define IEEE80211_MAC_QOSDATA_HEADERS_LEN \
  sizeof(ieee80211_mac_qosdata_headers_t)

typedef struct {
  uint8_t dsap;
  uint8_t ssap;
  uint8_t control;
  uint8_t oui[3];
  uint16_t type : 16;
} ieee80211_llc_headers_t;  // 8 bytes
#define IEEE80211_LLC_HEADERS_LEN sizeof(ieee80211_llc_headers_t)

typedef struct {
  ieee80211_mac_data_headers_t mac;  // 24 bytes
  ieee80211_llc_headers_t llc;       // 8 bytes
  uint8_t payload[0];
} ieee80211_dataframe_t;
#define IEEE80211_DATAFRAME_LEN sizeof(ieee80211_dataframe_t)

typedef struct {
  ieee80211_mac_qosdata_headers_t mac;  // 26 bytes
  ieee80211_llc_headers_t llc;          // 8 bytes
  uint8_t payload[0];
} ieee80211_qosdataframe_t;
#define IEEE80211_QOSDATAFRAME_LEN sizeof(ieee80211_qosdataframe_t)

typedef enum {
  ETHER_TYPE_UNKNOWN = 0,
  ETHER_TYPE_IPV4 = 0x0800,
  ETHER_TYPE_ARP = 0x0806,
  ETHER_TYPE_IPV6 = 0x86DD,
} ether_type_t;

typedef struct {
  ether_type_t type;
  uint8_t source[6];
  uint8_t destination[6];
  void* payload;
  uint16_t length;
} layer2_data_t;

layer2_data_t getLayer2Data(void* buffer, uint16_t length);

#endif
