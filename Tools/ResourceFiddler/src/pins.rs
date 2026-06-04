use hitman_commons::hash_list::HashList;
use hitman_commons::metadata::RuntimeID;
use rayon::prelude::*;
use rpkg_rs::resource::partition_manager::PartitionManager;
use rpkg_rs::resource::resource_info::ResourceInfo;
use rpkg_rs::resource::resource_partition::ResourcePartition;
use rpkg_rs::resource::runtime_resource_id::RuntimeResourceID;
use std::cell::RefCell;
use std::collections::{HashMap, HashSet};
use std::hash::Hash;
use std::path::Path;
use std::sync::{Arc, Mutex, ReentrantLock};

use crate::resource_lib;
use crate::utils::rrid_to_rid;

fn download_hash_list() -> HashList {
    println!("Downloading hash list...");

    let hash_list_data = reqwest::blocking::get(
        "https://github.com/glacier-modding/Hitman-Hashes/releases/latest/download/hash_list.sml",
    )
    .unwrap()
    .bytes()
    .unwrap();

    println!("Hash list downloaded! Decompressing...");

    HashList::from_compressed(&hash_list_data).unwrap()
}

struct PinConnection {
    from_entity_idx: usize,
    to_entity_idx: usize,
    from_pin_name: String,
    to_pin_name: String,
}

struct PinConnectionInfo {
    to_entity: Arc<ReentrantLock<RefCell<SubEntity>>>,
    from_pin_name: String,
    to_pin_name: String,
}

struct PinReferenceInfo {
    from_entity: Arc<ReentrantLock<RefCell<SubEntity>>>,
    from_pin_name: String,
    to_pin_name: String,
}

struct SubEntity {
    entity_id: u64,
    entity_index: usize,
    /// The resource this entity is from.
    resource_id: RuntimeResourceID,
    /// The resource describing the type of this entity.
    entity_type_id: RuntimeResourceID,
    entity_type: Option<String>,
    pin_connections: Vec<PinConnectionInfo>,
    input_pin_forwardings: Vec<PinConnectionInfo>,
    output_pin_forwardings: Vec<PinConnectionInfo>,
    connected_inputs: Vec<PinReferenceInfo>,
    forwarded_inputs: Vec<PinReferenceInfo>,
    forwarded_outputs: Vec<PinReferenceInfo>,
}

impl SubEntity {
    pub fn module_name(&self) -> Option<String> {
        self.entity_type.as_ref().and_then(|x| {
            if x.starts_with("[modules:/") {
                Some(x.clone())
            } else {
                None
            }
        })
    }
}

/// Read a pin connection from JSON.
fn read_pin_connection(pin: &serde_json::Value) -> PinConnection {
    PinConnection {
        from_entity_idx: pin["fromID"].as_u64().unwrap() as usize,
        to_entity_idx: pin["toID"].as_u64().unwrap() as usize,
        from_pin_name: pin["fromPinName"].as_str().unwrap().to_string(),
        to_pin_name: pin["toPinName"].as_str().unwrap().to_string(),
    }
}

/// Read a sub entity from JSON.
fn read_sub_entity<F>(
    sub_entity: &serde_json::Value,
    entity_index: usize,
    resource: &ResourceInfo,
    game: resource_lib::Game,
    try_get_path: &F,
) -> Arc<ReentrantLock<RefCell<SubEntity>>>
where
    F: Fn(&RuntimeID) -> Option<String>,
{
    let entity_id = sub_entity["entityId"].as_u64().unwrap();
    let entity_type_idx = sub_entity["entityTypeResourceIndex"].as_i64().unwrap();
    let (entity_type_rrid, _) = resource.references().get(entity_type_idx as usize).unwrap();
    let entity_type_name = rrid_to_rid(entity_type_rrid, game).and_then(|rid| try_get_path(&rid));

    Arc::new(ReentrantLock::new(RefCell::new(SubEntity {
        entity_id,
        entity_index,
        resource_id: *resource.rrid(),
        entity_type_id: *entity_type_rrid,
        entity_type: entity_type_name,
        pin_connections: Vec::new(),
        input_pin_forwardings: Vec::new(),
        output_pin_forwardings: Vec::new(),
        connected_inputs: Vec::new(),
        forwarded_inputs: Vec::new(),
        forwarded_outputs: Vec::new(),
    })))
}

struct ArcPtrEq(Arc<ReentrantLock<RefCell<SubEntity>>>);

impl PartialEq for ArcPtrEq {
    fn eq(&self, other: &Self) -> bool {
        Arc::ptr_eq(&self.0, &other.0)
    }
}

