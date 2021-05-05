#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

#define SENSOR 17
#define LED 5

#define DEV_NAME "ch8_mod_dev"

struct timer_info{
	struct timer_list timer;
	long delay_jiffies;
};
static struct timer_info my_timer;
static void my_timer_func(struct timer_list *t){
	struct timer_info *info = from_timer(info, t,timer);
	printk("timer: jiffies %ld\n",jiffies);
	gpio_set_value(LED,1);
	mod_timer(&my_timer.timer, jiffies+info->delay_jiffies);
	gpio_set_value(LED,0);
}

static int irq_num;

static int ch8_open(struct inode *inode, struct file *file){
	printk("sensor:open\n");
	enable_irq(irq_num);
	return 0;
}
static int ch8_release(struct inode *inode, struct file* file){
	printk("sensor:close\n");
	disable_irq(irq_num);
	return 0;
}
struct file_operations ch8_fops={
	.open=ch8_open,
	.release=ch8_release,
};
static irqreturn_t ch8_irq_isr(int irq, void* dev_id){
	unsigned long flag;
	local_irq_save(flag);

	printk("motion detected \n");
	
	add_timer(&my_timer.timer);
	
	local_irq_restore(flag);
	return IRQ_HANDLED;
}
static dev_t dev_num;
static struct cdev *cd_cdev;
static int __init ch8_init(void){
	int ret;
	printk("irq: init module\n");
	alloc_chrdev_region(&dev_num, 0,1,DEV_NAME);
	cd_cdev=cdev_alloc();
	cdev_init(cd_cdev,&ch8_fops);
	cdev_add(cd_cdev, dev_num, 1);

	my_timer.delay_jiffies=msecs_to_jiffies(2000);
	timer_setup(&my_timer.timer, my_timer_func,0);
	my_timer.timer.expires=jiffies+my_timer.delay_jiffies;

	gpio_request_one(SENSOR, GPIOF_IN, "SENSOR");
	gpio_request_one(LED, GPIOF_OUT_INIT_LOW,"LED");

	irq_num=gpio_to_irq(SENSOR);
	ret = request_irq(irq_num, ch8_irq_isr, IRQF_TRIGGER_RISING, "ch8_irq", NULL);
	if(ret){
		printk("unable to request IRQ %d\n",irq_num);	
		free_irq(irq_num,NULL);
	}
	else{
		disable_irq(irq_num);
	}
	return 0;
}
static void __exit ch8_exit(void){
	printk("ch8_exit: exit module\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);

	free_irq(irq_num, NULL);
	gpio_free(SENSOR);
	gpio_free(LED);
}
module_init(ch8_init);
module_exit(ch8_exit);
