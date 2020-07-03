
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Basic Engine includes
typedef struct resource* resource_t;
typedef struct renderer* renderer_t;

#include <utils/maple_types.h>

#include "mm/free_list_allocator.h"
#include "mm/tagged_heap.h"
#include "mm/pool_allocator.h"
#include "mm/mm.h"

#define MAPLE_VECTOR_MATH_IMPLEMENTATION
#include "utils/mstring.h"
#include "utils/vector_math.h"
#include "utils/hash_functions.h"

#include <platform/platform.h>
#include <graphics/resources.h>
#include <assets/asset.h>
#include <ui/dev_ui.h>
#include <graphics/render_command.h>
#include <frame_info/frame_params.h>

// Game Code
#define MAX_FLOAT 3.402823466e38
#define DegreesToRadian(d) (((d) * PI) / 180.0f)


#include "camera/camera.h"
#include "src/ray.h"
#include "src/intersections.h"
#include "src/material.h"

#include "src/intersections.cpp"
#include "src/material.cpp"
#include "src/ray.cpp"
#include "src/maple_raytracer.cpp"
