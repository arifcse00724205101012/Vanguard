#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include "glut.h"
#include "UI/MainMenu.h"
#include "Game/GameState.h"

// iGraphics function declarations
unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);
void iClear();
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iLine(double x1, double y1, double x2, double y2);
void iPolygon(double x[], double y[], int n);
void iFilledPolygon(double x[], double y[], int n);
void iFilledCircle(double x, double y, double r, int slices = 100);
void iCircle(double x, double y, double r, int slices = 100);
void iText(double x, double y, char *str, void *font);

// Texture state
static unsigned int g_menuBgTexture = 0;
static bool g_menuLoaded = false;

// Hover and notification state
static MenuButtonId g_hoveredButton = MENU_BTN_NONE;
static int g_settingsNoticeTimer = 0;

// Button dimensions and positions (aligned with 1200x780 coordinate system)
static const double BTN_WIDTH = 430.0;
static const double BTN_HEIGHT = 64.0;
static const double BTN_X = 385.0; // Centered: (1200 - 430) / 2 = 385
static const double BTN_Y_PLAY = 275.0;
static const double BTN_Y_SETTINGS = 191.0;
static const double BTN_Y_EXIT = 107.0;

static bool getButtonRect(MenuButtonId id, double &x, double &y, double &w, double &h) {
    w = BTN_WIDTH;
    h = BTN_HEIGHT;
    x = BTN_X;
    switch (id) {
        case MENU_BTN_PLAY:
            y = BTN_Y_PLAY;
            return true;
        case MENU_BTN_SETTINGS:
            y = BTN_Y_SETTINGS;
            return true;
        case MENU_BTN_EXIT:
            y = BTN_Y_EXIT;
            return true;
        default:
            return false;
    }
}

static bool isPointInsideButton(double px, double py, double bx, double by, double bw, double bh) {
    return (px >= bx && px <= bx + bw && py >= by && py <= by + bh);
}

// Audio state and playback for opening screen
static bool g_musicPlaying = false;

static bool checkAudioFileExists(const char *path) {
    if (!path || !path[0]) return false;
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

static bool findMusicFile(char *outPath, int maxLen) {
    const char *relCandidates[] = {
        "Music/Rebel Inc. Escalation(OST).mp3",
        "Music\\Rebel Inc. Escalation(OST).mp3",
        "../Music/Rebel Inc. Escalation(OST).mp3",
        "..\\Music\\Rebel Inc. Escalation(OST).mp3",
        "E:/Vanguard Project/Vanguard Project/Music/Rebel Inc. Escalation(OST).mp3",
        "E:\\Vanguard Project\\Vanguard Project\\Music\\Rebel Inc. Escalation(OST).mp3",
        NULL
    };

    for (int i = 0; relCandidates[i] != NULL; ++i) {
        if (checkAudioFileExists(relCandidates[i])) {
            strncpy(outPath, relCandidates[i], maxLen - 1);
            outPath[maxLen - 1] = '\0';
            return true;
        }
    }

    // Check directory relative to current executable location
    char exePath[MAX_PATH] = {0};
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) > 0) {
        char *lastSlash = strrchr(exePath, '\\');
        if (!lastSlash) lastSlash = strrchr(exePath, '/');
        if (lastSlash) {
            *lastSlash = '\0';
            char candidate[MAX_PATH];
            sprintf(candidate, "%s\\Music\\Rebel Inc. Escalation(OST).mp3", exePath);
            if (checkAudioFileExists(candidate)) {
                strncpy(outPath, candidate, maxLen - 1);
                outPath[maxLen - 1] = '\0';
                return true;
            }
            sprintf(candidate, "%s\\..\\Music\\Rebel Inc. Escalation(OST).mp3", exePath);
            if (checkAudioFileExists(candidate)) {
                strncpy(outPath, candidate, maxLen - 1);
                outPath[maxLen - 1] = '\0';
                return true;
            }
        }
    }

    return false;
}

