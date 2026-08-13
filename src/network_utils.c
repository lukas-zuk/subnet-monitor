#include <string.h>
#include <arpa/inet.h>
#include <libubus.h>

#include "network_utils.h"
#include "ubus_utils.h"

int subnet_mask(int prefix_len)
{
    return prefix_len ? ~0 << (32 - prefix_len) : 0;
}

void get_subnet(int ip, int mask, char *subnet_str, int subnet_size)
{
    int subnet = ip & mask;
    struct in_addr subnet_addr;
    subnet_addr.s_addr = htonl(subnet);

    inet_ntop(AF_INET, &subnet_addr, subnet_str, subnet_size);
}

int compare_subnets(struct network_data *network_arr, int network_arr_count, struct ubus_context *ctx)
{
    for(int i = 0; i < network_arr_count-1; i++)
    {
        for(int j = i+1; j < network_arr_count; j++)
        {
            if(strcmp(network_arr[i].subnet,network_arr[j].subnet) == 0)
            {
                int err = send_duplicates_notification(ctx, network_arr[i].subnet, network_arr[i].name,network_arr[j].name);
                
                if(err)
                    return 1;
            }      
        }
    }
    return 0;
}