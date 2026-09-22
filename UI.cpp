#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include "glut.h"
#include "UI/UI.h"
#include "UI/MainMenu.h"
#include "UI/OperationsScreen.h"
#include "Game/GameState.h"
#include "Map/Map.h"
#include "Operations/Operations.h"

// External mouse coordinates maintained by iGraphics
extern int iMouseX, iMouseY;

// iGraphics function declarations used by UI
void iClear();
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iLine(double x1, double y1, double x2, double y2);
void iText(double x, double y, char *str, void *font);
unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

// Texture handles for Gameplay UI
static bool g_uiTexturesLoaded = false;

// Wallpaper Background
static unsigned int g_texMainBg = 0;

// Vanguard Logo (top-left of gameplay HUD)
static unsigned int g_texVanguardLogo = 0;

// HUD Bar textures
static unsigned int g_texHudBar = 0;
static unsigned int g_texHudCoin = 0;
static unsigned int g_texHudRevenue = 0;
static unsigned int g_texHudEconomy = 0;
static unsigned int g_texHudSupport = 0;
static unsigned int g_texHudStability = 0;
static unsigned int g_texHudCalendar = 0;

// Zone Panel & Frame textures
static unsigned int g_texFrameZonePanel = 0;
static unsigned int g_texTitleZoneDetails = 0;
static unsigned int g_texEmblem = 0;
static unsigned int g_texZoneUrban = 0;
static unsigned int g_texPillZoneType = 0;

// Stat Bar textures
static unsigned int g_texBarTrack = 0;
static unsigned int g_texBarFillBlue = 0;
static unsigned int g_texBarFillPurple = 0;
static unsigned int g_texBarFillYellow = 0;
static unsigned int g_texBarFillGreen = 0;
static unsigned int g_texStatStability = 0;
static unsigned int g_texStatSupport = 0;
static unsigned int g_texStatEconomy = 0;
static unsigned int g_texStatSecurity = 0;

// Financial Summary & Status textures
static unsigned int g_texBoxFinancial = 0;
static unsigned int g_texFinanceChart = 0;
static unsigned int g_texControlShield = 0;
static unsigned int g_texThreatSkull = 0;
static unsigned int g_texActiveOpsGear = 0;

// Button textures
static unsigned int g_texBtnManage = 0;
static unsigned int g_texBtnManageHover = 0;
static unsigned int g_texBtnManagePressed = 0;
static unsigned int g_texBtnLocked = 0;
static unsigned int g_texLockSmall = 0;

// Robust Multi-Path Image Loader
static unsigned int safeLoadImage(const char *relPath) {
    if (!relPath) return 0;
    unsigned int tex = iLoadImage((char *)relPath);
    if (tex != 0) return tex;

    char alt[512];
    sprintf(alt, "Images/%s", relPath);
    tex = iLoadImage(alt);
    if (tex != 0) return tex;

    sprintf(alt, "../%s", relPath);
    tex = iLoadImage(alt);
    if (tex != 0) return tex;

    sprintf(alt, "E:/Vanguard Project/Vanguard Project/%s", relPath);
    tex = iLoadImage(alt);
    if (tex != 0) return tex;

    return 0;
}

// Helper to render texture with full Alpha Blending
static void renderSprite(int x, int y, int width, int height, unsigned int texture) {
    if (texture == 0) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    iShowImage(x, y, width, height, texture);
    glDisable(GL_BLEND);
}

