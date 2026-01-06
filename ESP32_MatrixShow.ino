/* ESP32_MatrixShow.ino
   Main controller for dual LED matrix multi-holiday display
   VERSION: V15.1.0-2026-01-04T10:30:00Z - Integrated ContentManager architecture
   
   V15.1.0-2026-01-04T10:30:00Z - Added ContentManager for auto-discovery
   V15.1.0-2026-01-04T10:30:00Z - WebController now uses modular WebPages/WebActions
   V15.1.0-2026-01-04T10:30:00Z - Schedule config moved to FFat JSON
*/

#include "Config.h"
#include "MatrixDisplay.h"
#include "Scenes.h"
#include "Animations.h"
#include "TestScenes.h"
#include "ThemeManager.h"
#include "WebController.h"
#include "HolidayAnimations.h"
#include "Logger.h"
#include "ContentManager.h"  // V15.1.0-2026-01-04T10:30:00Z - New auto-discovery system
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>
#include "esp_task_wdt.h"

// Global objects
MatrixDisplay display;
ThemeManager themes;    
WebController web;
ContentManager content;  // V15.1.0-2026-01-04T10:30:00Z - Content auto-discovery
Preferences preferences;

// V15.1.0-2026-01-04T10:30:00Z - Scheduler now uses ContentManager
// Global run mode still in preferences for compatibility
uint8_t global_run_mode = RUN_MODE_MANUAL;

bool schedulerForcedOff = false;

// Track state changes
unsigned long loopCount = 0;

// WiFi reconnection tracking
unsigned long lastWiFiCheck = 0;

// NTP Client Setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);  

// Forward declarations
bool isScheduledTime();
void loadSchedulerSettings();
void loadBrightness();