impl Eq for ArcPtrEq {}

impl Hash for ArcPtrEq {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        Arc::as_ptr(&self.0).hash(state);
    }
}

/// Checks if an output pin of an entity has an incoming connection.
fn has_incoming_output(
    entity: Arc<ReentrantLock<RefCell<SubEntity>>>,
    pin_name: &str,
    entities: &HashMap<(RuntimeResourceID, u64), HashSet<ArcPtrEq>>,
) -> bool {
    let key = (
        entity.lock().borrow().resource_id,
        entity.lock().borrow().entity_id,
    );

    entities
        .get(&key)
        .map(|x| {
            x.iter().any(|y| {
                let referencing_entity_guard = y.0.lock();
                let referencing_entity = referencing_entity_guard.borrow();

                referencing_entity
                    .forwarded_outputs
                    .iter()
                    .any(|z| Arc::ptr_eq(&z.from_entity, &entity) && z.from_pin_name == pin_name)
            })
        })
        .unwrap_or(false)
}

/// Checks if the input pin of an entity has an incoming connection.
fn has_incoming_input(
    entity: Arc<ReentrantLock<RefCell<SubEntity>>>,
    pin_name: &str,
    entities: &HashMap<(RuntimeResourceID, u64), HashSet<ArcPtrEq>>,
) -> bool {
    let key = (
        entity.lock().borrow().resource_id,
        entity.lock().borrow().entity_id,
    );

    entities
        .get(&key)
        .map(|x| {
            x.iter().any(|y| {
                let referencing_entity_guard = y.0.lock();
                let referencing_entity = referencing_entity_guard.borrow();

                referencing_entity
                    .forwarded_inputs
                    .iter()
                    .any(|z| Arc::ptr_eq(&z.from_entity, &entity) && z.from_pin_name == pin_name)
                    || referencing_entity.connected_inputs.iter().any(|z| {
                        Arc::ptr_eq(&z.from_entity, &entity) && z.from_pin_name == pin_name
                    })
            })
        })
        .unwrap_or(false)
}

