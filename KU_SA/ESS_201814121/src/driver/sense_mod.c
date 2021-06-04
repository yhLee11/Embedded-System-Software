//sense_mod.c
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <asm/delay.h>
#include <linux/cdev.h>
#include <linux/timer.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "sense_mod_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_SWITCH _IOWR(SIMPLE_IOCTL_NUM,IOCTL_NUM1,unsigned long *)

#define SWITCH 5

static int sw_irq_num;
spinlock_t spinlock;

struct sen_list{
	struct list_head list;
	struct timespec time;
	int sensor_num;
};
struct sen_list sen_list_head;

static irqreturn_t switch_irq_isr(int irq, void* dev_id){
	printk("switch_irq: Detect\n");
	
	unsigned long flags;
	struct sen_list *node = (struct sen_list*)kmalloc(sizeof(struct sen_list), GFP_ATOMIC);
	spin_lock_irqsave(&spinlock,flags);

	node->sensor_num=1;
	getnstimeofday(&node->time);
	
	list_add_tail(&node->list,&sen_list_head.list);
	
	printk("switch_irq:sensor_num %d, time %ld\n",node->sensor_num,node->time.tv_sec);
	spin_unlock_irqrestore(&spinlock,flags);

	return IRQ_HANDLED;
}
static long sense_mod_ioctl(struct file *file, unsigned int cmd,unsigned long arg){

	struct sen_list *tmp=0;
	struct list_head *ptr=0;
	struct list_head *q=0;
	int idx=0;
	unsigned long flags;

	switch(cmd){
		case IOCTL_SWITCH:
			printk("====IOCTL_SWITCH====\n");
			
			if(!list_empty(&sen_list_head.list)){
				spin_lock_irqsave(&spinlock,flags);
				list_for_each_safe(ptr, q, &sen_list_head.list){
					tmp=list_entry(ptr, struct sen_list, list);
					printk("[%d] time: %ld\n", idx++, tmp->time.tv_sec);
				}
				spin_unlock_irqrestore(&spinlock, flags);
			}
			else{
				printk("empty list\n");
			}

	}
			
	return idx;
}
static int sense_mod_open(struct inode *inode, struct file *file){
	printk("sense_mod_open\n");
	return 0;
}
static int sense_mod_release(struct inode *inode, struct file *file){
	printk("sense_mod_release\n");
	disable_irq(sw_irq_num);	
	return 0;
}
static dev_t dev_num;
static struct cdev *cd_cdev;
struct file_operations sense_mod_fops={
	.open=sense_mod_open,
	.release=sense_mod_release,
	.unlocked_ioctl=sense_mod_ioctl,
};
static int __init sense_mod_init(void){
	printk("sense_mod: Init Module\n");
	int ret_sw=0;

	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev=cdev_alloc();
	cdev_init(cd_cdev,&sense_mod_fops);
	cdev_add(cd_cdev, dev_num, 1);
	
	INIT_LIST_HEAD(&sen_list_head.list);
	spin_lock_init(&spinlock);

	gpio_request_one(SWITCH,GPIOF_IN,"SWITCH");
			
	sw_irq_num=gpio_to_irq(SWITCH);
	ret_sw=request_irq(sw_irq_num, switch_irq_isr, IRQF_TRIGGER_RISING, "switch_irq","NULL");
	if(ret_sw){
		printk("pir_irq unable to reset request IRQ\n");
		free_irq(sw_irq_num,NULL);
	}else{
		disable_irq(sw_irq_num);
		printk("switch_irq enable to set request IRQ\n");
	}	

	return 0;
}
static void __exit sense_mod_exit(void){
	struct sen_list* node=0;
	struct list_head* pos=0;
	struct list_head* q=0;
	
	printk("sense_mod: Exit Module\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);

	free_irq(sw_irq_num,NULL);

	gpio_free(SWITCH);

	list_for_each_safe(pos,q,&sen_list_head.list){
		node = list_entry(pos, struct sen_list, list);
		list_del(pos);
		kfree(node);
	}
}
module_init(sense_mod_init);
module_exit(sense_mod_exit);

