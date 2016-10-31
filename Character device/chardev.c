#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>


//Prototypes
static int __init init(void);
static void __exit cleanup(void);
static int device_open(struct inode *,struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev" /* Dev name as it appears in /proc/devices*/
#define BUF_LEN 80 /* Max length of the message from the device */

static int Major; //Major number of the devices
static int Device_Open = 0;

static char msg[BUF_LEN]; //Message given when asked
static char *msg_Ptr;

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

static int __init init(){
    Major = register_chrdev(0,DEVICE_NAME,&fops);
    if(Major < 0){
        printk(KERN_ALERT "Failure in registering the device. %d\n", Major);
        return Major;
  }
    printk(KERN_INFO "%s registered with major %d \n",DEVICE_NAME,Major);
    printk(KERN_INFO "create a device with 'mknod /dev/%s c %d 0'\n",DEVICE_NAME,Major);
    printk(KERN_INFO "Try to cat and echo the file and shit man.\n");
    return SUCCESS;
}

static void __exit cleanup(){
    unregister_chrdev(Major, DEVICE_NAME);
    printk(KERN_ALERT "Unregistered the device %s i guess? \n"DEVICE_NAME);
}   

static int device_open(struct inode *inode,struct file *file){
    static int counter = 0;
    if(Device_Open)
        return -EBUSY;

    Device_Open++;
    sprintf(msg, "I already told you %d times Hello world!\n", counter++);
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode,struct file *file){
    Device_Open--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset){
    int bytes_read = 0;
    if(*msg_Ptr == 0)
        return 0;
    while(length && *msg_Ptr){
        put_user(*(msg_Ptr++),buffer++);
        length--;   
        bytes_read++;
    }
    return bytes_read;
}

static ssize_t device_write(struct file *filp,const char *buff, size_t len, loff_t *off){
    printk(KERN_ALERT "Your gay lol\n");
    return -EINVAL;
}

module_init(init);
module_exit(cleanup);