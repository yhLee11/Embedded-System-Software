#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "ch3.h"

#define DEV_NAME "ch3_dev"

MODULE_LICENSE("GPL");

struct msg_list{
	struct list_head list;
	struct msg_st msg;
};

static struct msg_list msg_list_head;
struct msg_list *node;
struct msg_list *new_node; 

static long msg_mod_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

        struct msg_st *tmp = (struct msg_st*)kmalloc(sizeof(struct msg_st), GFP_KERNEL);
	struct list_head *ptr;
	
	switch(cmd){
		case CH3_IOCTL_READ:
			printk("----CH3_IOCTL_READ-----\n");
				
			if(list_empty(&msg_list_head.list)){
				printk("READ empty node\n");
						
				tmp->len=0;
				strcpy(tmp->str, "");
				if(copy_to_user((void __user *)arg, tmp, sizeof(tmp))){
					return -EFAULT;
				}
				
				printk("len:%d, str:%s",tmp->len, tmp->str);

			}
			else{
				printk("READ node and delete");
				ptr = msg_list_head.list.next;
				node = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
                                node=list_entry(ptr, struct msg_list, list);
				
				tmp->len = node->msg.len;
				strcpy(tmp->str, node->msg.str);

				if(copy_to_user((void __user *)arg, tmp, sizeof(tmp))){
					return -EFAULT;
				}
				
				printk("len:%d, str:%s\n",node->msg.len, node->msg.str);	
				list_del(&node->list);
			
			}

			break;


		case CH3_IOCTL_WRITE:
			printk("----CH3_IOCTL_WRITE----\n");

			if(copy_from_user(tmp, (void __user *)arg, sizeof(tmp))){
				return -EFAULT;
			}
			
			new_node = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
			new_node->msg.len = tmp->len;
			strcpy(new_node->msg.str, tmp->str);

			list_add_tail(&new_node->list, &msg_list_head.list);
			printk("WRITE new node and add it\n");
			printk("len:%d, str:%s\n", new_node->msg.len, new_node->msg.str);
			break;
		

		default:
			return -1;

	}
	return 0;
}

static int msg_mod_open(struct inode *inode, struct file *file){
	printk("msg_mod_open\n");
	return 0;
}

static int msg_mod_release(struct inode *inode, struct file *file){
	printk("msg_mod_release\n");
	return 0;
}

struct file_operations msg_mod_fops={
	.open = msg_mod_open,
	.release = msg_mod_release,
	.unlocked_ioctl = msg_mod_ioctl,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch3_mod_init(void){
	printk("======init module======\n");

	INIT_LIST_HEAD(&msg_list_head.list);

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &msg_mod_fops);
	cdev_add(cd_cdev, dev_num, 1);

	return 0;
}

static void __exit ch3_mod_exit(void){
	printk("======exit module======\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);

}

module_init(ch3_mod_init);
module_exit(ch3_mod_exit);
