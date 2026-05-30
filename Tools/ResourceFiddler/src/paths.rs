use rayon::prelude::*;
use rpkg_rs::resource::partition_manager::PartitionManager;
use std::collections::BTreeSet;
use std::io::Write;
use std::path::Path;
use std::sync::Mutex;

/// Scan every resource for possible paths. When `output` is provided, the discovered
/// paths are written there (one per line); otherwise they're printed to stdout.
pub fn run(partition_manager: &PartitionManager, output: Option<&Path>) {
    const NEEDLES: [&[u8]; 2] = [b"assembly:", b"modules:"];

    let work_items: Vec<_> = partition_manager
        .partitions
        .iter()
        .flat_map(|partition| {
            partition
                .latest_resources()
                .into_iter()
                .map(move |(resource, _)| (partition, resource))
                .collect::<Vec<_>>()
        })
        .collect();

    println!("Scanning {} resources for paths...", work_items.len());

    let paths: Mutex<BTreeSet<String>> = Mutex::new(BTreeSet::new());

    work_items.par_iter().for_each(|(partition, resource)| {
        let rid = resource.rrid();
        let Ok(data) = partition.read_resource(rid) else {
            return;
        };
        let bytes: &[u8] = &data;

        let mut local: Vec<String> = Vec::new();

        for needle in &NEEDLES {
            let mut search_from = 0;
            while let Some(rel) = bytes[search_from..]
                .windows(needle.len())
                .position(|w| w == *needle)
            {
                let match_at = search_from + rel;

                // Walk back through any opening brackets (paths can have one or two `[`s)
                // and stop at the first non-bracket byte.
                let mut start = match_at;
                while start > 0 && bytes[start - 1] == b'[' {
                    start -= 1;
                }

                // Walk forward until a null, a `"`, a non-ASCII byte, or a `</`.
                let mut end = match_at + needle.len();
                while end < bytes.len() {
                    let b = bytes[end];
                    if b == 0 || b == b'"' || !b.is_ascii() {
                        break;
                    }
                    if b == b'<' && bytes.get(end + 1) == Some(&b'/') {
                        break;
                    }
                    end += 1;
                }

                if let Ok(path) = std::str::from_utf8(&bytes[start..end]) {
                    local.push(path.to_string());
                }

                // Resume past this run so we don't re-match the same needle.
                search_from = end.max(match_at + needle.len());
            }
        }

        if !local.is_empty() {
            let mut paths = paths.lock().unwrap();
            paths.extend(local);
        }
    });

    let paths = paths.into_inner().unwrap();
    println!("Found {} unique paths.", paths.len());

    match output {
        Some(out_path) => {
            let file = std::fs::File::create(out_path).unwrap_or_else(|e| {
                eprintln!("Failed to open {} for writing: {}", out_path.display(), e);
                std::process::exit(1);
            });
            let mut writer = std::io::BufWriter::new(file);
            for path in &paths {
                writeln!(writer, "{}", path).unwrap_or_else(|e| {
                    eprintln!("Failed to write to {}: {}", out_path.display(), e);
                    std::process::exit(1);
                });
            }
            println!("Wrote paths to {}.", out_path.display());
        }
        None => {
            for path in &paths {
                println!("{}", path);
            }
        }
    }
}
