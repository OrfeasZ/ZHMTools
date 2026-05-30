use rayon::prelude::*;
use rpkg_rs::resource::partition_manager::PartitionManager;
use std::collections::HashSet;
use std::sync::atomic::{AtomicUsize, Ordering};

use crate::resource_lib;

/// Dump every resource whose type matches one of the given strings to
/// `<output_dir>/<TYPE>/<RID>.json`.
pub fn run(
    partition_manager: &PartitionManager,
    game: resource_lib::Game,
    types: &HashSet<String>,
    output_dir: &std::path::Path,
) {
    let work_items: Vec<_> = partition_manager
        .partitions
        .iter()
        .flat_map(|partition| {
            partition
                .latest_resources()
                .into_iter()
                .filter_map(|(resource, _)| {
                    let resource_type = resource.data_type();
                    if !types.contains(&resource_type) {
                        return None;
                    }
                    Some((partition, resource, resource_type))
                })
                .collect::<Vec<_>>()
        })
        .collect();

    println!(
        "Dumping {} resources across {} types to {}",
        work_items.len(),
        types.len(),
        output_dir.display()
    );

    // Pre-create the per-type subdirectories so the parallel writers don't race.
    for t in types {
        if let Err(e) = std::fs::create_dir_all(output_dir.join(t)) {
            eprintln!("Failed to create {}/{}: {}", output_dir.display(), t, e);
            std::process::exit(1);
        }
    }

    let written = AtomicUsize::new(0);
    let failed = AtomicUsize::new(0);
    let unsupported = AtomicUsize::new(0);

    work_items
        .par_iter()
        .for_each(|(partition, resource, resource_type)| {
            let rid = resource.rrid();

            if !resource_lib::is_resource_supported(game, resource_type) {
                unsupported.fetch_add(1, Ordering::Relaxed);
                return;
            }

            let resource_data = match partition.read_resource(rid) {
                Ok(d) => d,
                Err(e) => {
                    eprintln!("Failed to read {}.{}: {}", rid, resource_type, e);
                    failed.fetch_add(1, Ordering::Relaxed);
                    return;
                }
            };

            let json = match resource_lib::Converter::get(game, resource_type)
                .unwrap()
                .memory_to_string(&resource_data)
            {
                Ok(s) => s,
                Err(e) => {
                    eprintln!("Failed to convert {}.{} to JSON: {}", rid, resource_type, e);
                    failed.fetch_add(1, Ordering::Relaxed);
                    return;
                }
            };

            let path = output_dir
                .join(resource_type.as_str())
                .join(format!("{}.json", rid));
            if let Err(e) = std::fs::write(&path, json.as_str().unwrap()) {
                eprintln!("Failed to write {}: {}", path.display(), e);
                failed.fetch_add(1, Ordering::Relaxed);
                return;
            }

            written.fetch_add(1, Ordering::Relaxed);
        });

    println!("Written:     {}", written.load(Ordering::Relaxed));
    println!("Failed:      {}", failed.load(Ordering::Relaxed));
    let n_unsupported = unsupported.load(Ordering::Relaxed);
    if n_unsupported > 0 {
        println!(
            "Unsupported: {} (ResourceLib does not handle these types)",
            n_unsupported
        );
    }
}
