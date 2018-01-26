#ifndef __UBOOT_FB_TEST_H__
#define __UBOOT_FB_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <dma/dma.h>
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <led/led.h>

#define SUNXI_DE_BE0_BASE		0x01e60000
#define SUNXI_LCD0_BASE			0x01c0C000
#define SUNXI_CCM_BASE			0x01c20000

struct sunxi_de_fe_reg {
	u32_t enable;			/* 0x000 */
	u32_t frame_ctrl;			/* 0x004 */
	u32_t bypass;			/* 0x008 */
	u32_t algorithm_sel;		/* 0x00c */
	u32_t line_int_ctrl;		/* 0x010 */
	u8_t res0[0x0c];			/* 0x014 */
	u32_t ch0_addr;			/* 0x020 */
	u32_t ch1_addr;			/* 0x024 */
	u32_t ch2_addr;			/* 0x028 */
	u32_t field_sequence;		/* 0x02c */
	u32_t ch0_offset;			/* 0x030 */
	u32_t ch1_offset;			/* 0x034 */
	u32_t ch2_offset;			/* 0x038 */
	u8_t res1[0x04];			/* 0x03c */
	u32_t ch0_stride;			/* 0x040 */
	u32_t ch1_stride;			/* 0x044 */
	u32_t ch2_stride;			/* 0x048 */
	u32_t input_fmt;			/* 0x04c */
	u32_t ch3_addr;			/* 0x050 */
	u32_t ch4_addr;			/* 0x054 */
	u32_t ch5_addr;			/* 0x058 */
	u32_t output_fmt;			/* 0x05c */
	u32_t int_enable;			/* 0x060 */
	u32_t int_status;			/* 0x064 */
	u32_t status;			/* 0x068 */
	u8_t res2[0x04];			/* 0x06c */
	u32_t csc_coef00;			/* 0x070 */
	u32_t csc_coef01;			/* 0x074 */
	u32_t csc_coef02;			/* 0x078 */
	u32_t csc_coef03;			/* 0x07c */
	u32_t csc_coef10;			/* 0x080 */
	u32_t csc_coef11;			/* 0x084 */
	u32_t csc_coef12;			/* 0x088 */
	u32_t csc_coef13;			/* 0x08c */
	u32_t csc_coef20;			/* 0x090 */
	u32_t csc_coef21;			/* 0x094 */
	u32_t csc_coef22;			/* 0x098 */
	u32_t csc_coef23;			/* 0x09c */
	u32_t deinterlace_ctrl;		/* 0x0a0 */
	u32_t deinterlace_diag;		/* 0x0a4 */
	u32_t deinterlace_tempdiff;	/* 0x0a8 */
	u32_t deinterlace_sawtooth;	/* 0x0ac */
	u32_t deinterlace_spatcomp;	/* 0x0b0 */
	u32_t deinterlace_burstlen;	/* 0x0b4 */
	u32_t deinterlace_preluma;	/* 0x0b8 */
	u32_t deinterlace_tile_addr;	/* 0x0bc */
	u32_t deinterlace_tile_stride;	/* 0x0c0 */
	u8_t res3[0x0c];			/* 0x0c4 */
	u32_t wb_stride_enable;		/* 0x0d0 */
	u32_t ch3_stride;			/* 0x0d4 */
	u32_t ch4_stride;			/* 0x0d8 */
	u32_t ch5_stride;			/* 0x0dc */
	u32_t fe_3d_ctrl;			/* 0x0e0 */
	u32_t fe_3d_ch0_addr;		/* 0x0e4 */
	u32_t fe_3d_ch1_addr;		/* 0x0e8 */
	u32_t fe_3d_ch2_addr;		/* 0x0ec */
	u32_t fe_3d_ch0_offset;		/* 0x0f0 */
	u32_t fe_3d_ch1_offset;		/* 0x0f4 */
	u32_t fe_3d_ch2_offset;		/* 0x0f8 */
	u8_t res4[0x04];			/* 0x0fc */
	u32_t ch0_insize;			/* 0x100 */
	u32_t ch0_outsize;		/* 0x104 */
	u32_t ch0_horzfact;		/* 0x108 */
	u32_t ch0_vertfact;		/* 0x10c */
	u32_t ch0_horzphase;		/* 0x110 */
	u32_t ch0_vertphase0;		/* 0x114 */
	u32_t ch0_vertphase1;		/* 0x118 */
	u8_t res5[0x04];			/* 0x11c */
	u32_t ch0_horztapoffset0;		/* 0x120 */
	u32_t ch0_horztapoffset1;		/* 0x124 */
	u32_t ch0_verttapoffset;		/* 0x128 */
	u8_t res6[0xd4];			/* 0x12c */
	u32_t ch1_insize;			/* 0x200 */
	u32_t ch1_outsize;		/* 0x204 */
	u32_t ch1_horzfact;		/* 0x208 */
	u32_t ch1_vertfact;		/* 0x20c */
	u32_t ch1_horzphase;		/* 0x210 */
	u32_t ch1_vertphase0;		/* 0x214 */
	u32_t ch1_vertphase1;		/* 0x218 */
	u8_t res7[0x04];			/* 0x21c */
	u32_t ch1_horztapoffset0;		/* 0x220 */
	u32_t ch1_horztapoffset1;		/* 0x224 */
	u32_t ch1_verttapoffset;		/* 0x228 */
	u8_t res8[0x1d4];			/* 0x22c */
	u32_t ch0_horzcoef0[32];		/* 0x400 */
	u32_t ch0_horzcoef1[32];		/* 0x480 */
	u32_t ch0_vertcoef[32];		/* 0x500 */
	u8_t res9[0x80];			/* 0x580 */
	u32_t ch1_horzcoef0[32];		/* 0x600 */
	u32_t ch1_horzcoef1[32];		/* 0x680 */
	u32_t ch1_vertcoef[32];		/* 0x700 */
	u8_t res10[0x280];		/* 0x780 */
	u32_t vpp_enable;			/* 0xa00 */
	u32_t vpp_dcti;			/* 0xa04 */
	u32_t vpp_lp1;			/* 0xa08 */
	u32_t vpp_lp2;			/* 0xa0c */
	u32_t vpp_wle;			/* 0xa10 */
	u32_t vpp_ble;			/* 0xa14 */
};

