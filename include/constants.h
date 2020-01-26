#ifndef CONSTANTS_H
#define CONSTANTS_H

/** @file constants.h
 * Macro defining parameters used by several files.
 */

#define PI           3.14159265358979323846f

// Define texture units
static constexpr unsigned int COLOR_TEXTURE_UNIT = 0;
static constexpr unsigned int SKYBOX_TEXTURE_UNIT = 1;
static constexpr unsigned int SHADOW_TEXTURE_UNITS[] = {2, 3, 4};

static constexpr unsigned int NUM_CASCADES = (sizeof(SHADOW_TEXTURE_UNITS)/sizeof(*SHADOW_TEXTURE_UNITS));

#endif // CONSTANTS_H
