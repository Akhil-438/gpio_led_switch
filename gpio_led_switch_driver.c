#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>

#define DEVICE_NAME "gpiodemo"

static struct gpio_desc *led_gpio;
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

/* Write: user writes 0/1 to switch LED off/on */
static ssize_t gpiodemo_write(struct file *f, const char __user *buf,
                              size_t len, loff_t *off)
{
    char kbuf[16];
    int value;

    if (len > sizeof(kbuf) - 1)
        len = sizeof(kbuf) - 1;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    if (kstrtoint(kbuf, 10, &value))
        return -EINVAL;

    gpiod_set_value(led_gpio, value ? 1 : 0);
    return len;
}

static struct file_operations gpiodemo_fops = {
    .owner = THIS_MODULE,
    .write = gpiodemo_write
};

static int gpiodemo_probe(struct platform_device *pdev)
{
    int ret;

    led_gpio = devm_gpiod_get(&pdev->dev, "led", GPIOD_OUT_LOW);
    if (IS_ERR(led_gpio))
        return PTR_ERR(led_gpio);

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&c_dev, &gpiodemo_fops);
    ret = cdev_add(&c_dev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    cl = class_create(DEVICE_NAME);
    if (IS_ERR(cl)) {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(cl);
    }

    device_create(cl, NULL, dev, NULL, DEVICE_NAME);
    pr_info("gpiodemo: driver loaded\n");
    return 0;
}

static void gpiodemo_remove(struct platform_device *pdev)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, 1);
    pr_info("gpiodemo: driver removed\n");
}

static const struct of_device_id gpiodemo_dt_ids[] = {
    { .compatible = "gpiodemo" },
    { }
};
MODULE_DEVICE_TABLE(of, gpiodemo_dt_ids);

static struct platform_driver gpiodemo_driver = {
    .probe = gpiodemo_probe,
    .remove = gpiodemo_remove,
    .driver = {
        .name = DEVICE_NAME,
        .of_match_table = gpiodemo_dt_ids,
    },
};

module_platform_driver(gpiodemo_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akhil");
MODULE_DESCRIPTION("Driver (/dev/gpiodemo)");

