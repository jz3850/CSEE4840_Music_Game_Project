#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "audio.h"

#define DRIVER_NAME "audio"

/* Register Offsets */
#define AUDIO_CONTROL     0x00
#define AUDIO_FIFOSPACE   0x04
#define AUDIO_LEFTDATA    0x08
#define AUDIO_RIGHTDATA   0x0C

/* Control Bits */
#define CONTROL_CW (1 << 3)
#define CONTROL_WE (1 << 1)

static void __iomem *audio_base;

static long audio_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    audio_sample_t smp;

    switch (cmd) {
    case AUDIO_WRITE_SAMPLE:
        if (copy_from_user(&smp, (void __user *)arg, sizeof(smp)))
            return -EFAULT;

        uint32_t fifospace = ioread32(audio_base + AUDIO_FIFOSPACE);
        uint8_t ws_l = (fifospace >> 24) & 0xFF;
        uint8_t ws_r = (fifospace >> 16) & 0xFF;
        if (ws_l > 0 && ws_r > 0) {
            iowrite32((uint32_t)(uint16_t)smp.left, audio_base + AUDIO_LEFTDATA);
            iowrite32((uint32_t)(uint16_t)smp.right, audio_base + AUDIO_RIGHTDATA);            
        }
            
        break;

    case AUDIO_READ_STATUS: {
        int status = ioread32(audio_base + AUDIO_FIFOSPACE);
        if (copy_to_user((void __user *)arg, &status, sizeof(int)))
            return -EFAULT;
        break;
    }

    default:
        return -EINVAL;
    }

    return 0;
}

static ssize_t audio_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    size_t i;
    uint8_t sample[4];

    for (i = 0; i + 3 < count; i += 4) {
        if (copy_from_user(sample, buf + i, 4))
            return -EFAULT;

        int16_t left  = sample[0] | (sample[1] << 8);
        int16_t right = sample[2] | (sample[3] << 8);

        // wait until FIFO has space 
        int retries = 1000;
        while (retries--) {
            uint32_t fifospace = ioread32(audio_base + AUDIO_FIFOSPACE);
            uint8_t ws_l = (fifospace >> 24) & 0xFF;
            uint8_t ws_r = (fifospace >> 16) & 0xFF;

            if (ws_l > 0 && ws_r > 0) {
                iowrite32((uint32_t)(uint16_t)left, audio_base + AUDIO_LEFTDATA);
                iowrite32((uint32_t)(uint16_t)right, audio_base + AUDIO_RIGHTDATA);
                break;
            }

            udelay(10);  
        }

        // still no spcae when out of time, exit
        if (retries <= 0)
            break;
    }

    return i;  // return byte number written in
}


static const struct file_operations audio_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = audio_ioctl,
    .write = audio_write,
};

static struct miscdevice audio_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "audio",
    .fops = &audio_fops,
};

static int audio_probe(struct platform_device *pdev)
{
    struct resource *res;
    int ret;

    pr_info("audio: probe()\n");

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        pr_err("audio: failed to get mem resource\n");
        return -ENODEV;
    }

    audio_base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(audio_base)) {
        pr_err("audio: ioremap failed\n");
        return PTR_ERR(audio_base);
    }

    // Reset FIFO
    iowrite32(CONTROL_CW | CONTROL_WE, audio_base + AUDIO_CONTROL);
    udelay(10);
    iowrite32(CONTROL_WE, audio_base + AUDIO_CONTROL);

    ret = misc_register(&audio_misc_device);
    if (ret) {
        pr_err("audio: misc_register failed\n");
        return ret;
    }

    pr_info("audio: driver loaded, base = %p\n", audio_base);
    return 0;
}

static int audio_remove(struct platform_device *pdev)
{
    misc_deregister(&audio_misc_device);
    pr_info("audio: removed\n");
    return 0;
}

static const struct of_device_id audio_of_match[] = {
    { .compatible = "altr,audio-0.18" },
    {},
};
MODULE_DEVICE_TABLE(of, audio_of_match);

static struct platform_driver audio_driver = {
    .probe = audio_probe,
    .remove = audio_remove,
    .driver = {
        .name = DRIVER_NAME,
        .of_match_table = audio_of_match,
    },
};
module_platform_driver(audio_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Minimal audio driver with FIFO write for DE1-SoC");
