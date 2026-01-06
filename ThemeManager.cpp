/* ThemeManager.cpp
   Updated for TRUE Auto-Discovery
*/

#include "ThemeManager.h"
#include "Logger.h"
#include <Preferences.h>  
#include <FastLED.h>

extern Preferences preferences;
extern Logger logger;

// NOTE: TOTAL counts are removed because they are now dynamic!
#define TOTAL_TEST_PATTERNS 2
#define TOTAL_ANIMATIONS 14 
#define TOTAL_HALLOWEEN_ANIMS 2
#define TOTAL_THANKSGIVING_ANIMS 1
#define TOTAL_NEWYEAR_ANIMS 2
#define TOTAL_OSU_ANIMS 2

ThemeManager::ThemeManager() {
    disp = nullptr;
    sceneData = nullptr;
    contentMgr = nullptr; // Initialize to null
    
    // ... (rest of constructor same as before)
    staticScenes = nullptr;
    testScenes = nullptr;
    chaseAnimation = nullptr;
    snowfallEffect = nullptr;
    scrollAnimation = nullptr;
    countdownAnimation = nullptr;
    spinningSnowflake = nullptr;
    sparklingStar = nullptr;
    concentricRectangles = nullptr;
    concentricCircles = nullptr;
    concentricTriangles = nullptr;
    morphingGeometry = nullptr;
    flyingBats = nullptr;
    pirateShip = nullptr;
    bleedingWindows = nullptr;
    lightningFlash = nullptr;
    halloweenScroll = nullptr;
    spookyEyes = nullptr;
    thanksgivingScroll = nullptr;
    fallingLeaves = nullptr;
    newYearCountdown = nullptr;
    newYearScroll = nullptr;
    fireworks = nullptr;
    colorSpinningStar = nullptr;
    osuScroll = nullptr;
    ohioLetters = nullptr;
    osuColorWave = nullptr;
}

ThemeManager::~ThemeManager() {
    // ... (destructor same as before)
    delete osuColorWave;
    delete ohioLetters;
    delete osuScroll;
    delete colorSpinningStar;
    delete fireworks;
    delete newYearScroll;
    delete newYearCountdown;
    delete fallingLeaves;
    delete thanksgivingScroll;
    delete spookyEyes;
    delete halloweenScroll;
    delete lightningFlash;
    delete bleedingWindows;
    delete pirateShip;
    delete flyingBats;
    delete morphingGeometry;
    delete concentricTriangles;
    delete concentricCircles;
    delete concentricRectangles;
    delete sparklingStar;
    delete spinningSnowflake;
    delete countdownAnimation;
    delete scrollAnimation;
    delete snowfallEffect;
    delete chaseAnimation;
    delete testScenes;
    delete staticScenes;
    delete sceneData;
}

