#include <xboot.h>
#include <dma/dma.h>
#include <clk/clk.h>
#include <gpio/gpio.h>
#include <led/led.h>
#include <uboot-fb-test.h>

#define RES_MODE_640x480	0
#define RES_MODE_800x600	1
#define RES_MODE_1024x768	2
#define RES_MODE_960_720	3
#define RES_MODE_1152x864	4
#define RES_MODE_1280x1024	5
#define RES_MODE_1280x720	6
#define RES_MODE_1360x768	7
#define RES_MODE_1920x1080	8
#define RES_MODE_1920x1200	9
#define RES_MODES_COUNT		10
#define VESA_MODES_COUNT 19

/* Some mode definitions */
#define FB_SYNC_HOR_HIGH_ACT	1	/* horizontal sync high active	*/
#define FB_SYNC_VERT_HIGH_ACT	2	/* vertical sync high active	*/
#define FB_SYNC_EXT		4	/* external sync		*/
#define FB_SYNC_COMP_HIGH_ACT	8	/* composite sync high active	*/
#define FB_SYNC_BROADCAST	16	/* broadcast video timings	*/
					/* vtotal = 144d/288n/576i => PAL  */
					/* vtotal = 121d/242n/484i => NTSC */
#define FB_SYNC_ON_GREEN	32	/* sync on green */
#define FB_VMODE_NONINTERLACED	0	/* non interlaced */
#define FB_VMODE_INTERLACED	1	/* interlaced	*/
#define FB_VMODE_DOUBLE		2	/* double scan */
#define FB_VMODE_MASK		255

#define FB_VMODE_YWRAP		256	/* ywrap instead of panning	*/
#define FB_VMODE_SMOOTH_XPAN	512	/* smooth xpan possible (internally used) */
#define FB_VMODE_CONUPDATE	512	/* don't update x/yoffset	*/

struct ctfb_res_modes {
	int xres;		/* visible resolution		*/
	int yres;
	int refresh;		/* vertical refresh rate in hz  */
	/* Timing: All values in pixclocks, except pixclock (of course) */
	int pixclock;		/* pixel clock in ps (pico seconds) */
	int pixclock_khz;	/* pixel clock in kHz           */
	int left_margin;	/* time from sync to picture	*/
	int right_margin;	/* time from picture to sync	*/
	int upper_margin;	/* time from sync to picture	*/
	int lower_margin;
	int hsync_len;		/* length of horizontal sync	*/
	int vsync_len;		/* length of vertical sync	*/
	int sync;		/* see FB_SYNC_*		*/
	int vmode;		/* see FB_VMODE_*		*/
};

const struct ctfb_res_modes res_mode_init[RES_MODES_COUNT] = {
	/*  x     y  hz  pixclk ps/kHz   le   ri  up  lo   hs vs  s  vmode */
#ifndef CONFIG_VIDEO_STD_TIMINGS
	{ 640,  480, 60, 39721,  25180,  40,  24, 32, 11,  96, 2, 0, FB_VMODE_NONINTERLACED},
	{ 800,  600, 60, 27778,  36000,  64,  24, 22,  1,  72, 2, 0, FB_VMODE_NONINTERLACED},
	{1024,  768, 60, 15384,  65000, 168,   8, 29,  3, 144, 4, 0, FB_VMODE_NONINTERLACED},
	{ 960,  720, 80, 13100,  76335, 160,  40, 32,  8,  80, 4, 0, FB_VMODE_NONINTERLACED},
	{1152,  864, 60, 12004,  83300, 200,  64, 32, 16,  80, 4, 0, FB_VMODE_NONINTERLACED},
	{1280, 1024, 60,  9090, 110000, 200,  48, 26,  1, 184, 3, 0, FB_VMODE_NONINTERLACED},
#else
	{ 640,  480, 60, 39683,  25200,  48,  16, 33, 10,  96, 2, 0, FB_VMODE_NONINTERLACED},
	{ 800,  600, 60, 25000,  40000,  88,  40, 23,  1, 128, 4, FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT, FB_VMODE_NONINTERLACED},
	{1024,  768, 60, 15384,  65000, 160,  24, 29,  3, 136, 6, 0, FB_VMODE_NONINTERLACED},
	{ 960,  720, 75, 13468,  74250, 176,  72, 27,  1, 112, 2, 0, FB_VMODE_NONINTERLACED},
	{1152,  864, 75,  9259, 108000, 256,  64, 32,  1, 128, 3, FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT, FB_VMODE_NONINTERLACED},
	{1280, 1024, 60,  9259, 108000, 248,  48, 38,  1, 112, 3, FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT, FB_VMODE_NONINTERLACED},
#endif
	{1280,  720, 60, 13468,  74250, 220, 110, 20,  5,  40, 5, FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT, FB_VMODE_NONINTERLACED},
	{1360,  768, 60, 11696,  85500, 256,  64, 17,  3, 112, 7, 0, FB_VMODE_NONINTERLACED},
	{1920, 1080, 60,  6734, 148500, 148,  88, 36,  4,  44, 5, FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT, FB_VMODE_NONINTERLACED},
	{1920, 1200, 60,  6494, 154000,  80,  48, 26,  3,  32, 6, FB_SYNC_HOR_HIGH_ACT, FB_VMODE_NONINTERLACED},
};