pub fn run(
    partition_manager: PartitionManager,
    game: resource_lib::Game,
    crc_output: Option<&Path>,
) {
    let work_items: Vec<_> = partition_manager
        .partitions
        .iter()
        .flat_map(|partition| {
            partition
                .latest_resources()
                .into_iter()
                .filter_map(|(resource, patch_id)| {
                    let resource_type = resource.data_type();
                    if resource_type != "TBLU" {
                        return None;
                    }

                    Some((partition, resource))
                })
                .collect::<Vec<_>>()
        })
        .collect();

    let resource_count = work_items.len();
    print!("Extracting pins from {} resources...\n", resource_count);

    // Skip graph traversal when just outputting pin names.
    if let Some(crc_output) = crc_output {
        dump_pin_crcs(&work_items, game, crc_output);
        return;
    }

    let hash_list_entries = download_hash_list().entries.load_full();

    let try_get_path = |rid: &RuntimeID| {
        hash_list_entries
            .get(rid)
            .and_then(|x| x.path.clone())
            .map(|p| p.to_string())
    };

    // Thread safe map of rid -> input pins.
    let input_pin_map = Arc::new(Mutex::new(HashMap::<String, HashSet<String>>::new()));
    let output_pin_map = Arc::new(Mutex::new(HashMap::<String, HashSet<String>>::new()));

    let add_pin = |map: &Arc<Mutex<HashMap<String, HashSet<String>>>>, module: &str, pin: &str| {
        map.lock()
            .unwrap()
            .entry(module.to_string())
            .or_insert(HashSet::new())
            .insert(pin.to_string());
    };

    let entities = Arc::new(Mutex::new(
        Vec::<Arc<ReentrantLock<RefCell<SubEntity>>>>::new(),
    ));

    let add_entity = |entity: Arc<ReentrantLock<RefCell<SubEntity>>>| {
        entities.lock().unwrap().push(entity);
    };

    /*
    pinConnection (event): from.Output -> to.Input
    inputPinForwarding (inputCopying): from.Input -> to.Input
    outputPinForwarding (outputCopying): from.Output -> to.Output
    */

    // Process resources in parallel.
    work_items.par_iter().for_each(|(partition, resource)| {
        let resource_type = resource.data_type();
        let rid = resource.rrid();

        let resource_data = partition.read_resource(rid).unwrap();
        let converter = resource_lib::Converter::get(game, &resource_type).unwrap();
        let json_data = converter.memory_to_string(&resource_data).unwrap();

        let json: serde_json::Value = serde_json::from_str(json_data.as_str().unwrap()).unwrap();

        let sub_entities: Vec<_> = json["subEntities"]
            .as_array()
            .unwrap()
            .iter()
            .enumerate()
            .map(|(i, x)| read_sub_entity(x, i, resource, game, &try_get_path))
            .collect();

        let pin_connections = json["pinConnections"]
            .as_array()
            .unwrap()
            .iter()
            .map(|x| read_pin_connection(x))
            .collect::<Vec<_>>();

        let input_pin_forwardings = json["inputPinForwardings"]
            .as_array()
            .unwrap()
            .iter()
            .map(|x| read_pin_connection(x))
            .collect::<Vec<_>>();

        let output_pin_forwardings = json["outputPinForwardings"]
            .as_array()
            .unwrap()
            .iter()
            .map(|x| read_pin_connection(x))
            .collect::<Vec<_>>();

        // TODO: External pin connections

        for pin in pin_connections {
            let to_entity = &sub_entities[pin.to_entity_idx];

            {
                let to_entity_guard = to_entity.lock();
                let mut to_entity = to_entity_guard.borrow_mut();
                to_entity.connected_inputs.push(PinReferenceInfo {
                    from_entity: sub_entities[pin.from_entity_idx].clone(),
                    from_pin_name: pin.from_pin_name.clone(),
                    to_pin_name: pin.to_pin_name.clone(),
                });
            }

            sub_entities[pin.from_entity_idx]
                .lock()
                .borrow_mut()
                .pin_connections
                .push(PinConnectionInfo {
                    to_entity: to_entity.clone(),
                    from_pin_name: pin.from_pin_name.clone(),
                    to_pin_name: pin.to_pin_name.clone(),
                });
        }

        for pin in input_pin_forwardings {
            let to_entity = &sub_entities[pin.to_entity_idx];

            {
                let to_entity_guard = to_entity.lock();
                let mut to_entity = to_entity_guard.borrow_mut();
                to_entity.forwarded_inputs.push(PinReferenceInfo {
                    from_entity: sub_entities[pin.from_entity_idx].clone(),
                    from_pin_name: pin.from_pin_name.clone(),
                    to_pin_name: pin.to_pin_name.clone(),
                });
            }

            sub_entities[pin.from_entity_idx]
                .lock()
                .borrow_mut()
                .input_pin_forwardings
                .push(PinConnectionInfo {
                    to_entity: to_entity.clone(),
                    from_pin_name: pin.from_pin_name.clone(),
                    to_pin_name: pin.to_pin_name.clone(),
                });
        }

        for pin in output_pin_forwardings {
            let to_entity = &sub_entities[pin.to_entity_idx];

            {
                let to_entity_guard = to_entity.lock();
                let mut to_entity = to_entity_guard.borrow_mut();
                to_entity.forwarded_outputs.push(PinReferenceInfo {
                    from_entity: sub_entities[pin.from_entity_idx].clone(),
                    from_pin_name: pin.from_pin_name.clone(),
                    to_pin_name: pin.to_pin_name.clone(),
                });
            }

            sub_entities[pin.from_entity_idx]
                .lock()
                .borrow_mut()
                .output_pin_forwardings
                .push(PinConnectionInfo {
                    to_entity: to_entity.clone(),
                    from_pin_name: pin.from_pin_name.clone(),
                    to_pin_name: pin.to_pin_name.clone(),
                });
        }

        for entity in sub_entities {
            add_entity(entity);
        }
    });

    {
        let entities = entities.lock().unwrap();
        println!("Collected {} sub-entities.", entities.len());
    }

    // Create a map of reverse entity references.
    // The key is (rrid, entityid) and the value a list of entities that reference it.
    let mut reference_map = HashMap::new();

    {
        let entities = entities.lock().unwrap();

        for entity_arc in entities.iter() {
            let entity_guard = entity_arc.lock();
            let entity = entity_guard.borrow();

            for pin in &entity.connected_inputs {
                reference_map
                    .entry((
                        pin.from_entity.lock().borrow().resource_id,
                        pin.from_entity.lock().borrow().entity_id,
                    ))
                    .or_insert(HashSet::new())
                    .insert(ArcPtrEq(entity_arc.clone()));
            }

            for pin in &entity.forwarded_inputs {
                reference_map
                    .entry((
                        pin.from_entity.lock().borrow().resource_id,
                        pin.from_entity.lock().borrow().entity_id,
                    ))
                    .or_insert(HashSet::new())
                    .insert(ArcPtrEq(entity_arc.clone()));
            }

            for pin in &entity.forwarded_outputs {
                reference_map
                    .entry((
                        pin.from_entity.lock().borrow().resource_id,
                        pin.from_entity.lock().borrow().entity_id,
                    ))
                    .or_insert(HashSet::new())
                    .insert(ArcPtrEq(entity_arc.clone()));
            }
        }
    }

    println!("Reference map created. Collecting pins...");

    {
        // Take a snapshot of all entities to avoid holding the lock.
        let entities = entities.lock().unwrap();

        for entity_arc in entities.iter() {
            let entity_guard = entity_arc.lock();
            let entity = entity_guard.borrow();

            for pin in &entity.pin_connections {
                if let Some(module_name) = entity.module_name()
                    && !has_incoming_output(entity_arc.clone(), &pin.from_pin_name, &reference_map)
                {
                    add_pin(&output_pin_map, &module_name, &pin.from_pin_name);
                }

                if let Some(module_name) = pin.to_entity.lock().borrow().module_name()
                    && !has_incoming_input(pin.to_entity.clone(), &pin.to_pin_name, &reference_map)
                {
                    add_pin(&input_pin_map, &module_name, &pin.to_pin_name);
                }
            }

            for pin in &entity.input_pin_forwardings {
                if let Some(module_name) = entity.module_name()
                    && !has_incoming_input(entity_arc.clone(), &pin.from_pin_name, &reference_map)
                {
                    add_pin(&input_pin_map, &module_name, &pin.from_pin_name);
                }

                if let Some(module_name) = pin.to_entity.lock().borrow().module_name()
                    && !has_incoming_input(pin.to_entity.clone(), &pin.to_pin_name, &reference_map)
                {
                    add_pin(&input_pin_map, &module_name, &pin.to_pin_name);
                }
            }

            for pin in &entity.output_pin_forwardings {
                if let Some(module_name) = entity.module_name()
                    && !has_incoming_output(entity_arc.clone(), &pin.from_pin_name, &reference_map)
                {
                    add_pin(&output_pin_map, &module_name, &pin.from_pin_name);
                }

                if let Some(module_name) = pin.to_entity.lock().borrow().module_name()
                    && !has_incoming_output(pin.to_entity.clone(), &pin.to_pin_name, &reference_map)
                {
                    add_pin(&output_pin_map, &module_name, &pin.to_pin_name);
                }
            }
        }
    }

    let input_pin_map = input_pin_map.lock().unwrap();
    let output_pin_map = output_pin_map.lock().unwrap();

    println!("Input pins:");

    for (module, pins) in input_pin_map.iter() {
        println!("{}: {:?}", module, pins);
    }

    println!();
    println!("Output pins:");

    for (module, pins) in output_pin_map.iter() {
        println!("{}: {:?}", module, pins);
    }
}

