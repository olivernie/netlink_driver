#ifndef __NETLINK_COMMON_H__
#define __NETLINK_COMMON_H__

#include <linux/spinlock.h>

#define DRV_NAME "nl"
#define USER_NETLINK_SOCK     25
#define NL_GROUP_RX           1

typedef struct buffer_s {
    rwlock_t    rwlock;
    unsigned    buf_size;
    unsigned    data_len;
    char       *buf;
} buffer_t;

extern buffer_t rx_buff;
extern long rx_pkt_count;
extern long tx_pkt_count;

extern void nl_proc_create(void);
extern void nl_proc_remove(void);
extern void nl_output(void);


#endif  //__NETLINK_COMMON_H__