void playMenuMusic() {
    if (g_musicPlaying) return;

    char musicFile[MAX_PATH] = {0};
    if (!findMusicFile(musicFile, MAX_PATH)) {
        return;
    }

    char fullPath[MAX_PATH] = {0};
    GetFullPathNameA(musicFile, MAX_PATH, fullPath, NULL);

    char shortPath[MAX_PATH] = {0};
    if (GetShortPathNameA(fullPath, shortPath, MAX_PATH) == 0) {
        strcpy(shortPath, fullPath);
    }

    // Close any prior MCI alias before opening
    mciSendStringA("close menu_bgm", NULL, 0, NULL);

    char cmd[512];
    sprintf(cmd, "open \"%s\" type mpegvideo alias menu_bgm", shortPath);
    MCIERROR err = mciSendStringA(cmd, NULL, 0, NULL);
    if (err != 0) {
        sprintf(cmd, "open \"%s\" alias menu_bgm", shortPath);
        err = mciSendStringA(cmd, NULL, 0, NULL);
    }

    if (err == 0) {
        mciSendStringA("play menu_bgm repeat", NULL, 0, NULL);
        g_musicPlaying = true;
    }
}

void stopMenuMusic() {
    if (!g_musicPlaying) return;
    mciSendStringA("stop menu_bgm", NULL, 0, NULL);
    mciSendStringA("close menu_bgm", NULL, 0, NULL);
    g_musicPlaying = false;
}

void initMainMenu() {
    if (!g_menuLoaded) {
        g_menuBgTexture = iLoadImage((char *)"Images/Opening Screen.png");
        if (g_menuBgTexture == 0) {
            g_menuBgTexture = iLoadImage((char *)"E:/Vanguard Project/Vanguard Project/Vanguard Project/Images/Opening Screen.png");
        }
        g_menuLoaded = true;
    }
    playMenuMusic();
}

// Draw a military chamfered polygon (cut corners)
static void drawChamferedBox(double x, double y, double w, double h, double chamfer, bool filled) {
    double px[8] = {
        x + chamfer, x + w - chamfer,
        x + w, x + w,
        x + w - chamfer, x + chamfer,
        x, x
    };
    double py[8] = {
        y, y,
        y + chamfer, y + h - chamfer,
        y + h, y + h,
        y + h - chamfer, y + chamfer
    };

    if (filled) {
        iFilledPolygon(px, py, 8);
    } else {
        iPolygon(px, py, 8);
    }
}

// Render vector icons for buttons
static void drawButtonIcon(MenuButtonId id, double x, double y, bool isHovered) {
    double iconCenterX = x + 30.0;
    double iconCenterY = y + (BTN_HEIGHT / 2.0);

    if (id == MENU_BTN_PLAY) {
        // Play Arrow (right-pointing triangle)
        double triX[3] = { iconCenterX - 7.0, iconCenterX + 9.0, iconCenterX - 7.0 };
        double triY[3] = { iconCenterY - 9.0, iconCenterY, iconCenterY + 9.0 };

        if (isHovered) {
            iSetColor(255, 230, 80);
        } else {
            iSetColor(230, 168, 34);
        }
        iFilledPolygon(triX, triY, 3);
    } else if (id == MENU_BTN_SETTINGS) {
        // Mechanical Cog / Gear
        if (isHovered) {
            iSetColor(230, 240, 255);
        } else {
            iSetColor(170, 185, 205);
        }
        // Outer teeth lines
        for (int i = 0; i < 8; ++i) {
            double angle = (double)i * (3.14159265 / 4.0);
            double x1 = iconCenterX + cos(angle) * 4.0;
            double y1 = iconCenterY + sin(angle) * 4.0;
            double x2 = iconCenterX + cos(angle) * 9.0;
            double y2 = iconCenterY + sin(angle) * 9.0;
            iLine(x1, y1, x2, y2);
        }
        iCircle(iconCenterX, iconCenterY, 6.0, 16);
        iFilledCircle(iconCenterX, iconCenterY, 2.5, 12);
    } else if (id == MENU_BTN_EXIT) {
        // Exit Door with Arrow
        if (isHovered) {
            iSetColor(255, 180, 180);
        } else {
            iSetColor(170, 185, 205);
        }
        // Door bracket
        iLine(iconCenterX - 5.0, iconCenterY - 8.0, iconCenterX - 9.0, iconCenterY - 8.0);
        iLine(iconCenterX - 9.0, iconCenterY - 8.0, iconCenterX - 9.0, iconCenterY + 8.0);
        iLine(iconCenterX - 9.0, iconCenterY + 8.0, iconCenterX - 5.0, iconCenterY + 8.0);

        // Arrow pointing right
        iLine(iconCenterX - 6.0, iconCenterY, iconCenterX + 7.0, iconCenterY);
        iLine(iconCenterX + 4.0, iconCenterY - 4.0, iconCenterX + 7.0, iconCenterY);
        iLine(iconCenterX + 4.0, iconCenterY + 4.0, iconCenterX + 7.0, iconCenterY);
    }
}

