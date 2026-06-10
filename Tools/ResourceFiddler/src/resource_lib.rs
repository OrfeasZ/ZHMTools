use std::ffi::{CStr, CString};
use std::os::raw::{c_char, c_void};
use std::ptr::NonNull;
use std::slice;

/// Native FFI types shared across every ResourceLib_* library.
mod sys {
    use super::*;

    #[repr(C)]
    pub struct JsonString {
        pub json_data: *const c_char,
        pub str_size: usize,
    }

    #[repr(C)]
    pub struct ResourceMem {
        pub resource_data: *const c_void,
        pub data_size: usize,
    }

    #[repr(C)]
    pub struct ResourceTypesArray {
        pub types: *mut *const c_char,
        pub type_count: usize,
    }

    #[repr(C)]
    pub struct StringView {
        pub data: *const c_char,
        pub size: usize,
    }

    #[repr(C)]
    pub struct Allocator {
        pub alloc: unsafe extern "C" fn(size: usize, alignment: usize) -> *mut c_void,
        pub free: unsafe extern "C" fn(ptr: *mut c_void),
    }

    #[repr(C)]
    pub struct ResourceConverter {
        pub from_resource_file_to_json_file:
            unsafe extern "C" fn(*const c_char, *const c_char) -> bool,
        pub from_memory_to_json_file:
            unsafe extern "C" fn(*const c_void, usize, *const c_char) -> bool,
        pub from_resource_file_to_json_string:
            unsafe extern "C" fn(*const c_char) -> *mut JsonString,
        pub from_memory_to_json_string:
            unsafe extern "C" fn(*const c_void, usize) -> *mut JsonString,
        pub free_json_string: unsafe extern "C" fn(*mut JsonString),
    }

    #[repr(C)]
    pub struct ResourceGenerator {
        pub from_json_file_to_resource_file:
            unsafe extern "C" fn(*const c_char, *const c_char, bool) -> bool,
        pub from_json_string_to_resource_file:
            unsafe extern "C" fn(*const c_char, usize, *const c_char, bool) -> bool,
        pub from_json_file_to_resource_mem:
            unsafe extern "C" fn(*const c_char, bool) -> *mut ResourceMem,
        pub from_json_string_to_resource_mem:
            unsafe extern "C" fn(*const c_char, usize, bool) -> *mut ResourceMem,
        pub free_resource_mem: unsafe extern "C" fn(*mut ResourceMem),
    }
}

#[derive(Debug, thiserror::Error)]
pub enum ResourceLibError {
    #[error("Null pointer returned from library")]
    NullPointer,
    #[error("Operation returned false")]
    OperationFailed,
    #[error("String contains null byte")]
    InvalidString,
    #[error("UTF-8 conversion error")]
    Utf8Error(#[from] std::str::Utf8Error),
}

type Result<T> = std::result::Result<T, ResourceLibError>;

/// Free functions for one ResourceLib_* library instance.
#[allow(dead_code)]
struct GameApi {
    get_converter_for_resource: unsafe extern "C" fn(*const c_char) -> *mut sys::ResourceConverter,
    get_generator_for_resource: unsafe extern "C" fn(*const c_char) -> *mut sys::ResourceGenerator,
    get_supported_resource_types: unsafe extern "C" fn() -> *mut sys::ResourceTypesArray,
    free_supported_resource_types: unsafe extern "C" fn(*mut sys::ResourceTypesArray),
    is_resource_type_supported: unsafe extern "C" fn(*const c_char) -> bool,
    free_json_string: unsafe extern "C" fn(*mut sys::JsonString),
    get_property_name: unsafe extern "C" fn(u32) -> sys::StringView,
    get_allocator: unsafe extern "C" fn() -> *mut sys::Allocator,
    set_allocator: unsafe extern "C" fn(*mut sys::Allocator),
}

/// Binds one ResourceLib_* dylib: emits its extern block and a `GameApi`
/// static. `$prefix` is the C-symbol prefix (`HM3` etc.) and is pasted
/// onto every entry point.
macro_rules! bind_game {
    ($static:ident, $lib:literal, $prefix:ident) => {
        paste::paste! {
            #[link(name = $lib, kind = "dylib")]
            unsafe extern "C" {
                fn [<$prefix _GetConverterForResource>](resource_type: *const c_char) -> *mut sys::ResourceConverter;
                fn [<$prefix _GetGeneratorForResource>](resource_type: *const c_char) -> *mut sys::ResourceGenerator;
                fn [<$prefix _GetSupportedResourceTypes>]() -> *mut sys::ResourceTypesArray;
                fn [<$prefix _FreeSupportedResourceTypes>](array: *mut sys::ResourceTypesArray);
                fn [<$prefix _IsResourceTypeSupported>](resource_type: *const c_char) -> bool;
                fn [<$prefix _FreeJsonString>](json_string: *mut sys::JsonString);
                fn [<$prefix _GetPropertyName>](property_id: u32) -> sys::StringView;
                fn [<$prefix _GetAllocator>]() -> *mut sys::Allocator;
                fn [<$prefix _SetAllocator>](allocator: *mut sys::Allocator);
            }

            static $static: GameApi = GameApi {
                get_converter_for_resource: [<$prefix _GetConverterForResource>],
                get_generator_for_resource: [<$prefix _GetGeneratorForResource>],
                get_supported_resource_types: [<$prefix _GetSupportedResourceTypes>],
                free_supported_resource_types: [<$prefix _FreeSupportedResourceTypes>],
                is_resource_type_supported: [<$prefix _IsResourceTypeSupported>],
                free_json_string: [<$prefix _FreeJsonString>],
                get_property_name: [<$prefix _GetPropertyName>],
                get_allocator: [<$prefix _GetAllocator>],
                set_allocator: [<$prefix _SetAllocator>],
            };
        }
    };
}

bind_game!(HM2016_API, "ResourceLib_HM2016", HM2016);
bind_game!(HM2_API, "ResourceLib_HM2", HM2);
bind_game!(HM3_API, "ResourceLib_HM3", HM3);
bind_game!(KNT_API, "ResourceLib_KNT", KNT);

#[derive(Copy, Clone, Debug, PartialEq, Eq, Hash)]
pub enum Game {
    Hitman2016,
    Hitman2,
    Hitman3,
    FirstLight007,
}

impl Game {
    fn api(self) -> &'static GameApi {
        match self {
            Game::Hitman2016 => &HM2016_API,
            Game::Hitman2 => &HM2_API,
            Game::Hitman3 => &HM3_API,
            Game::FirstLight007 => &KNT_API,
        }
    }
}