int video_get_video_mode(unsigned int *xres, unsigned int *yres,
	unsigned int *depth, unsigned int *freq, const char **options)
{
	char *p = getenv("video-mode");
	if (!p)
		return 0;

	/* Skip over the driver name, which we don't care about. */
	p = strchr(p, ':');
	if (!p)
		return 0;

	/* Get the X-resolution*/
	while (*p && !isdigit(*p))
		p++;
	*xres = strtoul(p, &p, 10);
	if (!*xres)
		return 0;

	/* Get the Y-resolution */
	while (*p && !isdigit(*p))
		p++;
	*yres = strtoul(p, &p, 10);
	if (!*yres)
		return 0;

	/* Get the depth */
	while (*p && !isdigit(*p))
		p++;
	*depth = strtoul(p, &p, 10);
	if (!*depth)
		return 0;

	/* Get the frequency */
	while (*p && !isdigit(*p))
		p++;
	*freq = strtoul(p, &p, 10);
	if (!*freq)
		return 0;

	/* Find the extra options, if any */
	p = strchr(p, ',');
	*options = p ? p + 1 : NULL;

	return 1;
}

void video_get_ctfb_res_modes(int default_mode, unsigned int default_depth,
			      const struct ctfb_res_modes **mode_ret,
			      unsigned int *depth_ret,
			      const char **options)
{
	unsigned int i, xres, yres, depth, refresh;

	*mode_ret = &res_mode_init[default_mode];
	*depth_ret = default_depth;
	*options = NULL;

	if (!video_get_video_mode(&xres, &yres, &depth, &refresh, options))
		return;

	for (i = 0; i < RES_MODES_COUNT; i++) {
		if (res_mode_init[i].xres == xres &&
		    res_mode_init[i].yres == yres &&
		    res_mode_init[i].refresh == refresh) {
			*mode_ret = &res_mode_init[i];
			*depth_ret = depth;
			return;
		}
	}

	printf("video-mode %dx%d-%d@%d not available, falling back to %dx%d-%d@%d\r\n",
	       xres, yres, depth, refresh, (*mode_ret)->xres,
	       (*mode_ret)->yres, *depth_ret, (*mode_ret)->refresh);
}

static int
video_get_param_len(const char *start, char sep)
{
	int i = 0;
	while ((*start != 0) && (*start != sep)) {
		start++;
		i++;
	}
	return i;
}

void video_get_option_string(const char *options, const char *name,
			     char *dest, int dest_len, const char *def)
{
	const char *p = options;
	const int name_len = strlen(name);
	int i, len;

	while (p && (i = video_get_param_len(p, ',')) != 0) {
		if (strncmp(p, name, name_len) == 0 && p[name_len] == '=') {
			len = i - (name_len + 1);
			if (len >= dest_len)
				len = dest_len - 1;
			memcpy(dest, &p[name_len + 1], len);
			dest[len] = 0;
			return;
		}
		p += i;
		if (*p != 0)
			p++;	/* skip ',' */
	}
	strcpy(dest, def);
}

static int
video_search_param (char *start, char *param)
{
	int len, totallen, i;
	char *p = start;
	len = strlen (param);
	totallen = len + strlen (start);
	for (i = 0; i < totallen; i++) {
		if (strncmp (p++, param, len) == 0)
			return (i);
	}
	return -1;
}

