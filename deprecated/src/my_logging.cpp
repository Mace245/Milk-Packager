#include <Arduino.h>
extern "C" {
  #include "my_logging.h"
}

void my_logln(const char *msg) {
  Serial.println(msg);
}

void my_log(const char *msg) {
  Serial.print(msg);
}