/// Build a CString from a Rust str, mapping interior NULs to InvalidString.
fn to_c(s: &str) -> Result<CString> {
    CString::new(s).map_err(|_| ResourceLibError::InvalidString)
}

/// Turn a C bool return into `Result<()>`, with OperationFailed for `false`.
fn ok(success: bool) -> Result<()> {
    if success {
        Ok(())
    } else {
        Err(ResourceLibError::OperationFailed)
    }
}

/// Wrap a possibly-null pointer in NonNull, mapping null to OperationFailed.
/// (NullPointer is reserved for "lookup returned no entry" callers.)
fn non_null<T>(ptr: *mut T) -> Result<NonNull<T>> {
    NonNull::new(ptr).ok_or(ResourceLibError::OperationFailed)
}

/// A wrapper around a JsonString allocated by the library. The free
/// function pointer travels with the value so the JSON gets released
/// by whichever library allocated it.
pub struct SafeJsonString {
    ptr: NonNull<sys::JsonString>,
    free_fn: unsafe extern "C" fn(*mut sys::JsonString),
}

impl SafeJsonString {
    pub fn as_str(&self) -> Result<&str> {
        unsafe {
            let r = self.ptr.as_ref();
            let slice = slice::from_raw_parts(r.json_data as *const u8, r.str_size);
            std::str::from_utf8(slice).map_err(ResourceLibError::Utf8Error)
        }
    }
}

impl Drop for SafeJsonString {
    fn drop(&mut self) {
        unsafe { (self.free_fn)(self.ptr.as_ptr()) };
    }
}

/// A wrapper around ResourceMem allocated by the library. The free
/// function pointer travels with the value so the memory gets released
/// by whichever library allocated it.
pub struct SafeResourceMem {
    ptr: NonNull<sys::ResourceMem>,
    free_fn: unsafe extern "C" fn(*mut sys::ResourceMem),
}

impl SafeResourceMem {
    pub fn as_bytes(&self) -> &[u8] {
        unsafe {
            let r = self.ptr.as_ref();
            slice::from_raw_parts(r.resource_data as *const u8, r.data_size)
        }
    }
}

impl Drop for SafeResourceMem {
    fn drop(&mut self) {
        unsafe { (self.free_fn)(self.ptr.as_ptr()) };
    }
}

/// A wrapper around SupportedTypesArray allocated by the library. The free
/// function pointer travels with the value so the memory gets released
/// by whichever library allocated it.
pub struct SupportedTypes {
    ptr: NonNull<sys::ResourceTypesArray>,
    free_fn: unsafe extern "C" fn(*mut sys::ResourceTypesArray),
}

impl SupportedTypes {
    pub fn new(game: Game) -> Result<Self> {
        let api = game.api();
        let ptr = unsafe { (api.get_supported_resource_types)() };
        Ok(Self {
            ptr: NonNull::new(ptr).ok_or(ResourceLibError::NullPointer)?,
            free_fn: api.free_supported_resource_types,
        })
    }

    pub fn to_vec(&self) -> Vec<String> {
        unsafe {
            let r = self.ptr.as_ref();
            let raw = slice::from_raw_parts(r.types, r.type_count);
            raw.iter()
                .map(|&p| CStr::from_ptr(p).to_string_lossy().into_owned())
                .collect()
        }
    }
}

