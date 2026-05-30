use rpkg_rs::resource::partition_manager::PartitionManager;
use rpkg_rs::resource::resource_info::ResourceInfo;
use std::collections::HashMap;

/// Print every unique resource type (TEMP, TBLU, etc.) the mounted game
/// contains, along with how many resources of each there are and whether the
/// first resource of that type starts with the BIN1 magic.
pub fn run(partition_manager: &PartitionManager) {
    // For each type: (count, optional pointer to a sample resource for the BIN1 probe).
    struct Entry<'a> {
        count: usize,
        sample: Option<(
            &'a rpkg_rs::resource::resource_partition::ResourcePartition,
            &'a ResourceInfo,
        )>,
    }
    let mut entries: HashMap<String, Entry> = HashMap::new();

    for partition in &partition_manager.partitions {
        for (resource, _) in partition.latest_resources() {
            let entry = entries.entry(resource.data_type()).or_insert(Entry {
                count: 0,
                sample: None,
            });
            entry.count += 1;
            if entry.sample.is_none() {
                entry.sample = Some((partition, resource));
            }
        }
    }

    let mut rows: Vec<(String, usize, Option<bool>)> = entries
        .into_iter()
        .map(|(rt, entry)| {
            let is_bin1 = entry.sample.and_then(|(partition, resource)| {
                partition
                    .read_resource(resource.rrid())
                    .ok()
                    .map(|data| data.len() >= 4 && &data[..4] == b"BIN1")
            });
            (rt, entry.count, is_bin1)
        })
        .collect();
    rows.sort_by(|a, b| b.1.cmp(&a.1).then_with(|| a.0.cmp(&b.0)));

    println!("Found {} unique resource types:", rows.len());
    for (rt, count, is_bin1) in &rows {
        let tag = match is_bin1 {
            Some(true) => "(is BIN1)",
            Some(false) => "",
            None => "",
        };
        println!("  {} {} {}", rt, count, tag);
    }
}