struct sunxi_de_be_reg {
	u8_t res0[0x800];			/* 0x000 */
	u32_t mode;			/* 0x800 */
	u32_t backcolor;			/* 0x804 */
	u32_t disp_size;			/* 0x808 */
	u8_t res1[0x4];			/* 0x80c */
	u32_t layer0_size;		/* 0x810 */
	u32_t layer1_size;		/* 0x814 */
	u32_t layer2_size;		/* 0x818 */
	u32_t layer3_size;		/* 0x81c */
	u32_t layer0_pos;			/* 0x820 */
	u32_t layer1_pos;			/* 0x824 */
	u32_t layer2_pos;			/* 0x828 */
	u32_t layer3_pos;			/* 0x82c */
	u8_t res2[0x10];			/* 0x830 */
	u32_t layer0_stride;		/* 0x840 */
	u32_t layer1_stride;		/* 0x844 */
	u32_t layer2_stride;		/* 0x848 */
	u32_t layer3_stride;		/* 0x84c */
	u32_t layer0_addr_low32b;		/* 0x850 */
	u32_t layer1_addr_low32b;		/* 0x854 */
	u32_t layer2_addr_low32b;		/* 0x858 */
	u32_t layer3_addr_low32b;		/* 0x85c */
	u32_t layer0_addr_high4b;		/* 0x860 */
	u32_t layer1_addr_high4b;		/* 0x864 */
	u32_t layer2_addr_high4b;		/* 0x868 */
	u32_t layer3_addr_high4b;		/* 0x86c */
	u32_t reg_ctrl;			/* 0x870 */
	u8_t res3[0xc];			/* 0x874 */
	u32_t color_key_max;		/* 0x880 */
	u32_t color_key_min;		/* 0x884 */
	u32_t color_key_config;		/* 0x888 */
	u8_t res4[0x4];			/* 0x88c */
	u32_t layer0_attr0_ctrl;		/* 0x890 */
	u32_t layer1_attr0_ctrl;		/* 0x894 */
	u32_t layer2_attr0_ctrl;		/* 0x898 */
	u32_t layer3_attr0_ctrl;		/* 0x89c */
	u32_t layer0_attr1_ctrl;		/* 0x8a0 */
	u32_t layer1_attr1_ctrl;		/* 0x8a4 */
	u32_t layer2_attr1_ctrl;		/* 0x8a8 */
	u32_t layer3_attr1_ctrl;		/* 0x8ac */
	u8_t res5[0x110];			/* 0x8b0 */
	u32_t output_color_ctrl;		/* 0x9c0 */
	u8_t res6[0xc];			/* 0x9c4 */
	u32_t output_color_coef[12];	/* 0x9d0 */
};

struct sunxi_hdmi_reg {
	u32_t version_id;			/* 0x000 */
	u32_t ctrl;			/* 0x004 */
	u32_t irq;			/* 0x008 */
	u32_t hpd;			/* 0x00c */
	u32_t video_ctrl;			/* 0x010 */
	u32_t video_size;			/* 0x014 */
	u32_t video_bp;			/* 0x018 */
	u32_t video_fp;			/* 0x01c */
	u32_t video_spw;			/* 0x020 */
	u32_t video_polarity;		/* 0x024 */
	u8_t res0[0x58];			/* 0x028 */
	u8_t avi_info_frame[0x14];	/* 0x080 */
	u8_t res1[0x4c];			/* 0x094 */
	u32_t qcp_packet0;		/* 0x0e0 */
	u32_t qcp_packet1;		/* 0x0e4 */
	u8_t res2[0x118];			/* 0x0e8 */
	u32_t pad_ctrl0;			/* 0x200 */
	u32_t pad_ctrl1;			/* 0x204 */
	u32_t pll_ctrl;			/* 0x208 */
	u32_t pll_dbg0;			/* 0x20c */
	u32_t pll_dbg1;			/* 0x210 */
	u32_t hpd_cec;			/* 0x214 */
	u8_t res3[0x28];			/* 0x218 */
	u8_t vendor_info_frame[0x14];	/* 0x240 */
	u8_t res4[0x9c];			/* 0x254 */
	u32_t pkt_ctrl0;			/* 0x2f0 */
	u32_t pkt_ctrl1;			/* 0x2f4 */
	u8_t res5[0x8];			/* 0x2f8 */
	u32_t unknown;			/* 0x300 */
	u8_t res6[0xc];			/* 0x304 */
	u32_t audio_sample_count;		/* 0x310 */
	u8_t res7[0xec];			/* 0x314 */
	u32_t audio_tx_fifo;		/* 0x400 */
	u8_t res8[0xfc];			/* 0x404 */
	u32_t ddc_ctrl;			/* 0x500 */
	u32_t ddc_addr;			/* 0x504 */
	u32_t ddc_int_mask;		/* 0x508 */
	u32_t ddc_int_status;		/* 0x50c */
	u32_t ddc_fifo_ctrl;		/* 0x510 */
	u32_t ddc_fifo_status;		/* 0x514 */
	u32_t ddc_fifo_data;		/* 0x518 */
	u32_t ddc_byte_count;		/* 0x51c */
	u32_t ddc_cmnd;			/* 0x520 */
	u32_t ddc_exreg;			/* 0x524 */
	u32_t ddc_clock;			/* 0x528 */
	u8_t res9[0x14];			/* 0x52c */
	u32_t ddc_line_ctrl;		/* 0x540 */
};

/*
 * DE-FE register constants.
 */
#define SUNXI_DE_FE_WIDTH(x)			(((x) - 1) << 0)
#define SUNXI_DE_FE_HEIGHT(y)			(((y) - 1) << 16)
#define SUNXI_DE_FE_FACTOR_INT(n)		((n) << 16)
#define SUNXI_DE_FE_ENABLE_EN			(1 << 0)
#define SUNXI_DE_FE_FRAME_CTRL_REG_RDY		(1 << 0)
#define SUNXI_DE_FE_FRAME_CTRL_COEF_RDY		(1 << 1)
#define SUNXI_DE_FE_FRAME_CTRL_FRM_START	(1 << 16)
#define SUNXI_DE_FE_BYPASS_CSC_BYPASS		(1 << 1)
#define SUNXI_DE_FE_INPUT_FMT_ARGB8888		0x00000151
#define SUNXI_DE_FE_OUTPUT_FMT_ARGB8888		0x00000002

/*
 * DE-BE register constants.
 */
#define SUNXI_DE_BE_WIDTH(x)			(((x) - 1) << 0)
#define SUNXI_DE_BE_HEIGHT(y)			(((y) - 1) << 16)
#define SUNXI_DE_BE_MODE_ENABLE			(1 << 0)
#define SUNXI_DE_BE_MODE_START			(1 << 1)
#define SUNXI_DE_BE_MODE_DEFLICKER_ENABLE	(1 << 4)
#define SUNXI_DE_BE_MODE_LAYER0_ENABLE		(1 << 8)
#define SUNXI_DE_BE_MODE_INTERLACE_ENABLE	(1 << 28)
#define SUNXI_DE_BE_LAYER_STRIDE(x)		((x) << 5)
#define SUNXI_DE_BE_REG_CTRL_LOAD_REGS		(1 << 0)
#define SUNXI_DE_BE_LAYER_ATTR0_SRC_FE0		0x00000002
#define SUNXI_DE_BE_LAYER_ATTR1_FMT_XRGB8888	(0x09 << 8)
#define SUNXI_DE_BE_OUTPUT_COLOR_CTRL_ENABLE	1

