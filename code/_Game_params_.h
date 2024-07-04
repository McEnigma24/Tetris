#pragma once

// area size

#define PATH_config "config.txt"

#define HIDDEN_ARRAY_SIZE 4
#define SC_width  400
#define SC_height 1000

#define sq_lenf 25.0f
#define sq_leni 25
#define square \
Vector2f(sq_lenf, sq_lenf)


#define TIME_SLOT_default 300

#define COLOR_RANGE_low  (80)
#define COLOR_RANGE_high (200)
#define COLOR_RANGE_diff (COLOR_RANGE_high - COLOR_RANGE_low)

#define COLOR_background Color{ 0, 0, 0 }
#define COLOR_borders Color{ 255, 255, 255 }
#define COLOR_shadow Color{ 50, 50, 50 }

#define STATS_color Color{ 255, 255, 255 }
#define STATS_pos Vector2f(30, 0)
#define STATS_size 15