#include <linux/module.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define SEC_XFER_SIZE 512

// module attributes
MODULE_LICENSE("GPL"); // this avoids kernel taint warning
MODULE_DESCRIPTION("Laughing Out Loud Device Driver");
MODULE_AUTHOR("Spencer Krum");


static char msg[100]={0};
static short readPos=0;
static int times = 0;

// protoypes,else the structure initialization that follows fail
static int dev_open(struct inode *, struct file *);
static int dev_rls(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// structure containing callbacks
static struct file_operations fops = 
{
	.read = dev_read, // address of dev_read
	.open = dev_open,  // address of dev_open
	.write = dev_write, // address of dev_write 
	.release = dev_rls, // address of dev_rls
};


// called when module is loaded, similar to main()
int init_module(void)
{
	int t = register_chrdev(89,"lol",&fops); //register driver with major:89
	
	if (t<0) printk(KERN_ALERT "Device registration failed..\n");
	else printk(KERN_ALERT "Device registered...\n");

	return t;
}


// called when module is unloaded, similar to destructor in OOP
void cleanup_module(void)
{
	unregister_chrdev(89,"lol");
}


// called when 'open' system call is done on the device file
static int dev_open(struct inode *inod,struct file *fil)
{	
	times++;
	printk(KERN_ALERT"Device opened %d times\n",times);
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
	short ind = len-1;
	short count=0;
	memset(msg,0,100);
	readPos=0;
	while(len>0)
	{
		msg[count++] = buff[ind--]; //copy the given string to the driver but in reverse
		len--;
	}
	return count;
}

// called when 'close' system call is done on the device file
static int dev_rls(struct inode *inod,struct file *fil)
{
	printk(KERN_ALERT"Device closed\n");
	return 0;
}