int video_get_option_int(const char *options, const char *name, int def)
{
	const char *p = options;
	const int name_len = strlen(name);
	int i;

	while (p && (i = video_get_param_len(p, ',')) != 0) {
		if (strncmp(p, name, name_len) == 0 && p[name_len] == '=')
			return strtoul(&p[name_len + 1], NULL, 10);

		p += i;
		if (*p != 0)
			p++;	/* skip ',' */
	}
	return def;
}

#define GET_OPTION(name,var)				\
	if(strncmp(p,name,strlen(name))==0) {		\
		val_s=p+strlen(name);			\
		var=strtoul(val_s, NULL, 10);	\
	}

int video_get_params (struct ctfb_res_modes *pPar, char *penv)
{
	char *p, *s, *val_s;
	int i = 0;
	int bpp;
	int mode;

	/* first search for the environment containing the real param string */
	s = penv;

	if ((p = getenv (s)) != NULL)
		s = p;

	/*
	 * in case of the bootargs line, we have to start
	 * after "video=ctfb:"
	 */
	i = video_search_param (s, "video=ctfb:");
	if (i >= 0) {
		s += i;
		s += strlen ("video=ctfb:");
	}
	/* search for mode as a default value */
	p = s;
	mode = 0;		/* default */

	while ((i = video_get_param_len (p, ',')) != 0) {
		GET_OPTION ("mode:", mode)
			p += i;
		if (*p != 0)
			p++;	/* skip ',' */
	}

	if (mode >= RES_MODES_COUNT)
		mode = 0;

	*pPar = res_mode_init[mode];	/* copy default values */
	bpp = 24 - ((mode % 3) * 8);
	p = s;			/* restart */

	while ((i = video_get_param_len (p, ',')) != 0) {
		GET_OPTION ("x:", pPar->xres)
			GET_OPTION ("y:", pPar->yres)
			GET_OPTION ("refresh:", pPar->refresh)
			GET_OPTION ("le:", pPar->left_margin)
			GET_OPTION ("ri:", pPar->right_margin)
			GET_OPTION ("up:", pPar->upper_margin)
			GET_OPTION ("lo:", pPar->lower_margin)
			GET_OPTION ("hs:", pPar->hsync_len)
			GET_OPTION ("vs:", pPar->vsync_len)
			GET_OPTION ("sync:", pPar->sync)
			GET_OPTION ("vmode:", pPar->vmode)
			GET_OPTION ("pclk:", pPar->pixclock)
			GET_OPTION ("pclk_khz:", pPar->pixclock_khz)
			GET_OPTION ("depth:", bpp)
			p += i;
		if (*p != 0)
			p++;	/* skip ',' */
	}
	return bpp;
}

enum sunxi_monitor {
	sunxi_monitor_none,
	sunxi_monitor_dvi,
	sunxi_monitor_hdmi,
	sunxi_monitor_lcd,
	sunxi_monitor_vga,
	sunxi_monitor_composite_pal,
	sunxi_monitor_composite_ntsc,
	sunxi_monitor_composite_pal_m,
	sunxi_monitor_composite_pal_nc,
};
#define SUNXI_MONITOR_LAST sunxi_monitor_composite_pal_nc


static const char *sunxi_get_mon_desc(enum sunxi_monitor monitor)
{
	switch (monitor) {
	case sunxi_monitor_none:		return "none";
	case sunxi_monitor_dvi:			return "dvi";
	case sunxi_monitor_hdmi:		return "hdmi";
	case sunxi_monitor_lcd:			return "lcd";
	case sunxi_monitor_vga:			return "vga";
	case sunxi_monitor_composite_pal:	return "composite-pal";
	case sunxi_monitor_composite_ntsc:	return "composite-ntsc";
	case sunxi_monitor_composite_pal_m:	return "composite-pal-m";
	case sunxi_monitor_composite_pal_nc:	return "composite-pal-nc";
	}
	return NULL; /* never reached */
}



struct sunxi_display {
	enum sunxi_monitor monitor;
	unsigned int depth;
	unsigned int fb_addr;
	unsigned int fb_size;
} sunxi_display;

#define clrbits_le32(addr, clear) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) & ~(clear))

#define setbits_le32(addr, set) \
	write32(((virtual_addr_t)(addr)), read32(((virtual_addr_t)(addr))) | (set))

#define clrsetbits_le32(addr, clear, set) \
	write32(((virtual_addr_t)(addr)), (read32(((virtual_addr_t)(addr))) & ~(clear)) | (set))

