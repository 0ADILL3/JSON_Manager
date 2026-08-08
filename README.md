# JSON_Wrapper Library

`JSON_Wrapper` adalah *library* utilitas ringan untuk mikrokontroler ESP32 dan ESP8266 yang membungkus (*wrap*) pustaka `ArduinoJson` dan `LittleFS`. 

*Library* ini dirancang khusus untuk mempermudah penyimpanan dan manipulasi konfigurasi JSON pada memori *flash* perangkat secara aman (bebas fragmentasi memori) dan stabil (menangani *file system fault* otomatis).

## Fitur Utama
* **Manajemen Memori Bebas Fragmentasi**: Menggunakan `measureJson` dan `reserve()` untuk menghindari fragmentasi memori Heap (RAM).
* **Fail-Safe & Auto Format**: Penanganan terpusat pada file korup, pencegahan batas buka file, dan dukungan format otomatis untuk ESP32 baru.
* **Aman dari Buffer Overflow**: Menggunakan alokasi C-string statis untuk path nama file alih-alih `String`.
* **C++ Templates**: Menyediakan metode `set()` dan `get()` yang praktis dan mendukung nilai cadangan (*default value fallback*).

## Dependensi
Pastikan Anda telah menginstal *library* berikut melalui Arduino Library Manager:
* **ArduinoJson** (Direkomendasikan versi 7.x)

## Instalasi
1. Unduh repositori ini sebagai file `.zip`.
2. Buka Arduino IDE.
3. Buka menu **Sketch** -> **Include Library** -> **Add .ZIP Library...**
4. Pilih file `.zip` yang baru saja Anda unduh.

## Contoh Singkat (API)

```cpp
#include <JSON_Wrapper.h>

JSON_Wrapper config("/config.json");

void setup() {
  Serial.begin(115200);
  config.begin(); 
  config.load_from_file();

  // Membaca data dengan nilai default fallback
  String ssid = config.get<String>("ssid", "default_wifi");
  
  // Mengubah data dan menyimpan
  config.set<int>("boot_count", config.get<int>("boot_count", 0) + 1);
  config.save_to_file();
}

void loop() {}