// Textured Progress Bar with precision scissor clipping
static void renderTexturedProgressBar(double x, double y, double width, double height,
                                     double value, double maxValue, unsigned int fillTex,
                                     double fallbackR, double fallbackG, double fallbackB) {
    // 1. Render Track
    if (g_texBarTrack > 0) {
        renderSprite((int)x, (int)y, (int)width, (int)height, g_texBarTrack);
    } else {
        iSetColor(35, 45, 60);
        iFilledRectangle(x, y, width, height);
        iSetColor(55, 70, 95);
        iRectangle(x, y, width, height);
    }

    // 2. Render Fill with precision clipping
    if (maxValue > 0.0 && value > 0.0) {
        double ratio = value / maxValue;
        if (ratio > 1.0) ratio = 1.0;
        if (ratio < 0.0) ratio = 0.0;
        int fillWidth = (int)(width * ratio);

        if (fillWidth > 0) {
            if (fillTex > 0) {
                glEnable(GL_SCISSOR_TEST);
                glScissor((GLint)x, (GLint)y, (GLsizei)fillWidth, (GLsizei)height);
                renderSprite((int)x, (int)y, (int)width, (int)height, fillTex);
                glDisable(GL_SCISSOR_TEST);
            } else {
                iSetColor(fallbackR, fallbackG, fallbackB);
                iFilledRectangle(x, y, fillWidth, height);
            }
        }
    }
}

void initUI() {
    if (g_uiTexturesLoaded) return;

    initMapRenderer();
    initMainMenu();
    initOperationsScreen();

    // Wallpaper
    g_texMainBg = safeLoadImage("Images/UI/main_bg.jpg");

    // Vanguard Logo
    g_texVanguardLogo = safeLoadImage("Images/Vanguard_Logo.png");

    // HUD textures
    g_texHudBar = safeLoadImage("Images/UI/assets/ui/hud_bar.png");
    g_texHudCoin = safeLoadImage("Images/UI/assets/icons/hud/hud_coin.png");
    g_texHudRevenue = safeLoadImage("Images/UI/assets/icons/hud/hud_revenue.png");
    g_texHudEconomy = safeLoadImage("Images/UI/assets/icons/hud/hud_economy.png");
    g_texHudSupport = safeLoadImage("Images/UI/assets/icons/hud/hud_support.png");
    g_texHudStability = safeLoadImage("Images/UI/assets/icons/hud/hud_stability.png");
    g_texHudCalendar = safeLoadImage("Images/UI/assets/icons/hud/hud_calendar.png");

    // Side panel textures
    g_texFrameZonePanel = safeLoadImage("Images/UI/assets/frames/frame_zone_panel.png");
    g_texTitleZoneDetails = safeLoadImage("Images/UI/assets/icons/ui_titles/title_zone_details.png");
    g_texEmblem = safeLoadImage("Images/UI/assets/icons/zone/emblem.png");
    g_texZoneUrban = safeLoadImage("Images/UI/assets/icons/zone/zone_urban.png");
    g_texPillZoneType = safeLoadImage("Images/UI/assets/ui/pill_zone_type.png");

    // Stat bar & icon textures
    g_texBarTrack = safeLoadImage("Images/UI/assets/ui/bar_track.png");
    g_texBarFillBlue = safeLoadImage("Images/UI/assets/ui/bar_fill_blue.png");
    g_texBarFillPurple = safeLoadImage("Images/UI/assets/ui/bar_fill_purple.png");
    g_texBarFillYellow = safeLoadImage("Images/UI/assets/ui/bar_fill_yellow.png");
    g_texBarFillGreen = safeLoadImage("Images/UI/assets/ui/bar_fill_green.png");

    g_texStatStability = safeLoadImage("Images/UI/assets/icons/zone/stat_stability.png");
    g_texStatSupport = safeLoadImage("Images/UI/assets/icons/zone/stat_support.png");
    g_texStatEconomy = safeLoadImage("Images/UI/assets/icons/zone/stat_economy.png");
    g_texStatSecurity = safeLoadImage("Images/UI/assets/icons/zone/stat_security.png");

    // Financial & Status textures
    g_texBoxFinancial = safeLoadImage("Images/UI/assets/ui/box_financial.png");
    g_texFinanceChart = safeLoadImage("Images/UI/assets/icons/zone/finance_chart.png");
    g_texControlShield = safeLoadImage("Images/UI/assets/icons/zone/control_shield.png");
    g_texThreatSkull = safeLoadImage("Images/UI/assets/icons/zone/threat_skull.png");
    g_texActiveOpsGear = safeLoadImage("Images/UI/assets/icons/zone/active_ops_gear.png");

    // Button textures
    g_texBtnManage = safeLoadImage("Images/UI/assets/buttons/button_manage.png");
    g_texBtnManageHover = safeLoadImage("Images/UI/assets/buttons/button_manage_hover.png");
    g_texBtnManagePressed = safeLoadImage("Images/UI/assets/buttons/button_manage_pressed.png");
    g_texBtnLocked = safeLoadImage("Images/UI/assets/buttons/button_locked.png");
    g_texLockSmall = safeLoadImage("Images/UI/assets/icons/zone/lock_small.png");

    g_uiTexturesLoaded = true;
    printf("[Vanguard UI] UI textures loaded successfully (HUD: %u, Frame: %u, BG: %u)\n", g_texHudBar, g_texFrameZonePanel, g_texMainBg);
}