#define writel(v, a)	write32((virtual_addr_t)(a), (v))
#define readl(a)		read32((virtual_addr_t)(a))

unsigned int clock_get_pll6(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll6_cfg);
	int n = ((rval & CCM_PLL6_CTRL_N_MASK) >> CCM_PLL6_CTRL_N_SHIFT) + 1;
	int k = ((rval & CCM_PLL6_CTRL_K_MASK) >> CCM_PLL6_CTRL_K_SHIFT) + 1;

	return 24000000 * n * k;
}

void clock_set_de_mod_clock(u32_t *clk_cfg, unsigned int hz)
{
	int pll = clock_get_pll6() * 2;
	int div = 1;

	while ((pll / div) > hz)
		div++;

	writel(CCM_DE_CTRL_GATE | CCM_DE_CTRL_PLL6_2X | CCM_DE_CTRL_M(div),
	       clk_cfg);
}

static void sunxi_frontend_init(void) {}

static void sunxi_composer_init(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_de_be_reg * const de_be =
		(struct sunxi_de_be_reg *)SUNXI_DE_BE0_BASE;
	int i;

	sunxi_frontend_init();

	/* Reset off */
	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_DE_BE0);

	/* Clocks on */
	setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_DE_BE0);
	setbits_le32(&ccm->dram_clk_gate, 1 << CCM_DRAM_GATE_OFFSET_DE_BE0);
	clock_set_de_mod_clock(&ccm->be0_clk_cfg, 300000000);

	/* Engine bug, clear registers after reset */
	for (i = 0x0800; i < 0x1000; i += 4)
		writel(0, SUNXI_DE_BE0_BASE + i);

	setbits_le32(&de_be->mode, SUNXI_DE_BE_MODE_ENABLE);
}

void lcdc_init(struct sunxi_lcdc_reg * const lcdc)
{
	/* Init lcdc */
	writel(0, &lcdc->ctrl); /* Disable tcon */
	writel(0, &lcdc->int0); /* Disable all interrupts */

	/* Disable tcon0 dot clock */
	clrbits_le32(&lcdc->tcon0_dclk, SUNXI_LCDC_TCON0_DCLK_ENABLE);

	/* Set all io lines to tristate */
	writel(0xffffffff, &lcdc->tcon0_io_tristate);
	writel(0xffffffff, &lcdc->tcon1_io_tristate);
}

static void sunxi_lcdc_init(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	struct sunxi_lcdc_reg * const lcdc =
		(struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;

	/* Reset off */
	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_LCD0);

	/* Clock on */
	setbits_le32(&ccm->ahb_gate1, 1 << AHB_GATE_OFFSET_LCD0);

	lcdc_init(lcdc);
}

static void sunxi_drc_init(void)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	/* On sun6i the drc must be clocked even when in pass-through mode */
	setbits_le32(&ccm->ahb_reset1_cfg, 1 << AHB_RESET_OFFSET_DRC0);
	clock_set_de_mod_clock(&ccm->iep_drc0_clk_cfg, 300000000);
}

static void sunxi_engines_init(void)
{
	sunxi_composer_init();
	sunxi_lcdc_init();
	sunxi_drc_init();
}

static void sunxi_lcdc_panel_enable(void)
{
}

static void sunxi_frontend_mode_set(const struct ctfb_res_modes *mode,
				    unsigned int address) {}

static void sunxi_composer_mode_set(const struct ctfb_res_modes *mode,
				    unsigned int address)
{
	struct sunxi_de_be_reg * const de_be =
		(struct sunxi_de_be_reg *)SUNXI_DE_BE0_BASE;

	sunxi_frontend_mode_set(mode, address);

	writel(SUNXI_DE_BE_HEIGHT(mode->yres) | SUNXI_DE_BE_WIDTH(mode->xres),
	       &de_be->disp_size);
	writel(SUNXI_DE_BE_HEIGHT(mode->yres) | SUNXI_DE_BE_WIDTH(mode->xres),
	       &de_be->layer0_size);

	writel(SUNXI_DE_BE_LAYER_STRIDE(mode->xres), &de_be->layer0_stride);
	writel(address << 3, &de_be->layer0_addr_low32b);
	writel(address >> 29, &de_be->layer0_addr_high4b);

	writel(SUNXI_DE_BE_LAYER_ATTR1_FMT_XRGB8888, &de_be->layer0_attr1_ctrl);

	setbits_le32(&de_be->mode, SUNXI_DE_BE_MODE_LAYER0_ENABLE);
	if (mode->vmode == FB_VMODE_INTERLACED)
		setbits_le32(&de_be->mode,
			     SUNXI_DE_BE_MODE_DEFLICKER_ENABLE |
			     SUNXI_DE_BE_MODE_INTERLACE_ENABLE);
}

