/*
 * Main.cpp
 * Copyright (c) 2022, ZHAW
 * All rights reserved.
 */

#include <stdio.h>
#include <mbed.h>
// #include <EthernetInterface.h>
#include "EthernetInterface.h"
#include "NetworkInterface.h"

#include <stdio.h>
#include <string.h>

//#include <MQTTNetwork.h>
//#include <MQTTClient.h>
//#include <MQTTmbed.h> // Countdown
#include <MQTTClientMbedOs.h>
extern "C" void mbed_mac_address(char *s);

char printbuf[100];

int arrivedcount = 0;

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);
}

int main() {
    
    // create miscellaneous periphery objects
    
    DigitalOut led(LED1);
    
    ThisThread::sleep_for(500ms);
    
    UnbufferedSerial* serial = new UnbufferedSerial(PG_14, PG_9);
    
    // create ethernet interface
    
    printf("Create ethernet interface and webserver (please wait!)...\r\n");
    
    // DigitalOut enableRouter(PB_15);
    // enableRouter = 1;

    // Achtung umbedingt mbed_lib.json file importieren
    /*
    NetworkInterface *net = NetworkInterface::get_default_instance();
    MQTTNetwork mqttNet(net);
    MQTT::Client<MQTTNetwork, Countdown> client(mqttNet);

    const char* hostname = "192.168.0.10";
    int port = 1883;

    mqttNet.connect(hostname, port);
    //mqttNet.connect(mqtt_global::hostname, mqtt_global::port);*/

    const char* hostname = "192.168.0.10";
    int port = 1883;
    
    /*NetworkInterface *net = NetworkInterface::get_default_instance();
    TCPSocket socket;
    MQTTClient client(&socket);
    socket.open(net);
    socket.connect(hostname, port);
    //socket.connect(hostname, port);*/

    NetworkInterface *net = EthInterface::get_default_instance();

    if (!net) {
        printf("Error! No network inteface found.\n");
        return 0;
    }

    net->set_network("192.168.0.20", "255.255.255.0", "192.168.0.1");

    net->connect();
    printf("Ethernet Connected\n");

    // Show the network address
    SocketAddress a;
    net->get_ip_address(&a);
    printf("IP address: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");

    //MQTT
    TCPSocket socket; 
    SocketAddress addr; 
    socket.open(net);
    net->gethostbyname(hostname, &addr);
    addr.set_port(port); 
    socket.connect(addr);
    MQTTClient client(&socket); 
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer; 
    data.MQTTVersion = 3;

    data.clientID.cstring = (char *)"MQTT_CONNECT";
    client.connect(data);
    int rc;
    rc = client.subscribe("test", MQTT::QOS1, messageArrived);

    while (true) {

        MQTT::Message message;

        arrivedcount = 0;

        /*char buf[100] = "hello there";
        message.qos = MQTT::QOS1;
        message.payloadlen = strlen(buf)+1;
        message.payload = (void*)buf;
        rc = client.publish("test", message);*/
        while (arrivedcount == 0)
            client.yield(1000);      

        //ThisThread::sleep_for(250ms);
    }
}