// MODIFIED: Accepts ContentManager pointer
void ThemeManager::begin(MatrixDisplay* display, ContentManager* contentManager) {
    disp = display;
    contentMgr = contentManager; // Store it
    
    preferences.begin(PREFS_NAMESPACE, false);
    uint8_t savedBrightness = preferences.getUChar(BRIGHTNESS_KEY, 128);
    preferences.end();
    setBrightness(savedBrightness);
    
    sceneData = new SceneData(disp);
    
    // ... (Initialize all objects as before) ...
    staticScenes = new Scenes(disp);
    testScenes = new TestScenes(disp);
    chaseAnimation = new ChaseAnimation(disp);
    snowfallEffect = new SnowfallEffect(disp);
    scrollAnimation = new ScrollAnimation(disp);
    countdownAnimation = new MerryChristmasCount(disp);
    spinningSnowflake = new SpinningSnowflake(disp);
    sparklingStar = new SparklingStar(disp);
    concentricRectangles = new ConcentricRectangles(disp);
    concentricCircles = new ConcentricCircles(disp);
    concentricTriangles = new ConcentricTriangles(disp);
    morphingGeometry = new MorphingGeometry(disp);

    flyingBats = new FlyingBatsAnimation(disp);
    pirateShip = new PirateShipAnimation(disp);
    bleedingWindows = new BleedingWindowsAnimation(disp);
    lightningFlash = new LightningFlashAnimation(disp);
    halloweenScroll = new HalloweenScrollText(disp);
    spookyEyes = new SpookyTrackingEyesAnimation(disp);
    thanksgivingScroll = new ThanksgivingScrollText(disp);
    fallingLeaves = new FallingLeavesAnimation(disp);
    newYearCountdown = new NewYearCountdown(disp);
    newYearScroll = new NewYearScrollText(disp);
    fireworks = new FireworksAnimation(disp);
    colorSpinningStar = new ColorSpinningStarAnimation(disp);
    osuScroll = new OSUScrollText(disp);
    ohioLetters = new OHIOLetterAnimation(disp);
    osuColorWave = new OSUColorWaveAnimation(disp);

    // Begin all
    snowfallEffect->begin();
    chaseAnimation->begin();
    scrollAnimation->begin();
    countdownAnimation->begin();
    spinningSnowflake->begin();
    sparklingStar->begin();
    concentricRectangles->begin();
    concentricCircles->begin();
    concentricTriangles->begin();
    morphingGeometry->begin();

    flyingBats->begin();
    pirateShip->begin();
    bleedingWindows->begin();
    lightningFlash->begin();
    halloweenScroll->begin();
    thanksgivingScroll->begin();
    fallingLeaves->begin();
    newYearCountdown->begin();
    newYearScroll->begin();
    fireworks->begin();
    colorSpinningStar->begin();
    osuScroll->begin();
    ohioLetters->begin();
    osuColorWave->begin();

    loadRandomShowConfig();
    loadCountdownDates();
    
    disp->clear();
    disp->show();

    setTheme(THEME_OFF); 
}

void ThemeManager::update() {
    static uint8_t lastStaticScene = 255;
    static uint8_t lastStaticTheme = 255;
    
    // Helper to check if redraw needed
    bool redraw = (currentScene != lastStaticScene || currentTheme != lastStaticTheme);

    switch (currentTheme) {
        case THEME_OFF:
            disp->clear();
            disp->show();
            lastStaticScene = 255;
            break;

        case THEME_CHRISTMAS_STATIC:
            if (redraw) {
                drawDynamicScene(0, currentScene, "christmas");
                drawDynamicScene(1, currentScene, "christmas");
                disp->show();
                lastStaticScene = currentScene;
                lastStaticTheme = currentTheme;
            }
            break;

        case THEME_CHRISTMAS_ANIMATED:
            updateCurrentAnimation();
            disp->show();
            lastStaticScene = 255;
            break;

        case THEME_RANDOM_MAGIC:
            updateRandomMagic();
            disp->show();
            lastStaticScene = 255;
            break;
            
        case THEME_HALLOWEEN_STATIC:
            if (redraw) {
                drawDynamicScene(0, currentScene, "halloween");
                drawDynamicScene(1, currentScene, "halloween");
                disp->show();
                lastStaticScene = currentScene;
                lastStaticTheme = currentTheme;
            }
            break;
            
        case THEME_HALLOWEEN_ANIMATED:
            updateHalloweenAnimation();
            disp->show();
            lastStaticScene = 255;
            break;
            
        case THEME_THANKSGIVING_STATIC:
            if (redraw) {
                drawDynamicScene(0, currentScene, "thanksgiving");
                drawDynamicScene(1, currentScene, "thanksgiving");
                disp->show();
                lastStaticScene = currentScene;
                lastStaticTheme = currentTheme;
            }
            break;
            
        case THEME_THANKSGIVING_ANIMATED:
            updateThanksgivingAnimation();
            disp->show();
            lastStaticScene = 255;
            break;
            
        case THEME_NEWYEAR_STATIC:
            if (redraw) {
                drawDynamicScene(0, currentScene, "newyear");
                drawDynamicScene(1, currentScene, "newyear");
                disp->show();
                lastStaticScene = currentScene;
                lastStaticTheme = currentTheme;
            }
            break;
            
        case THEME_NEWYEAR_ANIMATED:
            updateNewYearAnimation();
            disp->show();
            lastStaticScene = 255;
            break;
            
        case THEME_OSU_STATIC:
            if (redraw) {
                drawDynamicScene(0, currentScene, "osu");
                drawDynamicScene(1, currentScene, "osu");
                disp->show();
                lastStaticScene = currentScene;
                lastStaticTheme = currentTheme;
            }
            break;
            
        case THEME_OSU_ANIMATED:
            updateOSUAnimation();
            disp->show();
            lastStaticScene = 255;
            break;

        case THEME_TEST_MODE:
            updateTestPattern();
            disp->show();
            lastStaticScene = 255;
            break;
    }
}

