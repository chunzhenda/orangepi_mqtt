/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-12-11 21:53:07
 * @LastEditTime : 2022-06-15 23:03:30
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>

#include "mqtt_config.h"
#include "mqtt_log.h"
#include "mqttclient.h"

#include "ca.inc"

// #define TEST_USEING_TLS  

static void topic1_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void *mqtt_publish_thread(void *arg)
{
    mqtt_client_t *client = (mqtt_client_t *)arg;

    char buf[100] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "welcome to mqttclient, this is a publish test...");

    sleep(2);

    mqtt_list_subscribe_topic(client);

    msg.payload = (void *) buf;
    
    while(1) {
        sprintf(buf, "welcome to mqttclient, this is a publish test, a rand number: %d ...", random_number());

        msg.qos = 0;
        mqtt_publish(client, "Linux_Topic", &msg);

        msg.qos = 1;
        mqtt_publish(client, "Linux_Topic", &msg);

        msg.qos = 2;
        mqtt_publish(client, "Linux_Topic", &msg);
        
        sleep(4);
    }
}

int main(void)
{
    int res;
    pthread_t thread1;
    mqtt_client_t *client = NULL;
    char client_id[32];
    char user_name[32];
    char password[32];

    printf("\nwelcome to mqttclient test...\n");

    random_string(client_id, 10);
    random_string(user_name, 10);
    random_string(password, 10);

    mqtt_log_init();

    client = mqtt_lease();

#ifdef TEST_USEING_TLS
    mqtt_set_port(client, "8883");
    mqtt_set_ca(client, (char*)test_ca_get());
#else
    mqtt_set_port(client, "1883");
#endif

    mqtt_set_host(client, "110.41.63.154");
    mqtt_set_client_id(client, "orange_pi");
    mqtt_set_user_name(client, "zhuoliang");
    mqtt_set_password(client, "82868753");
    mqtt_set_clean_session(client, 1);

    mqtt_connect(client);
    
    mqtt_subscribe(client, "ESP32_Topic", QOS0, topic1_handler);
    mqtt_subscribe(client, "ESP32_Topic", QOS1, NULL);
    mqtt_subscribe(client, "ESP32_Topic", QOS2, NULL);
    
    res = pthread_create(&thread1, NULL, mqtt_publish_thread, client);
    if(res != 0) {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    while (1) {
        sleep(100);
    }
}