/*
 * HDMI register constants.
 */
#define SUNXI_HDMI_X(x)				(((x) - 1) << 0)
#define SUNXI_HDMI_Y(y)				(((y) - 1) << 16)
#define SUNXI_HDMI_CTRL_ENABLE			(1 << 31)
#define SUNXI_HDMI_IRQ_STATUS_FIFO_UF		(1 << 0)
#define SUNXI_HDMI_IRQ_STATUS_FIFO_OF		(1 << 1)
#define SUNXI_HDMI_IRQ_STATUS_BITS		0x73
#define SUNXI_HDMI_HPD_DETECT			(1 << 0)
#define SUNXI_HDMI_VIDEO_CTRL_ENABLE		(1 << 31)
#define SUNXI_HDMI_VIDEO_CTRL_HDMI		(1 << 30)
#define SUNXI_HDMI_VIDEO_POL_HOR		(1 << 0)
#define SUNXI_HDMI_VIDEO_POL_VER		(1 << 1)
#define SUNXI_HDMI_VIDEO_POL_TX_CLK		(0x3e0 << 16)
#define SUNXI_HDMI_QCP_PACKET0			3
#define SUNXI_HDMI_QCP_PACKET1			0


#define SUNXI_HDMI_PAD_CTRL0_HDP		0xfe800000
#define SUNXI_HDMI_PAD_CTRL0_RUN		0xfe800000


#define SUNXI_HDMI_PAD_CTRL1			0x00d8c830
#define SUNXI_HDMI_PAD_CTRL1_HALVE		(1 << 6)


#define SUNXI_HDMI_PLL_CTRL			0xfa4ef708
#define SUNXI_HDMI_PLL_CTRL_DIV(n)		((n) << 4)
#define SUNXI_HDMI_PLL_CTRL_DIV_MASK		(0xf << 4)

#define SUNXI_HDMI_PLL_DBG0_PLL3		(0 << 21)
#define SUNXI_HDMI_PLL_DBG0_PLL7		(1 << 21)

#define SUNXI_HDMI_PKT_CTRL0			0x00000f21
#define SUNXI_HDMI_PKT_CTRL1			0x0000000f
#define SUNXI_HDMI_UNKNOWN_INPUT_SYNC		0x08000000

#define SUNXI_HMDI_DDC_CTRL_RESET		(1 << 0)
/* sun4i / sun5i / sun7i do not have a separate line_ctrl reg */
#define SUNXI_HMDI_DDC_CTRL_SDA_ENABLE		0
#define SUNXI_HMDI_DDC_CTRL_SCL_ENABLE		0
#define SUNXI_HMDI_DDC_CTRL_START		(1 << 30)
#define SUNXI_HMDI_DDC_CTRL_ENABLE		(1 << 31)


#define SUNXI_HMDI_DDC_ADDR_SLAVE_ADDR		(0x50 << 0)

#define SUNXI_HMDI_DDC_ADDR_OFFSET(n)		(((n) & 0xff) << 8)
#define SUNXI_HMDI_DDC_ADDR_EDDC_ADDR		(0x60 << 16)
#define SUNXI_HMDI_DDC_ADDR_EDDC_SEGMENT(n)	((n) << 24)


#define SUNXI_HDMI_DDC_FIFO_CTRL_CLEAR		(1 << 31)

#define SUNXI_HDMI_DDC_CMND_EXPLICIT_EDDC_READ	6
#define SUNXI_HDMI_DDC_CMND_IMPLICIT_EDDC_READ	7

/* N = 5,M=1 Fscl= Ftmds/2/10/2^N/(M+1) */
#define SUNXI_HDMI_DDC_CLOCK			0x0d

#define SUNXI_HMDI_DDC_LINE_CTRL_SCL_ENABLE	(1 << 8)
#define SUNXI_HMDI_DDC_LINE_CTRL_SDA_ENABLE	(1 << 9)


//===============================================================

struct sunxi_lcdc_reg {
	u32_t ctrl;			/* 0x00 */
	u32_t int0;			/* 0x04 */
	u32_t int1;			/* 0x08 */
	u8_t res0[0x04];			/* 0x0c */
	u32_t tcon0_frm_ctrl;		/* 0x10 */
	u32_t tcon0_frm_seed[6];		/* 0x14 */
	u32_t tcon0_frm_table[4];		/* 0x2c */
	u8_t res1[4];			/* 0x3c */
	u32_t tcon0_ctrl;			/* 0x40 */
	u32_t tcon0_dclk;			/* 0x44 */
	u32_t tcon0_timing_active;	/* 0x48 */
	u32_t tcon0_timing_h;		/* 0x4c */
	u32_t tcon0_timing_v;		/* 0x50 */
	u32_t tcon0_timing_sync;		/* 0x54 */
	u32_t tcon0_hv_intf;		/* 0x58 */
	u8_t res2[0x04];			/* 0x5c */
	u32_t tcon0_cpu_intf;		/* 0x60 */
	u32_t tcon0_cpu_wr_dat;		/* 0x64 */
	u32_t tcon0_cpu_rd_dat0;		/* 0x68 */
	u32_t tcon0_cpu_rd_dat1;		/* 0x6c */
	u32_t tcon0_ttl_timing0;		/* 0x70 */
	u32_t tcon0_ttl_timing1;		/* 0x74 */
	u32_t tcon0_ttl_timing2;		/* 0x78 */
	u32_t tcon0_ttl_timing3;		/* 0x7c */
	u32_t tcon0_ttl_timing4;		/* 0x80 */
	u32_t tcon0_lvds_intf;		/* 0x84 */
	u32_t tcon0_io_polarity;		/* 0x88 */
	u32_t tcon0_io_tristate;		/* 0x8c */
	u32_t tcon1_ctrl;			/* 0x90 */
	u32_t tcon1_timing_source;	/* 0x94 */
	u32_t tcon1_timing_scale;		/* 0x98 */
	u32_t tcon1_timing_out;		/* 0x9c */
	u32_t tcon1_timing_h;		/* 0xa0 */
	u32_t tcon1_timing_v;		/* 0xa4 */
	u32_t tcon1_timing_sync;		/* 0xa8 */
	u8_t res3[0x44];			/* 0xac */
	u32_t tcon1_io_polarity;		/* 0xf0 */
	u32_t tcon1_io_tristate;		/* 0xf4 */
	u8_t res4[0x108];			/* 0xf8 */
	u32_t mux_ctrl;			/* 0x200 */
	u8_t res5[0x1c];			/* 0x204 */
	u32_t lvds_ana0;			/* 0x220 */
	u32_t lvds_ana1;			/* 0x224 */
};

/*
 * LCDC register constants.
 */