impl Drop for SupportedTypes {
    fn drop(&mut self) {
        unsafe { (self.free_fn)(self.ptr.as_ptr()) };
    }
}

pub struct Converter {
    raw: &'static sys::ResourceConverter,
    api: &'static GameApi,
}

impl Converter {
    pub fn get(game: Game, resource_type: &str) -> Result<Self> {
        let api = game.api();
        let c_type = to_c(resource_type)?;
        let ptr = unsafe { (api.get_converter_for_resource)(c_type.as_ptr()) };
        unsafe {
            ptr.as_ref()
                .map(|r| Converter { raw: r, api })
                .ok_or(ResourceLibError::NullPointer)
        }
    }

    pub fn file_to_file(&self, input_path: &str, output_path: &str) -> Result<()> {
        let (c_in, c_out) = (to_c(input_path)?, to_c(output_path)?);
        ok(unsafe { (self.raw.from_resource_file_to_json_file)(c_in.as_ptr(), c_out.as_ptr()) })
    }

    pub fn memory_to_file(&self, data: &[u8], output_path: &str) -> Result<()> {
        let c_out = to_c(output_path)?;
        ok(unsafe {
            (self.raw.from_memory_to_json_file)(
                data.as_ptr() as *const c_void,
                data.len(),
                c_out.as_ptr(),
            )
        })
    }

    pub fn file_to_string(&self, input_path: &str) -> Result<SafeJsonString> {
        let c_in = to_c(input_path)?;
        let ptr = unsafe { (self.raw.from_resource_file_to_json_string)(c_in.as_ptr()) };
        Ok(SafeJsonString {
            ptr: non_null(ptr)?,
            free_fn: self.api.free_json_string,
        })
    }

    pub fn memory_to_string(&self, data: &[u8]) -> Result<SafeJsonString> {
        let ptr = unsafe {
            (self.raw.from_memory_to_json_string)(data.as_ptr() as *const c_void, data.len())
        };
        Ok(SafeJsonString {
            ptr: non_null(ptr)?,
            free_fn: self.api.free_json_string,
        })
    }
}

pub struct Generator {
    raw: &'static sys::ResourceGenerator,
}

impl Generator {
    pub fn get(game: Game, resource_type: &str) -> Result<Self> {
        let c_type = to_c(resource_type)?;
        let ptr = unsafe { (game.api().get_generator_for_resource)(c_type.as_ptr()) };
        unsafe {
            ptr.as_ref()
                .map(|r| Generator { raw: r })
                .ok_or(ResourceLibError::NullPointer)
        }
    }

    pub fn file_to_file(&self, json_path: &str, output_path: &str, compatible: bool) -> Result<()> {
        let (c_in, c_out) = (to_c(json_path)?, to_c(output_path)?);
        ok(unsafe {
            (self.raw.from_json_file_to_resource_file)(c_in.as_ptr(), c_out.as_ptr(), compatible)
        })
    }

    pub fn string_to_file(
        &self,
        json_str: &str,
        output_path: &str,
        compatible: bool,
    ) -> Result<()> {
        let (c_in, c_out) = (to_c(json_str)?, to_c(output_path)?);
        ok(unsafe {
            (self.raw.from_json_string_to_resource_file)(
                c_in.as_ptr(),
                json_str.len(),
                c_out.as_ptr(),
                compatible,
            )
        })
    }

    pub fn file_to_memory(&self, json_path: &str, compatible: bool) -> Result<SafeResourceMem> {
        let c_in = to_c(json_path)?;
        let ptr = unsafe { (self.raw.from_json_file_to_resource_mem)(c_in.as_ptr(), compatible) };
        Ok(SafeResourceMem {
            ptr: non_null(ptr)?,
            free_fn: self.raw.free_resource_mem,
        })
    }

    pub fn string_to_memory(&self, json_str: &str, compatible: bool) -> Result<SafeResourceMem> {
        let c_in = to_c(json_str)?;
        let ptr = unsafe {
            (self.raw.from_json_string_to_resource_mem)(c_in.as_ptr(), json_str.len(), compatible)
        };
        Ok(SafeResourceMem {
            ptr: non_null(ptr)?,
            free_fn: self.raw.free_resource_mem,
        })
    }
}

pub fn is_resource_supported(game: Game, resource_type: &str) -> bool {
    let Ok(c_str) = to_c(resource_type) else {
        return false;
    };
    unsafe { (game.api().is_resource_type_supported)(c_str.as_ptr()) }
}

pub fn get_property_name(game: Game, id: u32) -> Option<String> {
    unsafe {
        let view = (game.api().get_property_name)(id);
        if view.data.is_null() || view.size == 0 {
            return None;
        }
        let slice = slice::from_raw_parts(view.data as *const u8, view.size);
        Some(String::from_utf8_lossy(slice).into_owned())
    }
}
