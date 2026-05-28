use rodio::{OutputStream, Sink};
use rodio::source::{SineWave, Source};
use std::sync::OnceLock;
use std::sync::mpsc::{self, Sender};
use std::thread;
use std::time::Duration;

pub const SOUND_SHOOT:        i32 = 0;
pub const SOUND_EXPLOSION:    i32 = 1;
pub const SOUND_PLAYER_DEATH: i32 = 2;

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

            for sound_id in rx {
                if let Ok(sink) = Sink::try_new(&handle) {
                    match sound_id {
                        SOUND_SHOOT => {
                            let snd = SineWave::new(880.0).take_duration(Duration::from_millis(100)).amplify(0.5);
                            sink.append(snd);
                        },
                        SOUND_EXPLOSION => {
                            let snd = SineWave::new(150.0).take_duration(Duration::from_millis(200)).amplify(0.6);
                            sink.append(snd);
                        },
                        SOUND_PLAYER_DEATH => {
                            let snd = SineWave::new(200.0).take_duration(Duration::from_millis(500)).amplify(0.8);
                            sink.append(snd);
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
