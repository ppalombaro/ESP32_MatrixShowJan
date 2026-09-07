/* Countdown.cpp
   Countdown display implementation
   VERSION: V16.2.0-2026-01-10T18:05:00Z - Initial implementation with theme colors and flash behavior
*/

#include "Countdown.h"
#include "MatrixDisplay.h"
#include "ThemeManager.h"
#include "Logger.h"
#include <ArduinoJson.h>
#include <time.h>
#include "esp_partition.h"
#include "esp_spi_flash.h"

#define DATA_PARTITION_OFFSET 0x290000

// V16.2.0-2026-01-10T18:05:00Z - 3x5 digit font
const uint8_t Countdown::DIGIT_3X5[][5] = {
  {0b111, 0b101, 0b101, 0b101, 0b111}, // 0
  {0b010, 0b110, 0b010, 0b010, 0b111}, // 1
  {0b111, 0b001, 0b111, 0b100, 0b111}, // 2
  {0b111, 0b001, 0b111, 0b001, 0b111}, // 3
  {0b101, 0b101, 0b111, 0b001, 0b001}, // 4
  {0b111, 0b100, 0b111, 0b001, 0b111}, // 5
  {0b111, 0b100, 0b111, 0b101, 0b111}, // 6
  {0b111, 0b001, 0b001, 0b001, 0b001}, // 7
  {0b111, 0b101, 0b111, 0b101, 0b111}, // 8
  {0b111, 0b101, 0b111, 0b001, 0b111}  // 9
};

// V16.4.13 - 3x5 unit labels (D/H/M/S), same bit layout as DIGIT_3X5
const uint8_t Countdown::LABEL_D[5] = {0b110, 0b101, 0b101, 0b101, 0b110};
const uint8_t Countdown::LABEL_H[5] = {0b101, 0b101, 0b111, 0b101, 0b101};
const uint8_t Countdown::LABEL_M[5] = {0b101, 0b111, 0b111, 0b101, 0b101};
const uint8_t Countdown::LABEL_S[5] = {0b111, 0b100, 0b111, 0b001, 0b111};

Countdown::Countdown(MatrixDisplay* display, ThemeManager* themeMgr)
    : disp(display), themes(themeMgr), targetTime(0),
      lastUpdate(0), flashState(false), lastFlash(0) {
}

bool Countdown::loadFromJSON(const String& jsonPath) {
    // V16.2.0-2026-01-10T18:30:00Z - Read JSON from flash storage with human-readable date support
    Logger::instance().log("[Countdown] Loading: " + jsonPath);
    
    uint32_t flash_addr = DATA_PARTITION_OFFSET;
    uint32_t file_count = 0;
    
    if (esp_flash_read(NULL, &file_count, flash_addr, 4) != ESP_OK) {
        Logger::instance().log("[Countdown] Failed to read file count");
        return false;
    }
    
    flash_addr += 4;
    
    // Search for the file
    for (uint32_t i = 0; i < file_count; i++) {
        uint16_t path_len = 0;
        if (esp_flash_read(NULL, &path_len, flash_addr, 2) != ESP_OK) break;
        flash_addr += 2;
        
        char path_buf[256];
        if (esp_flash_read(NULL, path_buf, flash_addr, path_len) != ESP_OK) break;
        flash_addr += path_len;
        path_buf[path_len] = '\0';
        
        uint32_t content_len = 0;
        if (esp_flash_read(NULL, &content_len, flash_addr, 4) != ESP_OK) break;
        flash_addr += 4;
        
        if (String(path_buf) == jsonPath) {
            // Found it! Read content
            char* content = new char[content_len + 1];
            if (esp_flash_read(NULL, content, flash_addr, content_len) == ESP_OK) {
                content[content_len] = '\0';
                
                // Parse JSON
                DynamicJsonDocument doc(1024);
                DeserializationError error = deserializeJson(doc, content);
                delete[] content;
                
                if (error) {
                    Logger::instance().log("[Countdown] JSON parse error: " + String(error.c_str()));
                    return false;
                }
                
                // V16.2.0-2026-01-10T18:30:00Z - Support both Unix timestamp and human-readable date
                if (doc.containsKey("targetDate")) {
                    // Check if it's a number (Unix timestamp) or string (human-readable)
                    if (doc["targetDate"].is<long>()) {
                        targetTime = doc["targetDate"].as<long>();
                        Logger::instance().log("[Countdown] Target: " + String(targetTime));
                    } else if (doc["targetDate"].is<const char*>()) {
                        // Parse "YYYY-MM-DD HH:MM:SS" format
                        String dateStr = doc["targetDate"].as<String>();
                        targetTime = parseHumanDate(dateStr);
                        if (targetTime > 0) {
                            Logger::instance().log("[Countdown] Parsed target: " + String(targetTime));
                        } else {
                            Logger::instance().log("[Countdown] Failed to parse date: " + dateStr);
                            return false;
                        }
                    }
                    return true;
                }
                
                Logger::instance().log("[Countdown] No targetDate field");
                return false;
            }
            delete[] content;
            return false;
        }
        
        // Skip to next file
        flash_addr += content_len;
        uint32_t padding = (512 - (flash_addr % 512)) % 512;
        flash_addr += padding;
    }
    
    Logger::instance().log("[Countdown] File not found: " + jsonPath);
    return false;
}

void Countdown::begin() {
    lastUpdate = millis();
    flashState = false;
    lastFlash = 0;
}

