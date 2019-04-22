#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>
#include "my_error.h"

#include <linux/usb.h>



#define MODULE_NAME "usb mouse key"


struct usb_mouse_dev_t {
    struct usb_device *dev;
    size_t size;
    dma_addr_t phy_addr;
    void *vir_addr;
    struct urb *urb;
};

static struct usb_mouse_dev_t usb_mouse_dev;

static struct usb_device_id usb_mouse_key_id_table [] = {
    {
        USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
                          USB_INTERFACE_SUBCLASS_BOOT,
                          USB_INTERFACE_PROTOCOL_MOUSE)
    },

    { }	/* Terminating entry */
};

static void usb_mouse_key_print_desc(struct usb_device * dev)
{
    PRINT_INFO("bLength:0x%x \n",              dev->descriptor.bLength);
    PRINT_INFO("bDescriptorType:0x%x \n",      dev->descriptor.bDescriptorType);
    PRINT_INFO("bcdUSB:0x%x \n",               dev->descriptor.bcdUSB);
    PRINT_INFO("bDeviceClass:0x%x \n",         dev->descriptor.bDeviceClass);
    PRINT_INFO("bDeviceSubClass:0x%x \n",      dev->descriptor.bDeviceSubClass);
    PRINT_INFO("bDeviceProtocol:0x%x \n",      dev->descriptor.bDeviceProtocol);
    PRINT_INFO("bMaxPacketSize0:0x%x \n",      dev->descriptor.bMaxPacketSize0);
    PRINT_INFO("idVendor:0x%x \n",             dev->descriptor.idVendor);
    PRINT_INFO("idProduct:0x%x \n",            dev->descriptor.idProduct);
    PRINT_INFO("bcdDevice:0x%x \n",            dev->descriptor.bcdDevice);
    PRINT_INFO("iManufacturer:0x%x \n",        dev->descriptor.iManufacturer);
    PRINT_INFO("iProduct:0x%x \n",             dev->descriptor.iProduct);
    PRINT_INFO("iSerialNumber:0x%x \n",        dev->descriptor.iSerialNumber);
    PRINT_INFO("bNumConfigurations:0x%x \n",   dev->descriptor.bNumConfigurations);
}

static void usb_mouse_key_irq(struct urb *urb)
{
    int i = 0;
    struct usb_mouse_dev_t *p = (struct usb_mouse_dev_t *)urb->context;
    char *data = p->vir_addr;

    /* BYTE1 [0]left [1]right [2]mind
     *
    */
    //PRINT_INFO("left:%d, right:%d, mind:%d \n", data[1] & 0x1, data[1] & 0x2, data[1] & 0x4);

#if 1
    PRINT_INFO("data:");
    for (i = 0; i < p->size; i++)
    {
        printk("%2x ", data[i]);
    }
    printk("\n");
#endif

    if (usb_submit_urb(p->urb, GFP_KERNEL))
    {
        PRINT_INFO("submit urb fail \n");
    }
}

static int usb_mouse_key_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    struct usb_host_interface *interface;
    struct usb_endpoint_descriptor *endpoint;
    int error = -ENOMEM;
    int pipe;
    interface = intf->altsetting;
    endpoint = &interface->endpoint[0].desc;

    usb_mouse_dev.dev = interface_to_usbdev(intf);
#if 0
    usb_mouse_key_print_desc(usb_mouse_dev.dev);
#endif
    usb_mouse_dev.size = endpoint->wMaxPacketSize;
    PRINT_INFO("size:%d \n", usb_mouse_dev.size);
    pipe = usb_rcvintpipe(usb_mouse_dev.dev, endpoint->bEndpointAddress);

    usb_mouse_dev.vir_addr = usb_alloc_coherent(usb_mouse_dev.dev, usb_mouse_dev.size, GFP_ATOMIC, &usb_mouse_dev.phy_addr);
    if (!usb_mouse_dev.vir_addr)
    {
        PRINT_ERR("usb buffer alloc fail \n");
        return error;
    }

    usb_mouse_dev.urb = usb_alloc_urb(0, GFP_KERNEL);
    if (!usb_mouse_dev.urb)
    {
        PRINT_ERR("usb urb alloc fail \n");
        goto err_free_buff;
    }

    usb_fill_int_urb(usb_mouse_dev.urb,
                    usb_mouse_dev.dev,
                    pipe,
                    usb_mouse_dev.vir_addr,
                    usb_mouse_dev.size,
                    usb_mouse_key_irq,
                    &usb_mouse_dev,
                    endpoint->bInterval
    );

    usb_mouse_dev.urb->transfer_dma = usb_mouse_dev.phy_addr;
    usb_mouse_dev.urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

    if (usb_submit_urb(usb_mouse_dev.urb, GFP_KERNEL))
    {
        PRINT_ERR("submit urb fail \n");
        goto err_free_urb;
    }

    PRINT_INFO("usb mouse key connect \n");

    return 0;

err_free_buff:
    usb_free_coherent(usb_mouse_dev.dev, usb_mouse_dev.size, usb_mouse_dev.vir_addr, usb_mouse_dev.phy_addr);

err_free_urb:
    usb_free_urb(usb_mouse_dev.urb);

    return error;
}

static void usb_mouse_disconnect(struct usb_interface *intf)
{
    usb_kill_urb(usb_mouse_dev.urb);

    usb_free_urb(usb_mouse_dev.urb);

    usb_free_coherent(usb_mouse_dev.dev, usb_mouse_dev.size, usb_mouse_dev.vir_addr, usb_mouse_dev.phy_addr);

    PRINT_INFO("usb mouse key disconnect \n");
}

static struct usb_driver usb_mouse_key_driver = {
    .name = MODULE_NAME,
    .probe = usb_mouse_key_probe,
    .disconnect = usb_mouse_disconnect,
    .id_table = usb_mouse_key_id_table,
};

static int usb_mouse_key_init(void)
{
    if (usb_register(&usb_mouse_key_driver))
    {
        PRINT_ERR("usb reg fail \n");
        return -1;
    }

    PRINT_INFO("%s init \n", MODULE_NAME);

    return 0;
}

static void usb_mouse_key_exit(void)
{
    usb_deregister(&usb_mouse_key_driver);

    PRINT_INFO("%s exit \n", MODULE_NAME);
}

module_init(usb_mouse_key_init);
module_exit(usb_mouse_key_exit);

MODULE_LICENSE("GPL");