#define SUNXI_LCDC_X(x)				(((x) - 1) << 16)
#define SUNXI_LCDC_Y(y)				(((y) - 1) << 0)
#define SUNXI_LCDC_TCON_VSYNC_MASK		(1 << 24)
#define SUNXI_LCDC_TCON_HSYNC_MASK		(1 << 25)
#define SUNXI_LCDC_CTRL_IO_MAP_MASK		(1 << 0)
#define SUNXI_LCDC_CTRL_IO_MAP_TCON0		(0 << 0)
#define SUNXI_LCDC_CTRL_IO_MAP_TCON1		(1 << 0)
#define SUNXI_LCDC_CTRL_TCON_ENABLE		(1 << 31)
#define SUNXI_LCDC_TCON0_FRM_CTRL_RGB666	((1 << 31) | (0 << 4))
#define SUNXI_LCDC_TCON0_FRM_CTRL_RGB565	((1 << 31) | (5 << 4))
#define SUNXI_LCDC_TCON0_FRM_SEED		0x11111111
#define SUNXI_LCDC_TCON0_FRM_TAB0		0x01010000
#define SUNXI_LCDC_TCON0_FRM_TAB1		0x15151111
#define SUNXI_LCDC_TCON0_FRM_TAB2		0x57575555
#define SUNXI_LCDC_TCON0_FRM_TAB3		0x7f7f7777
#define SUNXI_LCDC_TCON0_CTRL_CLK_DELAY(n)	(((n) & 0x1f) << 4)
#define SUNXI_LCDC_TCON0_CTRL_ENABLE		(1 << 31)
#define SUNXI_LCDC_TCON0_DCLK_DIV(n)		((n) << 0)
#define SUNXI_LCDC_TCON0_DCLK_ENABLE		(0xf << 28)
#define SUNXI_LCDC_TCON0_TIMING_H_BP(n)		(((n) - 1) << 0)
#define SUNXI_LCDC_TCON0_TIMING_H_TOTAL(n)	(((n) - 1) << 16)
#define SUNXI_LCDC_TCON0_TIMING_V_BP(n)		(((n) - 1) << 0)
#define SUNXI_LCDC_TCON0_TIMING_V_TOTAL(n)	(((n) * 2) << 16)
#define SUNXI_LCDC_TCON0_LVDS_CLK_SEL_TCON0	(1 << 20)

#define SUNXI_LCDC_TCON0_LVDS_INTF_BITWIDTH(n)	((n) << 26)
#define SUNXI_LCDC_TCON0_LVDS_INTF_ENABLE	(1 << 31)
#define SUNXI_LCDC_TCON0_IO_POL_DCLK_PHASE(x)	((x) << 28)
#define SUNXI_LCDC_TCON1_CTRL_CLK_DELAY(n)	(((n) & 0x1f) << 4)
#define SUNXI_LCDC_TCON1_CTRL_INTERLACE_ENABLE	(1 << 20)
#define SUNXI_LCDC_TCON1_CTRL_ENABLE		(1 << 31)
#define SUNXI_LCDC_TCON1_TIMING_H_BP(n)		(((n) - 1) << 0)
#define SUNXI_LCDC_TCON1_TIMING_H_TOTAL(n)	(((n) - 1) << 16)
#define SUNXI_LCDC_TCON1_TIMING_V_BP(n)		(((n) - 1) << 0)
#define SUNXI_LCDC_TCON1_TIMING_V_TOTAL(n)	((n) << 16)
#define SUNXI_LCDC_MUX_CTRL_SRC0_MASK		(0xf << 0)
#define SUNXI_LCDC_MUX_CTRL_SRC0(x)		((x) << 0)
#define SUNXI_LCDC_MUX_CTRL_SRC1_MASK		(0xf << 4)
#define SUNXI_LCDC_MUX_CTRL_SRC1(x)		((x) << 4)

#define SUNXI_LCDC_LVDS_ANA0			0x40040320
#define SUNXI_LCDC_LVDS_ANA0_EN_MB		(1 << 31)
#define SUNXI_LCDC_LVDS_ANA0_DRVC		(1 << 24)
#define SUNXI_LCDC_LVDS_ANA0_DRVD(x)		((x) << 20)

#define SUNXI_LCDC_LVDS_ANA1_INIT1		(0x1f << 26 | 0x1f << 10)
#define SUNXI_LCDC_LVDS_ANA1_INIT2		(0x1f << 16 | 0x1f << 00)

