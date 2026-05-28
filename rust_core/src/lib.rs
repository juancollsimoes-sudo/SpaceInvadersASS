#![deny(unsafe_op_in_unsafe_fn)]

pub mod persistence;

pub use persistence::{save_high_score, load_high_score};
