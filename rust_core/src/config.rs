use serde::Deserialize;
use std::fs;

// ── Estructura compatible con C ────────────────────────────────────
#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct GameConfig {
    pub player_lives:       i32,
    pub player_speed:       f32,
    pub alien_shoot_rate:   f32,
    pub initial_difficulty: i32,
}

impl Default for GameConfig {
    fn default() -> Self {
        GameConfig {
            player_lives:       3,
            player_speed:       4.5,
            alien_shoot_rate:   0.02,
            initial_difficulty: 1,
        }
    }
}

// Estructura interna de serde para parsear el TOML
#[derive(Deserialize)]
struct TomlRoot {
    game: TomlGame,
}

#[derive(Deserialize)]
struct TomlGame {
    player_lives:       Option<i32>,
    player_speed:       Option<f32>,
    alien_shoot_rate:   Option<f32>,
    initial_difficulty: Option<i32>,
}

/// Carga la configuración desde settings.toml.
/// Si el archivo no existe o tiene errores, devuelve valores por defecto seguros.
#[no_mangle]
pub extern "C" fn load_game_config() -> GameConfig {
    let result = std::panic::catch_unwind(|| -> GameConfig {
        let content = fs::read_to_string("settings.toml")
            .unwrap_or_default();

        let parsed: Result<TomlRoot, _> = toml::from_str(&content);

        match parsed {
            Ok(root) => {
                let g = root.game;
                GameConfig {
                    player_lives:       g.player_lives.unwrap_or(3),
                    player_speed:       g.player_speed.unwrap_or(4.5),
                    alien_shoot_rate:   g.alien_shoot_rate.unwrap_or(0.02),
                    initial_difficulty: g.initial_difficulty.unwrap_or(1),
                }
            }
            Err(_) => GameConfig::default(),
        }
    });

    result.unwrap_or_else(|_| GameConfig::default())
}
