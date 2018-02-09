## 				XBOOT 部分功能小结

#### 1.XBOOT中GPIO操作 ####

XBOOT在gpio.h中提供有c api和lua api ，后面可以写个gpio的通用lua app，这样，以后想测什么功能，直接打开app。

在gpio.h中可以找到：   

```
   void gpio_set_cfg(int gpio, int cfg);
```

另外在l-gpio.c中可以找到：   
```lua
static const luaL_Reg m_gpio[] = {

	{"setCfg",		m_gpio_set_cfg},
	{"getCfg",		m_gpio_get_cfg},
	{"setPull",		m_gpio_set_pull},
	{"getPull",		m_gpio_get_pull},
	{"setDrv",		m_gpio_set_drv},
	{"getDrv",		m_gpio_get_drv},
	{"setRate",		m_gpio_set_rate},
	{"getRate", 	m_gpio_get_rate},
	{"setDir",		m_gpio_set_dir},
	{"getDir",		m_gpio_get_dir},
	{"setValue",	m_gpio_set_value},
	{"getValue",	m_gpio_get_value},
	{NULL,	NULL}
};
```
在c中可以这样写：

```c
gpio_set_cfg(F1C100S_GPIOA0,0x01);  //初始化端口功能
gpio_set_pull(F1C100S_GPIOA0,GPIO_PULL_NONE);   
gpio_set_drv(F1C100S_GPIOA0,GPIO_DRV_WEAKER);
gpio_set_direction(F1C100S_GPIOA0,GPIO_DIRECTION_OUTPUT);
gpio_set_rate(F1C100S_GPIOA0,GPIO_RATE_FAST);
{
	gpio_set_value(F1C100S_GPIOA0,1);  //PA0=1
	gpio_set_value(F1C100S_GPIOA0,0);  //PA0=0
}
```

另外也可以借用下面的LED 操作函数来变通处理   （不一定是led，可以用来测试，还有buzzer/vibrator）

driver/led/led-gpio.c   

driver/led/ledtrig-breathing.c   

driver/led/ledtrig-heartbeat.c   

首先在设备树可以看到   
```c

	"led-gpio@0": {
		"gpio": 0,
		"gpio-config": 1,
		"active-low": false,
		"default-brightness": 0
	},
	"led-gpio@0": {
		"gpio": 1,
		"gpio-config": 1,
		"active-low": false,
		"default-brightness": 0
	},

	"ledtrig-heartbeat@0": {
		"led-name": "led-gpio.1",
		"period-ms": 1260
	},

```
上面的“led-gpio”是驱动名，"led-gpio.1"是设备名，删除"led-gpio.1"部分的配置，然后shell进入"sys":   /sys/device/led/led-gpio.0/brightness    

> write 1 /sys/device/led/led-gpio.0/brightness
>
> write 0 /sys/device/led/led-gpio.0/brightness

把led看成一个gpio就可以了，向文件里面写数据。因为，xboot的shell没有开发 echo aaa > xxx ，这个重定向功能，所以需要一个专用命令写文件。

当然也可以下面那样看当前led的亮度

> cat /sys/device/led/led-gpio.0/brightness

上面的设备树定义：
```
"led-gpio@0": {    //@后面的数字代表起始索引，如果是0，就是自动分配
				//如果非零，就从这个值开始，尝试，直到这个设备名字没有占有
				//能注册到系统里面的去的所有资源，名字保证是唯一的
		"gpio": 0, //这里的“0”对应GPIO的端口号，可以去xxx-gpio.h里面去查看
		"gpio-config": 1,   //gpio的端口配置，1：gpio设定为输出。
		"active-low": false,
		"default-brightness": 0   //初始亮度（如果亮度可调的话）亮度的范围是0 - 1000，1000 最亮
	},
	"led-gpio@0": { //
		"gpio": 1,
		"gpio-config": 1,
		"active-low": false,
		"default-brightness": 0
	},
```
可以自己用c写一个命令，xboot里面有个system函数,通过system可以调用各种命令行,可以加分号，运行多个命令（例如：`system("echo aadfg; delay 3000; echo aaaas;");` ）。

