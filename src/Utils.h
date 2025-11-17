#ifndef UTILS_H
#define UTILS_H

class String;

void generateRandomString(char* buffer, int length);

String parseValue(const String& body, const String& key);
String urlDecode(const String& input);

#endif