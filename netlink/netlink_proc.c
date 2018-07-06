
#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/skbuff.h>

#include "netlink_common.h"
#include "netlink_debug.h"

#define NL_PROC_DIR             DRV_NAME
#define NL_PROC_RXPKT_NAME      "rx_packet"
#define NL_PROC_TRIGER_NAME     "rx_trigger"

int nl_debug = 1;

static int nl_rx_trigger_seq_show(struct seq_file *sf, void *v)
{
    seq_printf(sf,  "Trigger RX packet\n");
    return 0;
}

static int nl_rx_trigger_seq_open(struct inode *inode, struct file *file)
{
    DBG("open rx_trigger\n");
    return single_open(file, nl_rx_trigger_seq_show, NULL);
}

static ssize_t nl_rx_trigger_proc_write(struct file *file, const char *buf,
        size_t bufsize, loff_t *off)
{
    u32 val;

    if (bufsize > 0) {
        if (1 == sscanf(buf, "%u", &val)) {
            if (val == 1) {
                DBG("Trigger RX packet\n");
                nl_output();
            }
        }
    }
    return bufsize;
}

static int nl_rx_pkt_seq_show(struct seq_file *sf, void *v)
{   
    unsigned long flags;
	const u8 *ptr;
    int rowsize = 16;
	int i, linelen, len, remaining;
	unsigned char linebuf[32 * 3 + 2 + 32 + 1];

    
    seq_printf(sf,  "RX packet buffer:\n");
    read_lock_irqsave(&rx_buff.rwlock, flags);
    if (rx_buff.buf) {
        len = remaining = rx_buff.data_len;
        ptr = rx_buff.buf;

        for (i = 0; i < len; i += rowsize) {
            linelen = (remaining > rowsize) ? rowsize : remaining;
            remaining -= rowsize;

            hex_dump_to_buffer(ptr + i, linelen, rowsize, 1/*groupsize*/,
                       linebuf, sizeof(linebuf), false/*ascii*/);

            seq_printf(sf, "%.8x: %s\n", i, linebuf);
        }
    }
    read_unlock_irqrestore(&rx_buff.rwlock, flags);

    return 0;
}

static int nl_rx_pkt_seq_open(struct inode *inode, struct file *file)
{
    DBG("open rx_pkt\n");
    return single_open(file, nl_rx_pkt_seq_show, NULL);
}   

static ssize_t nl_rx_pkt_proc_write(struct file *filp, const char *buf,
                                size_t bufsize, loff_t * off)
{                               
    DBG("Update RX packet buffer\n");
    return bufsize;
}   

static int nl_stats_seq_show(struct seq_file *sf, void *v)
{   
    seq_printf(sf,  "RX packet: %lu\n", rx_pkt_count);
    seq_printf(sf,  "TX packet: %lu\n", tx_pkt_count);
    return 0;
}

static int nl_stats_seq_open(struct inode *inode, struct file *file)
{
    DBG("Show stats\n");
    return single_open(file, nl_stats_seq_show, NULL);
}   

static ssize_t nl_stats_proc_write(struct file *filp, const char *buf,
                                size_t bufsize, loff_t * off)
{
    if (bufsize > 0) {
        if (0 == strncmp(buf, "1", 1)) {
            DBG("Clear stats\n");
            rx_pkt_count = 0;
            tx_pkt_count = 0;
        }
    }
    return bufsize;
}

static const char * proc_node_names[] = {
    "rx_packet",
    "rx_trigger",
    "stats",
};

static const struct file_operations nl_proc_fops[] = {
    // rx_packet
    {
        .owner = THIS_MODULE,
        .open = nl_rx_pkt_seq_open,
        .read = seq_read,
        .write = nl_rx_pkt_proc_write,
        .llseek = seq_lseek,
        .release = single_release,
    },
    // rx_trigger
    {
        .owner = THIS_MODULE,
        .open = nl_rx_trigger_seq_open,
        .read = seq_read,
        .write = nl_rx_trigger_proc_write,
        .llseek = seq_lseek,
        .release = single_release,
    },
    // stats
    {
        .owner = THIS_MODULE,
        .open = nl_stats_seq_open,
        .read = seq_read,
        .write = nl_stats_proc_write,
        .llseek = seq_lseek,
        .release = single_release,
    }
};

static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *proc_entry[ARRAY_SIZE(nl_proc_fops)];

void nl_proc_create(void)
{
    char buf[64];
    u32 i;

	memset(proc_entry, 0, sizeof(proc_entry) );

	snprintf(buf, sizeof(buf), NL_PROC_DIR); 
	proc_dir = proc_mkdir(buf, NULL); 
	if (proc_dir == NULL) {
		pr_err("create proc dir %s failed\n", NL_PROC_DIR);
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0)
    for (i = 0; i < ARRAY_SIZE(nl_proc_fops); i++) {
        snprintf(buf, sizeof(buf), "%s", proc_node_names[i]);

        proc_entry[i] = proc_create_data(buf, 0644, proc_dir, &nl_proc_fops[i], NULL);
        if (proc_entry[i] == NULL) {
            pr_err("create proc %s failed\n", buf);
        }
        DBG("Create proc %s/%s\n", NL_PROC_DIR, proc_node_names[i]);
    }
#else
#error	Kernel version less than 3.10.0
#endif

}

void nl_proc_remove(void)
{
    char buf[64];
    u32  i;
    if (proc_dir) {
        for (i = 0; i < ARRAY_SIZE(nl_proc_fops); i++) {
            if (proc_entry[i]) {
                snprintf(buf, sizeof(buf), "%s", proc_node_names[i]);
                remove_proc_entry(buf, proc_dir);
            }
            proc_entry[i] = NULL;
            DBG("Remove proc %s/%s\n", NL_PROC_DIR, proc_node_names[i]);
        }

        remove_proc_entry(NL_PROC_DIR, NULL);
        proc_dir = NULL;
        DBG("Remove proc dir %s\n", NL_PROC_DIR);
    }

}