//===============================================================
struct sunxi_ccm_reg {
	u32_t pll1_cfg;		/* 0x00 pll1 control */
	u32_t reserved0;
	u32_t pll2_cfg;		/* 0x08 pll2 control */
	u32_t reserved1;
	u32_t pll3_cfg;		/* 0x10 pll3 control */
	u32_t reserved2;
	u32_t pll4_cfg;		/* 0x18 pll4 control */
	u32_t reserved3;
	u32_t pll5_cfg;		/* 0x20 pll5 control */
	u32_t reserved4;
	u32_t pll6_cfg;		/* 0x28 pll6 control */
	u32_t reserved5;
	u32_t pll7_cfg;		/* 0x30 pll7 control */
	u32_t sata_pll_cfg;	/* 0x34 SATA pll control (R40 only) */
	u32_t pll8_cfg;		/* 0x38 pll8 control */
	u32_t reserved7;
	u32_t mipi_pll_cfg;	/* 0x40 MIPI pll control */
	u32_t pll9_cfg;		/* 0x44 pll9 control */
	u32_t pll10_cfg;		/* 0x48 pll10 control */
	u32_t pll11_cfg;		/* 0x4c pll11 (ddr1) control (A33 only) */
	u32_t cpu_axi_cfg;	/* 0x50 CPU/AXI divide ratio */
	u32_t ahb1_apb1_div;	/* 0x54 AHB1/APB1 divide ratio */
	u32_t apb2_div;		/* 0x58 APB2 divide ratio */
	u32_t axi_gate;		/* 0x5c axi module clock gating */
	u32_t ahb_gate0;		/* 0x60 ahb module clock gating 0 */
	u32_t ahb_gate1;		/* 0x64 ahb module clock gating 1 */
	u32_t apb1_gate;		/* 0x68 apb1 module clock gating */
	u32_t apb2_gate;		/* 0x6c apb2 module clock gating */
	u32_t bus_gate4;          /* 0x70 gate 4 module clock gating */
	u8_t res3[0xc];
	u32_t nand0_clk_cfg;	/* 0x80 nand0 clock control */
	u32_t nand1_clk_cfg;	/* 0x84 nand1 clock control */
	u32_t sd0_clk_cfg;	/* 0x88 sd0 clock control */
	u32_t sd1_clk_cfg;	/* 0x8c sd1 clock control */
	u32_t sd2_clk_cfg;	/* 0x90 sd2 clock control */
	u32_t sd3_clk_cfg;	/* 0x94 sd3 clock control */
	u32_t ts_clk_cfg;		/* 0x98 transport stream clock control */
	u32_t ss_clk_cfg;		/* 0x9c security system clock control */
	u32_t spi0_clk_cfg;	/* 0xa0 spi0 clock control */
	u32_t spi1_clk_cfg;	/* 0xa4 spi1 clock control */
	u32_t spi2_clk_cfg;	/* 0xa8 spi2 clock control */
	u32_t spi3_clk_cfg;	/* 0xac spi3 clock control */
	u32_t i2s0_clk_cfg;	/* 0xb0 I2S0 clock control*/
	u32_t i2s1_clk_cfg;	/* 0xb4 I2S1 clock control */
	u32_t reserved10[2];
	u32_t spdif_clk_cfg;	/* 0xc0 SPDIF clock control */
	u32_t reserved11;
	u32_t sata_clk_cfg;	/* 0xc8 SATA clock control (R40 only) */
	u32_t usb_clk_cfg;	/* 0xcc USB clock control */
	u32_t gmac_clk_cfg;	/* 0xd0 GMAC clock control */
	u32_t reserved12[7];
	u32_t mdfs_clk_cfg;	/* 0xf0 MDFS clock control */
	u32_t dram_clk_cfg;	/* 0xf4 DRAM configuration clock control */
	u32_t dram_pll_cfg;	/* 0xf8 PLL_DDR cfg register, A33 only */
	u32_t mbus_reset;		/* 0xfc MBUS reset control, A33 only */
	u32_t dram_clk_gate;	/* 0x100 DRAM module gating */
	u32_t be0_clk_cfg;	/* 0x104 BE0 module clock */
	u32_t be1_clk_cfg;	/* 0x108 BE1 module clock */
	u32_t fe0_clk_cfg;	/* 0x10c FE0 module clock */
	u32_t fe1_clk_cfg;	/* 0x110 FE1 module clock */
	u32_t mp_clk_cfg;		/* 0x114 MP module clock */
	u32_t lcd0_ch0_clk_cfg;	/* 0x118 LCD0 CH0 module clock */
	u32_t lcd1_ch0_clk_cfg;	/* 0x11c LCD1 CH0 module clock */
	u32_t tve_clk_cfg;	/* 0x120 H3/H5 TVE module clock */
	u32_t reserved14[2];
	u32_t lcd0_ch1_clk_cfg;	/* 0x12c LCD0 CH1 module clock */
	u32_t lcd1_ch1_clk_cfg;	/* 0x130 LCD1 CH1 module clock */
	u32_t csi0_clk_cfg;	/* 0x134 CSI0 module clock */
	u32_t csi1_clk_cfg;	/* 0x138 CSI1 module clock */
	u32_t ve_clk_cfg;		/* 0x13c VE module clock */
	u32_t adda_clk_cfg;	/* 0x140 ADDA module clock */
	u32_t avs_clk_cfg;	/* 0x144 AVS module clock */
	u32_t dmic_clk_cfg;	/* 0x148 Digital Mic module clock*/
	u32_t reserved15;
	u32_t hdmi_clk_cfg;	/* 0x150 HDMI module clock */
	u32_t ps_clk_cfg;		/* 0x154 PS module clock */
	u32_t mtc_clk_cfg;	/* 0x158 MTC module clock */
	u32_t mbus0_clk_cfg;	/* 0x15c MBUS0 module clock */
	u32_t mbus1_clk_cfg;	/* 0x160 MBUS1 module clock */
	u32_t reserved16;
	u32_t mipi_dsi_clk_cfg;	/* 0x168 MIPI DSI clock control */
	u32_t mipi_csi_clk_cfg;	/* 0x16c MIPI CSI clock control */
	u32_t reserved17[4];
	u32_t iep_drc0_clk_cfg;	/* 0x180 IEP DRC0 module clock */
	u32_t iep_drc1_clk_cfg;	/* 0x184 IEP DRC1 module clock */
	u32_t iep_deu0_clk_cfg;	/* 0x188 IEP DEU0 module clock */
	u32_t iep_deu1_clk_cfg;	/* 0x18c IEP DEU1 module clock */
	u32_t reserved18[4];
	u32_t gpu_core_clk_cfg;	/* 0x1a0 GPU core clock config */
	u32_t gpu_mem_clk_cfg;	/* 0x1a4 GPU memory clock config */
	u32_t gpu_hyd_clk_cfg;	/* 0x1a0 GPU HYD clock config */
	u32_t reserved19[21];
	u32_t pll_lock;		/* 0x200 PLL Lock Time */
	u32_t pll1_lock;		/* 0x204 PLL1 Lock Time */
	u32_t reserved20[6];
	u32_t pll1_bias_cfg;	/* 0x220 PLL1 Bias config */
	u32_t pll2_bias_cfg;	/* 0x224 PLL2 Bias config */
	u32_t pll3_bias_cfg;	/* 0x228 PLL3 Bias config */
	u32_t pll4_bias_cfg;	/* 0x22c PLL4 Bias config */
	u32_t pll5_bias_cfg;	/* 0x230 PLL5 Bias config */
	u32_t pll6_bias_cfg;	/* 0x234 PLL6 Bias config */
	u32_t pll7_bias_cfg;	/* 0x238 PLL7 Bias config */
	u32_t pll8_bias_cfg;	/* 0x23c PLL8 Bias config */
	u32_t mipi_bias_cfg;	/* 0x240 MIPI Bias config */
	u32_t pll9_bias_cfg;	/* 0x244 PLL9 Bias config */
	u32_t pll10_bias_cfg;	/* 0x248 PLL10 Bias config */
	u32_t reserved21[5];
	u32_t pll5_tuning_cfg;	/* 0x260 PLL5 Tuning config */
	u32_t reserved21_5[7];
	u32_t pll1_pattern_cfg;	/* 0x280 PLL1 Pattern config */
	u32_t pll2_pattern_cfg;	/* 0x284 PLL2 Pattern config */
	u32_t pll3_pattern_cfg;	/* 0x288 PLL3 Pattern config */
	u32_t pll4_pattern_cfg;	/* 0x28c PLL4 Pattern config */
	u32_t pll5_pattern_cfg;	/* 0x290 PLL5 Pattern config */
	u32_t pll6_pattern_cfg;	/* 0x294 PLL6 Pattern config */
	u32_t pll7_pattern_cfg;	/* 0x298 PLL7 Pattern config */
	u32_t pll8_pattern_cfg;	/* 0x29c PLL8 Pattern config */
	u32_t mipi_pattern_cfg;	/* 0x2a0 MIPI Pattern config */
	u32_t pll9_pattern_cfg;	/* 0x2a4 PLL9 Pattern config */
	u32_t pll10_pattern_cfg;	/* 0x2a8 PLL10 Pattern config */
	u32_t pll11_pattern_cfg0; /* 0x2ac PLL11 Pattern config0, A33 only */
	u32_t pll11_pattern_cfg1; /* 0x2b0 PLL11 Pattern config0, A33 only */
	u32_t reserved22[3];
	u32_t ahb_reset0_cfg;	/* 0x2c0 AHB1 Reset 0 config */
	u32_t ahb_reset1_cfg;	/* 0x2c4 AHB1 Reset 1 config */
	u32_t ahb_reset2_cfg;	/* 0x2c8 AHB1 Reset 2 config */
	u32_t reserved23;
	u32_t apb1_reset_cfg;	/* 0x2d0 APB1 Reset config */
	u32_t reserved24;
	u32_t apb2_reset_cfg;	/* 0x2d8 APB2 Reset config */
	u32_t reserved25[5];
	u32_t ccu_sec_switch;	/* 0x2f0 CCU Security Switch, H3 only */
	u32_t reserved26[11];
	u32_t pll_lock_ctrl;	/* 0x320 PLL lock control, R40 only */
};