// GENERIC DRAW FUNCTION - This replaces all the specific draw functions
void ThemeManager::drawDynamicScene(int matrix, int index, String theme) {
    if (!contentMgr || !sceneData) return;
    
    // Ask ContentManager for all scenes of this theme
    std::vector<ContentItem> scenes = contentMgr->getScenes(theme);
    
    if (scenes.empty()) {
        disp->clearMatrix(matrix);
        return;
    }
    
    // Safety wrap index
    int safeIndex = index % scenes.size();
    
    // Render the file found by discovery
    String filename = scenes[safeIndex].filename;
    sceneData->renderSceneFromFile(filename.c_str(), matrix);
}

void ThemeManager::updateHalloweenAnimation() {
    switch (currentAnimation) {
        case 0: flyingBats->update(); break;
        case 1: pirateShip->update(); break;
        case 2: bleedingWindows->update(); break;
        case 3: lightningFlash->update(); break;
        case 4: 
            halloweenScroll->setMessage("HAPPY HALLOWEEN");
            halloweenScroll->update();
            break;
        case 5: spookyEyes->update(); break;
        default: break;
    }
}

void ThemeManager::updateThanksgivingAnimation() {
    switch (currentAnimation) {
        case 0: thanksgivingScroll->update(); break;
        case 1: fallingLeaves->update(); break;
        default: break;
    }
}

void ThemeManager::updateNewYearAnimation() {
    switch (currentAnimation) {
        case 0: newYearCountdown->update(); break;
        case 1: newYearScroll->update(); break;
        case 2: fireworks->update(); break;
        case 3: colorSpinningStar->update(); break;
        default: break;
    }
}

void ThemeManager::updateOSUAnimation() {
    switch (currentAnimation) {
        case 0: osuScroll->update(); break;
        case 1: ohioLetters->update(); break;
        case 2: osuColorWave->update(); break;
        default: break;
    }
}

void ThemeManager::updateCurrentAnimation() {
    switch (currentAnimation) {
        case 0: snowfallEffect->setMode(MODE_BASIC); snowfallEffect->update(); break;
        case 1: snowfallEffect->setMode(MODE_WIND_SINGLE); snowfallEffect->update(); break;
        case 2: snowfallEffect->setMode(MODE_WIND_VARY); snowfallEffect->update(); break;
        case 3: snowfallEffect->setMode(MODE_ACCUMULATE); snowfallEffect->update(); break;
        case 4: snowfallEffect->setMode(MODE_VARY_SIZE); snowfallEffect->update(); break;
        case 5: chaseAnimation->update(); break;
        case 6: scrollAnimation->update(); break;
        case 7: countdownAnimation->update(); break;
        case 8: sparklingStar->update(); break;
        case 9: spinningSnowflake->update(); break;
        case 10: concentricRectangles->update(); break;
        case 11: concentricCircles->update(); break;
        case 12: concentricTriangles->update(); break;
        case 13: morphingGeometry->update(); break;
        default: break;
    }
}

void ThemeManager::updateTestPattern() {
    if (currentTest >= 2) currentTest = 0; // Fixed count
    drawTestScene(0, currentTest);
    drawTestScene(1, currentTest);
}

