#ifndef __NETLINK_DEBUG_H__
#define __NETLINK_DEBUG_H__

#include <linux/printk.h>

extern int nl_debug;

#define DBG(fmt, ...) if (nl_debug) printk(KERN_DEBUG fmt, ##__VA_ARGS__)

#endif  //__NETLINK_DEBUG_H__
