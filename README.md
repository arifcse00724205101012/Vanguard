# Vanguard

Vanguard is a single-player, real-time regional strategy game built with C++, OpenGL, GLUT, and iGraphics. You lead a stabilization campaign across a nine-zone region: strengthen the economy, improve public support and stability, build infrastructure, deploy security forces, and contain an insurgency before it spreads.

> **Project status:** playable prototype / MVP. The game is currently distributed as a Visual Studio Win32 project and is designed for Windows.

## Game overview

The campaign begins in a fragile but controllable region. Every zone has its own stability, public support, economy, revenue, maintenance, operations, and control status. Your decisions affect the whole region through monthly income and expenses, while local investments determine whether individual zones can be secured.

The game has two connected layers:

- **Regional management:** select zones, monitor their metrics, construct operations, manage money, and improve long-term stability.
- **Tactical security:** deploy a limited number of security units, move them through connected zones, fight insurgents, destroy camps, and prevent enemy expansion.

After the campaign reaches Day 20, an insurgency rumor unlocks troop deployment and combat. The enemy does not appear immediately: a preparation window gives you time to position forces and reinforce vulnerable zones.

## Features

- Nine connected zones with different terrain types: urban, rural, forest, and mountain.
- Live day/month/year progression with normal and fast game speed states.
- Regional treasury simulation including funds, revenue, maintenance, debt, interest, reserves, and monthly balance.
- Three core regional metrics: stability, support, and economy.
- Four operation branches:
  - Welfare
  - Economic
  - Infrastructure
  - Security
- Multi-level operations with construction time, maintenance, revenue, prerequisites, compatibility rules, and zone synergies.
- Long-term economic effects from agriculture, industry, trade, offices, infrastructure, and other completed operations.
- Limited security force management: a maximum of four player security units and one unit per zone.
- Zone-to-zone troop movement using the map's adjacency network.
- Insurgent units, enemy camps, expansion, retreat, containment, and combat engagements.
- Combat and construction feedback through map events, banners, animations, sound effects, and music.
- Main menu, settings modal, gameplay HUD, zone details panel, operations screen, victory screen, and defeat screen.
- Victory and defeat conditions that reward both military control and civilian recovery.

## Zones

| Zone | Terrain | Strategic character |
| --- | --- | --- |
| Northbridge | Urban | Population and administrative hub |
| Greenfields | Rural | Agriculture and civilian support |
| Ironwood | Forest | Difficult terrain and insurgency risk |
| Stonewall Range | Mountain | Defensive terrain and restricted mobility |
| Riverland | Rural | Agriculture and regional connectivity |
| Central City | Urban | Central economic and transport hub |
| Farmland | Rural | Food production and economic recovery |
| Shadowwood | Forest | High-risk insurgent operating ground |
| Highpeak | Mountain | Remote, defensible frontier |

## How to play

1. Start the campaign from the main menu.
2. Select a zone on the map or use the number keys.
3. Read the zone's stability, support, economy, income, expenses, and active operations.
4. Open the Operations screen and invest in initiatives that solve the zone's most urgent problems.
5. Watch construction progress and make sure recurring maintenance remains affordable.
6. Before the insurgency emerges, build a stable treasury and improve vulnerable zones.
7. Once security is unlocked, deploy troops into controlled zones and move them toward threats.
8. Respond to enemy expansion quickly. A zone is not truly secured until it is under player control, has no enemy presence or surviving camp, and has at least 70% stability.

## Controls

### Main menu

| Input | Action |
| --- | --- |
| `Space` or `Enter` | Start the game |
| `Esc` | Close the settings modal, or exit from the main menu |
| Left click | Select menu buttons and settings controls |

### Gameplay

| Input | Action |
| --- | --- |
| Left click on a zone | Select the zone; click it again to return to the regional overview |
| `1`–`9` | Select zones 1–9 directly |
| Left click on a security unit | Select and begin dragging the unit |
| Left-drag a security unit to a zone | Order the selected unit to move there when the destination is reachable |
| Right click on a zone | Move the selected security unit to that zone |
| `O` | Open the Operations screen |
| `R` | Restart/reset the current campaign |
| Left/Right Arrow | Move between zones |
| Left click on `+ DEPLOY TROOP` | Deploy a security unit when unlocked, affordable, and allowed |
| Left click on `MANAGE OPERATIONS` | Open operations for the selected zone |

### Operations screen

