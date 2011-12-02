#include "us_world.h"
#ifdef __linux__
#include <sys/ioctl.h>        // Provides function ioctl()
#include <linux/if_packet.h>
#endif
#include "us_rawnet.h"

#ifdef __linux__
void join_multicast_group(int fd,char * interface_name,int protocol) {
    struct ifreq ifr;
    struct packet_mreq mreq;
    struct sockaddr_ll saddr;
    int interface_id;

    memset(&ifr,0,sizeof(ifr));
    strncpy(ifr.ifr_name,interface_name, sizeof(ifr.ifr_name)-1 );
    if ( ioctl(fd ,SIOCGIFINDEX,&ifr)<0 )
    {
        close( fd );
        return;
    }
    interface_id = ifr.ifr_ifindex;

    memset(&saddr,0,sizeof(saddr));
    saddr.sll_family = AF_PACKET;
    saddr.sll_ifindex = interface_id;
    saddr.sll_pkttype = PACKET_MULTICAST;
    saddr.sll_protocol = htons(protocol);
    if(bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0) {
		perror("bind");
		return;
	}

    memset(&mreq,0,sizeof(mreq));
    mreq.mr_ifindex=interface_id;
    mreq.mr_type=PACKET_MR_MULTICAST;
    mreq.mr_alen=6;
    mreq.mr_address[0]=0x91;
    mreq.mr_address[1]=0xe0;
    mreq.mr_address[2]=0xf0;
    mreq.mr_address[3]=0x00;
    mreq.mr_address[4]=0x00;
    mreq.mr_address[5]=0x01;
    
    if(setsockopt(fd,SOL_PACKET,PACKET_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0) {
        perror("setsockopt[SOL_SOCKET,PACKET_ADD_MEMBERSHIP]");
    } else {
        printf("joined multicast address\n");
    }
}
#endif

int main()
{
#ifdef __linux__
    int fd=socket(PF_PACKET,SOCK_RAW,htons(0x22f0) );
    if( fd>=0 )
    {
        uint8_t buf[2048];
        struct sockaddr_ll src_addr;

        join_multicast_group(fd,"eth1",0x22f0);

        socklen_t src_addr_len = sizeof(src_addr);
        int buf_len=0;
        do 
        {
            buf_len=recvfrom( fd, buf, sizeof(buf), 0, (struct sockaddr *)&src_addr, &src_addr_len );
            if( buf_len>=0 )
            {
                int i;
                printf( "Received EtherType 22f0 packet len=%d from: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        buf_len,
                        src_addr.sll_addr[0],
                        src_addr.sll_addr[1],
                        src_addr.sll_addr[2],
                        src_addr.sll_addr[3],
                        src_addr.sll_addr[4],
                        src_addr.sll_addr[5]
                      );
                for( i=0; i<buf_len; ++i )
                {
                    printf( "%02x ", buf[i] );
                }
                printf( "\n\n" );
            }
        } while( buf_len>0 );
    }
    perror("done");
#endif
}