/* apb1 bit field */
#define APB1_GATE_UART_SHIFT	(20)
#define APB1_GATE_UART_MASK		(0x7 << APB1_GATE_UART_SHIFT)
#define APB1_GATE_TWI_SHIFT	(16)
#define APB1_GATE_TWI_MASK		(0x7 << APB1_GATE_TWI_SHIFT)

/* apb2 bit field */
#define APB2_CLK_SRC_LOSC		(0x0 << 24)
#define APB2_CLK_SRC_OSC24M		(0x1 << 24)
#define APB2_CLK_SRC_PLL6		(0x2 << 24)
#define APB2_CLK_SRC_MASK		(0x3 << 24)
#define APB2_CLK_RATE_N_1		(0x0 << 16)
#define APB2_CLK_RATE_N_2		(0x1 << 16)
#define APB2_CLK_RATE_N_4		(0x2 << 16)
#define APB2_CLK_RATE_N_8		(0x3 << 16)
#define APB2_CLK_RATE_N_MASK		(3 << 16)
#define APB2_CLK_RATE_M(m)		(((m)-1) << 0)
#define APB2_CLK_RATE_M_MASK            (0x1f << 0)

/* apb2 gate field */
#define APB2_GATE_UART_SHIFT	(16)
#define APB2_GATE_UART_MASK		(0xff << APB2_GATE_UART_SHIFT)
#define APB2_GATE_TWI_SHIFT	(0)
#define APB2_GATE_TWI_MASK		(0xf << APB2_GATE_TWI_SHIFT)

/* cpu_axi_cfg bits */
#define AXI_DIV_SHIFT			0
#define ATB_DIV_SHIFT			8
#define CPU_CLK_SRC_SHIFT		16

#define AXI_DIV_1			0
#define AXI_DIV_2			1
#define AXI_DIV_3			2
#define AXI_DIV_4			3
#define ATB_DIV_1			0
#define ATB_DIV_2			1
#define ATB_DIV_4			2
#define CPU_CLK_SRC_OSC24M		1
#define CPU_CLK_SRC_PLL1		2

#define CCM_PLL1_CTRL_M(n)		((((n) - 1) & 0x3) << 0)
#define CCM_PLL1_CTRL_K(n)		((((n) - 1) & 0x3) << 4)
#define CCM_PLL1_CTRL_N(n)		((((n) - 1) & 0x1f) << 8)
#define CCM_PLL1_CTRL_P(n)		(((n) & 0x3) << 16)
#define CCM_PLL1_CTRL_EN		(0x1 << 31)

#define CCM_PLL3_CTRL_M_SHIFT		0
#define CCM_PLL3_CTRL_M_MASK		(0xf << CCM_PLL3_CTRL_M_SHIFT)
#define CCM_PLL3_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_PLL3_CTRL_N_SHIFT		8
#define CCM_PLL3_CTRL_N_MASK		(0x7f << CCM_PLL3_CTRL_N_SHIFT)
#define CCM_PLL3_CTRL_N(n)		((((n) - 1) & 0x7f) << 8)
#define CCM_PLL3_CTRL_INTEGER_MODE	(0x1 << 24)
#define CCM_PLL3_CTRL_LOCK		(0x1 << 28)
#define CCM_PLL3_CTRL_EN		(0x1 << 31)

#define CCM_PLL5_CTRL_M(n)		((((n) - 1) & 0x3) << 0)
#define CCM_PLL5_CTRL_K(n)		((((n) - 1) & 0x3) << 4)
#define CCM_PLL5_CTRL_N(n)		((((n) - 1) & 0x1f) << 8)
#define CCM_PLL5_CTRL_UPD		(0x1 << 20)
#define CCM_PLL5_CTRL_SIGMA_DELTA_EN	(0x1 << 24)
#define CCM_PLL5_CTRL_EN		(0x1 << 31)

/* suniv pll6 doesn't have postdiv 2, so k is set to 0 */
#define PLL6_CFG_DEFAULT		0x90041800

#define CCM_PLL6_CTRL_N_SHIFT		8
#define CCM_PLL6_CTRL_N_MASK		(0x1f << CCM_PLL6_CTRL_N_SHIFT)
#define CCM_PLL6_CTRL_K_SHIFT		4
#define CCM_PLL6_CTRL_K_MASK		(0x3 << CCM_PLL6_CTRL_K_SHIFT)
#define CCM_PLL6_CTRL_LOCK		(1 << 28)

#define CCM_SATA_PLL_DEFAULT		0x90005811 /* 100 MHz */

#define CCM_MIPI_PLL_CTRL_M_SHIFT	0
#define CCM_MIPI_PLL_CTRL_M_MASK	(0xf << CCM_MIPI_PLL_CTRL_M_SHIFT)
#define CCM_MIPI_PLL_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_MIPI_PLL_CTRL_K_SHIFT	4
#define CCM_MIPI_PLL_CTRL_K_MASK	(0x3 << CCM_MIPI_PLL_CTRL_K_SHIFT)
#define CCM_MIPI_PLL_CTRL_K(n)		((((n) - 1) & 0x3) << 4)
#define CCM_MIPI_PLL_CTRL_N_SHIFT	8
#define CCM_MIPI_PLL_CTRL_N_MASK	(0xf << CCM_MIPI_PLL_CTRL_N_SHIFT)
#define CCM_MIPI_PLL_CTRL_N(n)		((((n) - 1) & 0xf) << 8)
#define CCM_MIPI_PLL_CTRL_LDO_EN	(0x3 << 22)
#define CCM_MIPI_PLL_CTRL_EN		(0x1 << 31)

