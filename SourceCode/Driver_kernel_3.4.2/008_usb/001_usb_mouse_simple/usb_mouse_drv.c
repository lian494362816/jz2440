#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include "my_error.h"

#define USB_MOUSE_MODULE_NAME "usb mouse"

static struct usb_device_id usb_mouse_id_table [] = {
    {
        USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
                           USB_INTERFACE_SUBCLASS_BOOT,
                           USB_INTERFACE_PROTOCOL_MOUSE)
    },

    { }	/* Terminating entry */
};

static int usb_mouse_probe (struct usb_interface *intf, const struct usb_device_id *id)
{
    struct usb_device *dev = interface_to_usbdev(intf);

    printk("usb mouse cconnect \n");

    printk("bsdUSB:%x \n", dev->descriptor.bcdUSB);
    printk("idVendor:%x \n", dev->descriptor.idVendor);
    printk("idProduct:%x \n", dev->descriptor.idProduct);
    printk("bcdDevice:%x \n", dev->descriptor.bcdDevice);

    return 0;
}

static void usb_mouse_disconnect(struct usb_interface *intf)
{
    printk("use mouse disconnect \n");
}

static struct usb_driver usb_mouse_driver = {
	.name		= "myusbmouse",
	.probe		= usb_mouse_probe,
	.disconnect	= usb_mouse_disconnect,
	.id_table	= usb_mouse_id_table,
};

static int usb_mouse_init(void)
{
    int ret = 0;

    ret = usb_register(&usb_mouse_driver);
    if (ret)
    {
        PRINT_ERR("use reg fail \n");
        return -1;
    }

    PRINT_INFO("%s init ok! \n", USB_MOUSE_MODULE_NAME);

    return 0;
}

static void usb_mouse_exit(void)
{
    usb_deregister(&usb_mouse_driver);

    PRINT_INFO("%s exit \n", USB_MOUSE_MODULE_NAME);
}

module_init(usb_mouse_init);
module_exit(usb_mouse_exit);

MODULE_LICENSE("GPL");
