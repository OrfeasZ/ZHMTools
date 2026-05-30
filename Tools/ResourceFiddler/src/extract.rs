use rpkg_rs::resource::partition_manager::PartitionManager;
use rpkg_rs::resource::runtime_resource_id::RuntimeResourceID;

use crate::resource_lib;

pub fn run(partition_manager: PartitionManager, game: resource_lib::Game, rrid: RuntimeResourceID) {
    let Some((partition, info)) = partition_manager
        .partitions
        .iter()
        .find_map(|p| p.get_resource_info(&rrid).ok().map(|i| (p, i)))
    else {
        eprintln!("Resource {} not found in any mounted partition.", rrid);
        std::process::exit(1);
    };

    let resource_type = info.data_type();
    if !resource_lib::is_resource_supported(game, &resource_type) {
        eprintln!(
            "Resource type {} is not supported by this ResourceLib.",
            resource_type
        );
        std::process::exit(1);
    }

    let data = partition.read_resource(&rrid).unwrap_or_else(|e| {
        eprintln!("Failed to read resource {}: {}", rrid, e);
        std::process::exit(1);
    });

    let converter = resource_lib::Converter::get(game, &resource_type).unwrap_or_else(|e| {
        eprintln!("Failed to get converter for {}: {}", resource_type, e);
        std::process::exit(1);
    });

    let json = converter.memory_to_string(&data).unwrap_or_else(|e| {
        eprintln!("Failed to convert resource to JSON: {}", e);
        std::process::exit(1);
    });

    println!("{}", json.as_str().unwrap());
}