// REUSABLE UI COMPONENTS (Backward compatible fallback)

void drawPanel(double x, double y, double width, double height, const char *title) {
    if (g_texFrameZonePanel > 0 && width >= 300 && height >= 600) {
        renderSprite((int)x, (int)y, (int)width, (int)height, g_texFrameZonePanel);
        return;
    }

    // Fallback vector panel
    iSetColor(20, 26, 36);
    iFilledRectangle(x, y, width, height);
    iSetColor(45, 60, 85);
    iRectangle(x, y, width, height);

    if (title != NULL && strlen(title) > 0) {
        iSetColor(30, 42, 60);
        iFilledRectangle(x, y + height - 45, width, 45);
        iSetColor(45, 60, 85);
        iLine(x, y + height - 45, x + width, y + height - 45);
        iSetColor(255, 255, 255);
        iText(x + 20, y + height - 30, (char *)title, GLUT_BITMAP_HELVETICA_18);
    }
}

void drawProgressBar(double x, double y, double width, double height,
                     double value, double maxValue, double r, double g,
                     double b, const char *label, const char *valueFormat) {
    char textBuf[128];
    char valBuf[64];

    if (valueFormat != NULL) {
        sprintf(valBuf, valueFormat, value);
    } else {
        sprintf(valBuf, "%.1f%%", value);
    }

    if (label != NULL && strlen(label) > 0) {
        sprintf(textBuf, "%s: %s", label, valBuf);
    } else {
        sprintf(textBuf, "%s", valBuf);
    }

    // Label above bar
    iSetColor(230, 235, 240);
    iText(x, y + height + 6, textBuf, GLUT_BITMAP_HELVETICA_12);

    // Track & Fill
    renderTexturedProgressBar(x, y, width, height, value, maxValue, g_texBarFillBlue, r, g, b);
}

void drawBadge(double x, double y, double width, double height,
               const char *text, double r, double g, double b) {
    if (!text) return;

    if (g_texPillZoneType > 0) {
        renderSprite((int)x, (int)y, (int)width, (int)height, g_texPillZoneType);
        iSetColor(255, 255, 255);
        iText(x + 12, y + (height / 2) - 4, (char *)text, GLUT_BITMAP_HELVETICA_12);
        return;
    }

    iSetColor(r * 0.25, g * 0.25, b * 0.25);
    iFilledRectangle(x, y, width, height);
    iSetColor(r, g, b);
    iRectangle(x, y, width, height);
    iSetColor(255, 255, 255);
    iText(x + 8, y + (height / 2) - 4, (char *)text, GLUT_BITMAP_HELVETICA_12);
}

// TOP BAR (1200 x 70 at y: 710 - 780)
// Layout (matches reference image):
//  [0-190]   Vanguard Logo
//  [200-390] Funds (+net income)
//  [395-555] Rev | Maint
//  [560-700] Economy
//  [705-845] Support
//  [850-990] Stability
//  [995-1190] Day / Month / Year

