use rayon::prelude::*;
use rpkg_rs::resource::partition_manager::PartitionManager;
use std::collections::HashSet;
use std::path::Path;
use std::sync::Mutex;

use crate::resource_lib;

pub fn run(
    partition_manager: PartitionManager,
    game: resource_lib::Game,
    properties_path: &Path,
    custom_properties_path: &Path,
    output_path: &Path,
    user_properties: &[String],
) {
    // Property names gathered from CodeGen.
    let known_properties = read_property_names(properties_path);

    println!(
        "Loaded {} known property names from {}.",
        known_properties.len(),
        properties_path.display()
    );

    // User-specified properties, only added if they're not known already.
    let user_properties: Vec<String> = user_properties
        .iter()
        .map(|name| name.trim().to_string())
        .filter(|name| !name.is_empty() && !known_properties.contains(name))
        .collect();

    if !user_properties.is_empty() {
        println!(
            "Including {} user-specified property names.",
            user_properties.len()
        );
    }

    let work_items: Vec<_> = partition_manager
        .partitions
        .iter()
        .flat_map(|partition| {
            partition
                .latest_resources()
                .into_iter()
                .filter_map(|(resource, _)| {
                    if resource.data_type() != "TBLU" {
                        return None;
                    }

                    Some((partition, resource))
                })
                .collect::<Vec<_>>()
        })
        .collect();

    let resource_count = work_items.len();
    println!(
        "Extracting property aliases from {} resources...",
        resource_count
    );

    let property_aliases = Mutex::new(HashSet::<(String, String)>::new());

    // Process resources in parallel.
    work_items.par_iter().for_each(|(partition, resource)| {
        let resource_type = resource.data_type();
        let rid = resource.rrid();

        let resource_data = partition.read_resource(rid).unwrap();
        let converter = resource_lib::Converter::get(game, &resource_type).unwrap();
        let json_data = converter.memory_to_string(&resource_data).unwrap();

        let json: serde_json::Value = serde_json::from_str(json_data.as_str().unwrap()).unwrap();

        let local: Vec<(String, String)> = {
            let mut seen = HashSet::new();

            json["subEntities"]
                .as_array()
                .unwrap()
                .iter()
                .filter_map(|x| x["propertyAliases"].as_array())
                .flatten()
                .map(|alias| {
                    (
                        alias["sAliasName"].as_str().unwrap().to_string(),
                        alias["sPropertyName"].as_str().unwrap().to_string(),
                    )
                })
                .filter(|pair| seen.insert(pair.clone()))
                .collect()
        };

        property_aliases.lock().unwrap().extend(local);
    });

    let mut property_aliases: Vec<_> = property_aliases.into_inner().unwrap().into_iter().collect();
    property_aliases.sort();

    println!(
        "Collected {} unique property aliases.",
        property_aliases.len()
    );

    // Custom property names we have already named, one per line.
    let custom_properties = read_property_names(custom_properties_path);

    println!(
        "Loaded {} custom property names from {}.",
        custom_properties.len(),
        custom_properties_path.display()
    );

    // Combine the existing custom property names with the ones gathered from aliases.
    // Aliases can carry an empty property name (somehow...?), these are filtered out.
    let all_custom_properties: HashSet<String> = custom_properties
        .into_iter()
        .chain(property_aliases.iter().flat_map(|(alias_name, property_name)| {
            [alias_name.clone(), property_name.clone()]
        }))
        .chain(user_properties)
        .map(|name| name.trim().to_string())
        .filter(|name| !name.is_empty())
        .collect();

    // Right now, we don't filter custom properties for those already in the generated properties file. This is in case they're removed in a future update.
    //let mut all_custom_properties: Vec<_> = all_custom_properties.into_iter().filter(|prop| !known_properties.contains(prop)).collect();
    let mut all_custom_properties: Vec<_> = all_custom_properties.into_iter().collect();
    all_custom_properties.sort();

    // Match the line endings of the output file if it exists.
    let line_ending = match std::fs::read_to_string(output_path) {
        Ok(existing) if existing.contains("\r\n") => "\r\n",
        _ => "\n",
    };

    let output = all_custom_properties.join(line_ending);

    std::fs::write(output_path, output).unwrap_or_else(|e| {
        eprintln!("failed to write {}: {}", output_path.display(), e);
        std::process::exit(1);
    });

    println!(
        "\nWrote {} custom property names to {}.\n!!! Don't forget to run the bundler!",
        all_custom_properties.len(),
        output_path.display()
    );
}

fn read_property_names(path: &Path) -> HashSet<String> {
    std::fs::read_to_string(path)
        .unwrap_or_else(|e| {
            eprintln!("failed to read {}: {}", path.display(), e);
            std::process::exit(1);
        })
        .lines()
        .map(|line| line.trim())
        .filter(|line| !line.is_empty())
        .map(|line| line.to_string())
        .collect()
}
