#include <Arduino.h>
#include "Utils.h"

void generateRandomString(char* buffer, int length) {
  const char charset[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const int charsetSize = sizeof(charset) - 1;

  for (int i = 0; i < length; i++) {
    int index = random(charsetSize);
    buffer[i] = charset[index];
  }

  buffer[length] = '\0';  // null terminator
}

String parseValue(const String& body, const String& key) {
    int start = body.indexOf(key + "=");
    if (start < 0) return "";
    start += key.length() + 1;
    int end = body.indexOf('&', start);
    if (end < 0) end = body.length();
    return urlDecode(body.substring(start, end));
}

String urlDecode(const String& input) {
    String ret;
    char temp[] = "0x00";
    unsigned int len = input.length();
    unsigned int i = 0;
    while (i < len) {
        char c = input.charAt(i);
        if (c == '+') ret += ' ';
        else if (c == '%') {
            temp[2] = input.charAt(i + 1);
            temp[3] = input.charAt(i + 2);
            ret += strtol(temp, nullptr, 16);
            i += 2;
        } else ret += c;
        i++;
    }
    return ret;
}
