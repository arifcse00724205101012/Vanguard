#ifndef MAIN_MENU_H
#define MAIN_MENU_H

struct GameState;

// Button identifiers for Main Menu
enum MenuButtonId {
    MENU_BTN_NONE = 0,
    MENU_BTN_PLAY,
    MENU_BTN_SETTINGS,
    MENU_BTN_EXIT
};

// Lifecycle and render functions
void initMainMenu();
void renderMainMenu(const GameState &state);

// Audio playback functions
void playMenuMusic();
void stopMenuMusic();

// Input handling functions
void handleMainMenuMouseMove(int mx, int my);
void handleMainMenuMouseClick(GameState &state, int button, int mx, int my);

#endif // MAIN_MENU_H