fn dump_pin_crcs(
    work_items: &[(&ResourcePartition, &ResourceInfo)],
    game: resource_lib::Game,
    crc_output: &Path,
) {
    let pin_names = Mutex::new(HashSet::<String>::new());

    work_items.par_iter().for_each(|(partition, resource)| {
        let resource_type = resource.data_type();
        let resource_data = partition.read_resource(resource.rrid()).unwrap();
        let converter = resource_lib::Converter::get(game, &resource_type).unwrap();
        let json_data = converter.memory_to_string(&resource_data).unwrap();
        let json: serde_json::Value = serde_json::from_str(json_data.as_str().unwrap()).unwrap();

        let mut local = HashSet::new();

        for key in ["pinConnections", "inputPinForwardings", "outputPinForwardings", "pinConnectionOverrides", "pinConnectionOverrideDeletes"] {
            for pin in json[key].as_array().unwrap() {
                local.insert(pin["fromPinName"].as_str().unwrap().to_string());
                local.insert(pin["toPinName"].as_str().unwrap().to_string());
            }
        }

        pin_names.lock().unwrap().extend(local);
    });

    let pin_names = pin_names.into_inner().unwrap();
    let pin_count = pin_names.len();

    let crc_map: serde_json::Map<String, serde_json::Value> = pin_names
        .into_iter()
        .map(|pin| {
            let crc = crc32fast::hash(pin.as_bytes());
            (pin, crc.into())
        })
        .collect();

    let json = serde_json::to_string_pretty(&serde_json::Value::Object(crc_map)).unwrap();
    std::fs::write(crc_output, json).unwrap_or_else(|e| {
        eprintln!("failed to write {}: {}", crc_output.display(), e);
        std::process::exit(1);
    });

    println!(
        "Wrote {} unique pin name CRC32s to {}",
        pin_count,
        crc_output.display()
    );
}
