#pragma once

#include <type_traits>

template <class T>
struct ZHMTypeSupportsEquality : std::true_type {};

template <class T>
constexpr inline bool ZHMTypeSupportsEquality_v = ZHMTypeSupportsEquality<T>::value;

// Define any structures that we don't want to deduplicate below.
class SEntityTemplateReference;
template <> struct ZHMTypeSupportsEquality<SEntityTemplateReference> : std::false_type {};