use rodio::{OutputStream, Sink};
use rodio::source::{SineWave, Source};
use std::sync::OnceLock;
use std::sync::mpsc::{self, Sender};
use std::thread;
use std::time::Duration;

pub const SOUND_SHOOT:        i32 = 0;
pub const SOUND_EXPLOSION:    i32 = 1;
pub const SOUND_PLAYER_DEATH: i32 = 2;
pub const SOUND_POWERUP:      i32 = 3;

static AUDIO_TX: OnceLock<Sender<i32>> = OnceLock::new();

#[no_mangle]
pub extern "C" fn rust_init_audio() -> i32 {
    let result = std::panic::catch_unwind(|| -> Result<(), Box<dyn std::error::Error>> {
        let (tx, rx) = mpsc::channel::<i32>();
        AUDIO_TX.set(tx).ok();

        thread::spawn(move || {
            let (_stream, handle) = match OutputStream::try_default() {
                Ok(s) => s,
                Err(_) => return,
            };

            // Música de fondo (estilo space invaders)
            let bg_handle = handle.clone();
            thread::spawn(move || {
                if let Ok(sink) = Sink::try_new(&bg_handle) {
                    // Estilo Galaga: Patrón más rápido y melódico, volumen más alto
                    let freqs = [330.0, 392.0, 330.0, 261.63]; // Mi, Sol, Mi, Do
                    let mut i = 0;
                    loop {
                        let snd = SineWave::new(freqs[i])
                            .take_duration(Duration::from_millis(120))
                            .amplify(0.7); // Mucho más alto
                        sink.append(snd);
                        sink.sleep_until_end();
                        thread::sleep(Duration::from_millis(50)); // Pausa más corta
                        i = (i + 1) % 4;
                    }
                }
            });

            for sound_id in rx {
                if let Ok(sink) = Sink::try_new(&handle) {
                    match sound_id {
                        SOUND_SHOOT => {
                            let snd = SineWave::new(880.0).take_duration(Duration::from_millis(100)).amplify(0.5);
                            sink.append(snd);
                        },
                        SOUND_EXPLOSION => {
                            // Frecuencias medias-altas para que suene como un impacto ruidoso
                            let s1 = SineWave::new(300.0).take_duration(Duration::from_millis(250)).amplify(0.7);
                            let s2 = SineWave::new(350.0).take_duration(Duration::from_millis(250)).amplify(0.7);
                            let s3 = SineWave::new(400.0).take_duration(Duration::from_millis(250)).amplify(0.7);
                            sink.append(s1.mix(s2).mix(s3));
                        },
                        SOUND_PLAYER_DEATH => {
                            let snd = SineWave::new(200.0).take_duration(Duration::from_millis(500)).amplify(0.8);
                            sink.append(snd);
                        },
                        SOUND_POWERUP => {
                            // Arpegio triunfante (C5, E5, G5, C6)
                            let n1 = SineWave::new(523.25).take_duration(Duration::from_millis(100)).amplify(0.6);
                            let n2 = SineWave::new(659.25).take_duration(Duration::from_millis(100)).amplify(0.6);
                            let n3 = SineWave::new(783.99).take_duration(Duration::from_millis(100)).amplify(0.6);
                            let n4 = SineWave::new(1046.50).take_duration(Duration::from_millis(300)).amplify(0.6);
                            sink.append(n1);
                            sink.append(n2);
                            sink.append(n3);
                            sink.append(n4);
                        },
                        _ => {}
                    }
                    sink.detach();
                }
            }
        });
        Ok(())
    });

    match result {
        Ok(Ok(())) => 0,
        _ => -1,
    }
}

#[no_mangle]
pub extern "C" fn rust_play_sound(sound_id: i32) {
    let _ = std::panic::catch_unwind(|| {
        if let Some(tx) = AUDIO_TX.get() {
            let _ = tx.send(sound_id);
        }
    });
}
