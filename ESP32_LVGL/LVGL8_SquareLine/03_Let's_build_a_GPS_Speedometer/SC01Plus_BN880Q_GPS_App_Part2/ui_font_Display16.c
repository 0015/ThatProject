/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --font /Users/sil0/Desktop/_SquareLIneProjects/assets/Open 24 Display St.ttf -o /Users/sil0/Desktop/_SquareLIneProjects/assets/ui_font_Display16.c --format lvgl -r 0x20-0x7f --no-compress --no-prefilter
 ******************************************************************************/

#include "ui.h"

#ifndef UI_FONT_DISPLAY16
#define UI_FONT_DISPLAY16 1
#endif

#if UI_FONT_DISPLAY16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0x60,

    /* U+0022 "\"" */
    0xf0,

    /* U+0023 "#" */
    0x3, 0x8, 0x4, 0x30, 0x18, 0xc1, 0xaf, 0x70,
    0xc2, 0x1, 0x8, 0x37, 0xfe, 0x8, 0x40, 0x31,
    0x80, 0x42, 0x0,

    /* U+0024 "$" */
    0x20, 0x8f, 0xa8, 0xa2, 0x88, 0x3e, 0x24, 0x92,
    0x41, 0x78, 0x82, 0x0,

    /* U+0025 "%" */
    0xc7, 0x34, 0x86, 0x10, 0xc2, 0x0, 0x4b, 0x38,
    0x40,

    /* U+0026 "&" */
    0x70, 0x24, 0x12, 0x7, 0x0, 0x3, 0xc2, 0x11,
    0xf, 0x84, 0x43, 0xde, 0x0,

    /* U+0027 "'" */
    0xc0,

    /* U+0028 "(" */
    0xf2, 0x49, 0x4, 0x93, 0x80,

    /* U+0029 ")" */
    0xc4, 0x92, 0x41, 0x27, 0x0,

    /* U+002A "*" */
    0x27, 0xbe, 0xe2, 0x0,

    /* U+002B "+" */
    0x21, 0x9, 0xf2, 0x10, 0x80,

    /* U+002C "," */
    0x38,

    /* U+002D "-" */
    0xf8,

    /* U+002E "." */
    0x20,

    /* U+002F "/" */
    0x4, 0x20, 0x84, 0x10, 0x86, 0x10, 0x42, 0x8,
    0x0,

    /* U+0030 "0" */
    0xfe, 0x18, 0x65, 0x94, 0xca, 0x69, 0x86, 0x17,
    0x80,

    /* U+0031 "1" */
    0x17, 0xd1, 0x10, 0x11, 0x11, 0x0,

    /* U+0032 "2" */
    0x7c, 0x10, 0x41, 0x7, 0xe8, 0x20, 0x82, 0x7,
    0x80,

    /* U+0033 "3" */
    0xf8, 0x42, 0x10, 0xfc, 0x21, 0x8, 0x7c,

    /* U+0034 "4" */
    0x86, 0x18, 0x61, 0x85, 0xf0, 0x41, 0x4, 0x10,
    0x40,

    /* U+0035 "5" */
    0xfa, 0x8, 0x20, 0x83, 0xe0, 0x41, 0x4, 0x17,
    0x80,

    /* U+0036 "6" */
    0xfa, 0x8, 0x20, 0x83, 0xe8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0037 "7" */
    0xf8, 0x42, 0x10, 0x80, 0x21, 0x8, 0x42,

    /* U+0038 "8" */
    0xfe, 0x18, 0x61, 0x87, 0xe8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0039 "9" */
    0xfe, 0x18, 0x61, 0x85, 0xf0, 0x41, 0x4, 0x17,
    0x80,

    /* U+003A ":" */
    0xc0, 0x0, 0x80,

    /* U+003B ";" */
    0xc0, 0x0, 0xe0,

    /* U+003C "<" */
    0x17, 0xcc, 0x71,

    /* U+003D "=" */
    0xf8, 0x1, 0xf0,

    /* U+003E ">" */
    0x86, 0x13, 0x68,

    /* U+003F "?" */
    0xfc, 0x10, 0x41, 0x7, 0xf0, 0x20, 0x80, 0x8,
    0x0,

    /* U+0040 "@" */
    0xfe, 0x18, 0x69, 0x94, 0x8b, 0x69, 0x86, 0x17,
    0x80,

    /* U+0041 "A" */
    0xfe, 0x18, 0x61, 0x87, 0xe8, 0x61, 0x86, 0x18,
    0x40,

    /* U+0042 "B" */
    0xfe, 0x18, 0x61, 0x87, 0xe8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0043 "C" */
    0xfc, 0x21, 0x8, 0x2, 0x10, 0x84, 0x1e,

    /* U+0044 "D" */
    0xfe, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0045 "E" */
    0xfc, 0x21, 0x8, 0x7e, 0x10, 0x84, 0x1e,

    /* U+0046 "F" */
    0xfc, 0x21, 0x8, 0x7e, 0x10, 0x84, 0x20,

    /* U+0047 "G" */
    0xfa, 0x8, 0x20, 0x80, 0x68, 0x61, 0x86, 0x17,
    0x80,

    /* U+0048 "H" */
    0x86, 0x18, 0x61, 0x87, 0xe8, 0x61, 0x86, 0x18,
    0x40,

    /* U+0049 "I" */
    0xfb, 0xe0,

    /* U+004A "J" */
    0x0, 0x10, 0x41, 0x4, 0x8, 0x61, 0x86, 0x17,
    0x80,

    /* U+004B "K" */
    0x82, 0x18, 0xa4, 0xb3, 0xe8, 0x61, 0x86, 0x18,
    0x40,

    /* U+004C "L" */
    0x4, 0x21, 0x8, 0x2, 0x10, 0x84, 0x1e,

    /* U+004D "M" */
    0xff, 0xf8, 0x41, 0x84, 0x18, 0x41, 0x84, 0x10,
    0x0, 0x84, 0x18, 0x41, 0x84, 0x18, 0x41, 0x84,
    0x0,

    /* U+004E "N" */
    0x87, 0x1e, 0x6d, 0x9c, 0x8, 0x61, 0x86, 0x18,
    0x40,

    /* U+004F "O" */
    0xfe, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0050 "P" */
    0xfe, 0x18, 0x61, 0x87, 0xe8, 0x20, 0x82, 0x8,
    0x0,

    /* U+0051 "Q" */
    0xfe, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0xd7,
    0x8c, 0x0,

    /* U+0052 "R" */
    0xfe, 0x18, 0x61, 0x87, 0xe9, 0xa2, 0x8e, 0x10,
    0x0,

    /* U+0053 "S" */
    0xfa, 0x8, 0x20, 0x83, 0xe0, 0x41, 0x4, 0x17,
    0x80,

    /* U+0054 "T" */
    0x7c, 0x1, 0x4, 0x10, 0x40, 0x4, 0x10, 0x41,
    0x0,

    /* U+0055 "U" */
    0x2, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0x17,
    0xc0,

    /* U+0056 "V" */
    0x86, 0x18, 0x61, 0x86, 0x10, 0x33, 0x49, 0xe3,
    0x0,

    /* U+0057 "W" */
    0x84, 0x30, 0x86, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x40, 0x6, 0x23, 0x4a, 0x4f, 0x78, 0xc6, 0x0,

    /* U+0058 "X" */
    0x84, 0x99, 0xe1, 0x80, 0xf, 0x80, 0x18, 0x78,
    0x9b, 0x10,

    /* U+0059 "Y" */
    0x86, 0x10, 0x33, 0x49, 0xe3, 0x0, 0x10, 0x40,

    /* U+005A "Z" */
    0xf8, 0x10, 0xc6, 0x10, 0x2, 0x18, 0x41, 0x7,
    0xc0,

    /* U+005B "[" */
    0xf2, 0x49, 0x4, 0x93, 0x80,

    /* U+005C "\\" */
    0x81, 0x4, 0x8, 0x20, 0x41, 0x82, 0x8, 0x10,
    0x40,

    /* U+005D "]" */
    0xc4, 0x92, 0x41, 0x27, 0x0,

    /* U+005E "^" */
    0x0, 0x51, 0xb2, 0x2c, 0x60,

    /* U+005F "_" */
    0xf8,

    /* U+0060 "`" */
    0xd4,

    /* U+0061 "a" */
    0xfe, 0x18, 0x61, 0x87, 0xe8, 0x61, 0x86, 0x18,
    0x40,

    /* U+0062 "b" */
    0xfe, 0x18, 0x61, 0x87, 0xe8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0063 "c" */
    0xfc, 0x21, 0x8, 0x2, 0x10, 0x84, 0x1e,

    /* U+0064 "d" */
    0xfe, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0065 "e" */
    0xfc, 0x21, 0x8, 0x7e, 0x10, 0x84, 0x1e,

    /* U+0066 "f" */
    0xfc, 0x21, 0x8, 0x7e, 0x10, 0x84, 0x20,

    /* U+0067 "g" */
    0xfa, 0x8, 0x20, 0x80, 0x68, 0x61, 0x86, 0x17,
    0x80,

    /* U+0068 "h" */
    0x86, 0x18, 0x61, 0x87, 0xe8, 0x61, 0x86, 0x18,
    0x40,

    /* U+0069 "i" */
    0xfb, 0xe0,

    /* U+006A "j" */
    0x0, 0x10, 0x41, 0x4, 0x8, 0x61, 0x86, 0x17,
    0x80,

    /* U+006B "k" */
    0x82, 0x18, 0xa4, 0xb3, 0xe8, 0x61, 0x86, 0x18,
    0x40,

    /* U+006C "l" */
    0x4, 0x21, 0x8, 0x2, 0x10, 0x84, 0x1e,

    /* U+006D "m" */
    0xff, 0xf8, 0x41, 0x84, 0x18, 0x41, 0x84, 0x10,
    0x0, 0x84, 0x18, 0x41, 0x84, 0x18, 0x41, 0x84,
    0x0,

    /* U+006E "n" */
    0x87, 0x1e, 0x6d, 0x9c, 0x8, 0x61, 0x86, 0x18,
    0x40,

    /* U+006F "o" */
    0xfe, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0x17,
    0x80,

    /* U+0070 "p" */
    0xfe, 0x18, 0x61, 0x87, 0xe8, 0x20, 0x82, 0x8,
    0x0,

    /* U+0071 "q" */
    0xfe, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0xd7,
    0x8c, 0x0,

    /* U+0072 "r" */
    0xfe, 0x18, 0x61, 0x87, 0xe9, 0xa2, 0x8e, 0x10,
    0x0,

    /* U+0073 "s" */
    0xfa, 0x8, 0x20, 0x83, 0xe0, 0x41, 0x4, 0x17,
    0x80,

    /* U+0074 "t" */
    0x7c, 0x1, 0x4, 0x10, 0x40, 0x4, 0x10, 0x41,
    0x0,

    /* U+0075 "u" */
    0x2, 0x18, 0x61, 0x84, 0x8, 0x61, 0x86, 0x17,
    0xc0,

    /* U+0076 "v" */
    0x86, 0x18, 0x61, 0x86, 0x10, 0x33, 0x49, 0xe3,
    0x0,

    /* U+0077 "w" */
    0x84, 0x30, 0x86, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x40, 0x6, 0x23, 0x4a, 0x4f, 0x78, 0xc6, 0x0,

    /* U+0078 "x" */
    0x84, 0x99, 0xe1, 0x80, 0xf, 0x80, 0x18, 0x78,
    0x9b, 0x10,

    /* U+0079 "y" */
    0x86, 0x10, 0x33, 0x49, 0xe3, 0x0, 0x10, 0x40,

    /* U+007A "z" */
    0xf8, 0x10, 0xc6, 0x10, 0x2, 0x18, 0x41, 0x7,
    0xc0,

    /* U+007B "{" */
    0x24, 0x93, 0x89, 0x24, 0x80,

    /* U+007C "|" */
    0xfb, 0xdf,

    /* U+007D "}" */
    0x92, 0x48, 0xe4, 0x92, 0x0,

    /* U+007E "~" */
    0x66, 0x60
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 113, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 39, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 51, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 14},
    {.bitmap_index = 4, .adv_w = 263, .box_w = 15, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 23, .adv_w = 113, .box_w = 6, .box_h = 15, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 35, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 44, .adv_w = 158, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 26, .box_w = 1, .box_h = 2, .ofs_x = 0, .ofs_y = 14},
    {.bitmap_index = 58, .adv_w = 75, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 63, .adv_w = 76, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 68, .adv_w = 93, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 5},
    {.bitmap_index = 72, .adv_w = 101, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 77, .adv_w = 38, .box_w = 2, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 78, .adv_w = 84, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 79, .adv_w = 37, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 80, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 89, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 98, .adv_w = 76, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 129, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 138, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 147, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 172, .adv_w = 37, .box_w = 2, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 70, .box_w = 2, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 178, .adv_w = 87, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 181, .adv_w = 88, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 184, .adv_w = 87, .box_w = 4, .box_h = 6, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 187, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 196, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 214, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 223, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 230, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 239, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 253, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 262, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 271, .adv_w = 35, .box_w = 1, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 273, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 282, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 291, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 298, .adv_w = 204, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 315, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 333, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 342, .adv_w = 113, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 352, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 361, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 370, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 379, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 388, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 397, .adv_w = 193, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 413, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 423, .adv_w = 113, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 431, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 440, .adv_w = 75, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 445, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 454, .adv_w = 76, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 459, .adv_w = 131, .box_w = 7, .box_h = 5, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 464, .adv_w = 81, .box_w = 5, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 465, .adv_w = 128, .box_w = 2, .box_h = 3, .ofs_x = 3, .ofs_y = 9},
    {.bitmap_index = 466, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 475, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 484, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 491, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 500, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 507, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 514, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 523, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 532, .adv_w = 35, .box_w = 1, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 534, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 543, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 552, .adv_w = 113, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 559, .adv_w = 204, .box_w = 12, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 576, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 585, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 594, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 603, .adv_w = 113, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 613, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 622, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 631, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 640, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 649, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 658, .adv_w = 193, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 674, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 684, .adv_w = 113, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 692, .adv_w = 113, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 701, .adv_w = 75, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 706, .adv_w = 37, .box_w = 1, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 708, .adv_w = 75, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 713, .adv_w = 125, .box_w = 6, .box_h = 2, .ofs_x = 0, .ofs_y = 8}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ui_font_Display16 = {
#else
lv_font_t ui_font_Display16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UI_FONT_DISPLAY16*/

