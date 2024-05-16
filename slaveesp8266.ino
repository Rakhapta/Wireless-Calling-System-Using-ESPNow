#include <ESP8266WiFi.h>
#include <espnow.h>

// mac address tujuan
uint8_t mac_addr_tujuan[] = {0xA8, 0x42, 0xE3, 0x47, 0xD3, 0xC4};

// struktur data yang diterima
typedef struct struct_message {
  char mode;
} struct_message;
struct_message data_ku;

// callback Jika data terkirim
void OnDataSent(uint8_t *mac_addr, uint8_t _status) {
  Serial.print("Status Pengiriman: ");
  if (_status == 0){
    Serial.println("Sukses");
  }
  else{
    Serial.println("Gagal");
  }
}

// callback jika menerima data
void OnDataRecv(uint8_t *mac_addr, uint8_t *dataDiterima, uint8_t panjang) {
  memcpy(&data_ku, dataDiterima, sizeof(data_ku));
  Serial.print("Mode Received: ");
  Serial.println(data_ku.mode);
  Serial.println();
  
  // mengirim data
  esp_now_send(mac_addr_tujuan, (uint8_t *) &data_ku, sizeof(data_ku));
}
 
void setup() {
  Serial.begin(115200);
  
  // mengatur esp ke mode station
  WiFi.mode(WIFI_STA);

  // inisialisasi espnow
  if (esp_now_init() != 0) {
    Serial.println("Gagal Inisialisasi espnow");
    return;
  }
  // atur mode esp8266 
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  // mendaftarkan fungsi callback
  esp_now_register_send_cb(OnDataSent);
  
  // Menambahkan peer
  esp_now_add_peer(mac_addr_tujuan, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  
  // mendaftarkan fungsi callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  
}