/*
 * driver/ts-ili2117a.c
 *
 * Copyright(c) 2018-2038 Alan <444656575@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18626133286
 * QQ: 444656575
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <i2c/i2c.h>
#include <gpio/gpio.h>
#include <interrupt/interrupt.h>
#include <input/input.h>


u32 SLAVE_I2C_ID_DBBUS = (0xC4>>1); //0x62 // for MSG28xx/MSG58xxA/ILI2117A/ILI2118A
//u32 SLAVE_I2C_ID_DBBUS = (0xB2>>1); //0x59 // for MSG22xx
u32 SLAVE_I2C_ID_DWI2C = (0x4C>>1); //0x26 
//u32 SLAVE_I2C_ID_DWI2C = (0x82>>1); //0x41 // for ILI21xx

u16_t FIRMWARE_MODE_UNKNOWN_MODE = 0xFFFF;
u16_t FIRMWARE_MODE_DEMO_MODE = 0xFFFF;
u16_t FIRMWARE_MODE_DEBUG_MODE = 0xFFFF;
u16_t FIRMWARE_MODE_RAW_DATA_MODE = 0xFFFF;

u16_t DEMO_MODE_PACKET_LENGTH = 0; // If project use MSG28xx, set MUTUAL_DEMO_MODE_PACKET_LENGTH as default. If project use MSG22xx, set SELF_DEMO_MODE_PACKET_LENGTH as default. 
u16_t DEBUG_MODE_PACKET_LENGTH = 0; // If project use MSG28xx, set MUTUAL_DEBUG_MODE_PACKET_LENGTH as default. If project use MSG22xx, set SELF_DEBUG_MODE_PACKET_LENGTH as default. 
u16_t MAX_TOUCH_NUM = 0; // If project use MSG28xx, set MUTUAL_MAX_TOUCH_NUM as default. If project use MSG22xx, set SELF_MAX_TOUCH_NUM as default. 


struct ts_ili2117a_pdata_t {
	struct i2c_device_t * dev;
	int irq;
	int fingers;
	struct {
		int x, y;
		int press;
		int valid;
	} node[10];
};

struct ili2117a_firmware_t {
	u8_t reg;
	u32_t val;
};

static const struct ili2117a_firmware_t firmware[] = {
	{0xf0,0x2},
	{0x00,0x00000000},
	{0x7c,0x353a3534},

	{0xFF, 0xFFFFFFFF},
};

static bool_t ili2117a_read(struct i2c_device_t * dev, u8_t * buf, int len)
{
	struct i2c_msg_t msgs;
/*
    msgs[0].addr = dev->addr;
    msgs[0].flags = 0;
    msgs[0].len = 1;
    msgs[0].buf = &reg;
*/
    msgs.addr = dev->addr;
    msgs.flags = I2C_M_RD;
    msgs.len = len;
    msgs.buf = buf;

    if(i2c_transfer(dev->i2c, msgs, 2) != 2)
    	return FALSE;
    return TRUE;
}

static bool_t ili2117a_write(struct i2c_device_t * dev, u8_t * buf, int len)
{
	struct i2c_msg_t msg;
	u8_t mbuf[256];

	if(len > sizeof(mbuf) - 1)
		len = sizeof(mbuf) - 1;
/*	mbuf[0] = reg;
	memcpy(&mbuf[1], buf, len);
*/
    msg.addr = dev->addr;
    msg.flags = 0;
    msg.len = len + 1;
    msg.buf = &mbuf[0];

    if(i2c_transfer(dev->i2c, &msg, 1) != 1)
    	return FALSE;
    return TRUE;
}

static void RegSetLByteValue(u16_t nAddr, u8_t nData)
{
    u8_t szTxData[4] = {0x10, (nAddr >> 8) & 0xFF, nAddr & 0xFF, nData};
    ili2117a_write(SLAVE_I2C_ID_DBBUS, &szTxData[0], 4);
}

static void RegSetHByteValue(u16_t nAddr, u8_t nData)
{
    u8_t szTxData[4] = {0x10, (nAddr >> 8) & 0xFF, (nAddr & 0xFF) + 1, nData};
    ili2117a_write(SLAVE_I2C_ID_DBBUS, &szTxData[0], 4);
}

static u16_t RegGet16BitValue(u16_t nAddr)
{
    u8_t szTxData[3] = {0x10, (nAddr >> 8) & 0xFF, nAddr & 0xFF};
    u8_t szRxData[2] = {0};

    ili2117a_write(SLAVE_I2C_ID_DBBUS, &szTxData[0], 3);
    ili2117a_read(SLAVE_I2C_ID_DBBUS, &szRxData[0], 2);

    return (szRxData[1] << 8 | szRxData[0]);
}

static u32_t DbBusEnterSerialDebugMode(struct i2c_device_t * dev)
{
    u32_t rc = 0;
    u8_t buf[5];

    // Enter the Serial Debug Mode
    buf[0] = 0x53;
    buf[1] = 0x45;
    buf[2] = 0x52;
    buf[3] = 0x44;
    buf[4] = 0x42;

    rc = ili2117a_write(dev, buf, 5);
    
    return rc;
}

