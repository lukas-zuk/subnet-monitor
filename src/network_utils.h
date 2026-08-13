#ifndef NETWORK_UTILS

#define NETWORK_UTILS

struct network_data 
{
    char name[64];
    char subnet[16];
};

int subnet_mask(int prefix_len);
void get_subnet(int ip, int mask, char *subnet_str, int subnet_size);
int compare_subnets(struct network_data *network_arr, int network_arr_count, struct ubus_context *ctx);

#endif