#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h> 

#define JSON_FILENAME_MAX_LEN 32

#define DEBUG_JSON_WRAPPER 1

#if DEBUG_JSON_WRAPPER
  #define JSON_WRAPPER_LOG(x) do {Serial.print(x)} while (0)
  #define JSON_WRAPPER_LOG_F(fmt, ...) do {Serial.printf("\n[JSON_Wrapper] " fmt, ##__VA_ARGS__);} while (0)
  #define JSON_WRAPPER_LOG_LN(x) do {Serial.println(x)} while (0)
#else
  #define JSON_WRAPPER_LOG(...) do {} while (0)
  #define JSON_WRAPPER_LOG_F(...) do {} while (0)
  #define JSON_WRAPPER_LOG_LN(...) do {} while (0)
#endif

class JSON_Wrapper {
  private:
    JsonDocument doc_;
    char json_file_[JSON_FILENAME_MAX_LEN];

    bool deserialization_error_(DeserializationError error);

  public:
    /**
     * @brief Konstruktor default untuk inisialisasi JSON_Wrapper.
     */
    JSON_Wrapper();

    /**
     * @brief Menginisialisasi kelas dan melakukan mount pada sistem file LittleFS.
     * @param json_file Path/nama file JSON yang akan digunakan (default: "/data.json").
     */
    void begin(const char *json_file = "/data.json");

    /**
     * @brief Mem-parsing string JSON dan memasukkannya ke dalam JsonDocument internal.
     * @param json_string String yang berisi data JSON.
     * @return true jika proses parsing berhasil, false jika gagal.
     */
    bool parse(const String &json_string);

    /**
     * @brief Mengonversi (serialize) JsonDocument internal menjadi format string.
     * @return String yang berisi representasi data JSON.
     */
    String stringify();

    /**
     * @brief Memuat dan mem-parsing data JSON dari file internal ke dalam JsonDocument.
     * Jika file tidak ditemukan, file baru yang kosong akan dibuat secara otomatis.
     * @return true jika berhasil memuat data, false jika gagal atau file baru dibuat.
     */
    bool load_from_file();

    /**
     * @brief Menyimpan (serialize) isi JsonDocument internal ke dalam file.
     * @return true jika berhasil menyimpan ke file, false jika gagal membuka/menyimpan file.
     */
    bool save_to_file();

    /**
     * @brief Memeriksa apakah sebuah key (kunci) tertentu ada di dalam dokumen JSON.
     * @param key Nama kunci yang ingin dicari.
     * @return true jika key ditemukan dan tidak null, false sebaliknya.
     */
    bool has_key(const char *key);

    /**
     * @brief Memeriksa apakah file JSON yang dideklarasikan ada di dalam memori LittleFS.
     * @return true jika file ada, false jika tidak.
     */
    bool has_file();

    /**
     * @brief Membersihkan seluruh isi dari JsonDocument internal (mengosongkan data).
     */
    void clear();

    /**
     * @brief Menghapus file JSON dari sistem file LittleFS.
     * @return true jika file berhasil dihapus atau sudah tidak ada, false jika gagal dihapus.
     */
    bool remove_file();

    /**
     * @brief Mencetak isi JsonDocument dengan format yang rapi (pretty print) ke Serial Monitor.
     */
    void pretty_print();

    /**
     * @brief Fungsi statis untuk memindai dan menampilkan daftar semua file .json di LittleFS ke Serial Monitor.
     */
    static void list_json_files();

    /**
     * @brief Mengambil referensi langsung ke JsonDocument internal.
     * @return Referensi ke objek JsonDocument.
     */
    JsonDocument &get_JSON_Document();

    /**
     * @brief Memasukkan atau memperbarui nilai berdasarkan key tertentu.
     * @tparam T Tipe data dari value yang akan dimasukkan.
     * @param key Kunci (key) JSON tempat value akan disimpan.
     * @param value Nilai yang akan disimpan.
     */
    template <typename T>
    void set(const char *key, T value) {doc_[key] = value;}

    /**
     * @brief Mengambil nilai dari key tertentu di dalam JSON.
     * @tparam T Tipe data kembalian yang diharapkan.
     * @param key Kunci (key) JSON yang nilainya ingin diambil.
     * @param default_value Nilai fallback yang akan dikembalikan jika key tidak ditemukan.
     * @return Nilai dari key yang dicari, atau default_value jika gagal/tidak ditemukan.
     */
    template <typename T>
    T get(const char *key, T default_value)
    {
      if (doc_[key].is<T>()) {
        return doc_[key].as<T>();
      }
      return default_value;
    }
};