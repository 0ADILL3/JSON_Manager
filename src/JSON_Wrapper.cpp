#include "JSON_Wrapper.h"

JSON_Wrapper::JSON_Wrapper() {}

bool JSON_Wrapper::deserialization_error_(DeserializationError error)
{
  if (error)
  {
    Serial.printf("[JSON_Wrapper] Parse Error: %s\n", error.c_str());
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
      Serial.println("[JSON_Wrapper] LittleFS failed!");
      return;
    }
  #else
    if (!LittleFS.begin())
    {
      Serial.println("[JSON_Wrapper] LittleFS failed!");
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
    Serial.println("[JSON_Wrapper] File not found. Creating a new one...");
    doc_.clear();
    save_to_file();
    return false;
  }

  File file = LittleFS.open(json_file_, "r");
  if (!file)
  {
    Serial.println("[JSON_Wrapper] File open failed.");
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
    Serial.println("[JSON_Wrapper] File open failed.");
    return false;
  }

  serializeJson(doc_, file);

  file.close();
  return true;
}

bool JSON_Wrapper::has_key(const char *key) {return !doc_[key].isNull();}

bool JSON_Wrapper::has_file() {return LittleFS.exists(json_file_);}

void JSON_Wrapper::clear() {doc_.clear();}

bool JSON_Wrapper::remove_file()
{
  if (has_file())
  {
    if (LittleFS.remove(json_file_))
    {
      Serial.println("[JSON_Wrapper] File removed successfully.");
      return true;
    }
    else
    {
      Serial.println("[JSON_Wrapper] Failed to remove file.");
      return false;
    }
  }
  return true;
}

void JSON_Wrapper::pretty_print()
{
  serializeJsonPretty(doc_, Serial);
  Serial.println();
}

void JSON_Wrapper::list_json_files()
{
  Serial.println("[JSON_Wrapper] Scanning LittleFS for .json files...");

  #if defined(ESP32)
    File root = LittleFS.open("/");
    if (!root || !root.isDirectory())
    {
      Serial.println("[JSON_Wrapper] Failed to open root directory.");
      return;
    }

    File file = root.openNextFile();
    bool found = false;
    
    while (file)
    {
      String fileName = file.name();
      if (fileName.endsWith(".json"))
      {
        Serial.print("  -> /");
        Serial.print(fileName);
        Serial.print(" (");
        Serial.print(file.size());
        Serial.println(" bytes)");
        found = true;
      }
      file = root.openNextFile();
    }
    
    if (!found) Serial.println("  (.json file not found)");

  #elif defined(ESP8266)
    Dir dir = LittleFS.openDir("/");
    bool found = false;
    
    while (dir.next())
    {
      String fileName = dir.fileName();
      if (fileName.endsWith(".json"))
      {
        Serial.print("  -> ");
        Serial.print(fileName);
        Serial.print(" (");
        Serial.print(dir.fileSize());
        Serial.println(" bytes)");
        found = true;
      }
    }
    
    if (!found) Serial.println("  (.json file not found)");

  #else
    Serial.println("[JSON_Wrapper] Unsupported platform for listing files.");
  #endif

  Serial.println("------------------------------------------");
}

JsonDocument &JSON_Wrapper::get_JSON_Document() {return doc_;}