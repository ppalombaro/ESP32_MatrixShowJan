/* ThemeManager.h
   Updated for TRUE Auto-Discovery
*/

#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <Arduino.h>
#include "Config.h"       
#include "MatrixDisplay.h"
#include "Scenes.h"
#include "SceneData.h"
#include "Animations.h"   
#include "TestScenes.h"
#include "HolidayAnimations.h"
#include "ContentManager.h"  // ADDED: Link to content system

struct CountdownDate {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
};

class ThemeManager {
public:
    ThemeManager();
    ~ThemeManager(); 

    // MODIFIED: Pass ContentManager in begin()
    void begin(MatrixDisplay* display, ContentManager* contentManager);
    void update();
    void handleSerial();

    void setTheme(uint8_t newTheme);
    uint8_t getCurrentTheme();

    void setSpecificScene(int sceneIndex); 
    void setSpecificAnimation(int animIndex);
    void setSpecificTest(int testIndex);
    void stopAllAnimations();

    void brightnessUp();
    void brightnessDown();
    void setBrightness(uint8_t brightness);

    void setChristmasCountdownDate(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0);
    void setNewYearCountdownDate(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0);
    CountdownDate getChristmasCountdownDate();
    CountdownDate getNewYearCountdownDate();
    void resetCountdownDates();

    uint8_t currentScene = 0; 
    uint8_t currentAnimation = 0;
    uint8_t currentTest = 0;

    void setRandomScenesMask(uint8_t mask);
    void setRandomAnimationsMask(uint32_t mask);
    void setRandomTestsMask(uint8_t mask);
    uint8_t getRandomScenesMask();
    uint32_t getRandomAnimationsMask();
    uint8_t getRandomTestsMask();

private:
    MatrixDisplay* disp;
    SceneData* sceneData;
    ContentManager* contentMgr; // ADDED: Pointer to content system
    
    // Legacy wrappers
    Scenes* staticScenes;
    TestScenes* testScenes;

    // Animations
    ChaseAnimation* chaseAnimation;
    SnowfallEffect* snowfallEffect;
    ScrollAnimation* scrollAnimation; 
    MerryChristmasCount* countdownAnimation; 
    SpinningSnowflake* spinningSnowflake;    
    SparklingStar* sparklingStar;            
    ConcentricRectangles* concentricRectangles;
    ConcentricCircles* concentricCircles;
    ConcentricTriangles* concentricTriangles;
    MorphingGeometry* morphingGeometry;

    FlyingBatsAnimation* flyingBats;
    PirateShipAnimation* pirateShip;
    BleedingWindowsAnimation* bleedingWindows;
    LightningFlashAnimation* lightningFlash;
    HalloweenScrollText* halloweenScroll;
    SpookyTrackingEyesAnimation* spookyEyes;
    ThanksgivingScrollText* thanksgivingScroll;
    FallingLeavesAnimation* fallingLeaves;
    NewYearCountdown* newYearCountdown;
    NewYearScrollText* newYearScroll;
    FireworksAnimation* fireworks;
    ColorSpinningStarAnimation* colorSpinningStar;
    OSUScrollText* osuScroll;
    OHIOLetterAnimation* ohioLetters;
    OSUColorWaveAnimation* osuColorWave;

    uint8_t currentTheme = THEME_OFF; 
    unsigned long lastSceneChangeTime = 0; 
    
    uint8_t randomScenesMask = 0;
    uint32_t randomAnimationsMask = 0;
    uint8_t randomTestsMask = 0;
    uint8_t randomContentType = 0;

    CountdownDate christmasCountdownDate;
    CountdownDate newYearCountdownDate;
    
    void findNextRandomScene();
    void findNextRandomAnimation();
    void findNextRandomTest();
    void loadRandomShowConfig();
    void updateRandomMagic();
    unsigned long getCurrentInterval();
    
    // Updated to use Dynamic Discovery (no switch cases)
    void drawDynamicScene(int matrix, int index, String theme);
    // ADD THESE TWO LINES HERE:
    void drawTestScene(int matrix, uint8_t testIndex);
    void drawAnimation(int matrix, uint8_t animIndex);
    void updateCurrentAnimation();
    void updateTestPattern();
    
    void updateHalloweenAnimation();
    void updateThanksgivingAnimation();
    void updateNewYearAnimation();
    void updateOSUAnimation();
    
    void loadCountdownDates();
    void saveCountdownDates();
    void initializeDefaultDates();
    time_t countdownToEpoch(const CountdownDate& cd);
};

#endif