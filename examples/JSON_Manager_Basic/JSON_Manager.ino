#include <Arduino.h>
#include <JSON_Manager.h>

// Instansiasi objek config
JSON_Manager config;

void setup() {
  Serial.begin(115200);
  delay(2000); // Waktu jeda agar Serial Monitor siap
  
  Serial.println("\n--- Mulai Pengujian JSON_Manager ---");

  // 1. Inisialisasi LittleFS dengan nama file "/config.json" (dan format jika perlu pada ESP32)
  config.begin("/config.json");

  int boot_count = 0; // Variabel untuk menyimpan jumlah boot
  String device_name = "ESP_Default_Device"; // Variabel untuk menyimpan nama perangkat

  // 2. Muat konfigurasi dari file
  // Jika file belum ada, ia akan otomatis membuat file "/config.json" kosong
  if (config.load_from_file())
  {
    // 3. Baca data lama (jika ada) dan gunakan default value jika key tidak ditemukan
    boot_count = config.get<int>("boot_count", 0);
    device_name = config.get<String>("device_name", "ESP_Default_Device");
    
    Serial.print("Nama Perangkat saat ini: ");
    Serial.println(device_name);
    Serial.print("Total Boot: ");
    Serial.println(boot_count);
  }

  // 4. Ubah / Tambah data (Set)
  config.set<int>("boot_count", boot_count + 1);
  config.set<String>("device_name", "ESP_IoT_Node_1");
  config.set<bool>("is_active", true);

  // 5. Simpan perubahan ke memori Flash
  if (config.save_to_file()) {
    Serial.println("Konfigurasi berhasil diperbarui dan disimpan!");
  }

  // 6. Cetak ke layar untuk verifikasi
  Serial.println("\nIsi File JSON Saat Ini (Pretty Print):");
  config.pretty_print();
  
  // 7. Demonstrasi pengambilan data JSON mentah dalam satu baris (Stringify)
  Serial.println("\nIsi Stringify (Untuk dikirim via HTTP/MQTT):");
  String payload = config.stringify();
  Serial.println(payload);
  
  Serial.println("\n--- Pengujian Selesai ---");
}

void loop() {
  // Tidak ada proses di loop
}