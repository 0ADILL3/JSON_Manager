#include "JSON_Wrapper.h"

JSON_Wrapper::JSON_Wrapper() {}

bool JSON_Wrapper::deserialization_error_(DeserializationError error)
{
  if (error)
  {
    JSON_WRAPPER_LOG_F("Parse Error: %s\n", error.c_str());
    return false;
  }
  return true;
}

void JSON_Wrapper::begin(const char *json_file)
{
  strlcpy(json_file_, json_file, sizeof(json_file_));

  #if defined(ESP32)
    if (!LittleFS.begin(true))
    {
      JSON_WRAPPER_LOG_F("LittleFS failed!\n");
      return;
    }
  #else
    if (!LittleFS.begin())
    {
      JSON_WRAPPER_LOG_F("LittleFS failed!\n");
      return;
    }
  #endif
}

bool JSON_Wrapper::parse(const String &json_string)
{
  DeserializationError error = deserializeJson(doc_, json_string);

  return deserialization_error_(error);
}

String JSON_Wrapper::stringify()
{
  String temp_json_str;
  temp_json_str.reserve(measureJson(doc_)+1); 
  
  serializeJson(doc_, temp_json_str);
  return temp_json_str;
}

bool JSON_Wrapper::load_from_file()
{
  if (!has_file())
  {
    JSON_WRAPPER_LOG_F("File not found. Creating a new one...\n");
    doc_.clear();
    save_to_file();
    return false;
  }

  File file = LittleFS.open(json_file_, "r");
  if (!file)
  {
    JSON_WRAPPER_LOG_F("File open failed.\n");
    return false;
  }
  
  DeserializationError error = deserializeJson(doc_, file);
  
  file.close();
  return deserialization_error_(error);
}

bool JSON_Wrapper::save_to_file()
{
  File file = LittleFS.open(json_file_, "w");
  if (!file)
  {
    JSON_WRAPPER_LOG_F("File open failed.\n");
    return false;
  }

  serializeJson(doc_, file);

  file.close();
  return true;
}

bool JSON_Wrapper::has_key(const char *key) {return !doc_[key].isNull();}

void JSON_Wrapper::remove(const char *key){doc_.remove(key);}

bool JSON_Wrapper::has_file() {return LittleFS.exists(json_file_);}

void JSON_Wrapper::clear() {doc_.clear();}

bool JSON_Wrapper::remove_file()
{
  if (has_file())
  {
    if (LittleFS.remove(json_file_))
    {
      JSON_WRAPPER_LOG_F("File removed successfully.\n");
      return true;
    }
    else
    {
      JSON_WRAPPER_LOG_F("Failed to remove file.\n");
      return false;
    }
  }
  return true;
}

void JSON_Wrapper::print()
{
  serializeJson(doc_, Serial);
  JSON_WRAPPER_LOG_LN();
}

void JSON_Wrapper::pretty_print()
{
  serializeJsonPretty(doc_, Serial);
  JSON_WRAPPER_LOG_LN();
}

void JSON_Wrapper::list_json_files()
{
  JSON_WRAPPER_LOG_F("Scanning LittleFS for .json files...\n");

  #if defined(ESP32)
    File root = LittleFS.open("/");
    if (!root || !root.isDirectory())
    {
      JSON_WRAPPER_LOG_F("Failed to open root directory.\n");
      return;
    }

    File file = root.openNextFile();
    bool found = false;
    
    while (file)
    {
      String fileName = file.name();
      if (fileName.endsWith(".json"))
      {
        JSON_WRAPPER_LOG("  -> /");
        JSON_WRAPPER_LOG(fileName);
        JSON_WRAPPER_LOG(" (");
        JSON_WRAPPER_LOG(file.size());
        JSON_WRAPPER_LOG_LN(" bytes)");
        found = true;
      }
      file = root.openNextFile();
    }
    
    if (!found) JSON_WRAPPER_LOG_LN("  (.json file not found)");

  #elif defined(ESP8266)
    Dir dir = LittleFS.openDir("/");
    bool found = false;
    
    while (dir.next())
    {
      String fileName = dir.fileName();
      if (fileName.endsWith(".json"))
      {
        JSON_WRAPPER_LOG("  -> ");
        JSON_WRAPPER_LOG(fileName);
        JSON_WRAPPER_LOG(" (");
        JSON_WRAPPER_LOG(dir.fileSize());
        JSON_WRAPPER_LOG_LN(" bytes)");
        found = true;
      }
    }
    
    if (!found) JSON_WRAPPER_LOG_LN("  (.json file not found)");

  #else
    JSON_WRAPPER_LOG_F("Unsupported platform for listing files.\n");
  #endif

  JSON_WRAPPER_LOG_LN("------------------------------------------");
}

size_t JSON_Wrapper::get_array_size(const char *key)
{
  if (doc_[key].is<JsonArray>())
  {
    return doc_[key].as<JsonArray>().size();
  }
  return 0;
}

JsonDocument &JSON_Wrapper::get_JSON_Document() {return doc_;}