`system("write 0 /sys/device/led/led-gpio.0/brightness");      //PA0=0`  

#### 2.XBOOT的测试命令

\kernel\command目录下有个cmd_test.c，其中：

	static int do_test(int argc, char ** argv)
	{
		return 0;
	}
这个文件，就是专门留出来，试验各种代码的，任何想测试的api，代码，什么的，都往这个文件丢吧，然后，命令行下，敲test 就可以测试的想法了。

*查看某个命令的帮助则可以敲 help cmdxxx*

#### 3.pwm播放铃声与马达

pwm口，接上个喇叭，一条命令就可以播各种nokia铃声（rtttl格式）

写个设备树

```
“buzzer-pwm@0":{
	"pwm-name":"pwm-xx.x",
	"pwm-polarity“:true
},
```

对应好pwm-name，然后那个pwm口接喇叭，然后

```
write "aadams:d=4,o=5,b=160:8c,f,8a,f,8c,b4,2g,8f,e,8g,e,8e4,a4,2f,8c,f,8a,f,8c,b4,2g,8f,e,8c,d,8e,1f,8c,8d,8e,8f,1p,8d,8e,8f#,8g,1p,8d,8e,8f#,8g,p,8d,8e,8f#,8g,p,8c,8d,8e,8f
" play
```

aadams 是歌曲名字，可以随便 。 (play 的路径在 /sys/device/buzzer/buzzer-pwm.0/play)

添加vibrator设备，其实就是振动马达，然后向节点里写：

```
write "SOS" play 
write "i'm hummer" play
write "helo 12313" play
```

就将字符编码成马达振动了～

#### 4.确认设备树参数

尝试源码目录下

> jjj@jjj-desktop:~/xboot/xboot/src$ grep "ledtrig-breathing" . -rn
> ./driver/led/ledtrig-breathing.c:2: * driver/led/ledtrig-breathing.c
> ./driver/led/ledtrig-breathing.c:146:	.name	 = "ledtrig-breathing",
> ./arch/arm32/mach-h3/romdisk/boot/nanopi-m1.json:458:	"ledtrig-breathing@0": {
>
> "ledtrig-breathing@0": {
> "led-name": "led-pwm.0",
> "interval-ms": 20,
> "period-ms": 3000
> },

或者直接搜索，一般都能在某些平台，找到参考配置，如果实在没有，就看驱动文件。找驱动里面的字符串，基本就可以判断出来，这个设备树可以填充哪些参数了，例如：

	static struct device_t * ledtrig_breathing_probe(struct driver_t * drv, struct dtnode_t * n)
	{
	struct ledtrig_breathing_pdata_t * pdat;
	struct ledtrig_t * ledtrig;
	struct device_t * dev;
	struct led_t * led;
	
	led = search_led(dt_read_string(n, "led-name", NULL));    //此处的"led-name"为设备树参数
	if(!led)
		return NULL;
	
	pdat = malloc(sizeof(struct ledtrig_breathing_pdata_t));
	if(!pdat)
		return NULL;
	
	ledtrig = malloc(sizeof(struct ledtrig_t));
	if(!ledtrig)
	{
		free(pdat);
		return NULL;
	}
	
	timer_init(&pdat->timer, ledtrig_breathing_timer_function, ledtrig);
	pdat->led = led;
	pdat->interval = dt_read_int(n, "interval-ms", 20);  //此处的"interval-ms"为设备树参数
	pdat->period = dt_read_int(n, "period-ms", 3000);
	pdat->phase = 0;
	
	ledtrig->name = alloc_device_name(dt_read_name(n), dt_read_id(n));
	ledtrig->activity = ledtrig_breathing_activity;
	ledtrig->priv = pdat;
	
	timer_start_now(&pdat->timer, ms_to_ktime(pdat->interval));
#### 5.待续