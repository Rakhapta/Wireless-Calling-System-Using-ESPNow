#include <WiFi.h>
#include <esp_now.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// mac address tujuan
uint8_t mac_addr_tujuan1[] = {0xC8, 0xC9, 0xA3, 0x5D, 0x10, 0xA8};
uint8_t mac_addr_tujuan2[] = {0xEC, 0x64, 0xC9, 0xC4, 0x68, 0x18};

// struktur data yang dikirim
typedef struct struct_message {
  char mode;
} struct_message;
struct_message data_ku;

// variabel peerInfo
esp_now_peer_info_t peerInfo1, peerInfo2;

// callback Jika data terkirim
void cb_kirim(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Status Pengiriman: ");
  if(status == ESP_NOW_SEND_SUCCESS){
    Serial.println("Sukses");
  }
  else{
    Serial.println("Gagal");
  }
}

// callback jika menerima data
void cb_terima(const uint8_t *mac_addr, const uint8_t *dataDiterima, int panjang) {
  memcpy(&data_ku, dataDiterima, sizeof(data_ku));
  Serial.print("Mode Received: ");
  Serial.println(data_ku.mode);
  Serial.println();
}

const byte ROWS = 4; /* four rows */
const byte COLS = 4; /* four columns */
/* define the symbols on the buttons of the keypads */
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 12, 14, 27}; /* connect to the row pinouts of the keypad */
byte colPins[COLS] = {26, 25, 33, 32}; /* connect to the column pinouts of the keypad */

/* initialize an instance of class NewKeypad */
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

LiquidCrystal_I2C lcd(0x27,  16, 2);

char mode;
char slave;

void setup() {
  Serial.begin(115200);

  // mengatur esp ke mode station
  WiFi.mode(WIFI_STA);

  // inisialisasi espnow
  if (esp_now_init() != ESP_OK) {
    Serial.println("Gagal Inisialisasi espnow");
    return;
  }

  // mendaftarkan fungsi callback  
  esp_now_register_send_cb(cb_kirim);
  
  // Mendata Peer / Kawan
  memcpy(peerInfo1.peer_addr, mac_addr_tujuan1, 6);
  peerInfo1.channel = 0;  
  peerInfo1.encrypt = false;

  memcpy(peerInfo2.peer_addr, mac_addr_tujuan2, 6);
  peerInfo2.channel = 0;  
  peerInfo2.encrypt = false;
  
  // Menambahkan Peer / Kawan
  if (esp_now_add_peer(&peerInfo1) != ESP_OK){
    Serial.println("Gagal Menambah Peer 1");
    return;
  }

  if (esp_now_add_peer(&peerInfo2) != ESP_OK){
    Serial.println("Gagal Menambah Peer 2");
    return;
  }
  
  // mendaftarkan fungsi callback
  esp_now_register_recv_cb(cb_terima);

  lcd.init();               
  lcd.backlight();
}
 
void loop() {
  Serial.print("masukkan Mode: ");
  char customKey = customKeypad.waitForKey();

  // Menyimpan input keypad pertama ke variabel mode
  if (mode == '\0') {
    mode = customKey;
    Serial.print("Mode: ");
    Serial.println(mode);
  } 
  // Menyimpan input keypad kedua ke variabel slave
  else if (slave == '\0') {
    slave = customKey;
    Serial.print("Slave: ");
    Serial.println(slave);
  } 
  // Jika mode = A, maka kirim mode ke slave 1 atau slave 2
  else {
    data_ku.mode = mode;
    esp_err_t result;
    if (slave == '1') {
      result = esp_now_send(mac_addr_tujuan1, (uint8_t *)&data_ku, sizeof(data_ku));
    } else if (slave == '2') {
      result = esp_now_send(mac_addr_tujuan2, (uint8_t *)&data_ku, sizeof(data_ku));
    } else {
      Serial.println("Slave tidak valid");
      mode = '\0';
      slave = '\0';
      return;
    }
    
    if (result == ESP_OK) {
      Serial.println("Mode dikirim");
    } else {
      Serial.println("Gagal mengirim mode");
    }

    // Reset mode dan slave setelah pengiriman
    mode = '\0';
    slave = '\0';
  }
}