static void DbBusExitSerialDebugMode(void)
{
    u8_t data[1];

    // Exit the Serial Debug Mode
    data[0] = 0x45;

    ili2117a_write(SLAVE_I2C_ID_DBBUS, data, 1);

    // Delay some interval to guard the next transaction
//    udelay(200);        // delay about 0.2ms
}

static void DbBusNotStopMCU(void)
{
    u8_t data[1];

    // Not Stop the MCU
    data[0] = 0x36;

    ili2117a_write(SLAVE_I2C_ID_DBBUS, data, 1);
}

static void DbBusIICNotUseBus(void)
{
    u8_t data[1];

    // IIC Not Use Bus
    data[0] = 0x34;

    ili2117a_write(SLAVE_I2C_ID_DBBUS, data, 1);
}

static bool_t ili2117a_check(struct i2c_device_t * dev)
{
	u8_t buf;
	u16_t nChipType = 0;
	u32_t rc = 0;
	
	// Check chip type by using DbBus for MSG22XX/MSG28XX/MSG58XX/MSG58XXA/ILI2117A/ILI2118A.
    // Erase TP Flash first
    rc = DbBusEnterSerialDebugMode();
	if(rc<0)
	{
		return TRUE; 
	}
	// Stop MCU
    RegSetLByteValue(0x0FE6, 0x01);
	
	// Set Password
    RegSetLByteValue(0x1616,0xAA);
    RegSetLByteValue(0x1617,0x55);
    RegSetLByteValue(0x1618,0xA5);
    RegSetLByteValue(0x1619,0x5A);

    // disable cpu read, in,tial); read
    RegSetLByteValue(0x1608,0x20);
    RegSetLByteValue(0x1606,0x20);
    RegSetLByteValue(0x1607,0x00);

    // set info block
    RegSetLByteValue(0x1607,0x08);
    // set info double buffer
    RegSetLByteValue(0x1604,0x01);

    // set eflash mode to read mode
    RegSetLByteValue(0x1606,0x01);
    RegSetLByteValue(0x1610,0x01);
    RegSetLByteValue(0x1611,0x00);

    // set read address
    RegSetLByteValue(0x1600,0x05);
    RegSetLByteValue(0x1601,0x00);

    nChipType = RegGet16BitValue(0x160A) & 0xFFFF;
	if (nChipType == CHIP_TYPE_ILI2117A)
	{
		return TRUE;
	}
	
	DbBusIICNotUseBus();
    DbBusNotStopMCU();
    DbBusExitSerialDebugMode();
	
	return FALSE;
}

static void ili2117a_clear(struct i2c_device_t * dev)
{
	u8_t buf;
/*
	buf = 0x88;
	ili2117a_write(dev, 0xe0, &buf, 1);
	mdelay(20);

	buf = 0x03;
	ili2117a_write(dev, 0x80, &buf, 1);
	mdelay(5);

	buf = 0x04;
	ili2117a_write(dev, 0xe4, &buf, 1);
	mdelay(5);

	buf = 0x00;
	ili2117a_write(dev, 0xe0, &buf, 1);
	mdelay(20);*/
}

static bool_t ili2117a_reset(struct i2c_device_t * dev)
{
	u8_t buf;
/*
	buf = 0x88;
	ili2117a_write(dev, 0xe0, &buf, 1);
	mdelay(20);

	buf = 0x04;
	ili2117a_write(dev, 0xe4, &buf, 1);
	mdelay(10);

	buf = 0x00;
	ili2117a_write(dev, 0xbc, &buf, 1);
	mdelay(10);

	buf = 0x00;
	ili2117a_write(dev, 0xbd, &buf, 1);
	mdelay(10);

	buf = 0x00;
	ili2117a_write(dev, 0xbe, &buf, 1);
	mdelay(10);

	buf = 0x00;
	ili2117a_write(dev, 0xbf, &buf, 1);
	mdelay(10);
*/
	return TRUE;
}

static bool_t ili2117a_startup(struct i2c_device_t * dev)
{
	u8_t buf;
/*
	buf = 0x00;
	ili2117a_write(dev, 0xe0, &buf, 1);
	mdelay(10);*/
	return TRUE;
}

static bool_t ili2117a_load_firmware(struct i2c_device_t * dev, const struct ili2117a_firmware_t * fw)
{
/*	struct ili2117a_firmware_t * next = (struct ili2117a_firmware_t *)fw;
	u8_t buf[4];

	if(!next)
		return FALSE;

	while(!((next->reg == 0xFF) && (next->val == 0xFFFFFFFF)))
	{
		buf[0] = ((next->val) >> 0) & 0xff;
		buf[1] = ((next->val) >> 8) & 0xff;
		buf[2] = ((next->val) >> 16) & 0xff;
		buf[3] = ((next->val) >> 24) & 0xff;

		if(!ili2117a_write(dev, next->reg, buf, 4))
			return FALSE;
		next++;
	}
*/
	return TRUE;
}

