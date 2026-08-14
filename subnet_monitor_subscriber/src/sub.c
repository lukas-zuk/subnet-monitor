#include <stdio.h>
#include <libubus.h>
#include <libubox/blobmsg_json.h>

static int notify_cb(struct ubus_context *ctx,struct ubus_object *obj,struct ubus_request_data *req,const char *method,struct blob_attr *msg)
{
    printf("notification: %s\n", method);
    
    char *json = blobmsg_format_json(msg, true);
    printf("Received: %s\n", json);
    free(json);

    return 0;
}

struct ubus_subscriber sub = { .cb = notify_cb };

int main()
{
    struct ubus_context *ctx;
    ctx = ubus_connect(NULL);
	
    if (!ctx) 
    {
        printf("ubus err");
        return 1;
	}

    ubus_register_subscriber(ctx, &sub);
    
    uint32_t id;

    int err = ubus_lookup_id(ctx, "subnet_publisher", &id);

    if(err)
        printf("lookupid error\n");

    err = ubus_subscribe(ctx, &sub, id);

    if(err)
        printf("subscribe error: %s\n", ubus_strerror(err));

    uloop_init();
    ubus_add_uloop(ctx);
    uloop_run();

    printf("sw");
    return 0;
}