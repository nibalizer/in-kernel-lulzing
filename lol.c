#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#include <asm/uaccess.h>

#define SEC_XFER_SIZE 512

// module attributes
MODULE_LICENSE("GPL"); // this avoids kernel taint warning
MODULE_DESCRIPTION("Laughing Out Loud Device Driver");
MODULE_AUTHOR("Spencer Krum");


static int times = 0;

// protoypes,else the structure initialization that follows fail
static int dev_open(struct inode *, struct file *);
static int dev_rls(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// structure containing callbacks
static struct file_operations lol_fops = 
{
	.read = dev_read, // address of dev_read
	.open = dev_open,  // address of dev_open
	.write = dev_write, // address of dev_write 
	.release = dev_rls, // address of dev_rls
};


static struct miscdevice lol_dev = {
        /*
         * We don't care what minor number we end up with, so tell the
         * kernel to just pick one.
         */
        MISC_DYNAMIC_MINOR,
        /*
         * Name ourselves /dev/lol.
         */
        "lol",
        /*
         * What functions to call when a program performs file
         * operations on the device.
         */
        &lol_fops
};


static int __init
lol_init(void)
{
        int ret;

        /*
         * Create the "lol" device in the /sys/class/misc directory.
         * Udev will automatically create the /dev/lol device using
         * the default rules.
         */
        ret = misc_register(&lol_dev);
        if (ret)
                printk(KERN_ERR
                       "Unable to register Laughing Out Loud misc device\n");

        return ret;
}



module_init(lol_init);



static void __exit
lol_exit(void)
{
        misc_deregister(&lol_dev);
}

module_exit(lol_exit);




// called when 'open' system call is done on the device file
static int dev_open(struct inode *inod,struct file *fil)
{	
	times++;
	printk(KERN_ALERT"Someone is LOLing. This has happened %d times.\n", times);
	return 0;
}

// called when 'read' system call is done on the device file
static ssize_t dev_read(struct file *filp,char *buf,size_t len,loff_t *off)
{
  //much of this taken from mem.c and random.c in the kernel
	size_t written;

	if (!len)
		return 0;

	if (!access_ok(VERIFY_WRITE, buf, len))
		return -EFAULT;

	written = 0;
	while (len) {
		unsigned long unwritten;
		size_t chunk = len;
    int i = 0;

		if (chunk > PAGE_SIZE)
			chunk = PAGE_SIZE;	/* Just for latency reasons */
		/*unwritten = __clear_user(buf, chunk);*/
    /*fill chunk with LOLLOOLOLO of size chunk */

    for (i = 0 ; i < chunk; i++){
      if (i % 2 == 0) {
        strcat(buf, "L");
      }
      else {
        strcat(buf, "O");
      }
    }
    unwritten = 0; //because strcat doesn't let us detect errors

		written += chunk - unwritten;
		buf += chunk;
		len -= chunk;
	}
	return written ? written : -EFAULT;

}

// called when 'write' system call is done on the device file
static ssize_t dev_write(struct file *filp,const char *buff,size_t len,loff_t *off)
{
	return 0;
}

// called when 'close' system call is done on the device file
static int dev_rls(struct inode *inod,struct file *fil)
{
	printk(KERN_DEBUG"LOL device closed\n");
	return 0;
}
