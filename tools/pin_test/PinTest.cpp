/* PinTest.cpp
   Standalone hardware diagnostic firmware for ESP32_MatrixShow.
   Drives the matrix data pins with a steady, operator-selected value so the
   wiring/level-shifters/LED strips can be probed with a multimeter or scope
   in isolation from the normal animation firmware.

   Flash with:  pio run -e pintest -t upload
   Then open the serial monitor at 115200 baud and type `help`.

   Two independent test modes, selected per pin at runtime over Serial:
     - RAW mode:  a plain digitalWrite() held HIGH or LOW forever.
                  Use this to check continuity, voltage levels, shorts,
                  or level-shifter behavior with a multimeter.
     - LED mode:  FastLED pushes one solid color to every LED on that pin,
                  refreshed continuously. Use this to see a real, repeating
                  WS2811 data waveform on a scope, and to visually confirm
                  every physical LED lights.
*/

#include <Arduino.h>
#include <FastLED.h>

// Matrix data pins (mirrors src/Config.h — kept standalone on purpose so
// this diagnostic tool has zero dependency on WiFi/Secrets/app config).
#define PIN_MATRIX0 16   // "Right"
#define PIN_MATRIX1 17   // "Left"
#define PIN_MATRIX2 18   // Mega matrix (disabled in normal firmware)

#define LEDS_MATRIX0 500
#define LEDS_MATRIX1 500
#define LEDS_MATRIX2 2000

#define COLOR_ORDER RGB
#define LED_TYPE    WS2811

static CRGB leds0[LEDS_MATRIX0];
static CRGB leds1[LEDS_MATRIX1];
static CRGB leds2[LEDS_MATRIX2];

enum Mode { MODE_IDLE, MODE_RAW, MODE_LED };

struct PinState {
  int pin;
  int numLeds;
  CRGB* leds;
  Mode mode;
  int rawLevel;     // HIGH/LOW when mode == MODE_RAW
  CRGB ledColor;    // color when mode == MODE_LED
};

static PinState pins[3] = {
  { PIN_MATRIX0, LEDS_MATRIX0, leds0, MODE_IDLE, LOW, CRGB::Black },
  { PIN_MATRIX1, LEDS_MATRIX1, leds1, MODE_IDLE, LOW, CRGB::Black },
  { PIN_MATRIX2, LEDS_MATRIX2, leds2, MODE_IDLE, LOW, CRGB::Black },
};

static PinState* findPin(int pin) {
  for (auto& p : pins) if (p.pin == pin) return &p;
  return nullptr;
}

static void printHelp() {
  Serial.println();
  Serial.println(F("=== ESP32_MatrixShow Pin Test Tool ==="));
  Serial.println(F("Pins: 16 (Matrix0/Right), 17 (Matrix1/Left), 18 (Matrix2/Mega)"));
  Serial.println();
  Serial.println(F("  raw <pin|all> high|low   - hold pin at a steady logic level (multimeter/scope)"));
  Serial.println(F("  led <pin|all> <color> [brightness]"));
  Serial.println(F("                           - continuously refresh a solid color on that pin's strip"));
  Serial.println(F("                             colors: red green blue white off"));
  Serial.println(F("  stop <pin|all>           - release pin (Hi-Z input, LEDs off)"));
  Serial.println(F("  status                   - show current mode of each pin"));
  Serial.println(F("  help                     - show this message"));
  Serial.println();
  Serial.println(F("Examples: raw 16 high | led 17 red | led all white 32 | stop all"));
  Serial.println();
}

static void printStatus() {
  for (auto& p : pins) {
    Serial.printf("Pin %2d: ", p.pin);
    switch (p.mode) {
      case MODE_IDLE: Serial.println(F("idle (Hi-Z)")); break;
      case MODE_RAW:  Serial.printf("RAW steady %s\n", p.rawLevel == HIGH ? "HIGH" : "LOW"); break;
      case MODE_LED:  Serial.printf("LED solid (%d,%d,%d) on %d LEDs\n",
                                     p.ledColor.r, p.ledColor.g, p.ledColor.b, p.numLeds); break;
    }
  }
}

static void setRaw(PinState& p, int level) {
  p.mode = MODE_RAW;
  p.rawLevel = level;
  pinMode(p.pin, OUTPUT);
  digitalWrite(p.pin, level);
}

static void setLed(PinState& p, CRGB color) {
  p.mode = MODE_LED;
  p.ledColor = color;
}

static void setStop(PinState& p) {
  p.mode = MODE_IDLE;
  fill_solid(p.leds, p.numLeds, CRGB::Black);
  FastLED.show();
  pinMode(p.pin, INPUT);
}

static bool parseColor(const String& name, CRGB& out) {
  String c = name; c.toLowerCase();
  if (c == "red")   { out = CRGB::Red;   return true; }
  if (c == "green") { out = CRGB::Green; return true; }
  if (c == "blue")  { out = CRGB::Blue;  return true; }
  if (c == "white") { out = CRGB::White; return true; }
  if (c == "off")   { out = CRGB::Black; return true; }
  return false;
}

