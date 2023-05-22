#include "WiFi.h"


/* -------------------------------------------------------------------------- */
CWifi::CWifi() : _timeout(50000){
   mak[0] = 0;
   mak[1] = 0;
   mak[2] = 0;
   mak[3] = 0;
   mak[4] = 0;
   mak[5] = 0;
}
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
const char* CWifi::firmwareVersion() {
/* -------------------------------------------------------------------------- */
   return WIFI_FIRMWARE_LATEST_VERSION;
}


/* -------------------------------------------------------------------------- */
int CWifi::begin(const char* ssid) {
/* -------------------------------------------------------------------------- */

   return 0;
}

/* -------------------------------------------------------------------------- */
int CWifi::begin(const char* ssid, const char *passphrase) {
/* -------------------------------------------------------------------------- */
   string res = "";
   modem.begin();
   modem.write(string(PROMPT(_MODE)),res, "%s%d\r\n" , CMD_WRITE(_MODE), 1);
   

   if(!modem.write(string(PROMPT(_BEGINSTA)),res, "%s%s,%s\r\n" , CMD_WRITE(_BEGINSTA), ssid, passphrase)) {
      return WL_CONNECT_FAILED;
   }

   unsigned long start_time = millis();
   while(millis() - start_time < 10000){
      if(status() == WL_CONNECTED) {
         return WL_CONNECTED;
      } 
   }
  return WL_CONNECT_FAILED;
}

/* passphrase is needed so a default one will be set */
/* -------------------------------------------------------------------------- */
uint8_t CWifi::beginAP(const char *ssid) {
/* -------------------------------------------------------------------------- */
   return beginAP(ssid,"",1);;
}

/* -------------------------------------------------------------------------- */
uint8_t CWifi::beginAP(const char *ssid, uint8_t channel) {
/* -------------------------------------------------------------------------- */
   return beginAP(ssid,"",channel);
}

/* -------------------------------------------------------------------------- */
uint8_t CWifi::beginAP(const char *ssid, const char* passphrase) {
/* -------------------------------------------------------------------------- */
   return beginAP(ssid, passphrase,1);
}

/* -------------------------------------------------------------------------- */
uint8_t CWifi::beginAP(const char *ssid, const char* passphrase, uint8_t channel) {
/* -------------------------------------------------------------------------- */   
   string res = "";
   modem.begin();
   modem.write(string(PROMPT(_MODE)),res, "%s%d\r\n" , CMD_WRITE(_MODE), 2);

   if(!modem.write(string(PROMPT(_BEGINSOFTAP)),res, "%s%s,%s,%d\r\n" , CMD_WRITE(_BEGINSOFTAP), ssid, passphrase, channel)) {
      return WL_CONNECT_FAILED;
   }

   if(atoi(res.c_str()) == 1) {
      return true;
   }

   return false;
}




/* -------------------------------------------------------------------------- */
void CWifi::config(IPAddress local_ip) {
/* -------------------------------------------------------------------------- */
  IPAddress _gw(local_ip[0],local_ip[1], local_ip[2], 1);
  Serial.println(_gw);
  IPAddress _sm(255,255,255,0);
  IPAddress dns(0,0,0,0);
  return _config(local_ip, _gw, _sm,dns,dns);
}

