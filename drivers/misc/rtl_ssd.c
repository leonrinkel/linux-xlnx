#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#define DRIVER_NAME "rtl_ssd"
#define DEVICE_NAME "ssd"
#define BUFLEN 32

// dirty poc
// TODO: unregister, cleanup, free, etc.

struct rtl_ssd_instance {
	void __iomem *regs;
};

static int open_count = 0;
static char buffer[BUFLEN];

struct rtl_ssd_instance *chip;

static int misc_dev_open(struct inode *inode, struct file *file)
{
	if (open_count)
	{
		return -EBUSY;
	}

	open_count++;

	return 0;
}

static int misc_dev_close(struct inode *inode, struct file *file)
{
	open_count--;

	return 0;
}

static ssize_t misc_dev_write(struct file *file, const char *src, size_t len, loff_t *off)
{
	int bytes_written;
	for (
			bytes_written = 0;
			bytes_written < BUFLEN && bytes_written < len;
			bytes_written++
	)
	{
		get_user(buffer[bytes_written], src++);
	}

	long value;
	
	int ret;
	ret = kstrtol(buffer, 10, &value);
	if (ret != 0)
	{
		return ret;
	}

	__raw_writel((u32) value, chip->regs);

	return bytes_written;
}

static const struct file_operations misc_dev_fops = {
	.owner = THIS_MODULE,
	.open = misc_dev_open,
	.release = misc_dev_close,
	.write = misc_dev_write,
};

static struct miscdevice misc_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &misc_dev_fops,
};

static int rtl_ssd_probe(struct platform_device* pdev)
{
	//struct rtl_ssd_instance *chip;
	chip = devm_kzalloc(&pdev->dev, sizeof(*chip), GFP_KERNEL);
	if (!chip)
	{
		return -ENOMEM;
	}

	chip->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(chip->regs))
	{
		dev_err(&pdev->dev, "failed to remap\n");
		return PTR_ERR(chip->regs);
	}

	__raw_writel(69, chip->regs);

	int ret;
	ret = misc_register(&misc_dev);
	if (ret != 0)
	{
		dev_err(&pdev->dev, "failed to register miscdev\n");
		return ret;
	}

	printk(KERN_INFO "hello world from rtl_ssd\n");
	return 0;
}

static struct of_device_id rtl_ssd_match[] = {
	{ .compatible = "rtl_ssd_driver" },
	{0}
};
MODULE_DEVICE_TABLE(of, rtl_ssd_match);

static struct platform_driver rtl_ssd = {
	.probe = rtl_ssd_probe,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(rtl_ssd_match),
	},	
};

module_platform_driver(rtl_ssd);

MODULE_VERSION("0.0.1");
MODULE_DESCRIPTION("seven segment display driver");
MODULE_AUTHOR("Leon Rinkel <leon@rinkel.me>");
MODULE_LICENSE("GPL v2");
