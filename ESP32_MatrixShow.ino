/* ESP32_MatrixShow.ino
   Main controller for dual LED matrix multi-holiday display
   VERSION: V15.4.1-2026-01-06
   - FIX: Added missing <Preferences.h> include
   - FIX: Completed isScheduledTime() logic with midnight-wrap support
   - UserID-Defect2: Accurate Minute-level scheduling
   - Watchdog sync: Log matches 30s hardware timeout
*/

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Preferences.h>      // Required for permanent storage
#include "esp_task_wdt.h"      // Required for watchdog timer

// Project Headers
#include "Config.h"
#include "MatrixDisplay.h"
#include "ThemeManager.h"
#include "WebController.h"
#include "ContentManager.h"

// Global objects required by the system 
MatrixDisplay display;
ContentManager& content = ContentManager::instance();
ThemeManager& themes = ThemeManager::instance();
WebController& web = WebController::instance();
Preferences preferences;  

// NTP Client Setup
WiFiUDP ntpUDP;
// -18000 is the offset in seconds for EST (-5 hours)
NTPClient timeClient(ntpUDP, "pool.ntp.org", -18000, 60000); 

// Function Prototype
bool isScheduledTime();

void setup() {
  Serial.begin(115200);
  delay(1000); // Give serial time to stabilize
  
  Serial.println("\n--- Starting ESP32 Matrix Show V15.4.1 ---");

  // Watchdog Sync - 30s hardware timeout 
  esp_task_wdt_init(30, true); 
  esp_task_wdt_add(NULL); // Add main loop to watchdog
  Serial.println("System: Watchdog set to 30s");

  // Initialize System Components
x  if (!display.begin()) {
    Serial.println("Display Init Failed!");
  }
  
  if (!content.begin()) {
    Serial.println("Content Manager (FFat) Init Failed!");
  }

  // Pass references to dependent managers
  themes.begin(&display, &content);
  web.begin(&themes, &content);
  
  // Start NTP
  timeClient.begin();
  
  Serial.println("System Ready.");
}

void loop() {
  // Reset Watchdog Timer every loop iteration
  esp_task_wdt_reset();
  
  // Update network services
  timeClient.update();
  web.handleClient();
  
  // Logical control for the display
  if (isScheduledTime()) {
    themes.update(); // Run current theme/animation/scene
  } else {
    // If outside of scheduled hours, keep the display dark
    static bool was_on = true;
    if (was_on) {
      display.clear();
      display.show();
      was_on = false;
      Serial.println("Schedule: Display entering Sleep Mode (Off Time)");
    }
  }
}

/**
 * Calculates if the current time falls within the user-defined schedule.
 * Corrected to handle schedules that cross over midnight (e.g., 22:00 to 02:00).
 */
bool isScheduledTime() {
  ScheduleConfig config;
  
  // If schedule fails to load, default to always ON (true) for safety
  if (!content.loadSchedule(config)) {
    return true; 
  }

  // Convert current time and schedule bounds to total minutes from midnight
  int currentTotalMinutes = timeClient.getHours() * 60 + timeClient.getMinutes();
  int startTotalMinutes = config.startHour * 60 + config.startMinute;
  int endTotalMinutes = config.endHour * 60 + config.endMinute;

  // Case A: Simple schedule (e.g., 8:00 AM to 10:00 PM)
  if (startTotalMinutes < endTotalMinutes) {
    return (currentTotalMinutes >= startTotalMinutes && currentTotalMinutes < endTotalMinutes);
  } 
  // Case B: Overnight schedule (e.g., 10:00 PM to 2:00 AM)
  else if (startTotalMinutes > endTotalMinutes) {
    return (currentTotalMinutes >= startTotalMinutes || currentTotalMinutes < endTotalMinutes);
  }
  
  // Case C: Start equals End - interpret as "Run 24/7"
  return true;
}