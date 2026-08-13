#ifndef NETLINK_COMM

#define NETLINK_COMM

#include "network_utils.h"

int get_subnet_list(int fd, struct sockaddr_nl client, struct network_data *network_arr, int arr_size, int *arr_count);
int send_network_dump_request(int fd, struct sockaddr_nl client);
int read_network_list_from_response(int fd, struct sockaddr_nl client, struct network_data *network_arr, int arr_size, int *arr_count);
int check_for_conflicting_subnets(struct ubus_context *ctx);


#endif