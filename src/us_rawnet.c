/*
Copyright (c) 2010, Meyer Sound Laboratories, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Meyer Sound Laboratories, Inc. nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MEYER SOUND LABORATORIES, INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"
#include "us_allocator.h"
#include "us_rawnet.h"
#include "us_logger.h"

#if US_ENABLE_RAW_ETHERNET

#if defined(US_CONFIG_LINUX)
#include <linux/if_packet.h>
#endif

#if defined(US_CONFIG_MACOSX)
#include <net/if_dl.h>
#endif

#if US_ENABLE_PCAP==1
#include <pcap.h>


int us_rawnet_socket(
    us_rawnet_context_t *self,
    uint16_t ethertype,
    const char *interface_name
)
{
    int r=-1;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *p;

    self->m_ethertype = ethertype;

    p = pcap_open_live( interface_name, 65536,1, 1, errbuf );
    self->m_pcap = (void *)p;

    if( !p )
    {
        us_log_error( "pcap open error on interface '%s': %s", interface_name, errbuf );
    }
    else
    {
        int dl = pcap_datalink(p);

        if( dl != DLT_EN10MB && dl != DLT_IEEE802_11)
        {
            us_log_error("Interface %s is not an Ethernet or wireless interface", interface_name );
        }
        else
        {
            pcap_if_t *alldevs;
            pcap_if_t *d;
            pcap_addr_t *alladdrs;
            pcap_addr_t *a;

            self->m_interface_id = -1;
            if( pcap_findalldevs( &alldevs, errbuf )!=0 )
            {
                us_log_error("pcap_findalldevs failed");
            }
            else
            {
                for ( d = alldevs; d != NULL; d = d->next )
                {
                    self->m_interface_id++;

                    /* find the interface by name */
                    if ( strcmp ( interface_name, d->name ) ==0 )
                    {
                        /* now find the MAC address associated with it */
                        alladdrs = d->addresses;

                        for ( a = alladdrs; a != NULL; a = a->next )
                        {
#ifdef __APPLE__
                            /* Apple AF_LINK format depends on osx version */

                            if ( a->addr->sa_family == AF_LINK && a->addr->sa_data != NULL )
                            {
                                struct sockaddr_dl *  link = ( struct sockaddr_dl* ) a->addr->sa_data;

                                uint8_t mac[link->sdl_alen];
                                memcpy ( mac, LLADDR ( link ), link->sdl_alen );

                                if ( link->sdl_alen == 6 )
                                {
                                    /* older mac os x */
                                    memcpy( self->m_my_mac, &mac[0], 6 );
                                }
                                else if ( link->sdl_alen > 6 )
                                {
                                    /* newer mac os x */
                                    memcpy( self->m_my_mac, &mac[1], 6 );
                                }
                            }
#elif defined(_WIN32)
#error TODO get MAC address WIN32
#elif defined(__linux__)
                            if ( a->addr->sa_family == AF_PACKET )
                            {
                                struct sockaddr_ll * link = ( struct sockaddr_ll * ) a->addr;
                                memcpy (self->m_my_mac,link->sll_addr,6 );
                            }
#endif
                        }

                        break;
                    }
                }

                if( self->m_interface_id==-1 )
                {
                    us_log_error( "unable to get MAC address for interface '%s'", interface_name );
                }
                else
                {
                    self->m_fd = pcap_fileno ( p );
                    if( self->m_fd == -1 )
                    {
                        us_log_error( "Unable to get pcap fd");
                    }
                    else
                    {
                        /* enable ether protocol filter */
                        us_rawnet_join_multicast( self, 0 );
                        r=self->m_fd;
                    }
                }
            }
        }
    }

    if( r==-1 )
    {
        if( p )
        {
            pcap_close(p);
            self->m_pcap = 0;
        }
    }
    return r;
}

void us_rawnet_close(
    us_rawnet_context_t *self
)
{
    if( self->m_fd >=0 )
    {
        close( self->m_fd );
        self->m_fd = -1;
    }

    if( self->m_pcap )
    {
        pcap_close( self->m_pcap );
        self->m_pcap = 0;
    }
}

