#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>
//#include <linux/gpio.h>
#include <linux/delay.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Spaj Spajowski");
MODULE_DESCRIPTION("Some RaspberryPi thing, it will blink as much time as we tell to it");

//param how many times to blink
int count = 1;
module_param(count,int,0);

static int __init init_TheDiode(void);
static void __exit module_remove(void);

void blink(unsigned char * buff, int len);

static ssize_t write_file(struct file* f, const char * buff, size_t len, loff_t * offset);
static int open_file(struct inode * wezel, struct file * plik);
static int close_file(struct inode * wezel, struct file * plik);

//(vtable) = virtual table which has an array of pointers to virtual functions. 
struct file_operations fpos = {

    .write = write_file,
    .open = open_file,
    .release = close_file,
    .owner = THIS_MODULE

};

struct cdev cdev_file;
dev_t devno;
int err;
int gpio_pin = 17;



static int __init init_TheDiode(void)
{
    /*if(!gpio_is_valid(gpio_pin))
    {
        printk("Invalid GPIO Pin \"%d\"\n", gpio_pin);
        return -1;
    }

    if(gpio_request(gpio_pin, "TheDiode"))
    {
        printk("The GPIO Pin \"%d\" can not be requested\n", gpio_pin);
        return -1;
    }
    gpio_direction_output(gpio_pin,0);
    */
    //setting major and minor numbers
    devno = MKDEV(140, 0);
    register_chrdev_region(devno, 1, "TheDiode");
    
    cdev_init(&cdev_file, &fpos);
    cdev_file.owner = THIS_MODULE;

    err = cdev_add(&cdev_file,devno,1);

    if(err < 0)
    {
        printk("Devide add Error :/ \n");
        return -1;
    }
    else
    {
        printk("Device added to the system successfully :) \n");
    }
    
    return 0;
}

static void __exit module_remove(void)
{
    //gpio_set_value(gpio_pin,0);
    //gpio_free(gpio_pin);
    unregister_chrdev_region(devno,1);
    printk("Goodbye my friend (^-^)/ See you soon I guess (=^3^)= \n");
}


void blink(unsigned char * buff, int len)
{
    int val = 0;
    int temp = 0;
    int dec = 1;
    int y;
    int i;
    printk("len:%d > ", len);

    for(i = 0; i<len; i++)
    {
        
        temp = (int)(buff[i]) - 48;
        printk("%d | ", temp);
        dec = 1;
        
        for(y = 0; y<len-1-i;y++)
        {
            dec = dec * 10;
        }

        val = val + dec*temp;
    }

    printk("\n");

    for(i = 0; i<val;i++)
    {
        printk("%d + \n", i);
        //gpio_set_value(gpio_pin,1);
        msleep(100);
        //gpio_set_value(gpio_pin,0);
        msleep(100);
    }
    printk("\n");
}


static int open_file(struct inode * wezel, struct file * plik)
{
    return 0;
}

static int close_file(struct inode * wezel, struct file * plik)
{
    return 0;
}

static ssize_t write_file(struct file* f, const char * buff, size_t len, loff_t * offset)
{
    //during sending the message as string the len is +1 then so remember to -1
    //before to send data to the function

    unsigned long rest;
    unsigned char localbuff[257];
    rest = copy_from_user(localbuff,buff,(256 > len) ? len : 256);
    rest = ((256 > len) ? len : 256) - rest;
    localbuff[rest]='\0';
    blink(localbuff,len-1);
    printk("TheDiode recived: %s", &localbuff[0]);

    return len;
}


module_init(init_TheDiode);
module_exit(module_remove);