void ThemeManager::setTheme(uint8_t newTheme) {
    if (newTheme != currentTheme) {
        // Stop animations if we were playing them
        if (currentTheme == THEME_CHRISTMAS_ANIMATED || currentTheme == THEME_RANDOM_MAGIC ||
            currentTheme == THEME_HALLOWEEN_ANIMATED || currentTheme == THEME_THANKSGIVING_ANIMATED ||
            currentTheme == THEME_NEWYEAR_ANIMATED || currentTheme == THEME_OSU_ANIMATED) {
            stopAllAnimations();
        }
        
        currentTheme = newTheme;
        currentScene = 0;
        currentAnimation = 0;
        currentTest = 0;
        lastSceneChangeTime = millis();
        
        disp->clear();
        disp->show();
        
        // Restart animations for new theme
        if (newTheme == THEME_CHRISTMAS_ANIMATED) {
            drawAnimation(0, currentAnimation); 
        } else if (newTheme == THEME_RANDOM_MAGIC) {
            loadRandomShowConfig();
            updateRandomMagic(); // Immediate update
        } 
        else if (newTheme == THEME_HALLOWEEN_ANIMATED) {
            updateHalloweenAnimation(); // Trigger start
        }
        else if (newTheme == THEME_THANKSGIVING_ANIMATED) {
            updateThanksgivingAnimation();
        }
        else if (newTheme == THEME_NEWYEAR_ANIMATED) {
            updateNewYearAnimation();
        }
        else if (newTheme == THEME_OSU_ANIMATED) {
            updateOSUAnimation();
        }
    }
}

void ThemeManager::stopAllAnimations() {
    snowfallEffect->begin();
    chaseAnimation->begin();
    scrollAnimation->begin();
    countdownAnimation->begin();
    spinningSnowflake->begin();
    sparklingStar->begin();
    concentricRectangles->begin();
    concentricCircles->begin();
    concentricTriangles->begin();
    morphingGeometry->begin();
    flyingBats->begin();
    pirateShip->begin();
    bleedingWindows->begin();
    lightningFlash->begin();
    halloweenScroll->begin();
    thanksgivingScroll->begin();
    fallingLeaves->begin();
    newYearCountdown->begin();
    newYearScroll->begin();
    fireworks->begin();
    colorSpinningStar->begin();
    osuScroll->begin();
    ohioLetters->begin();
    osuColorWave->begin();
    spookyEyes->begin();
}

uint8_t ThemeManager::getCurrentTheme() {
    return currentTheme;
}

void ThemeManager::setSpecificScene(int sceneIndex) {
    // Determine which static theme is active to know which folder to check
    // Default to Christmas if unknown
    if (currentTheme == THEME_OFF) currentTheme = THEME_CHRISTMAS_STATIC;
    
    currentScene = (uint8_t)sceneIndex;
    // Force redraw will happen in update()
}

void ThemeManager::setSpecificAnimation(int animIndex) {
    currentAnimation = (uint8_t)animIndex;
    // Force redraw will happen in update()
}

void ThemeManager::setSpecificTest(int testIndex) {
    setTheme(THEME_TEST_MODE);
    if (testIndex >= 0 && testIndex < 6) {
        currentAnimation = (uint8_t)testIndex;
        // updateTestPattern will handle drawing
    }
}

void ThemeManager::loadRandomShowConfig() {
    preferences.begin(PREFS_NAMESPACE, true);
    randomScenesMask = preferences.getUChar(RANDOM_SCENES_MASK_KEY, 0xFF);
    randomAnimationsMask = preferences.getUInt(RANDOM_ANIMS_MASK_KEY, 0xFFFF);
    randomTestsMask = preferences.getUChar(RANDOM_TESTS_MASK_KEY, 0x07);
    preferences.end();
}

// ... Set/Get Mask functions unchanged ... 
void ThemeManager::setRandomScenesMask(uint8_t mask) {
    randomScenesMask = mask;
    preferences.begin(PREFS_NAMESPACE, false);
    preferences.putUChar(RANDOM_SCENES_MASK_KEY, randomScenesMask);
    preferences.end();
}
void ThemeManager::setRandomAnimationsMask(uint32_t mask) {
    randomAnimationsMask = mask;
    preferences.begin(PREFS_NAMESPACE, false);
    preferences.putUInt(RANDOM_ANIMS_MASK_KEY, randomAnimationsMask);
    preferences.end();
}
void ThemeManager::setRandomTestsMask(uint8_t mask) {
    randomTestsMask = mask;
    preferences.begin(PREFS_NAMESPACE, false);
    preferences.putUChar(RANDOM_TESTS_MASK_KEY, randomTestsMask);
    preferences.end();
}
uint8_t ThemeManager::getRandomScenesMask() { return randomScenesMask; }
uint32_t ThemeManager::getRandomAnimationsMask() { return randomAnimationsMask; }
uint8_t ThemeManager::getRandomTestsMask() { return randomTestsMask; }