ssize_t us_rawnet_send(
    us_rawnet_context_t *self,
    const uint8_t dest_mac[6],
    const void *payload,
    ssize_t payload_len
)
{
    bool r=false;
    pcap_t *m_pcap = (pcap_t *)self->m_pcap;

    if ( m_pcap )
    {
        uint8_t buffer[2048];
        uint8_t * data = buffer + 14;
        memcpy ( ( void* ) buffer, ( void* ) dest_mac, 6 );
        memcpy ( ( void* ) ( buffer+6 ), ( void* ) self->m_my_mac, 6 );
        buffer[12] = US_GET_BYTE_1 ( self->m_ethertype );
        buffer[13] = US_GET_BYTE_0 ( self->m_ethertype );
        memcpy ( data, payload, payload_len );
        r=pcap_sendpacket ( m_pcap,buffer,payload_len+14 ) ==0;
    }
    else
    {
        r=false;
    }
    return r ? payload_len : -1;
}

ssize_t us_rawnet_recv(
    us_rawnet_context_t *self,
    uint8_t src_mac[6],
    uint8_t dest_mac[6],
    void *payload_buf,
    ssize_t payload_buf_max_size
)
{
    ssize_t r=-1;
    pcap_t *m_pcap = (pcap_t *)self->m_pcap;

    if ( m_pcap )
    {
        const uint8_t *data;
        struct pcap_pkthdr *header;
        int e = pcap_next_ex ( m_pcap, &header,&data );

        if ( e==1 && header->caplen <= payload_buf_max_size )
        {
            r = header->caplen-14;
            memcpy ( payload_buf, &data[14], r );
            memcpy ( src_mac, &data[6], 6 );
            memcpy ( dest_mac, &data[0], 6 );
        }
    }
    return r;
}


bool us_rawnet_join_multicast(
    us_rawnet_context_t *self,
    const uint8_t multicast_mac[6]
)
{
    bool r=false;
    struct bpf_program fcode;
    pcap_t *p = (pcap_t *)self->m_pcap;
    char filter[1024];
    /* TODO: add multicast address to pcap filter here */
    sprintf ( filter, "ether proto 0x%04x", self->m_ethertype );

    if ( pcap_compile ( p, &fcode, filter, 1, 0xffffffff ) <0 )
    {
        pcap_close ( p );
        us_log_error ( "Unable to pcap_compile: '%s'", filter );
    }
    else
    {
        if ( pcap_setfilter ( p,&fcode ) <0 )
        {
            pcap_close ( p );
            us_log_error ( "Unable to pcap_setfilter" );
        }
        else
        {
            r=true;
        }
        pcap_freecode ( &fcode );
    }
    return r;
}

#endif

#if defined(__linux__) && US_ENABLE_PCAP==0

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
/* #include <linux/if_arp.h> for ARPHRD_ETHER -- cant include this here because linux headers are broken.  */

int us_rawnet_socket(
    us_rawnet_context_t *self,
    uint16_t ethertype,
    const char *interface_name
)
{
    int fd= socket(AF_PACKET,SOCK_RAW,htons(ethertype) );
    if( fd>=0 && interface_name )
    {
        int i;
        struct ifreq ifr;
        strncpy(ifr.ifr_name,interface_name, sizeof(ifr.ifr_name)-1 );
        if ( ioctl(fd ,SIOCGIFINDEX,&ifr)<0 )
        {
            close( fd );
            return -1;
        }
        self->m_interface_id = ifr.ifr_ifindex;
        if ( ioctl(fd ,SIOCGIFHWADDR,&ifr)<0 )
        {
            close(fd);
            return -1;
        }
        for ( i=0; i<6; ++i )
        {
            self->m_my_mac[i] = (uint8_t)ifr.ifr_hwaddr.sa_data[i];
        }
        self->m_fd=fd;
        self->m_ethertype = ethertype;
    }
    return fd;
}

void us_rawnet_close(
    us_rawnet_context_t *self
)
{
    if( self->m_fd >=0 )
    {
        close( self->m_fd );
        self->m_fd = -1;
    }

}


