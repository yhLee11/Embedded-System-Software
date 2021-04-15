//ku_ipc.c
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include "ku_ipc.h"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4
#define IOCTL_NUM5 IOCTL_START_NUM+5

#define	SIMPLE_IOCTL_NUM 'z'
#define OPEN _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define READ _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)
#define WRITE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM3, unsigned long *)
#define CLOSE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM4, unsigned long *)
#define CHECK _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM5, unsigned long *)
#define DEV_NAME "ku_ipc_dev"

MODULE_LICENSE("GPL");

static dev_t dev_num;
static struct cdev *cd_cdev;

wait_queue_head_t wq;
struct msg_buf{
	long type;
	int msqid;
	int msgsz;
	int msgflg;
	char text[128];
};
struct msg_list{
	struct list_head list;
	struct msg_buf msg;
	int count;
};

static struct msg_list msg_head[10];

static long ku_ipc_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	
	int ret;	
	struct msg_list *tmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
	struct list_head *ptr;
	
	switch(cmd){
		case CHECK:
			printk("ioctl_check\n");
			int id1 = arg;
			return msg_head[id1].count;
			
		case OPEN://msgget()
			printk("IOCTL OPEN: msgget()\n");
			int id2 = arg;
			return msg_head[id2].count;
			
							
		case WRITE://msgsnd()
			printk("IOCTL WRITE: msgsnd()\n");
			
			struct msg_list *node1 = (struct msg_list*)kmalloc(sizeof(struct msg_list),GFP_KERNEL);
			if(copy_from_user(&node1->msg, (void __user*)arg, sizeof(node1->msg))){
				return -EFAULT;
			}
			
			int id3 = node1->msg.msqid;

			switch(node1->msg.msgflg){	
				case 0:
					wait_event(wq, msg_head[id3].count < KUIPC_MAXMSG);
					
					list_add_tail(&node1->list, &msg_head[id3].list);
			
					msg_head[id3].count += 1;

					return 0;

				case KU_IPC_NOWAIT:
					if(msg_head[id3].count==KUIPC_MAXMSG){
						printk("queue[%d] is full\n",id3);
						return -1;
					}
					list_add_tail(&node1->list, &msg_head[id3].list);
					msg_head[id3].count+=1;
			}


		case READ://msgrcv()
			printk("IOCTL READ: msgrcv()\n");
				
			struct msg_list *node = (struct msg_list*)kmalloc(sizeof(struct msg_list),GFP_KERNEL);
			if(copy_from_user(&node->msg, (void __user*)arg, sizeof(node->msg))){
				return -EFAULT;
			}
						
			int id = node->msg.msqid;
			
			ptr = msg_head[id].list.next;
			switch(node->msg.msgflg){
				case 0:
					wait_event(wq, msg_head[id].count>0);
					list_for_each(ptr, &msg_head[id].list){
						tmp = list_entry(ptr, struct msg_list, list);
						if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz==node->msg.msgsz)){
							strcpy(node->msg.text, tmp->msg.text);
							break;
						}
						else if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz>node->msg.msgsz)){
							strncpy(node->msg.text, tmp->msg.text, node->msg.msgsz);
							break;
						}
						else if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz<node->msg.msgsz)){
							strncpy(node->msg.text, tmp->msg.text, tmp->msg.msgsz);
							break;
						}
						else{
							printk("not equal type\n");
							return -1;
						}	
			
					}
					if(copy_to_user((void __user*)arg, &node->msg, sizeof(node->msg)))
						return -EFAULT;
					return node->msg.msgsz;

				case KU_IPC_NOWAIT:
					if(msg_head[id].count==0) 
						return -1;
					list_for_each(ptr, &msg_head[id].list){
						tmp =list_entry(ptr, struct msg_list, list);
						if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz==node->msg.msgsz)){
							strcpy(node->msg.text, tmp->msg.text);
							break;
						}
						else if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz>node->msg.msgsz)){
						       	strncpy(node->msg.text, tmp->msg.text, node->msg.msgsz);
							break;
						}	
				 		else if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz<node->msg.msgsz)){
							strncpy(node->msg.text, tmp->msg.text, tmp->msg.msgsz);
							break;
						}
						else{
							printk("not equal type\n");
							return -1;
						}
					}
					if(copy_to_user((void __user*)arg, &node->msg, sizeof(node->msg)))
						return -EFAULT;
					return node->msg.msgsz;

				case KU_IPC_NOERROR:
					wait_event(wq, msg_head[id].count>0);
					
					list_for_each(ptr, &msg_head[id].list){
						tmp = list_entry(ptr, struct msg_list, list);
					     	if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz>node->msg.msgsz)){
							return -1;
						}
						if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz==node->msg.msgsz)){
							strcpy(node->msg.text, tmp->msg.text);
							break;
						}
						else if((tmp->msg.type==node->msg.type)&&(tmp->msg.msgsz<node->msg.msgsz)){
							strncpy(node->msg.text, tmp->msg.text, tmp->msg.msgsz);
							break;
						}
						else{
							printk("not equal type\n");
							return -1;
						}
	 				}
					if(copy_to_user((void *)arg, &node->msg, sizeof(node->msg)))
						return -EFAULT;
					return node->msg.msgsz;
			}
				

						

		case CLOSE:
			printk("IOCTL CLOSE: msgclose()\n");
			int id4 = arg;
			
			list_del(&msg_head[id4].list);
			
			msg_head[id].count -=1;
			//kfree?
		default:
			printk("IOCTL ERROR\n");
			return -1;
	}


}

static int ku_ipc_open(struct inode *inode, struct file *file){
	return 0;
}
static int ku_ipc_release(struct inode *inode, struct file *file){
	return 0;
}

struct file_operations ipc_fops = {
	.unlocked_ioctl = ku_ipc_ioctl,
	.open = ku_ipc_open,
	.release = ku_ipc_release,
};
static int __init ku_ipc_init(void){
	printk("ku_ipc: Init module\n");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ipc_fops);
	cdev_add(cd_cdev, dev_num, 1);

	int i=0;
	for(i=0; i<10;i++){
		INIT_LIST_HEAD(&msg_head[i].list);
		msg_head[i].count =0;
		msg_head[i].msg.msqid=i;
	}
	init_waitqueue_head(&wq);

	return 0;
}
static void __exit ku_ipc_exit(void){
	printk("ku_ipc: Exit module\n");
	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num, 1);
}
module_init(ku_ipc_init);
module_exit(ku_ipc_exit);