#define CCM_PLL10_CTRL_M_SHIFT		0
#define CCM_PLL10_CTRL_M_MASK		(0xf << CCM_PLL10_CTRL_M_SHIFT)
#define CCM_PLL10_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_PLL10_CTRL_N_SHIFT		8
#define CCM_PLL10_CTRL_N_MASK		(0x7f << CCM_PLL10_CTRL_N_SHIFT)
#define CCM_PLL10_CTRL_N(n)		((((n) - 1) & 0x7f) << 8)
#define CCM_PLL10_CTRL_INTEGER_MODE	(0x1 << 24)
#define CCM_PLL10_CTRL_LOCK		(0x1 << 28)
#define CCM_PLL10_CTRL_EN		(0x1 << 31)

#define CCM_PLL11_CTRL_N(n)		((((n) - 1) & 0x3f) << 8)
#define CCM_PLL11_CTRL_SIGMA_DELTA_EN	(0x1 << 24)
#define CCM_PLL11_CTRL_UPD		(0x1 << 30)
#define CCM_PLL11_CTRL_EN		(0x1 << 31)

#define CCM_PLL5_TUN_LOCK_TIME(x)	(((x) & 0x7) << 24)
#define CCM_PLL5_TUN_LOCK_TIME_MASK	CCM_PLL5_TUN_LOCK_TIME(0x7)
#define CCM_PLL5_TUN_INIT_FREQ(x)	(((x) & 0x7f) << 16)
#define CCM_PLL5_TUN_INIT_FREQ_MASK	CCM_PLL5_TUN_INIT_FREQ(0x7f)

#define AHB1_ABP1_DIV_DEFAULT		0x00003180 /* AHB1=PLL6/3,APB1=AHB1/2 */

#define AXI_GATE_OFFSET_DRAM		0

/* ahb_gate0 offsets */
#define AHB_GATE_OFFSET_USB_OHCI1	30
#define AHB_GATE_OFFSET_USB_OHCI0	29

#define AHB_GATE_OFFSET_USB_EHCI1	27
#define AHB_GATE_OFFSET_USB_EHCI0	26

#define AHB_GATE_OFFSET_USB0		24

#define AHB_GATE_OFFSET_SPI1		21
#define AHB_GATE_OFFSET_SPI0		20
#define AHB_GATE_OFFSET_MCTL		14
#define AHB_GATE_OFFSET_GMAC		17
#define AHB_GATE_OFFSET_NAND0		13
#define AHB_GATE_OFFSET_NAND1		12
#define AHB_GATE_OFFSET_MMC3		11
#define AHB_GATE_OFFSET_MMC2		10
#define AHB_GATE_OFFSET_MMC1		9
#define AHB_GATE_OFFSET_MMC0		8
#define AHB_GATE_OFFSET_MMC(n)		(AHB_GATE_OFFSET_MMC0 + (n))
#define AHB_GATE_OFFSET_DMA		6
#define AHB_GATE_OFFSET_SS		5

/* ahb_gate1 offsets */
#define AHB_GATE_OFFSET_DRC0		25
#define AHB_GATE_OFFSET_DE_FE0		14
#define AHB_GATE_OFFSET_DE_BE0		12
#define AHB_GATE_OFFSET_DE		12
#define AHB_GATE_OFFSET_HDMI		11
#define AHB_GATE_OFFSET_TVE		9
#define AHB_GATE_OFFSET_LCD1		5
#define AHB_GATE_OFFSET_LCD0		4


#define CCM_MMC_CTRL_M(x)		((x) - 1)
#define CCM_MMC_CTRL_OCLK_DLY(x)	((x) << 8)
#define CCM_MMC_CTRL_N(x)		((x) << 16)
#define CCM_MMC_CTRL_SCLK_DLY(x)	((x) << 20)
#define CCM_MMC_CTRL_OSCM24		(0x0 << 24)
#define CCM_MMC_CTRL_PLL6		(0x1 << 24)
#define CCM_MMC_CTRL_ENABLE		(0x1 << 31)

#define CCM_SATA_CTRL_ENABLE		(0x1 << 31)
#define CCM_SATA_CTRL_USE_EXTCLK	(0x1 << 24)

#define CCM_USB_CTRL_PHY0_RST (0x1 << 0)
#define CCM_USB_CTRL_PHY1_RST (0x1 << 1)
#define CCM_USB_CTRL_PHY2_RST (0x1 << 2)
#define CCM_USB_CTRL_PHY3_RST (0x1 << 3)
/* There is no global phy clk gate on sun6i, define as 0 */
#define CCM_USB_CTRL_PHYGATE 0
#define CCM_USB_CTRL_PHY0_CLK (0x1 << 8)
#define CCM_USB_CTRL_PHY1_CLK (0x1 << 9)
#define CCM_USB_CTRL_PHY2_CLK (0x1 << 10)
#define CCM_USB_CTRL_PHY3_CLK (0x1 << 11)

#define CCM_USB_CTRL_OHCI0_CLK (0x1 << 16)
#define CCM_USB_CTRL_OHCI1_CLK (0x1 << 17)

#define CCM_GMAC_CTRL_TX_CLK_SRC_MII	0x0
#define CCM_GMAC_CTRL_TX_CLK_SRC_EXT_RGMII 0x1
#define CCM_GMAC_CTRL_TX_CLK_SRC_INT_RGMII 0x2
#define CCM_GMAC_CTRL_GPIT_MII		(0x0 << 2)
#define CCM_GMAC_CTRL_GPIT_RGMII	(0x1 << 2)
#define CCM_GMAC_CTRL_RX_CLK_DELAY(x)	((x) << 5)
#define CCM_GMAC_CTRL_TX_CLK_DELAY(x)	((x) << 10)

#define MDFS_CLK_DEFAULT		0x81000002 /* PLL6 / 3 */

#define CCM_DRAMCLK_CFG_DIV(x)		((x - 1) << 0)
#define CCM_DRAMCLK_CFG_DIV_MASK	(0xf << 0)
#define CCM_DRAMCLK_CFG_DIV0(x)		((x - 1) << 8)
#define CCM_DRAMCLK_CFG_DIV0_MASK	(0xf << 8)
#define CCM_DRAMCLK_CFG_SRC_PLL5	(0x0 << 20)
#define CCM_DRAMCLK_CFG_SRC_PLL6x2	(0x1 << 20)
#define CCM_DRAMCLK_CFG_SRC_PLL11	(0x1 << 20) /* A64 only */
#define CCM_DRAMCLK_CFG_SRC_MASK	(0x3 << 20)
#define CCM_DRAMCLK_CFG_UPD		(0x1 << 16)
#define CCM_DRAMCLK_CFG_RST		(0x1 << 31)

#define CCM_DRAMPLL_CFG_SRC_PLL5	(0x0 << 16) /* Select PLL5 (DDR0) */
#define CCM_DRAMPLL_CFG_SRC_PLL11	(0x1 << 16) /* Select PLL11 (DDR1) */
#define CCM_DRAMPLL_CFG_SRC_MASK	(0x1 << 16)

