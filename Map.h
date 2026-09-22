#ifndef MAP_H
#define MAP_H

struct GameState;

void initZones(GameState &state);
void selectZone(GameState &state, int zoneId);
int findZoneAt(const GameState &state, int mouseX, int mouseY);
bool isPointInPolygon(int nvert, const double *vertx, const double *verty, double testx, double testy);

void initMapRenderer();
void renderMapAndZones(const GameState &state);

#endif // MAP_H