void clock_set_pll3(unsigned int clk)
{
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;

	if (clk == 0) {
		clrbits_le32(&ccm->pll3_cfg, CCM_PLL3_CTRL_EN);
		return;
	}

	/* PLL3 rate = 3000000 * m */
	writel(CCM_PLL3_CTRL_EN | CCM_PLL3_CTRL_INTEGER_MODE |
	       CCM_PLL3_CTRL_M(clk / 3000000), &ccm->pll3_cfg);
}

unsigned int clock_get_pll3(void)
{
	struct sunxi_ccm_reg *const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	uint32_t rval = readl(&ccm->pll3_cfg);
	int m = ((rval & CCM_PLL3_CTRL_M_MASK) >> CCM_PLL3_CTRL_M_SHIFT);
	return 3000000 * m;
}

void lcdc_pll_set(struct sunxi_ccm_reg *ccm, int tcon, int dotclock,
		  int *clk_div, int *clk_double, int is_composite)
{
	int value, n, m, min_m, max_m, diff;
	int best_n = 0, best_m = 0, best_diff = 0x0FFFFFFF;
	int best_double = 0;
	int use_mipi_pll = 0;

	if (tcon == 0) {
		min_m = 6;
		max_m = 127;
	} else {
		min_m = 1;
		max_m = 15;
	}

	/*
	 * Find the lowest divider resulting in a matching clock, if there
	 * is no match, pick the closest lower clock, as monitors tend to
	 * not sync to higher frequencies.
	 */
	for (m = min_m; m <= max_m; m++) {

		n = (m * dotclock) / 3000;

		if ((n >= 9) && (n <= 127)) {
			value = (3000 * n) / m;
			diff = dotclock - value;
			if (diff < best_diff) {
				best_diff = diff;
				best_m = m;
				best_n = n;
				best_double = 0;
			}
		}

		/* These are just duplicates */
		if (!(m & 1))
			continue;

		/* No double clock on DE2 */
		n = (m * dotclock) / 6000;
		if ((n >= 9) && (n <= 127)) {
			value = (6000 * n) / m;
			diff = dotclock - value;
			if (diff < best_diff) {
				best_diff = diff;
				best_m = m;
				best_n = n;
				best_double = 1;
			}
		}
	}

	{
		clock_set_pll3(best_n * 3000000);
		printf("dotclock: %dkHz = %dkHz: (%d * 3MHz * %d) / %d\r\n",
		      dotclock,
		      (best_double + 1) * clock_get_pll3() / best_m / 1000,
		      best_double + 1, best_n, best_m);
	}

	if (tcon == 0) {
		u32_t pll;

		if (use_mipi_pll)
			pll = CCM_LCD_CH0_CTRL_MIPI_PLL;
		else if (best_double)
			pll = CCM_LCD_CH0_CTRL_PLL3_2X;
		else
			pll = CCM_LCD_CH0_CTRL_PLL3;
		writel(CCM_LCD_CH0_CTRL_GATE | CCM_LCD_CH0_CTRL_RST | pll,
		       &ccm->lcd0_ch0_clk_cfg);
	}
	else {
		writel(CCM_LCD_CH1_CTRL_GATE |
		       (best_double ? CCM_LCD_CH1_CTRL_PLL3_2X :
				      CCM_LCD_CH1_CTRL_PLL3) |
		       CCM_LCD_CH1_CTRL_M(best_m), &ccm->lcd0_ch1_clk_cfg);
		if (is_composite)
			setbits_le32(&ccm->lcd0_ch1_clk_cfg,
				     CCM_LCD_CH1_CTRL_HALF_SCLK1);
	}

	*clk_div = best_m;
	*clk_double = best_double;
}

