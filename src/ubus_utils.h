#ifndef UBUS_UTILS

#define UBUS_UTILS

int add_object_to_ubus(struct ubus_context *ctx);
int send_duplicates_notification(struct ubus_context *ctx, char *subnet, char *name1, char *name2);

#endif