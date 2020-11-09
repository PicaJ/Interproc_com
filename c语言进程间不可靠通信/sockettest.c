#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

void *socketsend(){
	#define FACTORY_TEST_PORT 8313
    struct    sockaddr_in servaddr = {};
    socklen_t socklen  = sizeof(struct sockaddr_in);
    int       sock     = -1;
    char      txbuf[512];
    char      rxbuf[128];
    int       opt, ret, id;
	char *msg = "apply_settting";
	char * param = "param1";

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) 
        return -1;
    opt = 10; setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO , (char*)&opt, sizeof(int));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //lo, 回环地址
    servaddr.sin_port        = htons(FACTORY_TEST_PORT);
    snprintf(txbuf, sizeof(txbuf), "rpc! %d %s %s", 20, msg, param);
    ret = sendto  (sock, txbuf, strlen(txbuf) + 1, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	printf("socketsend first send:%s!\n",txbuf);
    ret = recvfrom(sock, rxbuf, sizeof(rxbuf)    , 0, (struct sockaddr*)&servaddr, &socklen);
	printf("socketsend rec:%s\n",rxbuf);
	sprintf(txbuf,"socketsend send again:%s",rxbuf);
	ret = sendto  (sock, txbuf, strlen(txbuf) + 1, 0, (struct sockaddr*)&servaddr, sizeof(servaddr));
	printf("socketsend again send:%s!\n",txbuf);
    close(sock);
}

int main(){
	struct sockaddr_in server = {};
	struct sockaddr_in client = {};
	socklen_t clientlen  = sizeof(client);
	struct    timeval tv = {1};
	int       sock       = -1;
	char      msg[512];
	char      msgsend[500];
	int       len;
	pthread_t socketsed;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("failed to open factory test socket !\n");
		return NULL;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(((in_addr_t) 0x00000000));
	server.sin_port        = htons(8313);
	if (bind(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("failed to bind factory test socket to port %d !\n", 8313);
		close(sock);
		return NULL;
	}

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, sizeof(tv));

	pthread_create(&socketsed,NULL,socketsend,NULL);

	memset(msg, 0, sizeof(msg));
    len = recvfrom(sock, (void*)msg, sizeof(msg), 0, (struct sockaddr*)&client, &clientlen);
	printf("main recv first%s\n",msg);
	sprintf(msgsend,"main send masge");
    sendto(sock, msgsend, len, 0, (struct sockaddr*)&client, clientlen);
	len = recvfrom(sock, (void*)msg, sizeof(msg), 0, (struct sockaddr*)&client, &clientlen);
	printf("main recv again:%s\n",msg);	
	close(socket);
	pthread_join(socketsed,NULL);
}