enum display_flags {
	DISPLAY_FLAGS_HSYNC_LOW		= 1 << 0,
	DISPLAY_FLAGS_HSYNC_HIGH	= 1 << 1,
	DISPLAY_FLAGS_VSYNC_LOW		= 1 << 2,
	DISPLAY_FLAGS_VSYNC_HIGH	= 1 << 3,

	/* data enable flag */
	DISPLAY_FLAGS_DE_LOW		= 1 << 4,
	DISPLAY_FLAGS_DE_HIGH		= 1 << 5,
	/* drive data on pos. edge */
	DISPLAY_FLAGS_PIXDATA_POSEDGE	= 1 << 6,
	/* drive data on neg. edge */
	DISPLAY_FLAGS_PIXDATA_NEGEDGE	= 1 << 7,
	DISPLAY_FLAGS_INTERLACED	= 1 << 8,
	DISPLAY_FLAGS_DOUBLESCAN	= 1 << 9,
	DISPLAY_FLAGS_DOUBLECLK		= 1 << 10,
};

struct timing_entry {
	u32_t min;
	u32_t typ;
	u32_t max;
};

struct display_timing {
	struct timing_entry pixelclock;

	struct timing_entry hactive;		/* hor. active video */
	struct timing_entry hfront_porch;	/* hor. front porch */
	struct timing_entry hback_porch;	/* hor. back porch */
	struct timing_entry hsync_len;		/* hor. sync len */

	struct timing_entry vactive;		/* ver. active video */
	struct timing_entry vfront_porch;	/* ver. front porch */
	struct timing_entry vback_porch;	/* ver. back porch */
	struct timing_entry vsync_len;		/* ver. sync len */

	enum display_flags flags;		/* display flags */
	int hdmi_monitor;			/* is hdmi monitor? */
};

static void sunxi_ctfb_mode_to_display_timing(const struct ctfb_res_modes *mode,
					      struct display_timing *timing)
{
	timing->pixelclock.typ = mode->pixclock_khz * 1000;

	timing->hactive.typ = mode->xres;
	timing->hfront_porch.typ = mode->right_margin;
	timing->hback_porch.typ = mode->left_margin;
	timing->hsync_len.typ = mode->hsync_len;

	timing->vactive.typ = mode->yres;
	timing->vfront_porch.typ = mode->lower_margin;
	timing->vback_porch.typ = mode->upper_margin;
	timing->vsync_len.typ = mode->vsync_len;

	if (mode->sync & FB_SYNC_HOR_HIGH_ACT)
		timing->flags |= DISPLAY_FLAGS_HSYNC_HIGH;
	else
		timing->flags |= DISPLAY_FLAGS_HSYNC_LOW;
	if (mode->sync & FB_SYNC_VERT_HIGH_ACT)
		timing->flags |= DISPLAY_FLAGS_VSYNC_HIGH;
	else
		timing->flags |= DISPLAY_FLAGS_VSYNC_LOW;
	if (mode->vmode == FB_VMODE_INTERLACED)
		timing->flags |= DISPLAY_FLAGS_INTERLACED;
}

static int lcdc_get_clk_delay(const struct display_timing *mode, int tcon)
{
	int delay;

	delay = mode->vfront_porch.typ + mode->vsync_len.typ +
		mode->vback_porch.typ;
	if (mode->flags & DISPLAY_FLAGS_INTERLACED)
		delay /= 2;
	if (tcon == 1)
		delay -= 2;

	return (delay > 30) ? 30 : delay;
}

