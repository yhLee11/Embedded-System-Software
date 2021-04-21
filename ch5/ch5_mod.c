#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define LED 5
#define SWITCH 12

struct timer_info{
	struct timer_list timer;
	long delay_jiffies;
};
static struct timer_info my_timer;

static void my_timer_func(struct timer_list *t){
	struct timer_info *info = from_timer(info, t, timer);
	int ret = 0;

	gpio_request_one(LED, GPIOF_OUT_INIT_LOW,"LED");
	ret = gpio_get_value(SWITCH);
	printk("ret %d\n",ret);
	if(ret){
		printk("LED ON\n");
		gpio_set_value(LED, 1);
	}
	else{
		gpio_set_value(LED, 0);
	}

	mod_timer(&my_timer.timer, jiffies + info->delay_jiffies);

}
static int __init ch5_mod_init(void){
	printk("ch5_mod LED+SWITCH+TIMER\n");

	gpio_request_one(SWITCH, GPIOF_IN, "SWITCH");

	my_timer.delay_jiffies = msecs_to_jiffies(500);
	timer_setup(&my_timer.timer, my_timer_func, 0);
	my_timer.timer.expires = jiffies + my_timer.delay_jiffies;
	add_timer(&my_timer.timer);
	
	return 0;
}	

static void __exit ch5_mod_exit(void){
	printk("ch5_mod exit\n");

	del_timer(&my_timer.timer);
	gpio_free(LED);
	gpio_free(SWITCH);

}

module_init(ch5_mod_init);
module_exit(ch5_mod_exit);