// Render individual military button with chamfered styling and hover glow
static void drawMilitaryButton(MenuButtonId id, bool isHovered) {
    double x, y, w, h;
    if (!getButtonRect(id, x, y, w, h)) return;

    double chamfer = 10.0;

    // 1. Background Fill
    if (isHovered) {
        if (id == MENU_BTN_PLAY) {
            iSetColor(32, 44, 62); // Lit navy highlight
        } else if (id == MENU_BTN_SETTINGS) {
            iSetColor(28, 38, 52); // Cool slate highlight
        } else {
            iSetColor(40, 26, 30); // Subtle crimson-tinted highlight for exit
        }
    } else {
        iSetColor(18, 24, 34); // Standard military dark card
    }
    drawChamferedBox(x, y, w, h, chamfer, true);

    // 2. Outer Glowing / Chamfered Border
    if (isHovered) {
        if (id == MENU_BTN_PLAY) {
            // Radiant Gold glow
            iSetColor(255, 215, 0);
            drawChamferedBox(x, y, w, h, chamfer, false);
            iSetColor(255, 235, 120);
            drawChamferedBox(x + 1.0, y + 1.0, w - 2.0, h - 2.0, chamfer - 1.0, false);
        } else if (id == MENU_BTN_SETTINGS) {
            // Luminous Cyan / Silver glow
            iSetColor(180, 220, 255);
            drawChamferedBox(x, y, w, h, chamfer, false);
            iSetColor(220, 240, 255);
            drawChamferedBox(x + 1.0, y + 1.0, w - 2.0, h - 2.0, chamfer - 1.0, false);
        } else {
            // Luminous Crimson / White glow
            iSetColor(240, 120, 120);
            drawChamferedBox(x, y, w, h, chamfer, false);
            iSetColor(255, 180, 180);
            drawChamferedBox(x + 1.0, y + 1.0, w - 2.0, h - 2.0, chamfer - 1.0, false);
        }
    } else {
        if (id == MENU_BTN_PLAY) {
            iSetColor(230, 168, 34); // Gold border for PLAY
        } else {
            iSetColor(120, 135, 155); // Tactical gray border
        }
        drawChamferedBox(x, y, w, h, chamfer, false);
    }

    // 3. Button Icon
    drawButtonIcon(id, x, y, isHovered);

    // 4. Button Titles & Subtitles
    const char *title = "";
    const char *subtitle = "";
    if (id == MENU_BTN_PLAY) {
        title = "PLAY";
        subtitle = "Begin a new deployment";
    } else if (id == MENU_BTN_SETTINGS) {
        title = "SETTINGS";
        subtitle = "Audio, display & controls";
    } else if (id == MENU_BTN_EXIT) {
        title = "EXIT";
        subtitle = "Quit to desktop";
    }

    // Title text
    if (isHovered) {
        iSetColor(255, 255, 255);
    } else {
        if (id == MENU_BTN_PLAY) {
            iSetColor(245, 245, 245);
        } else {
            iSetColor(220, 225, 235);
        }
    }
    iText(x + 58.0, y + 36.0, (char *)title, GLUT_BITMAP_HELVETICA_18);

    // Subtitle text
    if (isHovered) {
        iSetColor(200, 215, 235);
    } else {
        iSetColor(140, 155, 175);
    }
    iText(x + 58.0, y + 16.0, (char *)subtitle, GLUT_BITMAP_HELVETICA_12);
}

