#include "lcd.h"

static lcd_param_t st_lcd_4_3_param = {
    .name = "lcd_4_3",
    .bpp = 24,
    .width = 480,
    .height = 272,
    .frame_addr = 0x33b00000,
    .st_time = {
        .clk = 9,
        .vbpd = 2,
        .vfpd = 2,
        .vspw = 10,
        .hbpd = 2,
        .hfpd = 2,
        .hspw = 41,
    },
    .st_pin_polarity = {
        .vlck = NORMAL,/* normal fetched at failling edge */
        .vline = INVERT,/* hsync, normal high level */
        .vframe = INVERT,/* vsync, normal high level */
        .vvd = NORMAL,/* video data, normal '1' means high level */
        .vdec = NORMAL, /* data enable, normal high level  */
        .vpwren = NORMAL,/* power enable, normal high level */
        .vlend = NORMAL,/* line end, normal high level, not use */
    },
};

int lcd_4_3_add(void)
{
    return lcd_register(&st_lcd_4_3_param);
}