void ThemeManager::handleSerial() {
    if (Serial.available()) {
        char command = Serial.read();
        while(Serial.available()) Serial.read();
        
        switch (command) {
            case '+': brightnessUp(); break;
            case '-': brightnessDown(); break;
            case 's': setTheme(THEME_CHRISTMAS_STATIC); break;
            case 'a': setTheme(THEME_CHRISTMAS_ANIMATED); break;
            case 'r': setTheme(THEME_RANDOM_MAGIC); break;
            case 't': setTheme(THEME_TEST_MODE); break;
            case 'c': setTheme(THEME_OFF); break;
            case 'n':
                // Next item
                currentScene++; // Logic handles wrap-around in drawDynamicScene
                currentAnimation++;
                if (currentTheme == THEME_RANDOM_MAGIC) updateRandomMagic();
                break;
        }
    }
}

void ThemeManager::drawTestScene(int matrix, uint8_t testIndex) {
    if (!testScenes || matrix < 0 || matrix > 1) return;
    testIndex = testIndex % 6; // Hardcoded 6 test patterns

    switch (testIndex) {
        case 0: testScenes->drawColorRows(matrix); break;
        case 1: testScenes->drawColorColumns(matrix); break;
        case 2: testScenes->drawAllWhite(matrix); break;
        case 3: testScenes->drawAllRed(matrix); break;
        case 4: testScenes->drawCornerTest(matrix); break;
        case 5: testScenes->drawGridTest(matrix); break;
    }
}

void ThemeManager::drawAnimation(int matrix, uint8_t animIndex) {
    if (matrix != 0) return;
    // Just trigger the update logic
    currentAnimation = animIndex;
    updateCurrentAnimation();
}

// Helper to determine next state in Random Magic
void ThemeManager::findNextRandomScene() {
    // Just increment scene, drawDynamicScene handles the modulo wrapping based on file count
    currentScene++;
    randomContentType = 0;
}

void ThemeManager::findNextRandomAnimation() {
    currentAnimation++;
    currentAnimation %= TOTAL_ANIMATIONS; // Animations are still C++ coded, so we need a max count
    randomContentType = 1;
}

void ThemeManager::findNextRandomTest() {
    currentTest++;
    currentTest %= 6;
    randomContentType = 2;
}

unsigned long ThemeManager::getCurrentInterval() {
    return (randomContentType == 0 || randomContentType == 2) ? STATIC_SCENE_INTERVAL : ANIMATION_INTERVAL;
}

void ThemeManager::updateRandomMagic() {
    unsigned long currentTime = millis();
    if (currentTime - lastSceneChangeTime >= getCurrentInterval()) {
        lastSceneChangeTime = currentTime;
        
        // Simple random logic: 0=Scene, 1=Anim, 2=Test
        int choice = random(3);
        
        // Fallback logic if masked out
        if (choice == 0 && randomScenesMask == 0) choice = 1;
        if (choice == 1 && randomAnimationsMask == 0) choice = 2;
        if (choice == 2 && randomTestsMask == 0) choice = 0;

        if (choice == 0) findNextRandomScene();
        else if (choice == 1) findNextRandomAnimation();
        else findNextRandomTest();
        
        if (randomContentType == 0) {
            disp->clear(); 
            // Randomly pick a theme for this scene? 
            // For now, let's default to Christmas in Random Magic, or cycle themes?
            // Let's stick to Christmas for Random Magic scenes to keep it simple as per original
            drawDynamicScene(0, currentScene, "christmas");
            drawDynamicScene(1, currentScene, "christmas");
        } else if (randomContentType == 2) {
            disp->clear();
            drawTestScene(0, currentTest);
            drawTestScene(1, currentTest);
        }
    }
    
    if (randomContentType == 1) updateCurrentAnimation();
    else if (randomContentType == 2) updateTestPattern();
}

