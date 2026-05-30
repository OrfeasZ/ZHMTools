#![feature(reentrant_lock)]

mod dump;
mod extract;
mod paths;
mod pins;
mod resource_lib;
mod scan;
mod test;
mod types;
mod utils;

use clap::{Parser, Subcommand, ValueEnum};
use rpkg_rs::GlacierGame;
use rpkg_rs::resource::runtime_resource_id::RuntimeResourceID;
use std::collections::HashSet;
use std::env;
use std::path::PathBuf;

#[derive(Parser)]
#[command(name = "resource_fiddler", about = "Poke at Glacier game resources.")]
struct Cli {
    /// Path to the game's Retail directory.
    #[arg(long, short = 'r')]
    retail: PathBuf,

    /// Which game's package format to expect.
    #[arg(long, short = 'g', value_enum)]
    game: GameArg,

    #[command(subcommand)]
    mode: Mode,
}

#[derive(Copy, Clone, ValueEnum)]
enum GameArg {
    #[value(name = "HM2016", alias = "hm2016")]
    Hm2016,
    #[value(name = "HM2", alias = "hm2")]
    Hm2,
    #[value(name = "HM3", alias = "hm3")]
    Hm3,
    #[value(name = "KNT", alias = "knt")]
    Knt,
}

impl GameArg {
    fn to_pair(self) -> (GlacierGame, resource_lib::Game) {
        match self {
            GameArg::Hm2016 => (GlacierGame::HM2016, resource_lib::Game::Hitman2016),
            GameArg::Hm2 => (GlacierGame::HM2, resource_lib::Game::Hitman2),
            GameArg::Hm3 => (GlacierGame::HM3, resource_lib::Game::Hitman3),
            GameArg::Knt => (GlacierGame::Bond, resource_lib::Game::FirstLight007),
        }
    }
}

#[derive(Subcommand)]
enum Mode {
    /// Round-trip every (or a filtered subset of) resource through ResourceLib and report
    /// resources that aren't stable across JSON <-> BIN1 <-> JSON conversion.
    Test {
        /// Only test these resource hashes. Empty means test everything.
        hashes: Vec<String>,
    },

    /// Walk all TBLU resources and dump unique input/output pin names per module.
    Pins,

    /// Convert a single resource to JSON and print it to stdout.
    Extract {
        /// Resource hash (hex).
        hash: String,
    },

    /// Print every resource whose raw bytes contain the given needle.
    Scan {
        /// Byte string to look for inside each resource.
        needle: String,
    },

    /// List every unique resource type and its count.
    Types,

    /// Try to find and extract path-like strings from all game resources.
    /// If --output is given, the paths are written to that file (one per line) instead of stdout.
    Paths {
        /// Optional path to write the discovered paths to (one per line).
        #[arg(long, short = 'o')]
        output: Option<PathBuf>,
    },

    /// Convert every resource matching one of the given types to JSON under <output-dir>.
    Dump {
        /// Where to write the JSON files.
        output_dir: PathBuf,

        /// Resource types to dump (e.g. TEMP TBLU).
        #[arg(required = true)]
        types: Vec<String>,
    },
}

fn parse_rrid(hash: &str) -> RuntimeResourceID {
    RuntimeResourceID::from_hex_string(hash).unwrap_or_else(|e| {
        eprintln!("invalid resource hash {:?}: {}", hash, e);
        std::process::exit(1);
    })
}

fn main() {
    let cli_args: Vec<String> = env::args().collect();

    // Hardcoded fallback used when running from an IDE with no extra arguments.
    //let hardcoded_args = [
    //    "resource_fiddler", "-r", "C:/Games/HITMAN3/Retail", "-g", "HM3", "pins",
    //];
    //let hardcoded_args = [
    //    "resource_fiddler",
    //    "-r", "/home/orfeasz/.local/share/Steam/steamapps/common/007 First Light/Retail",
    //    "-g", "KNT", "extract", "0193ED6078F99215",
    //];
    //let hardcoded_args = [
    //    "resource_fiddler",
    //    "-r", "/home/orfeasz/Games/HITMAN3/Retail",
    //    "-g", "HM3", "paths"
    //];
    //let hardcoded_args = [
    //    "resource_fiddler",
    //    "-r", "/home/orfeasz/.local/share/Steam/steamapps/common/007 First Light/Retail",
    //    "-g", "KNT", "paths", "-o", "knt-paths.log"
    //];

    //let cli = if cli_args.len() > 1 {
    //    Cli::parse()
    //} else {
    //    Cli::parse_from(hardcoded_args)
    //};

    let cli = Cli::parse();

    let (game_version, game) = cli.game.to_pair();

    // Mount the game.
    println!("Mounting game...");
    let partition_manager = utils::mount_game(cli.retail, game_version);
    println!("Game mounted successfully!");

    match cli.mode {
        Mode::Test { hashes } => {
            let filter: HashSet<RuntimeResourceID> = hashes.iter().map(|h| parse_rrid(h)).collect();
            test::run(partition_manager, game, filter)
        }
        Mode::Pins => pins::run(partition_manager, game),
        Mode::Extract { hash } => extract::run(partition_manager, game, parse_rrid(&hash)),
        Mode::Scan { needle } => scan::run(&partition_manager, game, &needle),
        Mode::Types => types::run(&partition_manager),
        Mode::Paths { output } => paths::run(&partition_manager, output.as_deref()),
        Mode::Dump { output_dir, types } => {
            let types: HashSet<String> = types.into_iter().collect();
            dump::run(&partition_manager, game, &types, &output_dir)
        }
    }
}
