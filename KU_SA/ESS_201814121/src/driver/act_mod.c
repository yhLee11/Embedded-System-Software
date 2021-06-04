//act_mod.c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "act_mod_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+2
#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_SPEAKER _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long*)
#define SPEAKER 12

static void play(int note){
	int i=0;
	for(i=0;i<100;i++){
		gpio_set_value(SPEAKER,1);
		udelay(note);
		gpio_set_value(SPEAKER,0);
		udelay(note);
	}
}
static int speaker_open(struct inode *inode, struct file *file){
	printk("speaker open\n");
	return 0;
}
static int speaker_release(struct inode *inode, struct file *file){
	printk("speaker close\n");
	return 0;
}
static long speaker_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	printk("speaker ioctl\n");

	int cnt=(int)arg;
	printk("play count: %d\n",cnt);
	switch(cmd){
		case IOCTL_SPEAKER:
			printk("IOCTL_SPEAKER\n");
			int notes[]={1911,1702,1516,1431,1275,1136,1012};
			int i=cnt;

			for(i=0;i<7;i++){
				play(notes[i]);
				mdelay(500);
			}
			gpio_set_value(SPEAKER,0);
		
		default:
			printk("ioctl error\n");
			return -1;
	}
	return 0;
}
struct file_operations speaker_fops={
	.open=speaker_open,
	.release=speaker_release,
	.unlocked_ioctl=speaker_ioctl,
};
static dev_t dev_num;
static struct cdev *cd_cdev;
static int __init speaker_init(void){
	printk("speaker init module\n");
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev=cdev_alloc();
	cdev_init(cd_cdev,&speaker_fops);
	cdev_add(cd_cdev,dev_num,1);
	
	gpio_request_one(SPEAKER, GPIOF_OUT_INIT_LOW,"SPEAKER");
	
	return 0;
}
static void __exit speaker_exit(void){

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);

	gpio_set_value(SPEAKER,0);
	gpio_free(SPEAKER);
}
module_init(speaker_init);
module_exit(speaker_exit);