#define CCM_MBUS_RESET_RESET		(0x1 << 31)

#define CCM_DRAM_GATE_OFFSET_DE_FE0	24
#define CCM_DRAM_GATE_OFFSET_DE_FE1	25
#define CCM_DRAM_GATE_OFFSET_DE_BE0	26
#define CCM_DRAM_GATE_OFFSET_DE_BE1	27

#define CCM_LCD_CH0_CTRL_PLL3		(0 << 24)
#define CCM_LCD_CH0_CTRL_PLL7		(1 << 24)
#define CCM_LCD_CH0_CTRL_PLL3_2X	(2 << 24)
#define CCM_LCD_CH0_CTRL_PLL7_2X	(3 << 24)
#define CCM_LCD_CH0_CTRL_MIPI_PLL	(4 << 24)
/* No reset bit in ch0_clk_cfg (reset is controlled through ahb_reset1) */
#define CCM_LCD_CH0_CTRL_RST		0
#define CCM_LCD_CH0_CTRL_GATE		(0x1 << 31)

#define CCM_LCD_CH1_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_LCD_CH1_CTRL_HALF_SCLK1	0 /* no seperate sclk1 & 2 on sun6i */
#define CCM_LCD_CH1_CTRL_PLL3		(0 << 24)
#define CCM_LCD_CH1_CTRL_PLL7		(1 << 24)
#define CCM_LCD_CH1_CTRL_PLL3_2X	(2 << 24)
#define CCM_LCD_CH1_CTRL_PLL7_2X	(3 << 24)
#define CCM_LCD_CH1_CTRL_GATE		(0x1 << 31)

#define CCM_LCD0_CTRL_GATE		(0x1 << 31)
#define CCM_LCD0_CTRL_M(n)		((((n) - 1) & 0xf) << 0)

#define CCM_LCD1_CTRL_GATE		(0x1 << 31)
#define CCM_LCD1_CTRL_M(n)		((((n) - 1) & 0xf) << 0)

#define CCM_HDMI_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_HDMI_CTRL_PLL_MASK		(3 << 24)
#define CCM_HDMI_CTRL_PLL3		(0 << 24)
#define CCM_HDMI_CTRL_PLL7		(1 << 24)
#define CCM_HDMI_CTRL_PLL3_2X		(2 << 24)
#define CCM_HDMI_CTRL_PLL7_2X		(3 << 24)
#define CCM_HDMI_CTRL_DDC_GATE		(0x1 << 30)
#define CCM_HDMI_CTRL_GATE		(0x1 << 31)

#define CCM_HDMI_SLOW_CTRL_DDC_GATE	(1 << 31)

#define CCM_TVE_CTRL_GATE		(0x1 << 31)
#define CCM_TVE_CTRL_M(n)		((((n) - 1) & 0xf) << 0)


#define MBUS_CLK_DEFAULT		0x81000001 /* PLL6 / 2 */
#define MBUS_CLK_GATE			(0x1 << 31)

#define CCM_PLL5_PATTERN		0xd1303333
#define CCM_PLL11_PATTERN		0xf5860000

/* ahb_reset0 offsets */

#define AHB_RESET_OFFSET_SPI1		21
#define AHB_RESET_OFFSET_SPI0		20
#define AHB_RESET_OFFSET_GMAC		17
#define AHB_RESET_OFFSET_MCTL		14
#define AHB_RESET_OFFSET_MMC3		11
#define AHB_RESET_OFFSET_MMC2		10
#define AHB_RESET_OFFSET_MMC1		9
#define AHB_RESET_OFFSET_MMC0		8
#define AHB_RESET_OFFSET_MMC(n)		(AHB_RESET_OFFSET_MMC0 + (n))
#define AHB_RESET_OFFSET_SS		5

/* ahb_reset1 offsets */
#define AHB_RESET_OFFSET_SAT		26
#define AHB_RESET_OFFSET_DRC0		25
#define AHB_RESET_OFFSET_DE_FE0		14
#define AHB_RESET_OFFSET_DE_BE0		12
#define AHB_RESET_OFFSET_DE		12
#define AHB_RESET_OFFSET_HDMI		11
#define AHB_RESET_OFFSET_HDMI2		10
#define AHB_RESET_OFFSET_TVE		9

#define AHB_RESET_OFFSET_LCD1		5
#define AHB_RESET_OFFSET_LCD0		4


/* ahb_reset2 offsets */
#define AHB_RESET_OFFSET_EPHY		2
#define AHB_RESET_OFFSET_LVDS		0

/* apb1 reset */
#define APB1_RESET_UART_SHIFT	(20)
#define APB1_RESET_UART_MASK		(0x7 << APB1_RESET_UART_SHIFT)
#define APB1_RESET_TWI_SHIFT	(16)
#define APB1_RESET_TWI_MASK		(0x7 << APB1_RESET_TWI_SHIFT)


/* apb2 reset */
#define APB2_RESET_UART_SHIFT		(16)
#define APB2_RESET_UART_MASK		(0xff << APB2_RESET_UART_SHIFT)
#define APB2_RESET_TWI_SHIFT		(0)
#define APB2_RESET_TWI_MASK		(0xf << APB2_RESET_TWI_SHIFT)

/* CCM bits common to all Display Engine (and IEP) clock ctrl regs */
#define CCM_DE_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_DE_CTRL_PLL_MASK		(0xf << 24)
#define CCM_DE_CTRL_PLL3		(0 << 24)
#define CCM_DE_CTRL_PLL7		(1 << 24)
#define CCM_DE_CTRL_PLL6_2X		(2 << 24)
#define CCM_DE_CTRL_PLL8		(3 << 24)
#define CCM_DE_CTRL_PLL9		(4 << 24)
#define CCM_DE_CTRL_PLL10		(5 << 24)
#define CCM_DE_CTRL_GATE		(1 << 31)

/* CCM bits common to all Display Engine 2.0 clock ctrl regs */
#define CCM_DE2_CTRL_M(n)		((((n) - 1) & 0xf) << 0)
#define CCM_DE2_CTRL_PLL_MASK		(3 << 24)
#define CCM_DE2_CTRL_PLL6_2X		(0 << 24)
#define CCM_DE2_CTRL_PLL10		(1 << 24)
#define CCM_DE2_CTRL_GATE		(0x1 << 31)

/* CCU security switch, H3 only */
#define CCM_SEC_SWITCH_MBUS_NONSEC	(1 << 2)
#define CCM_SEC_SWITCH_BUS_NONSEC	(1 << 1)
#define CCM_SEC_SWITCH_PLL_NONSEC	(1 << 0)

#ifdef __cplusplus
}
#endif

#endif /* __UBOOT_FB_TEST_H__ */
