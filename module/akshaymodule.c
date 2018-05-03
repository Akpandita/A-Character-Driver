#include<linux/module.h>  // macro expansion for the printk() log level , KERN_ALERT
#include<linux/kernel.h>
#include<linux/fs.h>  // file_operations structure- which of course allows us to open,close - read,write to the device 
#include<linux/cdev.h>  // this is a string driver; makes cdev available
#include<linux/semaphore.h> //used to access semaphores; synchronization behaviors
#include<asm/uaccess.h>  // copy_to_user;copy_from_user

//(1) Create a struture for our fake device
struct fake_device{
	char data[100];
	struct semaphore sem;
}virtual_device;

//(2) To later register our device we need a cdev object and some other variables
struct cdev *mcdev;      //m stands 'my'
int major_number; 		// will store our major number - extracted from dev_t using macro - mknod /director/file c major minor
int ret; 				// will be used to hold return values of the functions; this is because the kernel stack is very small
						// so declearing variables all over the pass in our module functions eats up the stack very fast
dev_t dev_num;			// will hold major number that kernel gives us
						// name --> appears in /proc/devices
#define DEVICE_NAME "myDevice"

//(7) called on device_file open
// inode reference to the file on disk
// and contains information about that file
// struct file is reprents an abstract open file

int device_open(struct inode *inode,struct file *filp)
{
	// only allow one process to open this device by using a semaphore as mutual exclusive lock - mutex
	if(down_interruptible(&virtual_device.sem)!=0){
		printk(KERN_ALERT "akshay : could not lock device during open");
		return (-1);
	}

	printk(KERN_INFO "akshay : opened device");
	return 0;
}


//(8) called when user wants to get information from the device
ssize_t device_read(struct file* filp,char* bufStoreData, size_t bufCount,loff_t* curOffset){
	// take data from kernel space(device) to user space (process)
	// copy_to_user(destination,source,sizeToTransfer)

	printk(KERN_INFO "akshay : Reading from device");
	ret = copy_to_user(bufStoreData,virtual_device.data,bufCount);
	return ret;
}


//(9) called when user wants to find information to the device
ssize_t device_write(struct file* filp,const char* bufSourceData,size_t bufCount,loff_t* curOffset){
	//send a=data from user to kernel
	//copy_from_user (dest,source ,count)

	printk(KERN_INFO "akshay : writing to device");
	ret=copy_from_user(virtual_device.data,bufSourceData,bufCount);
	return ret;
}

//(10) called upon user close
int device_close(struct inode *inode,struct file *filp){
	// by calling up , which is opposite of down for semaphore , we release the mutex that we obtained at the device open
	// this has the effect of allowing other process to use the device now

	up(&virtual_device.sem);
	printk(KERN_INFO "akshay : closed device");
	return 0;
}

//(6) Tell the kernel which functions to call when user operates on our device file
struct file_operations fops ={
	.owner=THIS_MODULE,  		// prevent unloading of this module when operations are in use
	.open=device_open,			// points to the method to call when opening the device
	.release=device_close,		// points to the method to call when closing the device
	.write=device_write,		// points to the method to call when writing to the device
	.read=device_read			// points to the method to call when reading from the device
};

static int driver_entry(void){

	// (3) Register our device with the system : a two step process
	// step(1) use dynamic allocation to assign our device
	// a major number -- alloc_chrdev_region(dev_t*,uint fminor,unit count ,char *name)
	ret=alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
	if(ret<0){
		printk(KERN_ALERT "akshay : failed to allocate a major number");
		return ret;
	}
	major_number=MAJOR(dev_num); // extracts the major number and store in our variable macro
	printk(KERN_INFO "akshay : major number is %d",major_number);
	printk(KERN_INFO "\tuse \" mknod /dev/%s c %d 0\" for device file",DEVICE_NAME,major_number); //dmesg
 	//step(2)
	mcdev=cdev_alloc();  //create our cdev structure,initialize our cdev
	mcdev->ops=&fops;	// struct file_operations
	mcdev->owner=THIS_MODULE;
	//	now that we created cdev,we have to add it to the kernel
	//  int cdev_add(struct cdev* dev,dev_t num,unsigned int count)
	ret=cdev_add(mcdev,dev_num,1);
	if(ret<0){ //always check errors
		printk(KERN_ALERT "akshay : unable to add cdev to kernel");
		return ret;
	}
	//(4) Initialize our semaphore
	sema_init(&virtual_device.sem,1);  // initial value of 1
	return 0;
}

static void driver_exit(void){
	//(5) unregister everything in reverse order
	//(a)
	cdev_del(mcdev);
	//(b)
	unregister_chrdev_region(dev_num,1);
	printk(KERN_ALERT "akshay : unloaded module");
}


//inform the kernel where to start and stop with module/driver
module_init(driver_entry);
module_exit(driver_exit);