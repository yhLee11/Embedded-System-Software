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

//#include <linux/list.h>

//msg queue
/*#include <linux/types.h>
#include <linux/ipc.h>
#include <linux/msg.h>
*/

MODULE_LICENSE("GPL");

#define DEV_NAME "sense_mod_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
//#define IOCTL_NUM2 IOCTL_START_NUM+2
#define SIMPLE_IOCTL_NUM 'z'
//#define IOCTL_SWITCH _IOWR(SIMPLE_IOCTL_NUM,IOCTL_NUM1,unsigned long *)
#define IOCTL_PIR _IOWR(SIMPLE_IOCTL_NUM,IOCTL_NUM1,unsigned long *)

#define LED 4
//#define SWITCH 5
#define PIR 17
	
//static int switch_irq_num;
static int pir_irq_num;

struct sen_list{
	struct list_head list;
	int sensor_num;//1:pir
	ktime_t time;
};
struct user_buf{
	int sensor_num;
};
static struct sen_list sen_list_head;
/*
static irqreturn_t switch_irq_isr(int irq, void* dev_id){
	printk("switch_irq: Detect\n");
	struct sen_list *node = (struct sen_list*)kmalloc(sizeof(struct sen_list), GFP_KERNEL);
	
	ktime_t tmp_time;
	tmp_time=ktime_get();

	node->sensor_num=1;
	node->time=tmp_time;
	list_add_tail(&node->list,&sen_list_head.list);

	printk("switch_irq: time %lld\n",tmp_time);
	
	return IRQ_HANDLED;
}*/
static irqreturn_t pir_irq_isr(int irq, void* dev_id){
	struct sen_list *node = (struct sen_list*)kmalloc(sizeof(struct sen_list), GFP_KERNEL);

	ktime_t tmp_time;
	
	unsigned long flags;
	local_irq_save(flags);
	printk("pir_irq: Detect\n");
	tmp_time=ktime_get();
	local_irq_restore(flags);

	node->sensor_num=1;
	node->time=tmp_time;
	list_add_tail(&node->list,&sen_list_head.list);
	
	printk("pir_irq: time %lld\n",tmp_time);

	return IRQ_HANDLED;
}
static long sense_mod_ioctl(struct file *file, unsigned int cmd,unsigned long arg){
	//struct sen_list *node = (struct sen_list*)kmalloc(sizeof(struct sen_list),GFP_KERNEL);
	struct sen_list *tmp;
	struct list_head *ptr;
	struct user_buf *ub;

	ub=(struct user_buf*)arg;

	switch(cmd){
		case IOCTL_PIR:
			printk("====IOCTL_PIR====\n");

			//if empty
			if(list_empty(&sen_list_head.list)){
				printk("sense list is empty\n");
			}
			else{
				ptr=sen_list_head.list.next;
				tmp=(struct sen_list*)kmalloc(sizeof(struct sen_list),GFP_KERNEL);
				tmp=list_entry(ptr,struct sen_list, list);
				ub->sensor_num=tmp->sensor_num;
				if(copy_to_user((void __user*)arg, ub, sizeof(ub))){
					return -EFAULT;
				}
				printk("sensor_num:%d, time: %lld\n",ub->sensor_num, tmp->time);
			}
		}
			/*		
	switch(cmd){
		case IOCTL_SWITCH:
			printk("====IOCTL_SWITCH====\n");
			ptr=sen_list_head.list.next;
			tmp = (struct sen_list*)kmalloc(sizeof(struct sen_list),GFP_KERNEL);
			tmp = list_entry(ptr,struct sen_list, list);
			
			ub->sensor_num=tmp->sensor_num;
			if(copy_to_user((void __user*)arg, ub, sizeof(ub))){
				return EFAULT;
			}
			printk("sensor_num:%d, time:%lld\n",ub->sensor_num, tmp->time);
			
			 	
		case IOCTL_PIR:
			printk("====IOCTL_PIR====\n");
	
		default:
			printk("ioctl error\n");	
			return -1;
	}
	
	*/
	return 0;
}
static int sense_mod_open(struct inode *inode, struct file *file){
	printk("sense_mod_open\n");
//	enable_irq(switch_irq_num);
//	enable_irq(pir_irq_num);
	return 0;
}
static int sense_mod_release(struct inode *inode, struct file *file){
	printk("sense_mod_release\n");
	disable_irq(pir_irq_num);
//	disable_irq(switch_irq_num);
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
	
	INIT_LIST_HEAD(&sen_list_head.list);
	
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev=cdev_alloc();
	cdev_init(cd_cdev,&sense_mod_fops);
	cdev_add(cd_cdev, dev_num, 1);
	

	gpio_request_one(LED,GPIOF_IN,"LED");
//	gpio_request_one(SWITCH,GPIOF_IN,"SWITCH");

	int ret_pir=0;
//	int ret_sw=0;
	
//	switch_irq_num=gpio_to_irq(SWITCH);
//	ret_sw=request_irq(switch_irq_num, switch_irq_isr, IRQF_TRIGGER_RISING,"switch_irq",NULL);

//	if(ret_sw){
//		printk("switch_irq unable to reset request IRQ\n");
//	}else{
//		printk("switch_irq enable to set request IRQ\n");
//	}
		
	pir_irq_num=gpio_to_irq(PIR);
	ret_pir=request_irq(pir_irq_num, pir_irq_isr, IRQF_TRIGGER_RISING, "pir_irq","NULL");

	if(ret_pir){
		printk("pir_irq unable to reset request IRQ\n");
	}else{
		printk("pir_irq enable to set request IRQ\n");
	}	

	return 0;
}
static void __exit sense_mod_exit(void){
	printk("sense_mod: Exit Module\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);

//	disable_irq(switch_irq_num);
//	disable_irq(pir_irq_num);
//	free_irq(switch_irq_num,NULL);
	free_irq(pir_irq_num,NULL);

//	gpio_free(SWITCH);
	gpio_free(LED);
	gpio_free(PIR);

	//free queue
	
	
}

module_init(sense_mod_init);
module_exit(sense_mod_exit);