void lcdc_tcon0_mode_set(struct sunxi_lcdc_reg * const lcdc,
			 const struct display_timing *mode,
			 int clk_div, int for_ext_vga_dac,
			 int depth, int dclk_phase)
{
	int bp, clk_delay, total, val;

	/* Use tcon0 */
	clrsetbits_le32(&lcdc->ctrl, SUNXI_LCDC_CTRL_IO_MAP_MASK,
			SUNXI_LCDC_CTRL_IO_MAP_TCON0);

	clk_delay = lcdc_get_clk_delay(mode, 0);
	writel(SUNXI_LCDC_TCON0_CTRL_ENABLE |
	       SUNXI_LCDC_TCON0_CTRL_CLK_DELAY(clk_delay), &lcdc->tcon0_ctrl);

	writel(SUNXI_LCDC_TCON0_DCLK_ENABLE |
	       SUNXI_LCDC_TCON0_DCLK_DIV(clk_div), &lcdc->tcon0_dclk);

	writel(SUNXI_LCDC_X(mode->hactive.typ) |
	       SUNXI_LCDC_Y(mode->vactive.typ), &lcdc->tcon0_timing_active);

	bp = mode->hsync_len.typ + mode->hback_porch.typ;
	total = mode->hactive.typ + mode->hfront_porch.typ + bp;
	writel(SUNXI_LCDC_TCON0_TIMING_H_TOTAL(total) |
	       SUNXI_LCDC_TCON0_TIMING_H_BP(bp), &lcdc->tcon0_timing_h);

	bp = mode->vsync_len.typ + mode->vback_porch.typ;
	total = mode->vactive.typ + mode->vfront_porch.typ + bp;
	writel(SUNXI_LCDC_TCON0_TIMING_V_TOTAL(total) |
	       SUNXI_LCDC_TCON0_TIMING_V_BP(bp), &lcdc->tcon0_timing_v);

	writel(SUNXI_LCDC_X(mode->hsync_len.typ) |
	       SUNXI_LCDC_Y(mode->vsync_len.typ), &lcdc->tcon0_timing_sync);

	writel(0, &lcdc->tcon0_hv_intf);
	writel(0, &lcdc->tcon0_cpu_intf);

	if (depth == 18 || depth == 16) {
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[0]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[1]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[2]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[3]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[4]);
		writel(SUNXI_LCDC_TCON0_FRM_SEED, &lcdc->tcon0_frm_seed[5]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB0, &lcdc->tcon0_frm_table[0]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB1, &lcdc->tcon0_frm_table[1]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB2, &lcdc->tcon0_frm_table[2]);
		writel(SUNXI_LCDC_TCON0_FRM_TAB3, &lcdc->tcon0_frm_table[3]);
		writel(((depth == 18) ?
			SUNXI_LCDC_TCON0_FRM_CTRL_RGB666 :
			SUNXI_LCDC_TCON0_FRM_CTRL_RGB565),
		       &lcdc->tcon0_frm_ctrl);
	}

	val = SUNXI_LCDC_TCON0_IO_POL_DCLK_PHASE(dclk_phase);
	if (mode->flags & DISPLAY_FLAGS_HSYNC_LOW)
		val |= SUNXI_LCDC_TCON_HSYNC_MASK;
	if (mode->flags & DISPLAY_FLAGS_VSYNC_LOW)
		val |= SUNXI_LCDC_TCON_VSYNC_MASK;

	writel(val, &lcdc->tcon0_io_polarity);

	writel(0, &lcdc->tcon0_io_tristate);
}

