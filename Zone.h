#ifndef ZONE_H
#define ZONE_H

#include "Operations/Operations.h"

// Zone types (2 Urban, 3 Rural, 2 Forest, 2 Mountain)
enum ZoneType {
    ZONE_URBAN,
    ZONE_RURAL,
    ZONE_FOREST,
    ZONE_MOUNTAIN
};

// Zone state structure
struct Zone {
    int id;
    char name[64];
    ZoneType type;
    
    // Core attributes (0.0 to 100.0)
    double stability;
    double support;
    double economy;
    double security;
    
    // Security & control status
    bool enemyPresence;
    bool playerControl;
    int activeOperationsCount;
    
    // Zone Economic & Maintenance Contributions (Module 8)
    double zoneRevenue;
    double zoneMaintenance;
    double zoneNetIncome;

    // Operations in this zone (15 total)
    static const int MAX_OPERATIONS = 15;
    Operation operations[MAX_OPERATIONS];
    int totalOperations;
    
    // Exact polygon boundary coordinates for drawing and click detection
    static const int MAX_POLY_POINTS = 64;
    int polyCount;
    double polyX[MAX_POLY_POINTS];
    double polyY[MAX_POLY_POINTS];
    
    // Pin/badge center position on map
    double centerX;
    double centerY;

    // Adjacency connections (Module 2 Graph)
    static const int MAX_ADJACENT = 8;
    int adjacentZones[MAX_ADJACENT];
    int adjacentCount;
};

const char* getZoneTypeName(ZoneType type);
bool areZonesConnected(int zoneA, int zoneB);
int getAdjacentZones(int zoneId, int outAdjacent[8]);

#endif // ZONE_H
