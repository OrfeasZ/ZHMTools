use hitman_commons::metadata::RuntimeID;
use rpkg_rs::GlacierGame;
use rpkg_rs::resource::partition_manager::PartitionManager;
use rpkg_rs::resource::pdefs::bond_parser::BondParser;
use rpkg_rs::resource::pdefs::h2016_parser::H2016Parser;
use rpkg_rs::resource::pdefs::hm2_parser::HM2Parser;
use rpkg_rs::resource::pdefs::hm3_parser::HM3Parser;
use rpkg_rs::resource::pdefs::{GamePaths, PackageDefinitionParser, PackageDefinitionSource};
use rpkg_rs::resource::resource_partition::PatchId;
use rpkg_rs::resource::runtime_resource_id::RuntimeResourceID;
use std::path::PathBuf;

pub fn fmt_patch_id(patch_id: &PatchId) -> String {
    match patch_id {
        PatchId::Base => "base".to_string(),
        PatchId::Patch(id) => format!("patch {}", id),
    }
}

pub fn pretty_print_json(json: &str) -> String {
    match serde_json::from_str::<serde_json::Value>(json) {
        Ok(value) => serde_json::to_string_pretty(&value).unwrap_or_else(|_| json.to_string()),
        Err(_) => json.to_string(),
    }
}

pub fn print_json_diff(before: &str, after: &str) {
    let before_pretty = pretty_print_json(before);
    let after_pretty = pretty_print_json(after);

    let diff = similar::TextDiff::from_lines(&before_pretty, &after_pretty);
    for (idx, group) in diff.grouped_ops(3).iter().enumerate() {
        if idx > 0 {
            eprintln!("...");
        }
        for op in group {
            for change in diff.iter_changes(op) {
                let (sign, old_idx, new_idx) = match change.tag() {
                    similar::ChangeTag::Delete => ("-", change.old_index(), None),
                    similar::ChangeTag::Insert => ("+", None, change.new_index()),
                    similar::ChangeTag::Equal => (" ", change.old_index(), change.new_index()),
                };
                eprint!(
                    "{:>5} {:>5} {} {}",
                    old_idx.map(|i| (i + 1).to_string()).unwrap_or_default(),
                    new_idx.map(|i| (i + 1).to_string()).unwrap_or_default(),
                    sign,
                    change,
                );
            }
        }
    }
}

/// Convert a rpkg-rs RuntimeResourceID to a hitman_commons RuntimeID.
pub fn rrid_to_rid(rrid: &RuntimeResourceID) -> RuntimeID {
    let from_ent_type_rrid_u64: u64 = (*rrid).into();
    let from_ent_type_rid: RuntimeID = from_ent_type_rrid_u64.try_into().unwrap();
    from_ent_type_rid
}

pub fn mount_game(retail_path: PathBuf, game: GlacierGame) -> PartitionManager {
    let paths = match game {
        // Bond has no thumbs.dat; create paths manually.
        GlacierGame::Bond => {
            let runtime_path = retail_path
                .parent()
                .unwrap_or_else(|| {
                    eprintln!(
                        "retail path {} has no parent directory",
                        retail_path.display()
                    );
                    std::process::exit(1);
                })
                .join("Runtime");
            GamePaths {
                project_path: retail_path.clone(),
                package_definition_path: runtime_path.join("packagedefinition.txt"),
                runtime_path,
            }
        }
        _ => GamePaths::from_retail_directory(retail_path).unwrap_or_else(|e| {
            eprintln!("failed to discover game paths: {}", e);
            std::process::exit(1);
        }),
    };

    let pdef_bytes = std::fs::read(&paths.package_definition_path).unwrap_or_else(|e| {
        eprintln!(
            "failed to read {}: {}",
            paths.package_definition_path.display(),
            e
        );
        std::process::exit(1);
    });

    let parse_result = match game {
        GlacierGame::HM2016 => H2016Parser::parse(&pdef_bytes),
        GlacierGame::HM2 => HM2Parser::parse(&pdef_bytes),
        GlacierGame::HM3 => HM3Parser::parse(&pdef_bytes),
        GlacierGame::Bond => BondParser::parse(&pdef_bytes),
        _ => {
            eprintln!("unsupported game: {:?}", game);
            std::process::exit(1);
        }
    };

    let partitions = parse_result.unwrap_or_else(|e| {
        eprintln!("failed to parse packagedefinition.txt: {}", e);
        std::process::exit(1);
    });

    let mut manager = PartitionManager::new(
        paths.runtime_path,
        &PackageDefinitionSource::Custom(partitions),
    )
    .unwrap_or_else(|e| {
        eprintln!("failed to create partition manager: {}", e);
        std::process::exit(1);
    });

    manager.mount_partitions(|_, _| {}).unwrap_or_else(|e| {
        eprintln!("failed to mount partitions: {}", e);
        std::process::exit(1);
    });

    manager
}