static void forEachTarget(const String& target, void (*fn)(PinState&)) {
  if (target == "all") {
    for (auto& p : pins) fn(p);
    return;
  }
  int pinNum = target.toInt();
  PinState* p = findPin(pinNum);
  if (!p) {
    Serial.printf("Unknown pin '%s'. Valid: 16, 17, 18, all\n", target.c_str());
    return;
  }
  fn(*p);
}

static void handleLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  int sp1 = line.indexOf(' ');
  String cmd = (sp1 == -1) ? line : line.substring(0, sp1);
  String rest = (sp1 == -1) ? "" : line.substring(sp1 + 1);
  cmd.toLowerCase();

  if (cmd == "help" || cmd == "?") { printHelp(); return; }
  if (cmd == "status") { printStatus(); return; }

  int sp2 = rest.indexOf(' ');
  String target = (sp2 == -1) ? rest : rest.substring(0, sp2);
  String args = (sp2 == -1) ? "" : rest.substring(sp2 + 1);
  target.toLowerCase();

  if (cmd == "raw") {
    String levelStr = args; levelStr.toLowerCase();
    int level;
    if (levelStr == "high") level = HIGH;
    else if (levelStr == "low") level = LOW;
    else { Serial.println(F("Usage: raw <pin|all> high|low")); return; }

    if (target == "all") {
      for (auto& p : pins) setRaw(p, level);
    } else {
      PinState* p = findPin(target.toInt());
      if (!p) { Serial.println(F("Unknown pin. Valid: 16, 17, 18, all")); return; }
      setRaw(*p, level);
    }
    Serial.printf("OK: pin(s) %s held %s\n", target.c_str(), level == HIGH ? "HIGH" : "LOW");
    return;
  }

  if (cmd == "led") {
    int sp3 = args.indexOf(' ');
    String colorStr = (sp3 == -1) ? args : args.substring(0, sp3);
    String brightStr = (sp3 == -1) ? "" : args.substring(sp3 + 1);

    CRGB color;
    if (!parseColor(colorStr, color)) {
      Serial.println(F("Usage: led <pin|all> red|green|blue|white|off [brightness 0-255]"));
      return;
    }
    if (brightStr.length() > 0) {
      uint8_t b = constrain(brightStr.toInt(), 0, 255);
      FastLED.setBrightness(b);
    }

    if (target == "all") {
      for (auto& p : pins) setLed(p, color);
    } else {
      PinState* p = findPin(target.toInt());
      if (!p) { Serial.println(F("Unknown pin. Valid: 16, 17, 18, all")); return; }
      setLed(*p, color);
    }
    Serial.printf("OK: pin(s) %s -> LED %s\n", target.c_str(), colorStr.c_str());
    return;
  }

  if (cmd == "stop") {
    if (target.length() == 0) target = "all";
    if (target == "all") {
      for (auto& p : pins) setStop(p);
    } else {
      PinState* p = findPin(target.toInt());
      if (!p) { Serial.println(F("Unknown pin. Valid: 16, 17, 18, all")); return; }
      setStop(*p);
    }
    Serial.printf("OK: pin(s) %s released\n", target.c_str());
    return;
  }

  Serial.printf("Unknown command '%s'. Type 'help'.\n", cmd.c_str());
}

void setup() {
  Serial.begin(115200);
  delay(500);

  FastLED.addLeds<LED_TYPE, PIN_MATRIX0, COLOR_ORDER>(leds0, LEDS_MATRIX0);
  FastLED.addLeds<LED_TYPE, PIN_MATRIX1, COLOR_ORDER>(leds1, LEDS_MATRIX1);
  FastLED.addLeds<LED_TYPE, PIN_MATRIX2, COLOR_ORDER>(leds2, LEDS_MATRIX2);
  FastLED.setBrightness(32); // keep current sane for a bench PSU while probing
  FastLED.clear(true);

  // Auto-start every pin in RAW HIGH so a steady, predictable voltage is
  // present the instant this boots -- no serial command needed to begin
  // probing with a multimeter across ESP pin / level-shifter in / out.
  for (auto& p : pins) setRaw(p, HIGH);

  printHelp();
  printStatus();
  Serial.println(F("Auto-started: all pins RAW HIGH for voltage probing."));
  Serial.println(F("When ready, switch to the LED test without reflashing, e.g.: led all white"));
}

void loop() {
  static String buf;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') { handleLine(buf); buf = ""; }
    else if (c != '\r') { buf += c; }
  }

  bool anyLed = false;
  for (auto& p : pins) {
    if (p.mode == MODE_LED) {
      fill_solid(p.leds, p.numLeds, p.ledColor);
      anyLed = true;
    } else if (p.mode == MODE_RAW) {
      digitalWrite(p.pin, p.rawLevel); // keep re-asserting the steady level
    }
  }
  if (anyLed) FastLED.show();

  delay(20); // ~50Hz refresh for LED mode; harmless idle pacing otherwise
}
