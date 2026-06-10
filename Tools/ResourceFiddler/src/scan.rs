use rayon::prelude::*;
use rpkg_rs::resource::partition_manager::PartitionManager;
use rpkg_rs::resource::runtime_resource_id::RuntimeResourceID;
use std::sync::Mutex;

use crate::resource_lib;

/// Scan all supported resources for ones whose BIN1 bytes contain a given string.
pub fn run(partition_manager: &PartitionManager, game: resource_lib::Game, needle: &str) {
    let needle_bytes = needle.as_bytes();

    let work_items: Vec<_> = partition_manager
        .partitions
        .iter()
        .flat_map(|partition| {
            partition
                .latest_resources()
                .into_iter()
                .filter_map(move |(resource, _)| {
                    let resource_type = resource.data_type();
                    if !resource_lib::is_resource_supported(game, &resource_type) {
                        return None;
                    }
                    Some((partition, resource, resource_type))
                })
                .collect::<Vec<_>>()
        })
        .collect();

    println!(
        "Scanning {} resources for {:?}...",
        work_items.len(),
        needle
    );

    let matches: Mutex<Vec<(String, RuntimeResourceID, usize, usize)>> = Mutex::new(Vec::new());

    work_items
        .par_iter()
        .for_each(|(partition, resource, resource_type)| {
            let rid = resource.rrid();
            let Ok(data) = partition.read_resource(rid) else {
                return;
            };
            let bytes: &[u8] = &data;
            if let Some(offset) = bytes
                .windows(needle_bytes.len())
                .position(|w| w == needle_bytes)
            {
                matches.lock().unwrap().push((
                    resource_type.to_string(),
                    *rid,
                    bytes.len(),
                    offset,
                ));
            }
        });

    let matches = matches.into_inner().unwrap();
    println!(
        "Found {} resources containing {:?} in their BIN1 bytes",
        matches.len(),
        needle
    );
    for (rt, rid, size, offset) in matches.iter() {
        println!(
            "  {} {} ({} bytes, {:?} @ 0x{:x})",
            rid, rt, size, needle, offset
        );
    }
}
