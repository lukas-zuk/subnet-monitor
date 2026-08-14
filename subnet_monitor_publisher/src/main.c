#include <stdio.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <libubus.h>

#include "ubus_utils.h"
#include "netlink_comm.h"

int running = 1;

int main()
{
    int ret = 0;
    struct ubus_context *ctx;
    ctx = ubus_connect(NULL);
	
    if (!ctx) 
    {
        printf("Failed to connect to UBUS");
        return 1;
	}

    if(add_object_to_ubus(ctx) != 0)
    {
        printf("Failed to add UBUS object");
        return 1;
    }

    struct sockaddr_nl sa;

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;
    
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    bind(fd, (struct sockaddr *) &sa, sizeof(sa));

    char buf[8192];
    struct iovec iov = { buf, sizeof(buf) };
    struct msghdr msg = { &sa, sizeof(sa), &iov, 1, NULL, 0, 0 };
    
    while(running) 
    {
        int len = recvmsg(fd, &msg, 0);
        
        if (len < 0)
            break;

        struct nlmsghdr *header;
        for (header = (struct nlmsghdr *)buf; NLMSG_OK(header, len); header = NLMSG_NEXT(header, len))
        {
            if(check_for_conflicting_subnets(ctx) > 0)
            {
                running = 0;
                break;
            }            
        }
    }
    
    close(fd); 
    ubus_free(ctx);

    return ret;

}