void ThemeManager::setBrightness(uint8_t brightness) {
    if (disp) disp->setBrightness(brightness);
}
void ThemeManager::brightnessUp() {
    uint8_t current = FastLED.getBrightness();
    FastLED.setBrightness(constrain(current + 10, 10, 255));
    FastLED.show();
}
void ThemeManager::brightnessDown() {
    uint8_t current = FastLED.getBrightness();
    FastLED.setBrightness(constrain(current - 10, 10, 255));
    FastLED.show();
}

// ... Date functions remain unchanged ...
void ThemeManager::initializeDefaultDates() {
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    uint16_t currentYear = timeinfo->tm_year + 1900;
    
    christmasCountdownDate = {currentYear, 12, 25, 0, 0};
    newYearCountdownDate = {(uint16_t)(currentYear + 1), 1, 1, 0, 0};
}

void ThemeManager::loadCountdownDates() {
    preferences.begin(PREFS_NAMESPACE, true);
    if (preferences.isKey(COUNTDOWN_XMAS_YEAR_KEY)) {
        christmasCountdownDate.year = preferences.getUShort(COUNTDOWN_XMAS_YEAR_KEY);
        christmasCountdownDate.month = preferences.getUChar(COUNTDOWN_XMAS_MONTH_KEY);
        christmasCountdownDate.day = preferences.getUChar(COUNTDOWN_XMAS_DAY_KEY);
        christmasCountdownDate.hour = preferences.getUChar(COUNTDOWN_XMAS_HOUR_KEY);
        christmasCountdownDate.minute = 0;
    } else initializeDefaultDates();
    
    if (preferences.isKey(COUNTDOWN_NEWYEAR_YEAR_KEY)) {
        newYearCountdownDate.year = preferences.getUShort(COUNTDOWN_NEWYEAR_YEAR_KEY);
        newYearCountdownDate.month = preferences.getUChar(COUNTDOWN_NEWYEAR_MONTH_KEY);
        newYearCountdownDate.day = preferences.getUChar(COUNTDOWN_NEWYEAR_DAY_KEY);
        newYearCountdownDate.hour = preferences.getUChar(COUNTDOWN_NEWYEAR_HOUR_KEY);
        newYearCountdownDate.minute = 0;
    } 
    preferences.end();
}

void ThemeManager::saveCountdownDates() {
    preferences.begin(PREFS_NAMESPACE, false);
    preferences.putUShort(COUNTDOWN_XMAS_YEAR_KEY, christmasCountdownDate.year);
    preferences.putUChar(COUNTDOWN_XMAS_MONTH_KEY, christmasCountdownDate.month);
    preferences.putUChar(COUNTDOWN_XMAS_DAY_KEY, christmasCountdownDate.day);
    preferences.putUChar(COUNTDOWN_XMAS_HOUR_KEY, christmasCountdownDate.hour);
    preferences.putUShort(COUNTDOWN_NEWYEAR_YEAR_KEY, newYearCountdownDate.year);
    preferences.putUChar(COUNTDOWN_NEWYEAR_MONTH_KEY, newYearCountdownDate.month);
    preferences.putUChar(COUNTDOWN_NEWYEAR_DAY_KEY, newYearCountdownDate.day);
    preferences.putUChar(COUNTDOWN_NEWYEAR_HOUR_KEY, newYearCountdownDate.hour);
    preferences.end();
}

void ThemeManager::setChristmasCountdownDate(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute) {
    christmasCountdownDate = {year, month, day, hour, minute};
    saveCountdownDates();
}
void ThemeManager::setNewYearCountdownDate(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute) {
    newYearCountdownDate = {year, month, day, hour, minute};
    saveCountdownDates();
}
CountdownDate ThemeManager::getChristmasCountdownDate() { return christmasCountdownDate; }
CountdownDate ThemeManager::getNewYearCountdownDate() { return newYearCountdownDate; }
void ThemeManager::resetCountdownDates() { initializeDefaultDates(); saveCountdownDates(); }

time_t ThemeManager::countdownToEpoch(const CountdownDate& cd) {
    struct tm timeinfo = {0};
    timeinfo.tm_year = cd.year - 1900;
    timeinfo.tm_mon = cd.month - 1;
    timeinfo.tm_mday = cd.day;
    timeinfo.tm_hour = cd.hour;
    timeinfo.tm_min = cd.minute;
    return mktime(&timeinfo);
}