void Countdown::update() {
    unsigned long now = millis();
    if (now - lastUpdate < 1000) return;  // V16.2.0-2026-01-10T18:05:00Z - Update every second
    lastUpdate = now;
    
    // V16.4.13 - system clock (SNTP via configTzTime in main.cpp)
    time_t currentTime = time(nullptr);
    if (currentTime < 100000) {
        return;  // clock not synced yet - don't draw a bogus countdown
    }

    disp->clear();
    
    // Calculate time difference
    long diff = targetTime - currentTime;
    bool isZero = (diff <= 0);
    
    if (isZero) {
        diff = 0;  // V16.2.0-2026-01-10T18:05:00Z - Don't go negative
        
        // Flash toggle every 500ms
        if (now - lastFlash >= 500) {
            flashState = !flashState;
            lastFlash = now;
        }
    }
    
    // Calculate time components
    long days = diff / 86400;
    long hours = (diff % 86400) / 3600;
    long minutes = (diff % 3600) / 60;
    long seconds = diff % 60;
    
    // Clamp to 99 max
    if (days > 99) days = 99;
    if (hours > 99) hours = 99;
    if (minutes > 99) minutes = 99;
    if (seconds > 99) seconds = 99;
    
    // V16.4.13 - One metric per row, unit label at the left, digits shifted right.
    // Layout: Matrix 1 (LEFT window)  = Days    (top) / Hours   (bottom)
    //         Matrix 0 (RIGHT window) = Minutes (top) / Seconds (bottom)
    const int ROW1_Y = 4;
    const int ROW2_Y = 14;

    drawMetric(1, ROW1_Y, 'D', days,    isZero);
    drawMetric(1, ROW2_Y, 'H', hours,   isZero);
    drawMetric(0, ROW1_Y, 'M', minutes, isZero);
    drawMetric(0, ROW2_Y, 'S', seconds, isZero);

    disp->show();
}

void Countdown::drawMetric(int matrix, int y, char label, long value, bool shouldFlash) {
    // V16.4.13 - Theme colors: Label=color1, Box=color2, Numbers=color3
    CRGB headerColor = themes->getColor1();
    CRGB boxColor    = themes->getColor2();
    CRGB numberColor = themes->getColor3();

    const int LABEL_X  = 2;   // unit letter sits where the digits used to start
    const int DIGIT1_X = 9;   // digits shifted right ~7 columns
    const int DIGIT2_X = 13;

    // Unit label (D/H/M/S)
    drawLabel(matrix, LABEL_X, y, label, headerColor);

    // Box border around the number field
    drawRectBorder(matrix, DIGIT1_X - 2, y - 1, DIGIT2_X + 3, y + 5, boxColor);

    // Digits (blank while flashing "00" past target)
    if (!shouldFlash || flashState) {
        int digit1 = (value / 10) % 10;
        int digit2 = value % 10;
        drawDigit(matrix, DIGIT1_X, y, digit1, numberColor);
        drawDigit(matrix, DIGIT2_X, y, digit2, numberColor);
    }
}

void Countdown::drawDigit(int matrix, int x, int y, int digit, CRGB color) {
    if (digit < 0 || digit > 9) return;
    
    for (int row = 0; row < 5; row++) {
        uint8_t rowPattern = DIGIT_3X5[digit][row];
        for (int col = 0; col < 3; col++) {
            if (rowPattern & (0b100 >> col)) {
                disp->setPixel(matrix, x + col, y + row, color);
            }
        }
    }
}

void Countdown::drawRectBorder(int matrix, int x1, int y1, int x2, int y2, CRGB color) {
    // Draw horizontal lines
    for (int x = x1; x <= x2; x++) {
        disp->setPixel(matrix, x, y1, color);
        disp->setPixel(matrix, x, y2, color);
    }
    // Draw vertical lines
    for (int y = y1 + 1; y <= y2 - 1; y++) {
        disp->setPixel(matrix, x1, y, color);
        disp->setPixel(matrix, x2, y, color);
    }
}

void Countdown::drawLabel(int matrix, int x, int y, char label, CRGB color) {
    // V16.4.13 - Draw a 3x5 unit letter with its top-left at (x, y).
    const uint8_t* glyph = nullptr;
    switch (label) {
        case 'D': glyph = LABEL_D; break;
        case 'H': glyph = LABEL_H; break;
        case 'M': glyph = LABEL_M; break;
        case 'S': glyph = LABEL_S; break;
        default:  return;
    }
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            if (glyph[row] & (0b100 >> col)) {
                disp->setPixel(matrix, x + col, y + row, color);
            }
        }
    }
}

void Countdown::setTargetDate(time_t targetEpoch) {
    targetTime = targetEpoch;
}

// V16.2.0-2026-01-10T18:30:00Z - Parse "YYYY-MM-DD HH:MM:SS" to Unix timestamp (UTC)
time_t Countdown::parseHumanDate(const String& dateStr) {
    struct tm timeinfo;
    memset(&timeinfo, 0, sizeof(timeinfo));
    
    // Expected format: "2025-12-25 05:00:00"
    int year, month, day, hour, minute, second;
    
    if (sscanf(dateStr.c_str(), "%d-%d-%d %d:%d:%d", 
               &year, &month, &day, &hour, &minute, &second) == 6) {
        
        timeinfo.tm_year = year - 1900;  // Years since 1900
        timeinfo.tm_mon = month - 1;     // Months since January (0-11)
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = minute;
        timeinfo.tm_sec = second;
        
        // V16.4.13 - TZ is set (configTzTime), so mktime treats the fields as
        // local wall-clock and returns the correct UTC epoch. targetDate in the
        // JSON is therefore interpreted as local time, DST-aware.
        timeinfo.tm_isdst = -1;
        return mktime(&timeinfo);
    }
    
    Logger::instance().log("[Countdown] Invalid date format, expected: YYYY-MM-DD HH:MM:SS");
    return 0;
}