/* -------------------------------------------------------------------------- */
void CWifi::_config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {
/* -------------------------------------------------------------------------- */
   string res = "";
   modem.begin();
   string ip  = to_string(local_ip[0]) + ".";
          ip += to_string(local_ip[1]) + ".";
          ip += to_string(local_ip[2]) + ".";
          ip += to_string(local_ip[3]);

   string gw  = to_string(gateway[0]) + ".";
          gw += to_string(gateway[1]) + ".";
          gw += to_string(gateway[2]) + ".";
          gw += to_string(gateway[3]);       

   string nm  = to_string(subnet[0]) + ".";
          nm += to_string(subnet[1]) + ".";
          nm += to_string(subnet[2]) + ".";
          nm += to_string(subnet[3]);

   string _dns1  = to_string(dns1[0]) + ".";
          _dns1 += to_string(dns1[1]) + ".";
          _dns1 += to_string(dns1[2]) + ".";
          _dns1 += to_string(dns1[3]);
   
   string _dns2  = to_string(dns2[0]) + ".";
          _dns2 += to_string(dns2[1]) + ".";
          _dns2 += to_string(dns2[2]) + ".";
          _dns2 += to_string(dns2[3]);              

   modem.write(string(PROMPT(_SETIP)),res, "%s%s,%s,%s,%s,%s\r\n" , CMD_WRITE(_SETIP), ip.c_str(), gw.c_str(), nm.c_str(),_dns1.c_str(),_dns2.c_str());
}

/* -------------------------------------------------------------------------- */
void CWifi::config(IPAddress local_ip, IPAddress dns_server) {
/* -------------------------------------------------------------------------- */
   IPAddress _gw(local_ip[0],local_ip[1], local_ip[2], 1);
   Serial.println(_gw);
   IPAddress _sm(255,255,255,0);
   IPAddress dns(0,0,0,0);
   return _config(local_ip, _gw, _sm,dns_server,dns);
}

/* -------------------------------------------------------------------------- */
void CWifi::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway) {
/* -------------------------------------------------------------------------- */  
   IPAddress _sm(255,255,255,0);
   IPAddress dns(0,0,0,0);
   return _config(local_ip, gateway, _sm,dns_server,dns); 
}

/* -------------------------------------------------------------------------- */
void CWifi::config(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet) {
/* -------------------------------------------------------------------------- */
   IPAddress dns(0,0,0,0);
   return _config(local_ip, gateway, subnet,dns_server,dns); 
}

/* -------------------------------------------------------------------------- */
void CWifi::setDNS(IPAddress dns_server1) {
/* -------------------------------------------------------------------------- */
   IPAddress dns(0,0,0,0);
   return _config(localIP(), gatewayIP(), subnetMask(),dns_server1,dns);  
}

/* -------------------------------------------------------------------------- */
void CWifi::setDNS(IPAddress dns_server1, IPAddress dns_server2) {
/* -------------------------------------------------------------------------- */
   return _config(localIP(), gatewayIP(), subnetMask(),dns_server1,dns_server2); 
}

/* -------------------------------------------------------------------------- */
void CWifi::setHostname(const char* name) {
/* -------------------------------------------------------------------------- */
   string res = "";
   modem.write(string(DO_NOT_CHECK_CMD),res, "%s%s\r\n" , CMD_WRITE(_BEGINSOFTAP), name);
}

/* -------------------------------------------------------------------------- */
int CWifi::disconnect() {
/* -------------------------------------------------------------------------- */ 
   string res = "";
   modem.begin();

   if(modem.write(string(PROMPT(_DISCONNECT)),res,CMD(_DISCONNECT))) {
      return 1;
   }      
   return 0;

}

/* -------------------------------------------------------------------------- */
void CWifi::end(void) {
/* -------------------------------------------------------------------------- */   

}


static bool macStr2macArray(uint8_t *mac_out, const char *mac_in) {
   if(mac_in[2] != ':' || 
      mac_in[5] != ':' ||
      mac_in[8] != ':' ||
      mac_in[11] != ':' ||
      mac_in[14] != ':') {
      return false;
   }

   for(int i = 0; i < 6; i++) {
      std::string str_num(mac_in+(i*3),2);
      *(mac_out+i) = std::stoul(str_num,nullptr,16);
   }

   return true;
}


