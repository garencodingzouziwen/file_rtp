#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "llist.h"
#include "h264tortp.h"
#include "debug_print.h"

//$send_h264file_rtp test1.h264 127.0.0.1 1234

uint16_t dest_port;
uint8_t nal_buf[NAL_BUF_SIZE];
linklist client_ip_list;

static void add_client_list(linklist client_ip_list, char *ipaddr)
{
    struct sockaddr_in server_c;
    pnode pnode_tmp;
    const int on = 1;
    insert_nodulp_node(client_ip_list, ipaddr);
    pnode_tmp = search_node(client_ip_list, ipaddr);
    server_c.sin_family = AF_INET;
    server_c.sin_port = htons(dest_port);
    server_c.sin_addr.s_addr = inet_addr(ipaddr);
    pnode_tmp->send_fail_n = 0;
    pnode_tmp->node_info.socket_c = socket(AF_INET, SOCK_DGRAM, 0);
    /** 设置广播属性*/
    if (setsockopt(pnode_tmp->node_info.socket_c, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
    {
        fprintf(stderr, "initSvr: Socket options set error.\n");
        exit(errno);
    }
    if ((connect(pnode_tmp->node_info.socket_c, (const struct sockaddr *)&server_c, sizeof(struct sockaddr_in))) == -1)
    {
        perror("connect");
        exit(-1);
    }
    return;
}

int main(int argc, char **argv)
{
    FILE *fp;
    int ret;
    int len;
    if (argc < 4)
    {
        fprintf(stderr, "usage: %s <inputfile> <dstip> [dst_port]\n", argv[0]);
        return -1;
    }
    //fopen h264 file
    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        perror("fopen");
        exit(errno);
    }
    //port
    dest_port = atoi(argv[3]);
    client_ip_list = create_null_list_link();
    add_client_list(client_ip_list, argv[2]);
    //read h264 & rtp send h264
    printf("filename = %s ip = %s port = %d!\n", argv[1], argv[2], dest_port);
    while (copy_nal_from_file(fp, nal_buf, &len) != -1)
    {
        ret = h264nal2rtp_send(30, nal_buf, len, client_ip_list);
        if (ret != -1)
        {
            usleep(1000 * 30);
        }
        else
        {
            printf("h264nal2rtp_send() error!\n");
        }
    }
    fclose(fp);
    return 0;
}
