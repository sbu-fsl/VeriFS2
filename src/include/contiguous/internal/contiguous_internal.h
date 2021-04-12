#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <memory>
#include <type_traits>
#include <algorithm>

/**
    @file contiguous_internal.h
    @brief Includes all internal components in the right order.
*/

#include "contiguous_settings.h"

#include "adl_swap.h"

#include "wrapped_pair.h"

#include "is_transparent.h"

#include "wrapped_pair_compare.h"

#include "iterators.h"

#include "container_traits.h"