static void ili2117a_interrupt(void * data)
{
	struct input_t * input = (struct input_t *)data;
	struct ts_ili2117a_pdata_t * pdat = (struct ts_ili2117a_pdata_t *)input->priv;
	int fingers = pdat->fingers;
	u8_t buf[44];
	int x, y, id;
	int n, i;

	disable_irq(pdat->irq);

	if(ili2117a_read(pdat->dev, &buf[0], 4 + fingers * 4))
	{
		for(i = 0; i < fingers; i++)
		{
			pdat->node[i].valid = 0;
		}

		n = (buf[0] < fingers ? buf[0] : fingers);
		for(i = 0; i < n; i++)
		{
			x = ((buf[4 + 4 * i + 1] & 0x0f) << 8) | buf[4 + 4 * i + 0];
			y = ((buf[4 + 4 * i + 3] & 0x0f) << 8) | buf[4 + 4 * i + 2];
			id = ((buf[4 + 4 * i + 3] & 0xf0) >> 4) - 1;

			if(pdat->node[id].x != x || pdat->node[id].y != y)
			{
				if(pdat->node[id].press == 0)
				{
					push_event_touch_begin(input, x, y, id);
					pdat->node[id].press = 1;
				}
				else if(pdat->node[id].press == 1)
				{
					push_event_touch_move(input, x, y, id);
				}
			}
			pdat->node[id].x = x;
			pdat->node[id].y = y;
			pdat->node[id].valid = 1;
		}

		for(i = 0; i < fingers; i++)
		{
			if((pdat->node[i].press == 1) && (pdat->node[i].valid == 0))
			{
				push_event_touch_end(input, pdat->node[i].x, pdat->node[i].y, i);
				pdat->node[i].press = 0;
			}
		}
	}

	enable_irq(pdat->irq);
}

static int ts_ili2117a_ioctl(struct input_t * input, int cmd, void * arg)
{
	return -1;
}

static struct device_t * ts_ili2117a_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct ts_ili2117a_pdata_t * pdat;
	struct input_t * input;
	struct device_t * dev;
	struct i2c_device_t * i2cdev;
	int gpio = dt_read_int(n, "interrupt-gpio", -1);
	int irq = gpio_to_irq(gpio);

	if(!gpio_is_valid(gpio) || !irq_is_valid(irq))
		return NULL;

	i2cdev = i2c_device_alloc(dt_read_string(n, "i2c-bus", NULL), 0x40, 0);
	if(!i2cdev)
		return NULL;

	if(!ili2117a_check(i2cdev))
	{
		i2c_device_free(i2cdev);
		return NULL;
	}
	ili2117a_clear(i2cdev);
	ili2117a_reset(i2cdev);
	ili2117a_load_firmware(i2cdev, firmware);
	ili2117a_startup(i2cdev);
	ili2117a_reset(i2cdev);
	ili2117a_startup(i2cdev);

	pdat = malloc(sizeof(struct ts_ili2117a_pdata_t));
	if(!pdat)
	{
		i2c_device_free(i2cdev);
		return NULL;
	}

	input = malloc(sizeof(struct input_t));
	if(!input)
	{
		i2c_device_free(i2cdev);
		free(pdat);
		return NULL;
	}

	memset(pdat, 0, sizeof(struct ts_ili2117a_pdata_t));
	pdat->dev = i2cdev;
	pdat->irq = irq;
	pdat->fingers = dt_read_int(n, "maximum-fingers", 10);

	input->name = alloc_device_name(dt_read_name(n), -1);
	input->type = INPUT_TYPE_TOUCHSCREEN;
	input->ioctl = ts_ili2117a_ioctl;
	input->priv = pdat;

	gpio_set_pull(gpio, GPIO_PULL_UP);
	gpio_direction_input(gpio);
	request_irq(pdat->irq, ili2117a_interrupt, IRQ_TYPE_EDGE_RISING, input);

	if(!register_input(&dev, input))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
		return NULL;
	}
	dev->driver = drv;

	return dev;
}

static void ts_ili2117a_remove(struct device_t * dev)
{
	struct input_t * input = (struct input_t *)dev->priv;
	struct ts_ili2117a_pdata_t * pdat = (struct ts_ili2117a_pdata_t *)input->priv;

	if(input && unregister_input(input))
	{
		free_irq(pdat->irq);
		i2c_device_free(pdat->dev);

		free_device_name(input->name);
		free(input->priv);
		free(input);
	}
}

static void ts_ili2117a_suspend(struct device_t * dev)
{
}

static void ts_ili2117a_resume(struct device_t * dev)
{
}

static struct driver_t ts_ili2117a = {
	.name		= "ts-ili2117a",
	.probe		= ts_ili2117a_probe,
	.remove		= ts_ili2117a_remove,
	.suspend	= ts_ili2117a_suspend,
	.resume		= ts_ili2117a_resume,
};

static __init void ts_ili2117a_driver_init(void)
{
	register_driver(&ts_ili2117a);
}

static __exit void ts_ili2117a_driver_exit(void)
{
	unregister_driver(&ts_ili2117a);
}

driver_initcall(ts_ili2117a_driver_init);
driver_exitcall(ts_ili2117a_driver_exit);
