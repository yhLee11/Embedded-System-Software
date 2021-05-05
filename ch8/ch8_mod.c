#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define SENSOR 17
#define LED 5

#define DEV_NAME "ch8_mod_dev"

struct timer_info{
	struct timer_list timer;
	long delay_hiffies;
};
static struct timer_info my_timer;
static void my_timer_func(struct tier_list *t){
	struct timer_info *info = from_timer(info, t,timer);
	printk("timer: jiffies %ld\n",jiffies);
		

	
//	gpio_request_one(SENSOR, GPIOF_IN, "SENSOR");
}

static int irq_num;
//struct timespec start;
//struct timespec recent;

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
	

	local_irq_restore(flag);
	return IRQ_HANDLED;
}
static dev_t dev_num;
static struct cdev *ch_cdev;
static int __init ch8_init(void){
	int ret;
	printk("irq: init module\n");
	alloc_chrdev_region(&dev_num, 0,1,DEV_NAME);
	ch_cdev=cdev_alloc();
	cdev_init(cd_cdev,&ch8_fops);
	cdev_add(cd_cdev, dev_num, 1);

	gpio_request_one(SENSOR, GPIOF_IN, "SENSOR");
	gpio_request_one(LED, GPIOF_OUT_INIT_LOW,"LED");

	irq_num=gpio_to_irq(SENSOR);
	ret = request_irq(irq_num, ch8_irq_isr, IRQF_TRIGGER_RIGING, "ch8_irq", NULL);
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
	cdev_del(ch_cdev);
	unregister_chrdev_region(dev_num,1);

	free_irq(irq_num, NULL);
	gpio_free(SENSOR);
	gpio_free(LED);
}
module_init(ch8_init);
module_exit(ch8_exit);
