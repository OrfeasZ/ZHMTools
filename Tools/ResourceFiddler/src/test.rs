use rayon::prelude::*;
use rpkg_rs::resource::partition_manager::PartitionManager;
use rpkg_rs::resource::runtime_resource_id::RuntimeResourceID;
use std::collections::HashSet;
use std::sync::atomic::{AtomicUsize, Ordering};

use crate::resource_lib;
use crate::utils::{fmt_patch_id, print_json_diff};

pub fn run(
    partition_manager: PartitionManager,
    game: resource_lib::Game,
    resource_filter: HashSet<RuntimeResourceID>,
) {
    let total_resources = AtomicUsize::new(0);
    let failed_resources = AtomicUsize::new(0);

    let filter_active = !resource_filter.is_empty();

    // Collect all work items first to enable parallel processing.
    let work_items: Vec<_> = partition_manager
        .partitions
        .iter()
        .flat_map(|partition| {
            partition
                .latest_resources()
                .into_iter()
                .filter_map(|(resource, patch_id)| {
                    let resource_type = resource.data_type();
                    if !resource_lib::is_resource_supported(game, &resource_type) {
                        return None;
                    }

                    if filter_active && !resource_filter.contains(resource.rrid()) {
                        return None;
                    }

                    Some((partition, resource, patch_id))
                })
                .collect::<Vec<_>>()
        })
        .collect();

    if filter_active && work_items.len() < resource_filter.len() {
        let found: HashSet<RuntimeResourceID> =
            work_items.iter().map(|(_, r, _)| *r.rrid()).collect();
        for missing in resource_filter.difference(&found) {
            eprintln!(
                "Warning: resource {} not found in any mounted partition (or is unsupported).",
                missing
            );
        }
    }

    let resource_count = work_items.len();
    print!("Testing {} resources...\n", resource_count);

    // Process resources in parallel.
    work_items
        .par_iter()
        .for_each(|(partition, resource, patch_id)| {
            let resource_type = resource.data_type();
            let rid = resource.rrid();

            let resource_data = match partition.read_resource(rid) {
                Err(e) => {
                    eprintln!(
                        "Failed to read resource {}.{} from {} ({}): {}",
                        rid,
                        resource_type,
                        partition.partition_info().id,
                        fmt_patch_id(patch_id),
                        e
                    );
                    failed_resources.fetch_add(1, Ordering::Relaxed);
                    return;
                }
                Ok(data) => data,
            };

            // Convert to JSON.
            let json_data = match {
                let converter = resource_lib::Converter::get(game, &resource_type).unwrap();
                converter.memory_to_string(&resource_data)
            } {
                Ok(s) => s,
                Err(e) => {
                    eprintln!(
                        "Failed to convert resource {}.{} from {} to JSON ({}): {}",
                        rid,
                        resource_type,
                        partition.partition_info().id,
                        fmt_patch_id(patch_id),
                        e
                    );
                    failed_resources.fetch_add(1, Ordering::Relaxed);
                    return;
                }
            };

            // Convert back to resource.
            let resource_data_2 = match {
                let generator = resource_lib::Generator::get(game, &resource_type).unwrap();
                generator.string_to_memory(json_data.as_str().unwrap(), false)
            } {
                Ok(s) => s,
                Err(e) => {
                    eprintln!(
                        "Failed to convert resource {}.{} from {} to BIN1 ({}): {}",
                        rid,
                        resource_type,
                        partition.partition_info().id,
                        fmt_patch_id(patch_id),
                        e
                    );
                    failed_resources.fetch_add(1, Ordering::Relaxed);
                    return;
                }
            };

            // And back to JSON again.
            let json_data_2 = match {
                let converter = resource_lib::Converter::get(game, &resource_type).unwrap();
                converter.memory_to_string(resource_data_2.as_bytes())
            } {
                Ok(s) => s,
                Err(e) => {
                    eprintln!(
                        "Failed to convert resource {}.{} from {} to JSON ({}): {}",
                        rid,
                        resource_type,
                        partition.partition_info().id,
                        fmt_patch_id(patch_id),
                        e
                    );
                    failed_resources.fetch_add(1, Ordering::Relaxed);
                    return;
                }
            };

            if json_data.as_str().unwrap() != json_data_2.as_str().unwrap() {
                eprintln!(
                    "Resource {}.{} from {} ({}) is not stable after conversion.",
                    rid,
                    resource_type,
                    partition.partition_info().id,
                    fmt_patch_id(patch_id)
                );
                print_json_diff(json_data.as_str().unwrap(), json_data_2.as_str().unwrap());
                failed_resources.fetch_add(1, Ordering::Relaxed);
            } else {
                total_resources.fetch_add(1, Ordering::Relaxed);
            }

            let processed =
                total_resources.load(Ordering::Relaxed) + failed_resources.load(Ordering::Relaxed);
            if processed % 1000 == 0 {
                println!("Processed ~{}/{} resources...", processed, resource_count);
            }
        });

    println!(
        "Correct resources: {}",
        total_resources.load(Ordering::Relaxed)
    );

    println!(
        "Failed resources: {}",
        failed_resources.load(Ordering::Relaxed)
    );

    if failed_resources.load(Ordering::Relaxed) > 0 {
        std::process::exit(1);
    }
}