void renderTopBar(const GameState &state) {
    char buffer[128];

    // ---- 1. Background HUD Bar (full 1200 x 70 at y:710) ----
    if (g_texHudBar > 0) {
        renderSprite(0, 710, 1200, 70, g_texHudBar);
    } else {
        iSetColor(20, 26, 36);
        iFilledRectangle(0, 710, 1200, 70);
        iSetColor(45, 60, 85);
        iLine(0, 710, 1200, 710);
    }

    // ---- 2. Vanguard Logo (x:0, y:710, 190 x 70) ----
    if (g_texVanguardLogo > 0) {
        renderSprite(0, 710, 190, 70, g_texVanguardLogo);
    } else {
        iSetColor(255, 204, 0);
        iText(10, 738, (char *)"VANGUARD", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    // ---- 3. Funds  (icon at x:200, text at x:232) ----
    renderSprite(200, 731, 28, 28, g_texHudCoin);
    if (state.netIncome >= 0) {
        iSetColor(46, 204, 113);
        sprintf(buffer, "Funds: $%.0f (+$%.0f/d)", state.funds, state.netIncome);
    } else {
        iSetColor(231, 76, 60);
        sprintf(buffer, "Funds: $%.0f (-$%.0f/d)", state.funds, -state.netIncome);
    }
    iText(232, 739, buffer, GLUT_BITMAP_HELVETICA_12);

    // ---- 4. Revenue | Maint  (icon at x:395, text at x:427) ----
    renderSprite(395, 731, 28, 28, g_texHudRevenue);
    iSetColor(200, 215, 230);
    sprintf(buffer, "Rev: $%.0f | Maint: $%.0f", state.totalRevenue, state.totalMaintenance);
    iText(427, 739, buffer, GLUT_BITMAP_HELVETICA_12);

    // ---- 5. Economy  (icon at x:560, text at x:592) ----
    renderSprite(560, 731, 28, 28, g_texHudEconomy);
    iSetColor(241, 196, 15);
    sprintf(buffer, "Economy: %.1f%%", state.overallEconomy);
    iText(592, 739, buffer, GLUT_BITMAP_HELVETICA_12);

    // ---- 6. Support  (icon at x:705, text at x:737) ----
    renderSprite(705, 731, 28, 28, g_texHudSupport);
    iSetColor(205, 120, 240);
    sprintf(buffer, "Support: %.1f%%", state.overallSupport);
    iText(737, 739, buffer, GLUT_BITMAP_HELVETICA_12);

    // ---- 7. Stability  (icon at x:850, text at x:882) ----
    renderSprite(850, 731, 28, 28, g_texHudStability);
    iSetColor(52, 172, 240);
    sprintf(buffer, "Stability: %.1f%%", state.overallStability);
    iText(882, 739, buffer, GLUT_BITMAP_HELVETICA_12);

    // ---- 8. Day / Month / Year  (icon at x:995, text at x:1027) ----
    renderSprite(995, 731, 28, 28, g_texHudCalendar);
    if (!state.isInsurgencyUnlocked && state.gameDay < 20 && state.gameMonth == 1 && state.gameYear == 1) {
        iSetColor(225, 235, 245);
        sprintf(buffer, "Day %d (Outbreak: %d d)", state.gameDay, 20 - state.gameDay);
    } else {
        iSetColor(225, 235, 245);
        sprintf(buffer, "Day %d | M %d | Y %d", state.gameDay, state.gameMonth, state.gameYear);
    }
    iText(1027, 739, buffer, GLUT_BITMAP_HELVETICA_12);

    // ---- 9. Insurgency Outbreak Alert Banner (below bar) ----
    if (state.insurgencyAlertTimer > 0) {
        iSetColor(180, 20, 20);
        iFilledRectangle(195, 705, 810, 6);
        iSetColor(255, 50, 50);
        iRectangle(195, 705, 810, 6);
        iSetColor(255, 220, 50);
        iText(230, 695, (char *)"! INSURGENCY OUTBREAK: MILITARY & SECURITY OPERATIONS UNLOCKED !", GLUT_BITMAP_HELVETICA_10);
    }
}

// ZONE INFORMATION & OPERATIONS PANEL (Module 3, 5, 6, 8, 9)

void renderZoneDetailsView(const GameState &state, const Zone &z) {
    char buffer[128];

    // 1. Panel Header: Emblem + "ZONE DETAILS"
    renderSprite(860, 642, 32, 32, g_texEmblem);
    if (g_texTitleZoneDetails > 0) {
        renderSprite(898, 646, 170, 24, g_texTitleZoneDetails);
    } else {
        iSetColor(230, 235, 245);
        iText(898, 650, (char *)"ZONE DETAILS", GLUT_BITMAP_HELVETICA_18);
    }

    // 2. Zone Name & Type
    renderSprite(860, 592, 30, 30, g_texZoneUrban);
    iSetColor(241, 196, 15);
    sprintf(buffer, "%s", z.name);
    iText(898, 602, buffer, GLUT_BITMAP_HELVETICA_18);

    // Zone Type Pill
    const char *typeName = "Urban Zone";
    switch (z.type) {
        case ZONE_URBAN:    typeName = "Urban Zone";    break;
        case ZONE_RURAL:    typeName = "Rural Zone";    break;
        case ZONE_FOREST:   typeName = "Forest Zone";   break;
        case ZONE_MOUNTAIN: typeName = "Mountain Zone"; break;
    }
    renderSprite(898, 570, 100, 24, g_texPillZoneType);
    iSetColor(150, 205, 255);
    iText(915, 577, (char *)typeName, GLUT_BITMAP_HELVETICA_10);

    // 3. Four Progress Bars with Icons & Textured Tracks/Fills
    // Stability Bar
    renderSprite(858, 508, 24, 24, g_texStatStability);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Stability: %.1f%%", z.stability);
    iText(890, 528, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 513, 260, 14, z.stability, 100.0, g_texBarFillBlue, 52, 152, 219);

    // Public Support Bar
    renderSprite(858, 453, 24, 24, g_texStatSupport);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Public Support: %.1f%%", z.support);
    iText(890, 473, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 458, 260, 14, z.support, 100.0, g_texBarFillPurple, 155, 89, 182);

    // Economy Bar
    renderSprite(858, 398, 24, 24, g_texStatEconomy);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Economy: %.1f%%", z.economy);
    iText(890, 418, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 403, 260, 14, z.economy, 100.0, g_texBarFillYellow, 241, 196, 15);

    // Security Level Bar
    renderSprite(858, 343, 24, 24, g_texStatSecurity);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Security Level: %.1f%%", z.security);
    iText(890, 363, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 348, 260, 14, z.security, 100.0, g_texBarFillGreen, 46, 204, 113);

    // 4. Zone Financial Summary Box (320 x 90 at x: 850, y: 235)
    renderSprite(850, 235, 320, 90, g_texBoxFinancial);

    renderSprite(865, 292, 22, 22, g_texFinanceChart);
    iSetColor(241, 196, 15);
    iText(895, 298, (char *)"ZONE FINANCIAL SUMMARY", GLUT_BITMAP_HELVETICA_10);

    iSetColor(46, 204, 113);
    sprintf(buffer, "Revenue: +$%.1f/day", z.zoneRevenue);
    iText(895, 276, buffer, GLUT_BITMAP_HELVETICA_10);

    iSetColor(231, 76, 60);
    sprintf(buffer, "Maintenance: -$%.1f/day", z.zoneMaintenance);
    iText(1025, 276, buffer, GLUT_BITMAP_HELVETICA_10);

    iSetColor(230, 240, 255);
    sprintf(buffer, "Net Contribution: %s$%.1f/day", z.zoneNetIncome >= 0 ? "+" : "-", fabs(z.zoneNetIncome));
    iText(895, 254, buffer, GLUT_BITMAP_HELVETICA_10);

    // 5. Control & Threat Status Indicators (y: 200 & 172)
    renderSprite(860, 200, 22, 22, g_texControlShield);
    iSetColor(z.playerControl ? 46 : 231, z.playerControl ? 204 : 76, z.playerControl ? 113 : 60);
    sprintf(buffer, "Control: %s", z.playerControl ? "Secured" : "Contested");
    iText(888, 206, buffer, GLUT_BITMAP_HELVETICA_12);

    renderSprite(1005, 200, 22, 22, g_texThreatSkull);
    iSetColor(z.enemyPresence ? 231 : 46, z.enemyPresence ? 76 : 204, z.enemyPresence ? 60 : 113);
    sprintf(buffer, "Threat: %s", z.enemyPresence ? "Active" : "None");
    iText(1033, 206, buffer, GLUT_BITMAP_HELVETICA_12);

    renderSprite(860, 172, 22, 22, g_texActiveOpsGear);
    iSetColor(180, 195, 210);
    sprintf(buffer, "Active Operations: %d", z.activeOperationsCount);
    iText(888, 178, buffer, GLUT_BITMAP_HELVETICA_12);

    // 6. MANAGE OPERATIONS Button (300 x 50 at x: 860, y: 110)
    bool isManageHover = (iMouseX >= 860 && iMouseX <= 1160 && iMouseY >= 110 && iMouseY <= 160);
    unsigned int manageSprite = isManageHover ? g_texBtnManageHover : g_texBtnManage;
    renderSprite(860, 110, 300, 50, manageSprite);
    iSetColor(255, 255, 255);
    iText(915, 129, (char *)"MANAGE OPERATIONS ->", GLUT_BITMAP_HELVETICA_12);

    // 7. DEPLOY TROOP Button (300 x 50 at x: 860, y: 55)
    bool isSecUnlocked = state.isInsurgencyUnlocked || state.gameDay >= 20 || state.gameMonth > 1 || state.gameYear > 1;
    bool canDeploy = isSecUnlocked && (state.funds >= 40.0 && z.playerControl && state.totalSecurityUnits < GameState::MAX_SECURITY_UNITS);

    if (canDeploy) {
        bool isDeployHover = (iMouseX >= 860 && iMouseX <= 1160 && iMouseY >= 55 && iMouseY <= 105);
        unsigned int deploySprite = isDeployHover ? g_texBtnManageHover : g_texBtnManage;
        renderSprite(860, 55, 300, 50, deploySprite);
        iSetColor(255, 255, 255);
        iText(915, 74, (char *)"+ DEPLOY TROOP ($40)", GLUT_BITMAP_HELVETICA_12);
    } else {
        renderSprite(860, 55, 300, 50, g_texBtnLocked);
        renderSprite(895, 70, 18, 18, g_texLockSmall);
        iSetColor(140, 145, 155);
        if (!isSecUnlocked) {
            sprintf(buffer, "DEPLOY TROOP (UNLOCKS DAY 20)");
        } else {
            sprintf(buffer, "DEPLOY TROOP ($40 - LOCKED)");
        }
        iText(920, 74, buffer, GLUT_BITMAP_HELVETICA_10);
    }

    // 8. Instruction Hint  (y:42 clears the panel bottom-border frame at y:20)
    iSetColor(110, 135, 165);
    iText(865, 42, (char *)"Click zone to view | Click troop to command.", GLUT_BITMAP_HELVETICA_10);
}

// REGIONAL OVERVIEW — shown when no zone is selected
static void renderRegionalOverview(const GameState &state) {
    char buffer[128];

    // Aggregate stats across all zones
    double avgStability = 0, avgSupport = 0, avgEconomy = 0, avgSecurity = 0;
    double totalRev = 0, totalMaint = 0, totalNet = 0;
    int controlledCount = 0, enemyCount = 0, activeOpsTotal = 0;

    for (int i = 0; i < state.totalZones; ++i) {
        const Zone &z = state.zones[i];
        avgStability   += z.stability;
        avgSupport     += z.support;
        avgEconomy     += z.economy;
        avgSecurity    += z.security;
        totalRev       += z.zoneRevenue;
        totalMaint     += z.zoneMaintenance;
        totalNet       += z.zoneNetIncome;
        if (z.playerControl) ++controlledCount;
        if (z.enemyPresence) ++enemyCount;
        activeOpsTotal += z.activeOperationsCount;
    }
    if (state.totalZones > 0) {
        avgStability /= state.totalZones;
        avgSupport   /= state.totalZones;
        avgEconomy   /= state.totalZones;
        avgSecurity  /= state.totalZones;
    }

    // Header
    renderSprite(860, 642, 32, 32, g_texEmblem);
    iSetColor(241, 196, 15);
    iText(898, 650, (char *)"REGIONAL OVERVIEW", GLUT_BITMAP_HELVETICA_18);

    iSetColor(130, 155, 185);
    iText(860, 600, (char *)"Averaged metrics across all sectors", GLUT_BITMAP_HELVETICA_12);

    // Averaged Progress Bars
    renderSprite(858, 508, 24, 24, g_texStatStability);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Avg Stability: %.1f%%", avgStability);
    iText(890, 528, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 513, 260, 14, avgStability, 100.0, g_texBarFillBlue, 52, 152, 219);

    renderSprite(858, 453, 24, 24, g_texStatSupport);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Avg Support: %.1f%%", avgSupport);
    iText(890, 473, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 458, 260, 14, avgSupport, 100.0, g_texBarFillPurple, 155, 89, 182);

    renderSprite(858, 398, 24, 24, g_texStatEconomy);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Avg Economy: %.1f%%", avgEconomy);
    iText(890, 418, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 403, 260, 14, avgEconomy, 100.0, g_texBarFillYellow, 241, 196, 15);

    renderSprite(858, 343, 24, 24, g_texStatSecurity);
    iSetColor(230, 235, 245);
    sprintf(buffer, "Avg Security: %.1f%%", avgSecurity);
    iText(890, 363, buffer, GLUT_BITMAP_HELVETICA_12);
    renderTexturedProgressBar(890, 348, 260, 14, avgSecurity, 100.0, g_texBarFillGreen, 46, 204, 113);

    // Status
    renderSprite(860, 290, 22, 22, g_texControlShield);
    iSetColor(controlledCount >= state.totalZones ? 46 : 241,
              controlledCount >= state.totalZones ? 204 : 196,
              controlledCount >= state.totalZones ? 113 : 15);
    sprintf(buffer, "Controlled: %d / %d zones", controlledCount, state.totalZones);
    iText(888, 296, buffer, GLUT_BITMAP_HELVETICA_12);

    renderSprite(1005, 290, 22, 22, g_texThreatSkull);
    iSetColor(enemyCount > 0 ? 231 : 46, enemyCount > 0 ? 76 : 204, enemyCount > 0 ? 60 : 113);
    sprintf(buffer, "Threats: %d active", enemyCount);
    iText(1033, 296, buffer, GLUT_BITMAP_HELVETICA_12);

    // Regional Financial Summary Box
    renderSprite(850, 160, 320, 90, g_texBoxFinancial);
    renderSprite(865, 217, 22, 22, g_texFinanceChart);
    iSetColor(241, 196, 15);
    iText(895, 223, (char *)"REGIONAL FINANCIAL SUMMARY", GLUT_BITMAP_HELVETICA_10);

    iSetColor(46, 204, 113);
    sprintf(buffer, "Revenue: +$%.1f/day", totalRev);
    iText(895, 201, buffer, GLUT_BITMAP_HELVETICA_10);

    iSetColor(231, 76, 60);
    sprintf(buffer, "Maint: -$%.1f/day", totalMaint);
    iText(1025, 201, buffer, GLUT_BITMAP_HELVETICA_10);

    iSetColor(230, 240, 255);
    sprintf(buffer, "Net Regional Income: %s$%.1f/day", totalNet >= 0 ? "+" : "-", fabs(totalNet));
    iText(895, 179, buffer, GLUT_BITMAP_HELVETICA_10);

    // Zone Breakdown Pills
    int urbanC = 0, ruralC = 0, forestC = 0, mountainC = 0;
    for (int i = 0; i < state.totalZones; ++i) {
        switch (state.zones[i].type) {
            case ZONE_URBAN:    ++urbanC;    break;
            case ZONE_RURAL:    ++ruralC;    break;
            case ZONE_FOREST:   ++forestC;   break;
            case ZONE_MOUNTAIN: ++mountainC; break;
        }
    }
    sprintf(buffer, "Urban: %d",  urbanC);    drawBadge(860,  100, 68, 20, buffer, 52,  152, 219);
    sprintf(buffer, "Rural: %d",  ruralC);    drawBadge(935,  100, 68, 20, buffer, 46,  204, 113);
    sprintf(buffer, "Forest: %d", forestC);  drawBadge(1010, 100, 72, 20, buffer, 39,  174, 96);
    sprintf(buffer, "Mount: %d",  mountainC); drawBadge(1089, 100, 72, 20, buffer, 149, 165, 166);

    // Bottom hint
    iSetColor(95, 120, 150);
    iText(860, 38, (char *)"Click a zone on the map to view details.", GLUT_BITMAP_HELVETICA_12);
}

void renderSidePanel(const GameState &state) {
    bool zoneSelected = (state.selectedZoneId >= 0 && state.selectedZoneId < state.totalZones);

    // Frame (340 x 680 at 840, 20)
    if (g_texFrameZonePanel > 0) {
        renderSprite(840, 20, 340, 680, g_texFrameZonePanel);
    } else {
        drawPanel(840, 20, 340, 680, zoneSelected ? "ZONE DETAILS" : "REGIONAL OVERVIEW");
    }

    if (zoneSelected) {
        const Zone &z = state.zones[state.selectedZoneId];
        renderZoneDetailsView(state, z);
    } else {
        renderRegionalOverview(state);
    }
}

// MAIN RENDER LOOP

void renderGame(const GameState &state) {
    // Lazy initialize textures on the first render call when OpenGL context is fully active
    if (!g_uiTexturesLoaded) {
        initUI();
    }

    iClear();

    switch (state.currentScreen) {
        case SCREEN_GAMEPLAY:
            // Background Wallpaper
            if (g_texMainBg > 0) {
                renderSprite(0, 0, 1200, 780, g_texMainBg);
            }
            renderMapAndZones(state);
            renderSidePanel(state);
            renderTopBar(state);
            break;

        case SCREEN_OPERATIONS:
            // Background Wallpaper
            if (g_texMainBg > 0) {
                renderSprite(0, 0, 1200, 780, g_texMainBg);
            }
            renderOperationsScreen(state);
            break;

        case SCREEN_MENU:
            renderMainMenu(state);
            break;

        case SCREEN_VICTORY:
            iSetColor(20, 40, 30);
            iFilledRectangle(0, 0, 1200, 780);
            iSetColor(46, 204, 113);
            iText(480, 500, (char *)"VICTORY ACHIEVED", GLUT_BITMAP_TIMES_ROMAN_24);
            iSetColor(255, 255, 255);
            iText(420, 420, (char *)"All zones stabilized. Press [R] to Restart.", GLUT_BITMAP_HELVETICA_18);
            break;

        case SCREEN_DEFEAT:
            iSetColor(40, 20, 20);
            iFilledRectangle(0, 0, 1200, 780);
            iSetColor(231, 76, 60);
            iText(480, 500, (char *)"AUTHORITY COLLAPSED", GLUT_BITMAP_TIMES_ROMAN_24);
            iSetColor(255, 255, 255);
            iText(420, 420, (char *)"Instability overwhelmed region. Press [R] to Restart.", GLUT_BITMAP_HELVETICA_18);
            break;
    }
}