// Procedural fallback if Opening Screen.png fails to load
static void renderProceduralFallback() {
    // 1. Dark military gradient background
    for (int y = 0; y < 780; y += 4) {
        double ratio = (double)y / 780.0;
        iSetColor(10.0 + ratio * 8.0, 14.0 + ratio * 12.0, 20.0 + ratio * 16.0);
        iFilledRectangle(0, y, 1200, 4);
    }

    // 2. Subtle grid lines
    iSetColor(25, 35, 48);
    for (int gx = 100; gx < 1200; gx += 100) {
        iLine(gx, 0, gx, 780);
    }
    for (int gy = 80; gy < 780; gy += 80) {
        iLine(0, gy, 1200, gy);
    }

    // 3. Vanguard Emblem & Title
    iSetColor(255, 204, 0);
    iText(480, 580, (char *)"VANGUARD", GLUT_BITMAP_TIMES_ROMAN_24);

    // Wings / Emblem lines
    iSetColor(180, 190, 205);
    iLine(340, 590, 460, 590);
    iLine(320, 585, 460, 585);
    iLine(740, 590, 860, 590);
    iLine(740, 585, 880, 585);

    // 4. Tagline
    iSetColor(210, 220, 235);
    iText(450, 530, (char *)"REBUILD . STABILIZE . SURVIVE", GLUT_BITMAP_HELVETICA_18);
    iLine(320, 536, 430, 536);
    iLine(770, 536, 880, 536);

    // 5. Render Buttons
    drawMilitaryButton(MENU_BTN_PLAY, g_hoveredButton == MENU_BTN_PLAY);
    drawMilitaryButton(MENU_BTN_SETTINGS, g_hoveredButton == MENU_BTN_SETTINGS);
    drawMilitaryButton(MENU_BTN_EXIT, g_hoveredButton == MENU_BTN_EXIT);

    // 6. Version and Copyright
    iSetColor(110, 125, 145);
    iText(30, 25, (char *)"v0.1.0 - Early Access Build", GLUT_BITMAP_HELVETICA_12);
    iText(1020, 25, (char *)"(c) 2027 Vanguard Studios", GLUT_BITMAP_HELVETICA_12);
}

void renderMainMenu(const GameState &state) {
    if (!g_menuLoaded) {
        initMainMenu();
    }

    // Play/continue opening screen music
    playMenuMusic();

    if (g_menuBgTexture > 0) {
        // Draw cinematic opening screen image (1200x780)
        iShowImage(0, 0, 1200, 780, g_menuBgTexture);

        // If any button is hovered, draw the active hover state over that button
        if (g_hoveredButton == MENU_BTN_PLAY) {
            drawMilitaryButton(MENU_BTN_PLAY, true);
        } else if (g_hoveredButton == MENU_BTN_SETTINGS) {
            drawMilitaryButton(MENU_BTN_SETTINGS, true);
        } else if (g_hoveredButton == MENU_BTN_EXIT) {
            drawMilitaryButton(MENU_BTN_EXIT, true);
        }
    } else {
        renderProceduralFallback();
    }

    // Settings "Coming Soon" notification toast
    if (g_settingsNoticeTimer > 0) {
        g_settingsNoticeTimer--;
        iSetColor(20, 28, 40);
        iFilledRectangle(385, 360, 430, 42);
        iSetColor(52, 152, 219);
        iRectangle(385, 360, 430, 42);
        iSetColor(240, 245, 255);
        iText(415, 376, (char *)"SETTINGS COMING SOON - Default Audio & Video Active", GLUT_BITMAP_HELVETICA_12);
    }
}

void handleMainMenuMouseMove(int mx, int my) {
    double x, y, w, h;

    if (getButtonRect(MENU_BTN_PLAY, x, y, w, h) && isPointInsideButton(mx, my, x, y, w, h)) {
        g_hoveredButton = MENU_BTN_PLAY;
    } else if (getButtonRect(MENU_BTN_SETTINGS, x, y, w, h) && isPointInsideButton(mx, my, x, y, w, h)) {
        g_hoveredButton = MENU_BTN_SETTINGS;
    } else if (getButtonRect(MENU_BTN_EXIT, x, y, w, h) && isPointInsideButton(mx, my, x, y, w, h)) {
        g_hoveredButton = MENU_BTN_EXIT;
    } else {
        g_hoveredButton = MENU_BTN_NONE;
    }
}

void handleMainMenuMouseClick(GameState &state, int button, int mx, int my) {
    if (button != GLUT_LEFT_BUTTON) return;

    double x, y, w, h;

    if (getButtonRect(MENU_BTN_PLAY, x, y, w, h) && isPointInsideButton(mx, my, x, y, w, h)) {
        // Stop opening screen music and transition directly into gameplay
        stopMenuMusic();
        changeGameScreen(state, SCREEN_GAMEPLAY);
    } else if (getButtonRect(MENU_BTN_SETTINGS, x, y, w, h) && isPointInsideButton(mx, my, x, y, w, h)) {
        // Non-blocking notification
        g_settingsNoticeTimer = 180; // ~3 seconds
    } else if (getButtonRect(MENU_BTN_EXIT, x, y, w, h) && isPointInsideButton(mx, my, x, y, w, h)) {
        // Stop music and perform clean application exit
        stopMenuMusic();
        exit(0);
    }
}
