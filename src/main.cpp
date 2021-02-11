#include <Arduino.h>
#include <WiFi.h>
#include <lwip/api.h>
#include <lwip/pbuf.h>
#include <lwip/ip.h>
#include <lwip/ip4_addr.h>
#include <lwip/icmp.h>

#define ip4_addr_get_byte_val(ipaddr, idx) ((u8_t)(((ipaddr).addr >> (idx * 8)) & 0xff))
#define ip4_addr1_val(ipaddr) ip4_addr_get_byte_val(ipaddr, 0)
#define ip4_addr2_val(ipaddr) ip4_addr_get_byte_val(ipaddr, 1)
#define ip4_addr3_val(ipaddr) ip4_addr_get_byte_val(ipaddr, 2)
#define ip4_addr4_val(ipaddr) ip4_addr_get_byte_val(ipaddr, 3)

typedef struct {
    struct netconn *nc ;
    uint8_t type ;
} netconn_events;

const char *ssid = "Monis Hotspot";
const char *password = "moni2wifi";

struct netconn *server, *client;
err_t err;

void ip4_debug_print(struct ip_hdr *iphdr);

static void netCallback(struct netconn *conn, enum netconn_evt evt, uint16_t length)
{
    //Show some callback information (debug)
    printf("sock:%u\tsta:%u\tevt:%u\tlen:%u\ttyp:%u\tfla:%02X\terr:%d\n", \
            (uint32_t)conn,conn->state,evt,length,conn->type,conn->flags,conn->last_err);
}

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

  server = netconn_new_with_callback(NETCONN_TCP, netCallback);

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
          struct netbuf *packet;

          err = netconn_recv(client, &packet);
          if(err == ERR_OK)
          {
            struct ip_hdr *iphdr = (struct ip_hdr *)packet->p->payload;
            ip4_debug_print(iphdr);
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

void ip4_debug_print(struct ip_hdr *iphdr)
{
  char strOut[1024];
  Serial.print("IP header:\n");
  Serial.print("+-------------------------------+\n");
  sprintf_P(strOut, (PGM_P)F("|%2"S16_F" |%2"S16_F" |  0x%02"X16_F" |     %5"U16_F"     | (v, hl, tos, len)\n"), 
                                                          (u16_t)IPH_V(iphdr),
                                                          (u16_t)IPH_HL(iphdr),
                                                          (u16_t)IPH_TOS(iphdr),
                                                          lwip_ntohs(IPH_LEN(iphdr)));
  Serial.print(strOut);
  Serial.print("+-------------------------------+\n");
  sprintf_P(strOut, (PGM_P)F("|    %5"U16_F"      |%"U16_F"%"U16_F"%"U16_F"|    %4"U16_F"   | (id, flags, offset)\n"),
                         lwip_ntohs(IPH_ID(iphdr)),
                         (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 15 & 1),
                         (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 14 & 1),
                         (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 13 & 1),
                         (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) & IP_OFFMASK));
  Serial.print(strOut);
  Serial.print("+-------------------------------+\n");
  sprintf_P(strOut, (PGM_P)F("|  %3"U16_F"  |  %3"U16_F"  |    0x%04"X16_F"     | (ttl, proto, chksum)\n"),
                         (u16_t)IPH_TTL(iphdr),
                         (u16_t)IPH_PROTO(iphdr),
                         lwip_ntohs(IPH_CHKSUM(iphdr)));
  Serial.print(strOut);
  Serial.print("+-------------------------------+\n");
  sprintf_P(strOut, (PGM_P)F("|  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  | (src)\n"),
                         ip4_addr1_val(iphdr->src),
                         ip4_addr2_val(iphdr->src),
                         ip4_addr3_val(iphdr->src),
                         ip4_addr4_val(iphdr->src));
  Serial.print(strOut);
  Serial.print("+-------------------------------+\n");
  sprintf_P(strOut, (PGM_P)F("|  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  | (dest)\n"),
                         ip4_addr1_val(iphdr->dest),
                         ip4_addr2_val(iphdr->dest),
                         ip4_addr3_val(iphdr->dest),
                         ip4_addr4_val(iphdr->dest));
  Serial.print(strOut);
  Serial.print("+-------------------------------+\n");
}