| Input | Action |
| --- | --- |
| Left click on a category tab | Browse Welfare, Economic, Infrastructure, or Security operations |
| Left click on an operation node | Select an operation |
| Left click on a level | Inspect a level's cost, requirements, and effects |
| Left click on the action button | Purchase or start the selected operation when requirements are met |
| `Esc` or `B` | Return to gameplay |

### End screens

| Input | Action |
| --- | --- |
| `R` | Restart the campaign |
| `M` or `Esc` | Return to the main menu |
| Left click | Use the available victory/defeat buttons |

## Winning and losing

### Victory

You win when all nine zones are secured. For a zone to count as secured, it must:

- Be under player control.
- Have no enemy presence.
- Have no undestroyed enemy camp.
- Have at least 70% stability.

### Defeat

The campaign ends if any of the following occurs:

- The enemy controls at least 60% of the region.
- Average regional stability falls to 20% or lower.
- Funds and monthly income are both zero or below.

## Operations and progression

Operations have a lifecycle: available, purchased, constructing, completed, active, or locked. Many advanced initiatives require an earlier operation, a minimum zone metric, or a compatible terrain type. Operations can improve stability, support, economy, security, revenue, or troop mobility, but they may also add construction and recurring maintenance costs.

The operation tree includes initiatives such as education, healthcare, food and water, sanitation, industry, offices, agriculture, trade, electricity, telecommunications, roads, troop support, radar, and air support. Several operations have up to three levels, allowing a zone to develop gradually instead of spending the entire treasury at once.

## Building from source

### Requirements

- Windows 10 or later (Win32)
- Visual Studio with C++ desktop development tools
- A compiler/toolset compatible with the project configuration (`v120`, originally Visual Studio 2013)
- Win32 build support

The repository includes the iGraphics headers/libraries and the legacy GLUT/GLAUX runtime files required by the project. If your Visual Studio installation does not provide the legacy `v120` toolset, retarget the solution to an installed Win32 toolset before building.

### Build steps

1. Clone the repository:

   ```powershell
   git clone https://github.com/arifcse00724205101012/Vanguard.git
   cd Vanguard
   ```

2. Open `Vanguard.sln` in Visual Studio.
3. Select `Debug` or `Release` and the `Win32` platform.
4. Build the solution with **Build > Build Solution**.
5. Run the generated executable with the repository root as its working directory. The game loads image and audio assets using paths such as `Images/...` and `Music/...`, so running from a different working directory may cause assets to be missing.

The project starts a 1200×780 iGraphics window titled `Vanguard - Strategy Game (MPV)`.

## Project structure

```text
Vanguard/
├── main.cpp                    # iGraphics entry point and input routing
├── Vanguard.sln                # Visual Studio solution
├── Vanguard.vcxproj            # Win32 C++ project
├── src/
│   ├── Conditions/             # Victory, defeat, and insurgency timing rules
│   ├── Economy/                # Revenue, maintenance, and regional metrics
│   ├── Game/                   # Central state and game-loop logic
│   ├── Map/                    # Zones, adjacency, selection, and map rendering
│   ├── Operations/             # Operation definitions, prerequisites, and progress
│   ├── Security/               # Combat, containment, and engagement resolution
│   ├── UI/                     # Menu, HUD, zone panel, and operations screen
│   └── Units/                  # Player security units and insurgent AI
├── Images/                     # Maps, UI textures, icons, and game artwork
├── Music/                      # Audio assets used by the game
└── iGraphics/                  # iGraphics/OpenGL support files and libraries
```

## Technical notes

- The game uses a central `GameState` structure shared by the gameplay systems.
- A one-second timer advances the campaign calendar and monthly simulation.
- Continuous updates handle unit movement, animations, combat, construction, and ending conditions.
- The map and UI use relative asset paths; keep the executable's working directory at the repository root.
- The current prototype does not expose a save/load system; restarting the campaign reinitializes the in-memory game state.

## Credits and asset licensing

Vanguard is an academic/prototype project. The repository contains project artwork, UI assets, fonts/runtime dependencies, and audio files used by the prototype. Before redistributing a public build, verify that every third-party asset has permission for the intended distribution and add the applicable attribution or license information here.

## Contributing

1. Create a feature branch.
2. Keep gameplay changes separated from asset changes where possible.
3. Test the Win32 Debug and/or Release build from the repository root.
4. Describe any new controls, dependencies, asset requirements, or balance changes in the pull request.

## License

No project license has been declared yet. Until a license is added, all rights to the original Vanguard source and original project assets remain with their respective owners.

