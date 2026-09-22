#define _CRT_SECURE_NO_WARNINGS
#include "Map/Map.h"
#include "Map/Zone.h"
#include "Game/GameState.h"
#include "Operations/Operations.h"
#include "Security/SecuritySystem.h"
#include <stdio.h>
#include <string.h>

// iGraphics functions used for rendering
void iSetColor(double r, double g, double b);
void iFilledRectangle(double left, double bottom, double dx, double dy);
void iRectangle(double left, double bottom, double dx, double dy);
void iPolygon(double x[], double y[], int n);
unsigned int iLoadImage(char filename[]);
void iShowImage(int x, int y, int width, int height, unsigned int texture);

#include "glut.h"

static unsigned int g_mapTexture = 0;
static unsigned int g_mapFrameTexture = 0;
static bool g_mapLoaded = false;

// Ray-casting algorithm for testing if a point (px, py) is inside polygon (vx, vy)
bool isPointInPolygon(int nvert, const double *vertx, const double *verty, double testx, double testy) {
    int i, j;
    bool c = false;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i])) {
            c = !c;
        }
    }
    return c;
}

void initZones(GameState &state) {
    state.totalZones = 9;

    // Zone 1 (ID 1): 1. Northbridge (Urban Zone)
    state.zones[0].id = 1;
    strncpy(state.zones[0].name, "1. Northbridge", 63);
    state.zones[0].type = ZONE_URBAN;
    state.zones[0].stability = 50.0;
    state.zones[0].support = 60.0;
    state.zones[0].economy = 70.0;
    state.zones[0].security = 40.0;
    state.zones[0].enemyPresence = false;
    state.zones[0].playerControl = true;
    state.zones[0].activeOperationsCount = 0;
    {
		double px[] = { 29, 70, 111, 222, 260, 247, 244, 248, 257, 267, 268, 263, 256, 253, 264, 276, 193, 93, 83, 29 };
		double py[] = { 531, 621, 645, 678, 635, 609, 587, 571, 565, 549, 533, 513, 494, 472, 453, 437, 452, 453, 457, 531 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[0].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[0].polyX[k] = px[k];
            state.zones[0].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[0].centerX = (int)(sumX / count);
        state.zones[0].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[0]);

    // Zone 2 (ID 2): 2. Greenfields (Rural Zone)
    state.zones[1].id = 2;
    strncpy(state.zones[1].name, "2. Greenfields", 63);
    state.zones[1].type = ZONE_RURAL;
    state.zones[1].stability = 55.0;
    state.zones[1].support = 65.0;
    state.zones[1].economy = 60.0;
    state.zones[1].security = 45.0;
    state.zones[1].enemyPresence = false;
    state.zones[1].playerControl = true;
    state.zones[1].activeOperationsCount = 0;
    {
		double px[] = { 269, 286, 375, 479, 470, 474, 423, 373, 320, 275, 285, 261, 268 };
		double py[] = { 626, 638, 662, 631, 498, 484, 450, 457, 450, 479, 542, 592, 625 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[1].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[1].polyX[k] = px[k];
            state.zones[1].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[1].centerX = (int)(sumX / count);
        state.zones[1].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[1]);

    // Zone 3 (ID 3): 3. Ironwood (Forest Zone)
    state.zones[2].id = 3;
    strncpy(state.zones[2].name, "3. Ironwood", 63);
    state.zones[2].type = ZONE_FOREST;
    state.zones[2].stability = 45.0;
    state.zones[2].support = 50.0;
    state.zones[2].economy = 40.0;
    state.zones[2].security = 35.0;
    state.zones[2].enemyPresence = false;
    state.zones[2].playerControl = true;
    state.zones[2].activeOperationsCount = 0;
    {
		double px[] = { 483, 537, 559, 700, 711, 639, 583, 492, 473, 469, 481 };
		double py[] = { 632, 666, 668, 623, 593, 449, 431, 464, 483, 498, 631 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[2].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[2].polyX[k] = px[k];
            state.zones[2].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[2].centerX = (int)(sumX / count);
        state.zones[2].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[2]);

    // Zone 4 (ID 4): 4. Stonewall Range (Mountain Zone)
    state.zones[3].id = 4;
    strncpy(state.zones[3].name, "4. Stonewall Range", 63);
    state.zones[3].type = ZONE_MOUNTAIN;
    state.zones[3].stability = 40.0;
    state.zones[3].support = 45.0;
    state.zones[3].economy = 35.0;
    state.zones[3].security = 30.0;
    state.zones[3].enemyPresence = false;
    state.zones[3].playerControl = true;
    state.zones[3].activeOperationsCount = 0;
    {
		double px[] = { 712, 639, 656, 739, 761, 816, 817, 808, 711 };
		double py[] = { 594, 450, 413, 318, 306, 342, 451, 543, 592 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[3].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[3].polyX[k] = px[k];
            state.zones[3].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[3].centerX = (int)(sumX / count);
        state.zones[3].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[3]);

    // Zone 5 (ID 5): 5. Riverland (Rural Zone)
    state.zones[4].id = 5;
    strncpy(state.zones[4].name, "5. Riverland", 63);
    state.zones[4].type = ZONE_RURAL;
    state.zones[4].stability = 50.0;
    state.zones[4].support = 55.0;
    state.zones[4].economy = 50.0;
    state.zones[4].security = 40.0;
    state.zones[4].enemyPresence = false;
    state.zones[4].playerControl = true;
    state.zones[4].activeOperationsCount = 0;
    {
		double px[] = { 83, 40, 73, 117, 289, 262, 259, 257, 267, 277, 274, 192, 82 };
		double py[] = { 457, 368, 238, 202, 269, 328, 360, 382, 406, 422, 440, 452, 456 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[4].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[4].polyX[k] = px[k];
            state.zones[4].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[4].centerX = (int)(sumX / count);
        state.zones[4].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[4]);

    // Zone 6 (ID 6): 6. Central City (Urban Zone - Center Hub)
    state.zones[5].id = 6;
    strncpy(state.zones[5].name, "6. Central City", 63);
    state.zones[5].type = ZONE_URBAN;
    state.zones[5].stability = 60.0;
    state.zones[5].support = 70.0;
    state.zones[5].economy = 80.0;
    state.zones[5].security = 50.0;
    state.zones[5].enemyPresence = false;
    state.zones[5].playerControl = true;
    state.zones[5].activeOperationsCount = 0;
    {
		double px[] = { 319, 369, 424, 456, 463, 449, 423, 393, 349, 314, 297, 284, 279, 287, 319 };
		double py[] = { 450, 458, 451, 397, 359, 296, 266, 250, 260, 280, 303, 330, 373, 398, 450 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[5].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[5].polyX[k] = px[k];
            state.zones[5].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[5].centerX = (int)(sumX / count);
        state.zones[5].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[5]);

    // Zone 7 (ID 7): 7. Farmland (Rural Zone)
    state.zones[6].id = 7;
    strncpy(state.zones[6].name, "7. Farmland", 63);
    state.zones[6].type = ZONE_RURAL;
    state.zones[6].stability = 45.0;
    state.zones[6].support = 55.0;
    state.zones[6].economy = 55.0;
    state.zones[6].security = 35.0;
    state.zones[6].enemyPresence = false;
    state.zones[6].playerControl = true;
    state.zones[6].activeOperationsCount = 0;
    {
		double px[] = { 457, 490, 582, 638, 657, 737, 579, 460, 475, 481, 483, 479, 465, 458 };
		double py[] = { 443, 465, 431, 449, 413, 319, 214, 263, 292, 324, 366, 398, 420, 442 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[6].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[6].polyX[k] = px[k];
            state.zones[6].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[6].centerX = (int)(sumX / count);
        state.zones[6].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[6]);

    // Zone 8 (ID 8): 8. Shadowwood (Forest Zone)
    state.zones[7].id = 8;
    strncpy(state.zones[7].name, "8. Shadowwood", 63);
    state.zones[7].type = ZONE_FOREST;
    state.zones[7].stability = 30.0;
    state.zones[7].support = 35.0;
    state.zones[7].economy = 25.0;
    state.zones[7].security = 20.0;
    state.zones[7].enemyPresence = true;
    state.zones[7].playerControl = false;
    state.zones[7].activeOperationsCount = 0;
    {
		double px[] = { 118, 95, 195, 279, 373, 396, 387, 290, 118 };
		double py[] = { 202, 116, 56, 46, 93, 170, 215, 267, 202 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[7].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[7].polyX[k] = px[k];
            state.zones[7].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[7].centerX = (int)(sumX / count);
        state.zones[7].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[7]);

    // Zone 9 (ID 9): 9. Highpeak (Mountain Zone)
    state.zones[8].id = 9;
    strncpy(state.zones[8].name, "9. Highpeak", 63);
    state.zones[8].type = ZONE_MOUNTAIN;
    state.zones[8].stability = 35.0;
    state.zones[8].support = 40.0;
    state.zones[8].economy = 30.0;
    state.zones[8].security = 25.0;
    state.zones[8].enemyPresence = false;
    state.zones[8].playerControl = true;
    state.zones[8].activeOperationsCount = 0;
    {
		double px[] = { 374, 462, 523, 641, 768, 815, 781, 738, 578, 459, 386, 396, 374 };
		double py[] = { 95, 43, 50, 45, 96, 219, 292, 319, 214, 263, 216, 172, 95 };
        int count = sizeof(px) / sizeof(px[0]);
        state.zones[8].polyCount = count;
        double sumX = 0, sumY = 0;
        for (int k = 0; k < count; ++k) {
            state.zones[8].polyX[k] = px[k];
            state.zones[8].polyY[k] = py[k];
            sumX += px[k];
            sumY += py[k];
        }
        state.zones[8].centerX = (int)(sumX / count);
        state.zones[8].centerY = (int)(sumY / count);
    }
    initZoneOperations(state.zones[8]);

    // Populate adjacency connections for all zones (Module 2 Graph)
    for (int i = 0; i < state.totalZones; ++i) {
        state.zones[i].adjacentCount = getAdjacentZones(i, state.zones[i].adjacentZones);
    }
}

void selectZone(GameState &state, int zoneId) {
    if (zoneId >= 0 && zoneId < state.totalZones) {
        state.selectedZoneId = zoneId;
    } else {
        state.selectedZoneId = -1;
    }
}

int findZoneAt(const GameState &state, int mouseX, int mouseY) {
    // 1. First test Central City (Zone index 5) since it sits at the geometric center
    if (state.totalZones > 5) {
        if (isPointInPolygon(state.zones[5].polyCount, state.zones[5].polyX, state.zones[5].polyY, mouseX, mouseY)) {
            return 5;
        }
    }

    // 2. Test all other zones
    for (int i = 0; i < state.totalZones; ++i) {
        if (i == 5) continue;

        if (isPointInPolygon(state.zones[i].polyCount, state.zones[i].polyX, state.zones[i].polyY, mouseX, mouseY)) {
            return i;
        }
    }
    return -1;
}

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

void initMapRenderer() {
    if (!g_mapLoaded) {
        g_mapTexture = safeLoadImage("Images/Map.png");
        if (g_mapTexture == 0) {
            g_mapTexture = safeLoadImage("Images/vanguard_map_800x680.png");
        }
        g_mapFrameTexture = safeLoadImage("Images/UI/assets/frames/frame_map.png");
        g_mapLoaded = true;
    }
}

void renderMapAndZones(const GameState &state) {
    if (!g_mapLoaded) {
        initMapRenderer();
    }

    // 1. Draw Map Background
    if (g_mapTexture > 0) {
        iShowImage(20, 20, 800, 680, g_mapTexture);
    } else {
        iSetColor(25, 35, 48);
        iFilledRectangle(20, 20, 800, 680);
    }

    // 2. Draw Zone Border Overlays using iPolygon
    for (int i = 0; i < state.totalZones; ++i) {
        const Zone &z = state.zones[i];
        if (z.polyCount < 3 || z.polyCount > Zone::MAX_POLY_POINTS) continue;

        if (i == state.selectedZoneId) {
            // Selected Zone: Bright glowing golden accent border
            iSetColor(255, 215, 0); // Gold
            iPolygon((double *)z.polyX, (double *)z.polyY, z.polyCount);

            // Double outline stroke for distinct highlight
            iSetColor(255, 245, 180);
            double tempX[Zone::MAX_POLY_POINTS], tempY[Zone::MAX_POLY_POINTS];
            for (int k = 0; k < z.polyCount; ++k) {
                double dirX = z.polyX[k] - z.centerX;
                double dirY = z.polyY[k] - z.centerY;
                tempX[k] = z.polyX[k] + (dirX > 0 ? 1.5 : -1.5);
                tempY[k] = z.polyY[k] + (dirY > 0 ? 1.5 : -1.5);
            }
            iPolygon(tempX, tempY, z.polyCount);
        } else {
            // Unselected Zones: Crisp white border overlay matching the map
            iSetColor(255, 255, 255);
            iPolygon((double *)z.polyX, (double *)z.polyY, z.polyCount);
        }
    }

    // 3. Draw Units, Waypoints, Enemy Camps & Combat (Module 9, 10, 11)
    renderUnitsAndCombatOnMap(state);

    // 4. Frame Map Bezel Overlay (800x680 at 20, 20) with Alpha Blending
    if (g_mapFrameTexture > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        iShowImage(20, 20, 800, 680, g_mapFrameTexture);
        glDisable(GL_BLEND);
    } else {
        // Border fallback around map viewport
        iSetColor(50, 70, 95);
        iRectangle(20, 20, 800, 680);
    }
}
