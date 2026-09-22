#define _CRT_SECURE_NO_WARNINGS
#include "Map/Zone.h"

// 9x9 Adjacency Matrix for Vanguard's 9 zones (indices 0 to 8)
static const bool g_zoneAdjacency[9][9] = {
    // 0: Northbridge
    { false, true,  false, false, true,  true,  false, false, false },
    // 1: Greenfields
    { true,  false, true,  false, false, true,  false, false, false },
    // 2: Ironwood
    { false, true,  false, true,  false, false, true,  false, false },
    // 3: Stonewall Range
    { false, false, true,  false, false, false, true,  false, true  },
    // 4: Riverland
    { true,  false, false, false, false, true,  false, true,  false },
    // 5: Central City (Central Hub connecting to all direct neighbors)
    { true,  true,  false, false, true,  false, true,  true,  true  },
    // 6: Farmland
    { false, false, true,  true,  false, true,  false, false, true  },
    // 7: Shadowwood
    { false, false, false, false, true,  true,  false, false, true  },
    // 8: Highpeak
    { false, false, false, true,  false, true,  true,  true,  false }
};

const char* getZoneTypeName(ZoneType type) {
    switch (type) {
        case ZONE_URBAN: return "Urban Zone";
        case ZONE_RURAL: return "Rural Zone";
        case ZONE_FOREST: return "Forest Zone";
        case ZONE_MOUNTAIN: return "Mountain Zone";
        default: return "Unknown";
    }
}

bool areZonesConnected(int zoneA, int zoneB) {
    if (zoneA < 0 || zoneA >= 9 || zoneB < 0 || zoneB >= 9) return false;
    return g_zoneAdjacency[zoneA][zoneB];
}

int getAdjacentZones(int zoneId, int outAdjacent[8]) {
    if (zoneId < 0 || zoneId >= 9) return 0;
    int count = 0;
    for (int j = 0; j < 9; ++j) {
        if (g_zoneAdjacency[zoneId][j]) {
            outAdjacent[count++] = j;
        }
    }
    return count;
}
