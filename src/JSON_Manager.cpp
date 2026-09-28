#include "JSON_Manager.h"

JSON_Manager::JSON_Manager() {}

bool JSON_Manager::deserialization_error_(DeserializationError error)
{
  if (error)
  {
    JSON_MANAGER_LOG_F("Parse Error: %s\n", error.c_str());
    return false;
  }
  return true;
}

void JSON_Manager::begin(const char *json_file)
{
  strlcpy(json_file_, json_file, sizeof(json_file_));

  #if defined(ESP32)
    if (!LittleFS.begin(true))
    {
      JSON_MANAGER_LOG_F("LittleFS failed!\n");
      return;
    }
  #else
    if (!LittleFS.begin())
    {
      JSON_MANAGER_LOG_F("LittleFS failed!\n");
      return;
    }
  #endif
}

bool JSON_Manager::parse(const String &json_string)
{
  DeserializationError error = deserializeJson(doc_, json_string);

  return deserialization_error_(error);
}

String JSON_Manager::stringify()
{
  String temp_json_str;
  temp_json_str.reserve(measureJson(doc_)+1); 
  
  serializeJson(doc_, temp_json_str);
  return temp_json_str;
}

bool JSON_Manager::load_from_file()
{
  if (!has_file())
  {
    JSON_MANAGER_LOG_F("File not found. Creating a new one...\n");
    doc_.clear();
    save_to_file();
    return false;
  }

  File file = LittleFS.open(json_file_, "r");
  if (!file)
  {
    JSON_MANAGER_LOG_F("File open failed.\n");
    return false;
  }
  
  DeserializationError error = deserializeJson(doc_, file);
  
  file.close();
  return deserialization_error_(error);
}

bool JSON_Manager::save_to_file()
{
  File file = LittleFS.open(json_file_, "w");
  if (!file)
  {
    JSON_MANAGER_LOG_F("File open failed.\n");
    return false;
  }

  serializeJson(doc_, file);

  file.close();
  return true;
}

bool JSON_Manager::has_file() {return LittleFS.exists(json_file_);}

bool JSON_Manager::remove_file()
{
  if (has_file())
  {
    if (LittleFS.remove(json_file_))
    {
      JSON_MANAGER_LOG_F("File removed successfully.\n");
      return true;
    }
    else
    {
      JSON_MANAGER_LOG_F("Failed to remove file.\n");
      return false;
    }
  }
  return true;
}

bool JSON_Manager::has_key(const char *key) {return !doc_[key].isNull();}

void JSON_Manager::remove_key(const char *key){doc_.remove(key);}

void JSON_Manager::clear() {doc_.clear();}

void JSON_Manager::print()
{
  serializeJson(doc_, Serial);
  JSON_MANAGER_LOG_LN();
}

void JSON_Manager::pretty_print()
{
  serializeJsonPretty(doc_, Serial);
  JSON_MANAGER_LOG_LN();
}

size_t JSON_Manager::file_size()
{
  if (!has_file()) return 0;

  File file = LittleFS.open(json_file_, "r");
  if (!file) return 0;

  size_t size = file.size();
  file.close();
  return size;
}

void JSON_Manager::storage_info()
{
  JSON_MANAGER_LOG_F("--- LittleFS Storage Info ---");

  #if defined(ESP32)
    size_t total_bytes = LittleFS.totalBytes();
    size_t used_bytes = LittleFS.usedBytes();

  #elif defined(ESP8266)
    FSInfo fs_info;
    LittleFS.info(fs_info);
    size_t total_bytes = fs_info.totalBytes;
    size_t used_bytes = fs_info.usedBytes;

  #else
    JSON_MANAGER_LOG_F("Unsupported platform for storage info.\n");
    return;
  #endif

  size_t available_bytes = total_bytes - used_bytes;

  JSON_MANAGER_LOG_F("Total Space : %u bytes", total_bytes);
  JSON_MANAGER_LOG_F("Used Space  : %u bytes", used_bytes);
  JSON_MANAGER_LOG_F("Available   : %u bytes\n", available_bytes);
}

void JSON_Manager::list_json_files()
{
  JSON_MANAGER_LOG_F("Scanning LittleFS for .json files...\n");

  #if defined(ESP32)
    File root = LittleFS.open("/");
    if (!root || !root.isDirectory())
    {
      JSON_MANAGER_LOG_F("Failed to open root directory.\n");
      return;
    }

    File file = root.openNextFile();
    bool found = false;
    
    while (file)
    {
      String fileName = file.name();
      if (fileName.endsWith(".json"))
      {
        JSON_MANAGER_LOG("  -> /");
        JSON_MANAGER_LOG(fileName);
        JSON_MANAGER_LOG(" (");
        JSON_MANAGER_LOG(file.size());
        JSON_MANAGER_LOG_LN(" bytes)");
        found = true;
      }
      file = root.openNextFile();
    }
    
    if (!found) JSON_MANAGER_LOG_LN("  (.json file not found)");

  #elif defined(ESP8266)
    Dir dir = LittleFS.openDir("/");
    bool found = false;
    
    while (dir.next())
    {
      String fileName = dir.fileName();
      if (fileName.endsWith(".json"))
      {
        JSON_MANAGER_LOG("  -> ");
        JSON_MANAGER_LOG(fileName);
        JSON_MANAGER_LOG(" (");
        JSON_MANAGER_LOG(dir.fileSize());
        JSON_MANAGER_LOG_LN(" bytes)");
        found = true;
      }
    }
    
    if (!found) JSON_MANAGER_LOG_LN("  (.json file not found)");

  #else
    JSON_MANAGER_LOG_F("Unsupported platform for listing files.\n");
  #endif

  JSON_MANAGER_LOG_LN("------------------------------------------");
}

size_t JSON_Manager::get_array_size(const char *key)
{
  if (doc_[key].is<JsonArray>())
  {
    return doc_[key].as<JsonArray>().size();
  }
  return 0;
}

JsonDocument &JSON_Manager::get_JSON_Document() {return doc_;}