ssize_t us_rawnet_send(
    us_rawnet_context_t *self,
    const uint8_t dest_mac[6],
    const void *payload,
    ssize_t payload_len
)
{
    ssize_t r=-1;
    ssize_t sent_len;
    struct sockaddr_ll socket_address;
    uint8_t buffer[ETH_FRAME_LEN];
    unsigned char *etherhead = buffer;
    unsigned char* data = buffer + 14;
    struct ethhdr *eh = (struct ethhdr *)etherhead;
    socket_address.sll_family   = PF_PACKET;
    socket_address.sll_protocol = htons(self->m_ethertype);
    socket_address.sll_ifindex  = self->m_interface_id;
    socket_address.sll_hatype   = 1; /*ARPHRD_ETHER; */
    socket_address.sll_pkttype  = PACKET_OTHERHOST;
    socket_address.sll_halen    = ETH_ALEN;
    memcpy(socket_address.sll_addr,self->m_my_mac,ETH_ALEN );
    socket_address.sll_addr[6]  = 0x00;
    socket_address.sll_addr[7]  = 0x00;
    memcpy((void*)buffer, (void*)dest_mac, ETH_ALEN);
    memcpy((void*)(buffer+ETH_ALEN), (void*)self->m_my_mac, ETH_ALEN);
    eh->h_proto = htons(self->m_ethertype);
    memcpy( data, payload, payload_len );
    do
    {
        sent_len = sendto(self->m_fd, buffer, payload_len+14, 0,
                          (struct sockaddr*)&socket_address, sizeof(socket_address));
    }
    while( sent_len<0 && (errno==EINTR || errno==EAGAIN) );
    if( sent_len>=0 )
    {
        r=sent_len-14;
    }

    return r;
}

ssize_t us_rawnet_recv(
    us_rawnet_context_t *self,
    uint8_t src_mac[6],
    uint8_t dest_mac[6],
    void *payload_buf,
    ssize_t payload_buf_max_size
)
{
    ssize_t r=-1;
    ssize_t buf_len;
    uint8_t buf[2048];

    do
    {
        buf_len=recv(
                    self->m_fd,
                    buf, sizeof(buf),
                    0
                );
    }
    while(buf_len<0  && (errno==EINTR || errno==EAGAIN ) );

    if( buf_len>=0 )
    {
        if( src_mac )
        {
            memcpy( src_mac, &buf[6], 6 );
        }
        if( dest_mac )
        {
            memcpy ( dest_mac, &buf[0], 6 );
        }
        if( payload_buf && (payload_buf_max_size > buf_len-14) )
        {
            memcpy( payload_buf, &buf[14], buf_len-14 );
            r = buf_len-14;
        }
    }
    return r;
}


bool us_rawnet_join_multicast(
    us_rawnet_context_t *self,
    const uint8_t multicast_mac[6]
)
{
    bool r=false;
    struct packet_mreq mreq;
    struct sockaddr_ll saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sll_family = AF_PACKET;
    saddr.sll_ifindex = self->m_interface_id;
    saddr.sll_pkttype = PACKET_MULTICAST;
    saddr.sll_protocol = htons(self->m_ethertype);
    if(bind(self->m_fd, (struct sockaddr *) &saddr, sizeof(saddr)) >=0 )
    {
        memset(&mreq,0,sizeof(mreq));
        mreq.mr_ifindex=self->m_interface_id;
        mreq.mr_type=PACKET_MR_MULTICAST;
        mreq.mr_alen=6;
        mreq.mr_address[0]=multicast_mac[0];
        mreq.mr_address[1]=multicast_mac[1];
        mreq.mr_address[2]=multicast_mac[2];
        mreq.mr_address[3]=multicast_mac[3];
        mreq.mr_address[4]=multicast_mac[4];
        mreq.mr_address[5]=multicast_mac[5];
        if(setsockopt(self->m_fd,SOL_PACKET,PACKET_ADD_MEMBERSHIP,&mreq,sizeof(mreq))>=0)
        {
            r=true;
        }
        else
        {
            us_log_error("us_rawnet_join_multicast setsockopt[SOL_SOCKET,PACKET_ADD_MEMBERSHIP] error %s", strerror(errno) );
        }
    }
    else
    {
        us_log_error( "us_rawnet_join_multicast bind error: %s", strerror(errno));
    }
    return r;
}

#endif

#endif