/* -------------------------------------------------------------------------- */
uint8_t* CWifi::macAddress(uint8_t* mac) {
/* -------------------------------------------------------------------------- */   
  string res = "";
  modem.begin();
  if(modem.write(string(PROMPT(_MODE)),res, "%s" , CMD_READ(_MODE)))  {
      if(atoi(res.c_str()) == 1) {
         if(modem.write(string(PROMPT(_MACSTA)),res, "%s" , CMD_READ(_MACSTA)))  {
            macStr2macArray(mac, res.c_str());
            return mac;
         }
      }
      else if(atoi(res.c_str()) == 2) {
         if(modem.write(string(PROMPT(_MACSOFTAP)),res, "%s" , CMD_READ(_MACSOFTAP)))  {
            macStr2macArray(mac, res.c_str());
            return mac;
         }
      }
   }

   for(int i = 0; i < 6; i++) 
   {
      mac[i] = mak[i];
   }

   return mac;
}




/* -------------------------------------------------------------------------- */
int8_t CWifi::scanNetworks() {
/* -------------------------------------------------------------------------- */
   modem.begin();
   access_points.clear();
   string res;
   
   vector<string> aps;
   if(modem.write(string(PROMPT(_WIFISCAN)),res,CMD(_WIFISCAN))) {

      split(aps, res, string("\r\n"));
      for(uint16_t i = 0; i < aps.size(); i++) {
         CAccessPoint ap;
         vector<string> tokens;
         split(tokens, aps[i], string("|"));
         if(tokens.size() >= 5) {
            ap.ssid            = tokens[0];
            ap.bssid           = tokens[1];
            macStr2macArray(ap.uint_bssid, ap.bssid.c_str());
            ap.rssi            = tokens[2];
            ap.channel         = tokens[3];
            ap.encryption_mode = tokens[4];
            access_points.push_back(ap);
         }
      } 
   }

   return (int8_t)access_points.size();
}

/* -------------------------------------------------------------------------- */
IPAddress CWifi::softAPIP() {
/* -------------------------------------------------------------------------- */
   string res = "";
   if(modem.write(string(PROMPT(_IPSOFTAP)),res,  CMD(_IPSOFTAP))) {
      IPAddress local_IP;
      local_IP.fromString(res.c_str());
      return local_IP;
   }
   return IPAddress(0,0,0,0);
}

/* -------------------------------------------------------------------------- */
IPAddress CWifi::dnsIP(int n) {
/* -------------------------------------------------------------------------- */
   modem.begin();
   string res;
   if(n == 0) {
      if(modem.write(string(PROMPT(_IPSTA)),res, "%s%d\r\n" , CMD_WRITE(_IPSTA), DNS1_ADDR)) {
         IPAddress dns_IP;
         dns_IP.fromString(res.c_str());
         return dns_IP;
      }
   }
   else if(n == 1) {
      if(modem.write(string(PROMPT(_IPSTA)),res, "%s%d\r\n" , CMD_WRITE(_IPSTA), DNS2_ADDR)) {
         IPAddress dns_IP;
         dns_IP.fromString(res.c_str());
         return dns_IP;
      }
   }
   return IPAddress(0,0,0,0);  
}


/* -------------------------------------------------------------------------- */
IPAddress CWifi::localIP() {
/* -------------------------------------------------------------------------- */
   modem.begin();
   string res = "";
   if(modem.write(string(PROMPT(_IPSTA)),res, "%s%d\r\n" , CMD_WRITE(_IPSTA), IP_ADDR)) {
      IPAddress local_IP;
      local_IP.fromString(res.c_str());
      return local_IP;
   }
   return IPAddress(0,0,0,0);
}

/* -------------------------------------------------------------------------- */
IPAddress CWifi::subnetMask() {
/* -------------------------------------------------------------------------- */
   modem.begin();
   string res = "";
   if(modem.write(string(PROMPT(_IPSTA)),res, "%s%d\r\n" , CMD_WRITE(_IPSTA), NETMASK_ADDR)) {
      IPAddress subnetMask;
      subnetMask.fromString(res.c_str());
      return subnetMask;
   }
   return IPAddress(0,0,0,0);
}

