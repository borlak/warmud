/* structures/defines for random.c */

#define MAX_ADJ  180
#define MAX_MATL 120
#define MAX_WEAPON 50
#define MAX_WEAPON_MOD 36
#define MAX_METAL_ARMOR 8
#define MAX_METAL_ARMOR_TYPE 20
#define MAX_METAL_ARMOR_ADJ 28
#define MAX_LEATHER_ARMOR 11
#define MAX_LEATHER_ARMOR_TYPE 13
#define MAX_LEATHER_ARMOR_ADJ 20

/*
 * structure for material properties
 */
struct matl_data
{
    char	*name;
    int		value;
    int		hardness;
    int		flags;
    int		color;
};



/*
 * structure for adjective data
 */
struct adj_data
{
    char	*adjective;
    int		modifier;
    int		flags;
};

/*
 * structure for weapon modifiers
 */
struct weapon_mod_data
{
    char	*adjective;
    int		hitroll;
    int		damroll;
    int		weight;
};

/*
 * structure for weapons
 */
struct weapon_data
{
    char	*a_an;
    char	*name;
    int		no_dice;
    int		size_dice;
    int		dam_type;
    int		obj_type;
    int		weight;
    int		cost;
    int		rent;
    int		flags;
};


/*
 * struct for armor types
 */
struct metal_armor_type_data
{
    char	*pattern;
    int		modifier;
    int		weight;
    int		wear;
};

/*
 * struct for armor adjectives
 */
struct metal_armor_adj_data
{
    char	*name;
    int		modifier;
    int		weight;
    int		extra;
};

/*
 * struct for metal armors
 */
struct metal_armor_data
{
    char	*name;
    int		armor;
};

/*
 * leather armor structs
 */
struct leather_armor_adj_data
{
    char	*name;
    int		modifier;
};

struct leather_armor_type_data
{
    char	*pattern;
    int		weight;
    int		wear;
};

struct leather_armor_data
{
    char	*name;
    int		armor;
    int		extra;
};


/*
 *    item properties
 */
#define MATL_LEGEND                   1
#define MATL_METAL                    2
#define MATL_MINERAL                  4
#define MATL_ORGANIC                  8
#define MATL_GEMSTONE                16
#define MATL_WOOD                    32
#define MATL_PRECIOUS                64
#define MATL_SEMI_PREC              128
#define MATL_BRITTLE                256
#define MATL_FLAMMABLE              512
#define MATL_ELEMENT               1024
#define MATL_SPARK                 2048
#define MATL_POWDER                4096
#define MATL_LIQUID                8192

/*
 *   colors
 */
#define COLOR_BLACK                   1
#define COLOR_WHITE                   2
#define COLOR_RED                     4
#define COLOR_ORANGE                  8
#define COLOR_YELLOW                 16
#define COLOR_GREEN                  32
#define COLOR_BLUE                   64
#define COLOR_PURPLE                128
#define COLOR_SILVER                256
#define COLOR_GOLD                  512
#define COLOR_BROWN                1024
#define COLOR_LIGHT                2048
#define COLOR_DARK                 4096
#define COLOR_TRANSPARENT          8192
#define COLOR_TRANSLUCENT         16384


/*
 *  adjective bits
 */
#define ADJ_JEWELRY                   1
#define ADJ_POSITIVE                  2
#define ADJ_NEGATIVE                  4
#define ADJ_COLOR                     8
#define ADJ_QUALITY                  16
#define ADJ_SHAPE                    32
#define ADJ_SCROLL                   64
#define ADJ_STAFF                   128

/*
 * Tables for renaming items.
 */
#define TABLE_SCROLL		      0
#define TABLE_JEWELRY		      1
#define TABLE_STAFF		      2


/*
 * Vnums
 */
#define VNUM_ARMOR		9592
#define VNUM_WEAPON		9596
#define VNUM_JEWELRY_START	9596
