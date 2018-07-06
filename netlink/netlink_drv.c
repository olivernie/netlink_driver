#include <linux/version.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/printk.h>
#include <linux/skbuff.h>

#include "netlink_common.h"
#include "netlink_debug.h"

#define  MAX_PACKET_SIZE    2048

buffer_t rx_buff;

static struct sock *nl_sock;
long rx_pkt_count = 0;
long tx_pkt_count = 0;


static char data[] = 
"\x01\x00\x5e\x00\x00\x16\x24\x8a\x07\xa4\x6b\x89\x08\x00\x46\xc0"
"\x00\x28\x00\x00\x40\x00\x01\x02\xda\x81\x0a\x14\x1f\x64\xe0\x00"
"\x00\x16\x94\x04\x00\x00\x22\x00\xe9\xf9\x00\x00\x00\x01\x03\x00"
"\x00\x00\xef\x01\x02\x03\x55\xaa\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x01\x00\x5e\x00\x00\x16\x24\x8a\x07\xa4\x6b\x89\x08\x00\x46\xc0"
"\x00\x28\x00\x00\x40\x00\x01\x02\xda\x81\x0a\x14\x1f\x64\xe0\x00"
"\x00\x16\x94\x04\x00\x00\x22\x00\xe9\xf9\x00\x00\x00\x01\x03\x00"
"\x00\x00\xef\x01\x02\x03\x55\xaa\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x00\x01\x02\x03\x04\x05\x06\x07"
"\x01\x00\x5e\x00\x00\x16\x24\x8a\x07\xa4\x6b\x89\x08\x00\x46\xc0"
"\x00\x28\x00\x00\x40\x00\x01\x02\xda\x81\x0a\x14\x1f\x64\xe0\x00";

static void skb_clear(struct sk_buff *skb)
{
    skb_trim(skb, 0);       // give up data, then skb->len = 0.
}

static int skb_set(struct sk_buff *skb, void *data, unsigned int len)
{
	void *tmp;
    if (len  > skb_tailroom(skb)) {
        return -ENOBUFS;
    }

	tmp = skb_put(skb, len);
	memcpy(tmp, data, len);
    return 0;
}

static void skb_dump(struct sk_buff *skb, int rx)
{           
	DBG("%s packet:\n", rx ? "RX" : "TX");
    print_hex_dump(KERN_DEBUG, "", DUMP_PREFIX_NONE,
            16, 1, skb->data, skb->len, false);
}   

static void nl_input(struct sk_buff *skb)
{
    if (!skb) {
        DBG("%s: skb is null\n", __FUNCTION__);
        return;
    }

    tx_pkt_count++;
	skb_dump(skb, 0);	// rx: 1, tx: 0.
}

void nl_output(void)
{
    int rc;
    u32 nl_group = NL_GROUP_RX;
    unsigned long flags;
    struct sk_buff *skb;

	if (!nl_sock) {
        DBG("%s: netlink_is not initialized!\n", __FUNCTION__);
    }

    skb = dev_alloc_skb(MAX_PACKET_SIZE);
    if (!skb) return;

    read_lock_irqsave(&rx_buff.rwlock, flags);
    if (rx_buff.buf && rx_buff.data_len > 0) {
        skb_set(skb, rx_buff.buf, rx_buff.data_len);
    }
    else {
        goto _error_unlock;
    }
    read_unlock_irqrestore(&rx_buff.rwlock, flags);

    rc = netlink_broadcast(nl_sock, skb, 0/*pid*/, nl_group, GFP_ATOMIC);
    if (rc < 0) {
        pr_err("%s: netlink_broadcast failed (rc=%d)\n", __FUNCTION__, rc);
    }

    rx_pkt_count++;
    return;

_error_unlock:
    read_unlock_irqrestore(&rx_buff.rwlock, flags);
//_error_free:
    dev_kfree_skb_any(skb);
}

static int __init nl_init(void)
{
    unsigned long flags;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0)
    struct netlink_kernel_cfg cfg = { 
        .groups = 0,
        .cb_mutex = NULL,
        .input  = nl_input,
        .bind = NULL,           
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
        .flags  = NL_CFG_F_NONROOT_RECV,
#endif
    };  
#endif

    if (nl_sock != NULL) {   
        /* The socket already exist return an error. */
        return -EINVAL;
    }

    rwlock_init(&rx_buff.rwlock);
    write_lock_irqsave(&rx_buff.rwlock, flags);
    rx_buff.buf = kmalloc(2048, GFP_ATOMIC);
    if (rx_buff.buf) {
        memcpy(rx_buff.buf, data, sizeof(data) - 1);
        rx_buff.buf_size = 2048;
        rx_buff.data_len = sizeof(data) - 1;
    }
    else {
        write_unlock_irqrestore(&rx_buff.rwlock, flags);
        return -ENOMEM;
    }
    write_unlock_irqrestore(&rx_buff.rwlock, flags);

    /* Initialize the pipe between API in user space and this kernel module. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 9, 0)
    nl_sock = netlink_kernel_create(&init_net, USER_NETLINK_SOCK, &cfg);
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 6, 0)
    nl_sock = netlink_kernel_create(&init_net, USER_NETLINK_SOCK, THIS_MODULE, &cfg);
#else
    nl_sock = netlink_kernel_create(&init_net,
                                      USER_NETLINK_SOCK,
                                      0,  
                                      nl_input,
                                      NULL,
                                      THIS_MODULE);
#endif

    if (nl_sock == NULL)
    {   
        pr_err("BUG! netlink_kernel_create failed.\n");
        return -EIO;
    }

    DBG("netlink created: %d\n", USER_NETLINK_SOCK);

    nl_proc_create();

    return 0;
}

static void __exit nl_exit(void)
{
    unsigned long flags;
    nl_proc_remove();

    write_lock_irqsave(&rx_buff.rwlock, flags);
    if (rx_buff.buf) {
        kfree(rx_buff.buf);
        rx_buff.buf = NULL;
    }
    write_unlock_irqrestore(&rx_buff.rwlock, flags);

    if (nl_sock) {
        netlink_kernel_release(nl_sock);
        nl_sock = NULL;
    }
    DBG("netlink released: %d\n", USER_NETLINK_SOCK);
}

module_init(nl_init);
module_exit(nl_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Oliver Nie <nieyuguo@gmail.com>");
MODULE_DESCRIPTION("netlink driver");

