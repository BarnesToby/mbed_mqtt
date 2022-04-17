#include "mbed.h"
#include "rtos.h"
#include <stdio.h>
#include <string.h>
#include <mbed.h>
#include "EthernetInterface.h"
#include "NetworkInterface.h"
#include <stdio.h>
#include <string.h>
#include <MQTTClientMbedOs.h>


#define COM_FLAG 1

Thread communication_thread;

void positionRequest(MQTT::MessageData& md);
void sonnenSegel(MQTT::MessageData& md);
void altitudeRequest(MQTT::MessageData& md);
void azimuthRequest(MQTT::MessageData& md);

void communication();

extern "C" void mbed_mac_address(char *s);
char printbuf[100];
int arrivedcount = 0;
const char* hostname = "192.168.50.223";
int port = 1883;


// Spawns a thread to run blink for 5 seconds
int main() {
	UnbufferedSerial* serial = new UnbufferedSerial(PG_14, PG_9);
	
    communication_thread.start(callback(communication));
    ThisThread::sleep_for(5000);
    communication_thread.flags_set(COM_FLAG);
    communication_thread.join();
}

// Blink function toggles the LED in a long running loop
void communication() {

    MQTT::Message message;	
	
	NetworkInterface *net = EthInterface::get_default_instance();

    if (!net) {
        printf("Error! No network inteface found.\n");
    }
    net->set_dhcp(true);
    //net->set_network("192.168.0.20", "255.255.255.0", "192.168.0.1");

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
    if(client.subscribe("Anfrage/Position", MQTT::QOS1, positionRequest) != NSAPI_ERROR_OK){
        // error Handler
    }
    if(client.subscribe("Anfrage/Sonnensegel", MQTT::QOS1, sonnenSegel) != NSAPI_ERROR_OK){
        // error Handler
    }
    if(client.subscribe("Anfrage/Sonnenparameter/Altitude", MQTT::QOS1, altitudeRequest) != NSAPI_ERROR_OK){
        // error Handler
    }
    if(client.subscribe("Anfrage/Sonnenparameter/Azimuth", MQTT::QOS1, azimuthRequest) != NSAPI_ERROR_OK){
        // error Handler
    }

    
    const char* test_topic = "Fahrzeugparameter/Location";
	
    while (!ThisThread::flags_wait_any_for(COM_FLAG, 1000)) {
        static MQTT::Message pubmsg;

        static char testchar[] = "5.27/3.69";
        // strcpy(testchar, (char) 10);

        memset(&pubmsg, '\0', sizeof(pubmsg));
        pubmsg.payload = (void*)"a much longer message that we can shorten to the extent that we need to payload up to 11";
        pubmsg.payloadlen = 11;
        pubmsg.qos = MQTT::QOS1;
        pubmsg.retained = false;
        pubmsg.dup = false;

        if(client.publish(test_topic, pubmsg) != NSAPI_ERROR_OK){
            //error Handler
        }

        /* wait for the message to be received */
        uint8_t wait_seconds = 10;
        while (wait_seconds-- > 0){
            client.yield(100);
        }
    }
}

void positionRequest(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);
}

void sonnenSegel(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);
}

void altitudeRequest(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);
}

void azimuthRequest(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    sprintf(printbuf, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);
}

