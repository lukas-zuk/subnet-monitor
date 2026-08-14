#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include "ubus_utils.h"

static int check_subscribers(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method,struct blob_attr *msg);

static struct blob_buf b;


static const struct ubus_method publisher_methods[] = {
	UBUS_METHOD_NOARG("has_subscribers", check_subscribers),
};

static struct ubus_object_type publisher = UBUS_OBJECT_TYPE("subnet_monitor", publisher_methods);

static struct ubus_object publisher_obj = {
	.name = "subnet_publisher",
	.type = &publisher,
	.methods = publisher_methods,
	.n_methods = 1,
};

int add_object_to_ubus(struct ubus_context *ctx)
{
	int err = ubus_add_object(ctx, &publisher_obj);
    if(err != 0)
        return 1;

    return 0;
}

int send_duplicates_notification(struct ubus_context *ctx, char *subnet, char *name1, char *name2)
{
	blob_buf_init(&b, 0);
	blobmsg_add_string(&b, "msg", "Conflicting subnets");
	
	void *obj = blobmsg_open_table(&b, "data");
	blobmsg_add_string(&b, "subnet", subnet);
	blobmsg_add_string(&b, "name_1", name1);
	blobmsg_add_string(&b, "name_2", name2);
	blobmsg_close_table(&b, obj);
	
	int err = ubus_notify(ctx, &publisher_obj, "subnet_duplication", b.head, 1000);
	blob_buf_free(&b);

	if (err) // failed to send out notification
		return 1;

	syslog(LOG_INFO, "Found duplicate subnet (%s) for networks %s and %s",subnet, name1, name2);

	return 0;
}

static int check_subscribers(struct ubus_context *ctx, struct ubus_object *obj, struct ubus_request_data *req, const char *method,struct blob_attr *msg)
{	
	if (publisher_obj.has_subscribers)
	  printf("Someone is subscribed\n");
        else
	  printf("No subscribers\n");
	
	return 0; // not impelemented, ubus requires at least 1 method
}
