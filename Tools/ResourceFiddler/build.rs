use std::env;

fn main() {
    match env::var("PROFILE").unwrap().as_str() {
        "release" => {
            println!("cargo:rustc-link-search=../../release/");
        }
        _ => {
            println!("cargo:rustc-link-search=../../debug/");
        }
    }

    if env::var("CARGO_CFG_TARGET_OS").as_deref() == Ok("linux") {
        println!("cargo:rustc-link-arg=-Wl,-rpath,$ORIGIN");
    }
}