/* -------------------------------------------------------------------------- */
IPAddress CWifi::gatewayIP() {
/* -------------------------------------------------------------------------- */
   modem.begin();
   string res = "";
   if(modem.write(string(PROMPT(_IPSTA)),res, "%s%d\r\n" , CMD_WRITE(_IPSTA), GATEWAY_ADDR)) {
      IPAddress gateway_IP;
      gateway_IP.fromString(res.c_str());
      return gateway_IP;
   }
  return IPAddress(0,0,0,0);
}

/* -------------------------------------------------------------------------- */
const char* CWifi::SSID(uint8_t networkItem) {
/* -------------------------------------------------------------------------- */
  if(networkItem < access_points.size()) {
      return access_points[networkItem].ssid.c_str();   
  }
  return nullptr;
}
/* -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- */
int32_t CWifi::RSSI(uint8_t networkItem) {
  if(networkItem < access_points.size()) {
      return atoi(access_points[networkItem].rssi.c_str());   
  }
  return -1000;
}
/* -------------------------------------------------------------------------- */ 

static uint8_t Encr2wl_enc(string e) {
   if (e == string("open")) {
      return ENC_TYPE_NONE;
   } else if (e == string("WEP")) {
      return ENC_TYPE_WEP;
   } else if (e == string("WPA")) {
      return ENC_TYPE_WPA;
   } else if (e == string("WPA2")) {
      return ENC_TYPE_WPA2;
   } else if (e == string("WPA+WPA2")) {
      return ENC_TYPE_WPA2;
   } else if (e == string("WPA2-EAP")) {
      return ENC_TYPE_WPA2_ENTERPRISE;
   } else if (e == string("WPA2+WPA3")) {
      return ENC_TYPE_WPA3;
   } else if (e == string("WPA3")) {
      return ENC_TYPE_WPA3;
   } else {
      return ENC_TYPE_UNKNOWN;
   }
 }

/* -------------------------------------------------------------------------- */
uint8_t CWifi::encryptionType(uint8_t networkItem) {
  if(networkItem < access_points.size()) {
      return Encr2wl_enc(access_points[networkItem].encryption_mode);   
  }
  return 0;
}
/* -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- */
uint8_t* CWifi::BSSID(uint8_t networkItem, uint8_t* bssid) {
  if(networkItem < access_points.size()) {
      for(int i = 0; i < 6; i++) {
         *(bssid + i) = access_points[networkItem].uint_bssid[i];
      }
      return bssid;   
   }
  return nullptr;
}
/* -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- */
uint8_t CWifi::channel(uint8_t networkItem) { 
   if(networkItem < access_points.size()) {
      return atoi(access_points[networkItem].channel.c_str());   
   }
   return 0;
}
/* -------------------------------------------------------------------------- */ 

/* -------------------------------------------------------------------------- */ 
const char* CWifi::SSID() {
/* -------------------------------------------------------------------------- */    
  return ""; 
}

/* -------------------------------------------------------------------------- */ 
uint8_t* CWifi::BSSID(uint8_t* bssid) {
/* -------------------------------------------------------------------------- */    
  return nullptr;
}

/* -------------------------------------------------------------------------- */ 
int32_t CWifi::RSSI() {
/* -------------------------------------------------------------------------- */    
  return 0;
}

/* -------------------------------------------------------------------------- */ 
uint8_t CWifi::encryptionType() {
/* -------------------------------------------------------------------------- */    
  return 0;
}

/* -------------------------------------------------------------------------- */
uint8_t CWifi::status() {
/* -------------------------------------------------------------------------- */   
   string res = "";
   if(modem.write(string(PROMPT(_GETSTATUS)), res, CMD_READ(_GETSTATUS))) {
      return atoi(res.c_str());
   }
   return 0;
}

/* -------------------------------------------------------------------------- */
int CWifi::hostByName(const char* aHostname, IPAddress& aResult) {
/* -------------------------------------------------------------------------- */   
  return 0;
}



uint8_t CWifi::reasonCode() {
  return 0;
}

unsigned long CWifi::getTime() {
  return 0;
}



void CWifi::setTimeout(unsigned long timeout) {
   (void)(timeout);  
}


CWifi WiFi;

