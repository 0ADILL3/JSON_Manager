#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h> 

#define JSON_FILENAME_MAX_LEN 32

class JSON_Wrapper {
  private:
    JsonDocument doc_;
    char json_file_[JSON_FILENAME_MAX_LEN];

    bool deserialization_error_(DeserializationError error);

  public:
    // Constructor
    JSON_Wrapper();
    // Initialize the JSON_Wrapper and mount LittleFS
    void begin(const char *json_file = "/data.json");
    // Parse a JSON string and populate the internal JsonDocument
    bool parse(const String &json_string);
    // Convert the internal JsonDocument to a JSON string
    String stringify();
    // Load the JSON data from the specified JSON file into the internal JsonDocument
    bool load_from_file();
    // Save the internal JsonDocument to the specified JSON file
    bool save_to_file();
    // Check if the specified key exists in the internal JsonDocument
    bool has_key(const char *key);
    // Check if the specified JSON file exists
    bool has_file();
    // Clear the internal JsonDocument
    void clear();
    // Remove the specified JSON file from the filesystem
    bool remove_file();
    // Print the internal JsonDocument in a human-readable format to the Serial Monitor
    void pretty_print();
    // List all JSON files in the LittleFS filesystem
    static void list_json_files();
    // Get a reference to the internal JsonDocument for direct manipulation
    JsonDocument &get_JSON_Document();
    // Set a key-value pair in the internal JsonDocument
    template <typename T>
    void set(const char *key, T value) {doc_[key] = value;}
    // Get the value associated with the specified key from the internal JsonDocument, or return a default value if the key does not exist
    template <typename T>
    T get(const char *key, T default_value)
    {
      if (doc_[key].is<T>()) {
        return doc_[key].as<T>();
      }
      return default_value;
    }
};