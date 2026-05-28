#[repr(C, align(4))]
pub struct Enemy {
    pub status: u8,
    pub pattern: u8,
    pub padding: u16,
    pub x: i32,
    pub y: i32,
    pub vel_x: i32,
    pub vel_y: i32,
    pub phase: i32,
    pub _padding2: i32,
    pub _padding3: i32,
}

#[no_mangle]
pub unsafe extern "C" fn rust_update_enemy(enemy: *mut Enemy, player_x: i32) {
    if enemy.is_null() { return; }
    let e = unsafe { &mut *enemy };
    
    match e.pattern {
        0 => { // PATTERN_DIAGONAL
            e.x += e.vel_x;
            e.y += e.vel_y;
            if e.x <= 0 {
                e.x = 0;
                e.vel_x = -e.vel_x;
            } else if e.x + 30 >= 800 { // WINDOW_WIDTH - ENEMY_WIDTH
                e.x = 800 - 30;
                e.vel_x = -e.vel_x;
            }
        },
        1 => { // PATTERN_SINE
            e.x += e.vel_x;
            e.y += e.vel_y;
            if e.x <= 0 {
                e.x = 0;
                e.vel_x = -e.vel_x;
            } else if e.x + 30 >= 800 {
                e.x = 800 - 30;
                e.vel_x = -e.vel_x;
            }
        },
        2 => { // PATTERN_KAMIKAZE
            e.y += e.vel_y;
            if e.x < player_x {
                e.x += e.vel_x.abs();
            } else if e.x > player_x {
                e.x -= e.vel_x.abs();
            }
        },
        _ => {}
    }
}
