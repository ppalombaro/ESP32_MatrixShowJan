/* WebPages.cpp
   HTML page generation with PROGMEM templates
   VERSION: V15.1.0-2026-01-04T10:15:00Z - Split from WebController
   
   V15.1.0-2026-01-04T10:15:00Z - Created modular HTML generation
*/

#include "WebPages.h"

// V15.1.0-2026-01-04T10:15:00Z - HTML templates in PROGMEM
const char HTML_HEADER[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<title>LED Matrix Control</title>
<style>
body { font-family: Arial, sans-serif; max-width: 800px; margin: 20px auto; padding: 20px; background: #1a1a1a; color: #fff; }
.card { background: #2a2a2a; border-radius: 10px; padding: 20px; margin: 20px 0; }
h1, h2, h3 { color: #4CAF50; }
button { background: #4CAF50; color: white; border: none; padding: 12px 24px; margin: 5px; border-radius: 5px; cursor: pointer; font-size: 16px; }
button:hover { background: #45a049; }
button.danger { background: #f44336; }
button.danger:hover { background: #da190b; }
.btn-group { display: flex; flex-wrap: wrap; gap: 10px; margin: 10px 0; }
input[type=range] { width: 100%; }
input[type=checkbox] { margin-right: 10px; }
.checkbox-group { display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: 10px; }
.checkbox-item { padding: 8px; background: #333; border-radius: 5px; }
a { color: #4CAF50; text-decoration: none; }
a:hover { text-decoration: underline; }
</style>
</head>
<body>
)rawliteral";

const char HTML_NAV[] PROGMEM = R"rawliteral(
<div class='card'>
<h1>LED Matrix Controller</h1>
<nav>
<a href='/'>Home</a> | 
<a href='/config'>Configuration</a> | 
<a href='/preview'>Preview</a> | 
<a href='/countdownconfig'>Countdown</a> | 
<a href='/logs'>Logs</a>
</nav>
</div>
)rawliteral";

const char HTML_FOOTER[] PROGMEM = R"rawliteral(
<div class='card' style='text-align: center; color: #666;'>
LED Matrix Show V15.1.0 | 
<a href='https://github.com' target='_blank'>Docs</a>
</div>
</body>
</html>
)rawliteral";

WebPages::WebPages(ContentManager* cm) : contentMgr(cm) {
}

String WebPages::buildPage(const String& content) {
    // V15.1.0-2026-01-04T10:15:00Z - Assemble complete page
    String page;
    page.reserve(8192);  // Pre-allocate to reduce fragmentation
    
    page += FPSTR(HTML_HEADER);
    page += FPSTR(HTML_NAV);
    page += content;
    page += FPSTR(HTML_FOOTER);
    
    return page;
}

String WebPages::getHomePage(uint8_t currentTheme, uint8_t brightness) {
    // V15.1.0-2026-01-04T10:15:00Z - Main control page
    String html;
    html.reserve(4096);
    
    html += "<div class='card'><h2>Display Control</h2>";
    
    // Theme selector
    html += "<h3>Theme</h3><div class='btn-group'>";
    html += "<button onclick=\"location.href='/mode?theme=0'\">Off</button>";
    html += "<button onclick=\"location.href='/mode?theme=1'\">Christmas</button>";
    html += "<button onclick=\"location.href='/mode?theme=2'\">Halloween</button>";
    html += "<button onclick=\"location.href='/mode?theme=3'\">Thanksgiving</button>";
    html += "<button onclick=\"location.href='/mode?theme=4'\">New Year</button>";
    html += "<button onclick=\"location.href='/mode?theme=5'\">Ohio State</button>";
    html += "<button onclick=\"location.href='/mode?theme=99'\">Random Magic</button>";
    html += "</div>";
    
    html += "<p>Current Theme: <strong>";
    const char* themeNames[] = {"Off", "Christmas", "Halloween", "Thanksgiving", "New Year", "Ohio State", "Random Magic"};
    html += currentTheme <= 5 ? themeNames[currentTheme] : (currentTheme == 99 ? themeNames[6] : "Unknown");
    html += "</strong></p></div>";
    
    // Brightness control
    html += "<div class='card'><h2>Brightness</h2>";
    html += "<input type='range' min='10' max='255' value='" + String(brightness) + "' ";
    html += "oninput=\"fetch('/brightness?val='+this.value)\">";
    html += "<p>Current: <span id='bright'>" + String(brightness) + "</span></p></div>";
    
    // Quick actions
    html += "<div class='card'><h2>Quick Actions</h2><div class='btn-group'>";
    html += "<button onclick=\"location.href='/clear'\">Clear Display</button>";
    html += "<button onclick=\"location.href='/config'\">Configuration</button>";
    html += "<button onclick=\"location.href='/preview'\">Preview Content</button>";
    html += "</div></div>";
    
    return buildPage(html);
}

String WebPages::getConfigPage(const ScheduleConfig& schedule) {
    // V15.1.0-2026-01-04T10:15:00Z - Configuration page
    String html;
    html.reserve(4096);
    
    html += "<div class='card'><h2>Schedule Configuration</h2>";
    html += "<form action='/saveconfig' method='GET'>";
    
    html += "<h3>Show Times</h3>";
    html += "<p>Start: <input type='number' name='startH' min='0' max='23' value='" + String(schedule.startHour) + "'> : ";
    html += "<input type='number' name='startM' min='0' max='59' value='" + String(schedule.startMinute) + "'></p>";
    html += "<p>End: <input type='number' name='endH' min='0' max='23' value='" + String(schedule.endHour) + "'> : ";
    html += "<input type='number' name='endM' min='0' max='59' value='" + String(schedule.endMinute) + "'></p>";
    
    html += "<h3>Mode</h3>";
    html += "<select name='mode'>";
    html += "<option value='random'" + String(schedule.mode == "random" ? " selected" : "") + ">Random</option>";
    html += "<option value='sequential'" + String(schedule.mode == "sequential" ? " selected" : "") + ">Sequential</option>";
    html += "</select>";
    
    html += "<p><input type='checkbox' name='enabled' value='1'" + String(schedule.enabled ? " checked" : "") + ">Enable Scheduler</p>";
    
    html += "<button type='submit'>Save Configuration</button>";
    html += "</form></div>";
    
    return buildPage(html);
}

String WebPages::getPreviewPage(uint8_t currentTheme) {
    // V15.1.0-2026-01-04T10:15:00Z - Auto-generated preview from ContentManager
    String html;
    html.reserve(8192);
    
    // Determine current theme name for filtering
    String themeFilter = "";
    if (currentTheme == 1) themeFilter = "christmas";
    else if (currentTheme == 2) themeFilter = "halloween";
    else if (currentTheme == 3) themeFilter = "thanksgiving";
    else if (currentTheme == 4) themeFilter = "newyear";
    else if (currentTheme == 5) themeFilter = "osu";
    
    // Scenes
    html += "<div class='card'><h2>Scenes</h2><div class='btn-group'>";
    auto scenes = contentMgr->getScenes(themeFilter);
    for (int i = 0; i < scenes.size(); i++) {
        html += "<button onclick=\"location.href='/setscene?id=" + String(i) + "'\">";
        html += scenes[i].displayName;
        html += "</button>";
    }
    if (scenes.empty()) {
        html += "<p>No scenes available for current theme</p>";
    }
    html += "</div></div>";
    
    // Animations
    html += "<div class='card'><h2>Animations</h2><div class='btn-group'>";
    auto animations = contentMgr->getAnimations(themeFilter);
    for (int i = 0; i < animations.size(); i++) {
        html += "<button onclick=\"location.href='/setanim?id=" + String(i) + "'\">";
        html += animations[i].displayName;
        html += "</button>";
    }
    if (animations.empty()) {
        html += "<p>No animations available for current theme</p>";
    }
    html += "</div></div>";
    
    // Tests
    auto tests = contentMgr->getTests();
    if (!tests.empty()) {
        html += "<div class='card'><h2>Test Patterns</h2><div class='btn-group'>";
        for (int i = 0; i < tests.size(); i++) {
            html += "<button onclick=\"location.href='/settest?id=" + String(i) + "'\">";
            html += tests[i].displayName;
            html += "</button>";
        }
        html += "</div></div>";
    }
    
    return buildPage(html);
}

String WebPages::getRandomConfigPage(const ScheduleConfig& schedule) {
    // V15.1.0-2026-01-04T10:15:00Z - Checkbox-based random configuration
    String html;
    html.reserve(8192);
    
    html += "<div class='card'><h2>Random Show Configuration</h2>";
    html += "<p>Select which content can appear during random shows:</p>";
    html += "<form action='/randomconfig' method='GET'>";
    
    // Get all content
    auto scenes = contentMgr->getScenes();
    auto animations = contentMgr->getAnimations();
    
    // Scenes checkboxes
    if (!scenes.empty()) {
        html += "<h3>Scenes</h3><div class='checkbox-group'>";
        for (auto& item : scenes) {
            bool checked = false;
            for (auto& allowed : schedule.allowedContent) {
                if (allowed == item.filename) {
                    checked = true;
                    break;
                }
            }
            html += "<div class='checkbox-item'>";
            html += "<input type='checkbox' name='content' value='" + item.filename + "'";
            if (checked) html += " checked";
            html += ">" + item.displayName + "</div>";
        }
        html += "</div>";
    }
    
    // Animations checkboxes
    if (!animations.empty()) {
        html += "<h3>Animations</h3><div class='checkbox-group'>";
        for (auto& item : animations) {
            bool checked = false;
            for (auto& allowed : schedule.allowedContent) {
                if (allowed == item.filename) {
                    checked = true;
                    break;
                }
            }
            html += "<div class='checkbox-item'>";
            html += "<input type='checkbox' name='content' value='" + item.filename + "'";
            if (checked) html += " checked";
            html += ">" + item.displayName + "</div>";
        }
        html += "</div>";
    }
    
    html += "<button type='submit' style='margin-top: 20px;'>Save Random Config</button>";
    html += "</form></div>";
    
    return buildPage(html);
}

String WebPages::getCountdownConfigPage(uint16_t xmasYear, uint8_t xmasMonth, uint8_t xmasDay,
                                       uint16_t nyYear, uint8_t nyMonth, uint8_t nyDay) {
    // V15.1.0-2026-01-04T10:15:00Z - Countdown date configuration
    String html;
    html.reserve(2048);
    
    html += "<div class='card'><h2>Countdown Configuration</h2>";
    
    html += "<h3>Christmas Countdown</h3>";
    html += "<form action='/setxmascountdown' method='GET'>";
    html += "<p>Year: <input type='number' name='year' min='2024' max='2030' value='" + String(xmasYear) + "'></p>";
    html += "<p>Month: <input type='number' name='month' min='1' max='12' value='" + String(xmasMonth) + "'></p>";
    html += "<p>Day: <input type='number' name='day' min='1' max='31' value='" + String(xmasDay) + "'></p>";
    html += "<button type='submit'>Set Christmas Date</button>";
    html += "</form>";
    
    html += "<h3>New Year Countdown</h3>";
    html += "<form action='/setnycountdown' method='GET'>";
    html += "<p>Year: <input type='number' name='year' min='2024' max='2030' value='" + String(nyYear) + "'></p>";
    html += "<p>Month: <input type='number' name='month' min='1' max='12' value='" + String(nyMonth) + "'></p>";
    html += "<p>Day: <input type='number' name='day' min='1' max='31' value='" + String(nyDay) + "'></p>";
    html += "<button type='submit'>Set New Year Date</button>";
    html += "</form>";
    
    html += "<button class='danger' onclick=\"location.href='/resetcountdown'\">Reset to Defaults</button>";
    html += "</div>";
    
    return buildPage(html);
}

String WebPages::getLogsPage(const String* logs, int maxLogs, int currentIndex) {
    // V15.1.0-2026-01-04T10:15:00Z - System logs display
    String html;
    html.reserve(4096);
    
    html += "<div class='card'><h2>System Logs</h2>";
    html += "<div style='background: #1a1a1a; padding: 15px; border-radius: 5px; font-family: monospace;'>";
    
    for (int i = 0; i < maxLogs; i++) {
        int idx = (currentIndex + i) % maxLogs;
        if (logs[idx].length() > 0) {
            html += logs[idx] + "<br>";
        }
    }
    
    html += "</div>";
    html += "<button onclick='location.reload()'>Refresh</button>";
    html += "</div>";
    
    return buildPage(html);
}
