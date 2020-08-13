#include <ArduinoJson.h>

String jsonRead(String &json, String name);

int jsonReadtoInt(String &json, String name);

String jsonWrite(String &json, String name, String volume);

String jsonWrite(String &json, String name, int volume);

String jsonWrite(String &json, String name, float volume);

void saveConfig ();

String readFile(String fileName, size_t len );

String writeFile(String fileName, String strings );

String selectToMarker (String str, String found);

String selectToMarkerLast (String str, String found);

String deleteBeforeDelimiter(String str, String found);

String deleteBeforeDelimiterTo(String str, String found);

String deleteToMarkerLast (String str, String found);