// V15.1.0-2026-01-04T10:30:00Z - Web server task runs on Core 0
void webServerTask(void* parameter) {
  while (true) {
    web.handleClient();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// ESP32_MatrixShow.ino - Version 15.2.1 - 2026-01-04 13:45

// Replace the WDT initialization code in setup() around line 67:
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // v15.2.1 - Use direct API call instead of deprecated config struct
  esp_task_wdt_init(30, true);  // 30 second timeout, panic on timeout
  esp_task_wdt_add(NULL);  // Add current task to WDT
  
  Serial.println("Watchdog timer enabled (10s timeout)");
  
  Serial.println("\n========================================");
  Serial.println("ESP32 Matrix Display - V15.1.0");  // V15.1.0-2026-01-04T10:30:00Z
  Serial.println("Features: Auto-Discovery, Modular Web, FFat Config");  // V15.1.0-2026-01-04T10:30:00Z
  Serial.println("Starting setup...");
  Serial.println("========================================\n");

  // V15.1.0-2026-01-04T10:30:00Z - Initialize content discovery FIRST
  Serial.println("Step 1: Initializing content manager...");
  if (!content.begin()) {
    Serial.println("WARNING: Content manager initialization failed - using hardcoded content");
  } else {
    Serial.printf("Content discovered: %d scenes, %d animations, %d tests\n",
                  content.getTotalScenes(), 
                  content.getTotalAnimations(),
                  content.getTotalTests());
  }

  // Load Scheduler Settings
  Serial.println("Step 2: Loading scheduler settings...");
  loadSchedulerSettings();
  loadBrightness();
  FastLED.setDither(DISABLE_DITHER);

  // Initialize Hardware
  Serial.println("Step 3: Initializing LED matrices...");
  display.begin();
  
  Serial.println("Step 4: Initializing theme manager...");
  themes.begin(&display, &content);
  
  // V15.1.0-2026-01-04T10:30:00Z - Pass ContentManager to WebController
  Serial.println("Step 5: Starting WiFi and web server...");
  web.begin(&themes, &content);

  // Initialize Time Client
  Serial.println("Step 6: Starting NTP client...");
  timeClient.begin();
  
  Serial.println("Step 7: Initializing logger...");
  logger.begin(&timeClient);
  
  // Initialize schedulerForcedOff based on current state
  timeClient.update();
  if (global_run_mode == RUN_MODE_SCHEDULE) {
    bool inSchedule = isScheduledTime();
    if (inSchedule) {
      schedulerForcedOff = true;
      logger.log("Startup: Within schedule window - will activate Random Magic on first loop");
    } else {
      schedulerForcedOff = false;
      logger.log("Startup: Outside schedule window - display will remain OFF");
    }
  }
  
  Serial.print("NTP Client started. Current run mode: ");
  Serial.println(global_run_mode == RUN_MODE_MANUAL ? "MANUAL" : "SCHEDULE");
  
  // V15.1.0-2026-01-04T10:30:00Z - Display discovered content summary
  Serial.println("\n========================================");
  Serial.println("Content Discovery Summary:");
  Serial.printf("  Total Scenes: %d\n", content.getTotalScenes());
  Serial.printf("  Total Animations: %d\n", content.getTotalAnimations());
  Serial.printf("  Total Tests: %d\n", content.getTotalTests());
  Serial.println("Available Themes:");
  Serial.println("  - Christmas | Halloween | Thanksgiving");
  Serial.println("  - New Year | Ohio State");
  Serial.println("========================================\n");
  
  logger.log("Setup complete!");
  Serial.println("Serial Commands: s/a/r/t/c/n/0-9/+/-");
  Serial.println("Web Interface: http://matrix.local\n");
  
  // V15.1.0-2026-01-04T10:30:00Z - Create web server task on Core 0
  Serial.println("Step 8: Creating web server task on Core 0...");
  xTaskCreatePinnedToCore(
    webServerTask,
    "WebServer",
    8192,
    NULL,
    1,
    NULL,
    0
  );
  logger.log("Web server task started on Core 0 - animations will run smooth on Core 1!");
  
  Serial.println("Monitoring activity...\n");
}

void loop() {
  esp_task_wdt_reset();
  
  unsigned long now = millis();
  loopCount++;
  
  // WiFi auto-reconnection check every 5 minutes
  if (now - lastWiFiCheck > 300000) {
    lastWiFiCheck = now;
    if (WiFi.status() != WL_CONNECTED) {
      logger.log("WiFi connection lost! Attempting reconnect...");
      WiFi.disconnect();
      delay(100);
      WiFi.begin(WIFI_SSID, WIFI_PASS);
      
      int attempts = 0;
      while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(500);
        Serial.print(".");
        attempts++;
      }
      
      if (WiFi.status() == WL_CONNECTED) {
        logger.log("WiFi reconnected successfully!");
      } else {
        logger.log("WiFi reconnection failed - continuing without web access");
      }
    }
  }
  
  logger.updateHeartbeat(themes.getCurrentTheme(), ESP.getFreeHeap());
  
  // Update NTP only once per minute
  static unsigned long lastNTPUpdate = 0;
  if (now - lastNTPUpdate > 60000 || lastNTPUpdate == 0) {
    timeClient.update();
    lastNTPUpdate = now;
  }

  // V15.1.0-2026-01-04T10:30:00Z - Scheduler now uses FFat schedule.json
  if (global_run_mode == RUN_MODE_SCHEDULE) {
    bool inSchedule = isScheduledTime();
    
    if (inSchedule && schedulerForcedOff) {
      themes.setTheme(THEME_RANDOM_MAGIC);
      schedulerForcedOff = false;
      logger.log("Scheduler START - Random Magic activated");
    } else if (!inSchedule && !schedulerForcedOff) {
      themes.setTheme(THEME_OFF);
      schedulerForcedOff = true;
      logger.log("Scheduler STOP - Display OFF");
    }
  } else {
    schedulerForcedOff = false;
  }

  // Update display
  themes.update();

  // Handle serial commands
  themes.handleSerial();
}

bool isScheduledTime() {
  // V15.1.0-2026-01-04T10:30:00Z - Load schedule from ContentManager
  ScheduleConfig config;
  if (!content.loadSchedule(config) || !config.enabled) {
    return false;  // No valid schedule or disabled
  }
  
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  // Convert to minutes for easier comparison
  int currentTime = currentHour * 60 + currentMinute;
  int startTime = config.startHour * 60 + config.startMinute;
  int endTime = config.endHour * 60 + config.endMinute;
  
  if (startTime <= endTime) {
    return (currentTime >= startTime && currentTime < endTime);
  } else {
    // Wraps midnight
    return (currentTime >= startTime || currentTime < endTime);
  }
}

void loadSchedulerSettings() {
  preferences.begin(PREFS_NAMESPACE, true);
  
  global_run_mode = preferences.getUChar(RUN_MODE_KEY, RUN_MODE_MANUAL);
  // V15.1.0-2026-01-04T10:30:00Z - Start/end hours now in schedule.json, kept for compatibility

  preferences.end();
  Serial.printf("Scheduler: Mode=%d\n", global_run_mode);
}

void loadBrightness() {
  preferences.begin(PREFS_NAMESPACE, true);
  uint8_t savedBrightness = preferences.getUChar(BRIGHTNESS_KEY, DEFAULT_BRIGHTNESS);
  preferences.end();
  
  display.setBrightness(savedBrightness);
  Serial.printf("Brightness: %d\n", savedBrightness);
}