static void sunxi_lcdc_tcon0_mode_set(const struct ctfb_res_modes *mode,int for_ext_vga_dac)
{
	struct sunxi_lcdc_reg * const lcdc =
		(struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;
	struct sunxi_ccm_reg * const ccm =
		(struct sunxi_ccm_reg *)SUNXI_CCM_BASE;
	int clk_div, clk_double;
	struct display_timing timing;

/*	for (pin = SUNXI_GPD(0); pin <= SUNXI_GPD(27); pin++) {
		sunxi_gpio_set_cfgpin(pin, SUNXI_GPD_LCD0);
	}*/

	lcdc_pll_set(ccm, 0, mode->pixclock_khz, &clk_div, &clk_double, 0);

	sunxi_ctfb_mode_to_display_timing(mode, &timing);
	lcdc_tcon0_mode_set(lcdc, &timing, clk_div, for_ext_vga_dac,
			    sunxi_display.depth, 1);
}

static void sunxi_frontend_enable(void) {}

static void sunxi_composer_enable(void)
{
	struct sunxi_de_be_reg * const de_be =
		(struct sunxi_de_be_reg *)SUNXI_DE_BE0_BASE;

	sunxi_frontend_enable();

	setbits_le32(&de_be->reg_ctrl, SUNXI_DE_BE_REG_CTRL_LOAD_REGS);
	setbits_le32(&de_be->mode, SUNXI_DE_BE_MODE_START);
}

void lcdc_enable(struct sunxi_lcdc_reg * const lcdc, int depth)
{
	setbits_le32(&lcdc->ctrl, SUNXI_LCDC_CTRL_TCON_ENABLE);
}

static void sunxi_lcdc_backlight_enable(void)
{
}

static void sunxi_mode_set(const struct ctfb_res_modes *mode,
			   unsigned int address)
{
	struct sunxi_lcdc_reg * const lcdc =
		(struct sunxi_lcdc_reg *)SUNXI_LCD0_BASE;

	switch (sunxi_display.monitor) {
	case sunxi_monitor_none:
		break;
	case sunxi_monitor_dvi:
	case sunxi_monitor_hdmi:

		break;
	case sunxi_monitor_lcd:
		sunxi_lcdc_panel_enable();
		sunxi_composer_mode_set(mode, address);
		sunxi_lcdc_tcon0_mode_set(mode, 0);
		sunxi_composer_enable();
		lcdc_enable(lcdc, sunxi_display.depth);
		sunxi_lcdc_backlight_enable();
		break;
	case sunxi_monitor_vga:
		break;
	case sunxi_monitor_composite_pal:
	case sunxi_monitor_composite_ntsc:
	case sunxi_monitor_composite_pal_m:
	case sunxi_monitor_composite_pal_nc:
		break;
	}
}

void video_hw_init(void)
{
	const struct ctfb_res_modes *mode;
	struct ctfb_res_modes custom;
	const char *options;

	int i, overscan_offset, overscan_x, overscan_y;
	unsigned int fb_dma_addr;
	char mon[16];
	char *lcd_mode = "x:800,y:480,depth:18,pclk_khz:33000,le:87,ri:40,up:31,lo:13,hs:1,vs:1,sync:3,vmode:0";

	memset(&sunxi_display, 0, sizeof(struct sunxi_display));

	video_get_ctfb_res_modes(RES_MODE_1024x768, 24, &mode,
				 &sunxi_display.depth, &options);

	overscan_x = video_get_option_int(options, "overscan_x", -1);
	overscan_y = video_get_option_int(options, "overscan_y", -1);
	sunxi_display.monitor = sunxi_monitor_lcd;
	video_get_option_string(options, "monitor", mon, sizeof(mon),
				sunxi_get_mon_desc(sunxi_display.monitor));
	for (i = 0; i <= SUNXI_MONITOR_LAST; i++) {
		if (strcmp(mon, sunxi_get_mon_desc(i)) == 0) {
			sunxi_display.monitor = i;
			break;
		}
	}
	if (i > SUNXI_MONITOR_LAST)
		printf("Unknown monitor: '%s', falling back to '%s'\r\n",
		       mon, sunxi_get_mon_desc(sunxi_display.monitor));



	switch (sunxi_display.monitor) {
	case sunxi_monitor_none:
		return ;
	case sunxi_monitor_dvi:
	case sunxi_monitor_hdmi:
		break;
	case sunxi_monitor_lcd:
		sunxi_display.depth = video_get_params(&custom, lcd_mode);
		mode = &custom;
		break;
	case sunxi_monitor_vga:
		sunxi_display.depth = 18;
		break;
	case sunxi_monitor_composite_pal:
	case sunxi_monitor_composite_ntsc:
	case sunxi_monitor_composite_pal_m:
	case sunxi_monitor_composite_pal_nc:
		sunxi_display.depth = 24;
		break;
	}

	/* Yes these defaults are quite high, overscan on composite sucks... */
	if (overscan_x == -1)
		overscan_x =  0;
	if (overscan_y == -1)
		overscan_y =  0;

	sunxi_display.fb_size =
		(mode->xres * mode->yres * 4 + 0xfff) & ~0xfff;
	overscan_offset = (overscan_y * mode->xres + overscan_x) * 4;
	/* We want to keep the fb_base for simplefb page aligned, where as
	 * the sunxi dma engines will happily accept an unaligned address. */
	if (overscan_offset)
		sunxi_display.fb_size += 0x1000;

	printf("Setting up a %dx%d%s %s console (overscan %dx%d)\r\n",
	       mode->xres, mode->yres,
	       (mode->vmode == FB_VMODE_INTERLACED) ? "i" : "",
	       sunxi_get_mon_desc(sunxi_display.monitor),
	       overscan_x, overscan_y);

	sunxi_engines_init();

	sunxi_display.fb_addr = 0x80800000;
	fb_dma_addr = sunxi_display.fb_addr;
	if (overscan_offset) {
		fb_dma_addr += 0x1000 - (overscan_offset & 0xfff);
		sunxi_display.fb_addr += (overscan_offset + 0xfff) & ~0xfff;
	}
	sunxi_mode_set(mode, fb_dma_addr);
}
