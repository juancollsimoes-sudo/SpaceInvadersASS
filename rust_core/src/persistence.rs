use std::fs;
use std::io::{self, Read, Write};
use std::path::Path;

const SCORE_FILE: &str = "high_score.dat";

// XOR simple para ofuscación básica (no es cifrado criptográfico)
const XOR_KEY: u8 = 0xA7;

fn obfuscate(data: &[u8]) -> Vec<u8> {
    data.iter().map(|b| b ^ XOR_KEY).collect()
}

/// Guarda el high score en disco de forma segura.
/// Devuelve 0 en éxito, -1 en error.
#[no_mangle]
pub extern "C" fn save_high_score(score: i32) -> i32 {
    let result = std::panic::catch_unwind(|| -> io::Result<()> {
        let bytes = score.to_le_bytes();
        let encoded = obfuscate(&bytes);
        let mut file = fs::File::create(SCORE_FILE)?;
        file.write_all(&encoded)?;
        Ok(())
    });

    match result {
        Ok(Ok(())) => 0,
        _ => -1,
    }
}

/// Carga el high score desde disco.
/// Devuelve 0 si el archivo no existe, está corrupto o hay cualquier error.
#[no_mangle]
pub extern "C" fn load_high_score() -> i32 {
    let result = std::panic::catch_unwind(|| -> Option<i32> {
        if !Path::new(SCORE_FILE).exists() {
            return Some(0);
        }
        let mut file = fs::File::open(SCORE_FILE).ok()?;
        let mut encoded = Vec::new();
        file.read_to_end(&mut encoded).ok()?;
        if encoded.len() < 4 {
            return None; // archivo corrupto
        }
        let decoded = obfuscate(&encoded[..4]);
        let arr: [u8; 4] = decoded.try_into().ok()?;
        Some(i32::from_le_bytes(arr))
    });

    match result {
        Ok(Some(score)) => score,
        _ => 0, // valor seguro por defecto
    }
}
