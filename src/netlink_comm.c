#include <linux/rtnetlink.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <libubus.h>
#include <syslog.h>

#include "network_utils.h"

void log_errors(int err);

int send_network_dump_request(int fd, struct sockaddr_nl client)
{
    struct { struct nlmsghdr header; struct ifaddrmsg ifm; } request;

    memset(&request, 0, sizeof(request));

    request.header.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    request.header.nlmsg_type = RTM_GETADDR;
    request.header.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    request.header.nlmsg_seq = 1;

    struct iovec request_vector = { .iov_base = &request, .iov_len = request.header.nlmsg_len };
    struct msghdr message = { .msg_name = &client, .msg_namelen = sizeof(client), .msg_iov = &request_vector, .msg_iovlen = 1 };

    if (sendmsg(fd, &message, 0) < 0)
        return 1; // failed to send message

    return 0;
}

int read_network_list_from_response(int fd, struct sockaddr_nl client, struct network_data *network_arr, int arr_size, int *arr_count)
{
    int network_count = 0;
    char buf[8192];

    struct iovec response_vector = { .iov_base = buf, .iov_len = sizeof(buf) };
    struct msghdr response_msg = { .msg_name = &client, .msg_namelen = sizeof(client), .msg_iov = &response_vector, .msg_iovlen = 1 };
    
    ssize_t len = recvmsg(fd, &response_msg, 0);
    
    if (len < 0)
        return 2; // failed to read message


    struct nlmsghdr *response_header;

    for (response_header = (struct nlmsghdr *)buf; NLMSG_OK(response_header, len); response_header = NLMSG_NEXT(response_header, len))
    {
        if (response_header->nlmsg_type == NLMSG_DONE) 
            break;

        if (response_header->nlmsg_type == NLMSG_ERROR)
            return 3; // failed to get response

        if (response_header->nlmsg_type == RTM_NEWADDR) 
        {
            struct ifaddrmsg *addr_msg = NLMSG_DATA(response_header);

            struct rtattr *attr;
            int attrlen = IFA_PAYLOAD(response_header);

            for (attr = IFA_RTA(addr_msg); RTA_OK(attr, attrlen); attr = RTA_NEXT(attr, attrlen))
            {
                if (attr->rta_type == IFA_LOCAL && addr_msg->ifa_family == AF_INET) 
                {
                    char ifname[64];
                    char subnet[16];

                    if_indextoname(addr_msg->ifa_index, ifname);

                    struct in_addr *ip_addr = RTA_DATA(attr);
                    int ip = ntohl(ip_addr -> s_addr);
                    
                    int prefix_len = addr_msg -> ifa_prefixlen;
                    int mask = subnet_mask(prefix_len);
                                
                    get_subnet(ip,mask,subnet,sizeof(subnet));
                    
                    struct network_data network;
                    strncpy(network.name,ifname,sizeof(network.name));
                    strncpy(network.subnet,subnet,sizeof(network.subnet));

                    network_arr[network_count] = network;
                    network_count++;
                }
            }
        } 
    }
    *arr_count = network_count;
    return 0; 
}

int get_subnet_list(int fd, struct sockaddr_nl client, struct network_data *network_arr, int arr_size, int *arr_count)
{
    int err = send_network_dump_request(fd, client);

    if(err)
        return err;

    err = read_network_list_from_response(fd,client,network_arr,arr_size,arr_count);

    return err;
}

int check_for_conflicting_subnets(struct ubus_context *ctx)
{
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    
    if (fd < 0)
    {
        log_errors(4); // failed to create socket
        return 1;
    }
        
    struct sockaddr_nl unicast_client;
    memset(&unicast_client, 0, sizeof(unicast_client));
    unicast_client.nl_family = AF_NETLINK;

    if (bind(fd, (struct sockaddr *)&unicast_client, sizeof(unicast_client)) < 0) 
    {
        close(fd);
        log_errors(5); // failed to bind socket
        return 1; 
    }

    struct network_data network_arr[16];
    int network_arr_count;

    int err = get_subnet_list(fd, unicast_client,network_arr, sizeof(network_arr), &network_arr_count);

    if(err)
    {
        log_errors(err);
        return 1;        
    }

    compare_subnets(network_arr, network_arr_count, ctx);
    return 0;
}

void log_errors(int err)
{
    switch(err)
    {
        case 1:
            syslog(LOG_ERR, "Failed to send address request");
            break;
        case 2:
            syslog(LOG_ERR, "Failed to bind socket to client");
            break;
        case 3:
            syslog(LOG_ERR, "Failed to get response message");
            break;
        case 4:
            syslog(LOG_ERR, "Failed to create socket");
            break;
        case 5:
            syslog(LOG_ERR, "Failed to bind socket to client");
            break;
        default:
            syslog(LOG_ERR, "Unknown error");
            break;
    }        
}
