#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#define DRIVER_NAME "rtl_ssd"

struct rtl_ssd_instance {
	void __iomem *regs;
};

static int rtl_ssd_probe(struct platform_device* pdev)
{
	struct rtl_ssd_instance *chip;
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
