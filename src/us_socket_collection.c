/*
 Copyright (c) 2014, J.D. Koftinoff Software, Ltd.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of J.D. Koftinoff Software, Ltd.. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL J.D. KOFTINOFF SOFTWARE, LTD.. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "us_world.h"
#include "us_socket_collection.h"

void us_socket_collection_init( us_socket_collection_t *self ) {
    self->user_flag = 0;
    self->num_sockets = 0;
    self->do_early_tick = false;
    self->send_data = 0;
    self->interested_in_reading = 0;
    self->interested_in_writing = 0;
    self->readable = 0;
    self->writable = 0;
    self->tick = 0;
    self->close = us_socket_collection_close_plain_socket;
}

void us_socket_collection_destroy( us_socket_collection_t *self ) {
    int i;
    for( i=0;i<self->num_sockets; ++i ) {
        int fd = self->socket_fd[i];
        self->close( self, fd, self->socket_context[i] );
    }
}

void us_socket_collection_close_plain_socket( us_socket_collection_t *self, int fd, void *socket_context  ) {
    if( fd!=-1 ) {
        (void)socket_context;
        closesocket(fd);
        us_socket_collection_remove_fd(self, fd);
    }
}

void us_socket_collection_cleanup( us_socket_collection_t *self ) {
    int i;
    for( i=self->num_sockets-1; i>=0; --i ) {
        int fd = self->socket_fd[i];
        if (fd==-1) {
            size_t len_to_copy = (US_SOCKET_COLLECTION_MAX_SOCKETS-(i+1)) * sizeof(int);
            if( len_to_copy>0 ) {
                memcpy(&self->socket_fd[i],&self->socket_fd[i+1],len_to_copy);
                memcpy(&self->socket_context[i],&self->socket_context[i+1],len_to_copy);
            }
            self->num_sockets--;
            break;
        }
    }
}

bool us_socket_collection_add_fd( us_socket_collection_t *self, int fd, void *socket_context ) {
    bool r=false;
    if( fd!=-1 ) {
        if (self->num_sockets<US_SOCKET_COLLECTION_MAX_SOCKETS) {
            self->socket_fd[self->num_sockets] = fd;
            self->socket_context[self->num_sockets] = socket_context;
            self->num_sockets++;
            r=true;
        }
    }
    return r;
}

bool us_socket_collection_remove_fd( us_socket_collection_t *self, int fd ) {
    bool r=false;
    int i;
    for( i=0; i<self->num_sockets; ++i ) {
        if( fd == self->socket_fd[i] ) {
            self->socket_fd[i] = -1;
        }
    }
    return r;
};

int us_socket_collection_fill_read_set( us_socket_collection_t *self, fd_set *readable, int current_largest_fd ) {
    int i;
    int max_fd=current_largest_fd;

    if( self->interested_in_reading && self->receive_data && self->readable ) {
        for( i=0; i<self->num_sockets; ++i ) {
            int fd = self->socket_fd[i];
            if( fd!=-1 ) {
                if( self->interested_in_reading( self, self->socket_context[i], fd ) ) {
                    FD_SET(fd,readable);
                    if( fd>max_fd) {
                        max_fd=fd;
                    }
                }
            }
        }
    }

    return max_fd;
}

int us_socket_collection_fill_write_set( us_socket_collection_t *self, fd_set *writable, int current_largest_fd ) {
    int i;
    int max_fd=current_largest_fd;

    if( self->interested_in_writing && self->send_data && self->writable ) {
        for( i=0; i<self->num_sockets; ++i ) {
            int fd = self->socket_fd[i];
            if( fd!=-1 ) {
                if( self->interested_in_writing( self, self->socket_context[i], fd ) ) {
                    FD_SET(fd,writable);
                    if( fd>max_fd) {
                        max_fd=fd;
                    }
                }
            }
        }
    }

    return max_fd;
}

void us_socket_collection_handle_readable_set(
    us_socket_collection_t *self,
    fd_set const *readable_set,
    uint64_t current_time_in_milliseconds ) {
    int i;
    for( i=0; i<self->num_sockets; ++i ) {
        int fd=self->socket_fd[i];
        if( fd!=-1 ) {
            if( FD_ISSET(fd,readable_set)) {
                uint8_t buf[4096];
                struct sockaddr fromaddr;
                socklen_t fromaddrlen = sizeof(fromaddr);
                ssize_t recvlen;

                recvlen = self->receive_data(
                    self,
                    self->socket_context[i],
                    fd,
                    buf,
                    sizeof(buf),
                    (struct sockaddr *)&fromaddr,
                    &fromaddrlen );

                self->readable(
                    self,
                    self->socket_context[i],
                    fd,
                    current_time_in_milliseconds,
                    (struct sockaddr *)&fromaddr,
                    fromaddrlen,
                    buf,
                    recvlen);
            }
        }
    }
}

void us_socket_collection_handle_writable_set(
    us_socket_collection_t *self,
    fd_set const *writable_set,
    uint64_t current_time_in_milliseconds )  {
    int i;
    for( i=0; i<self->num_sockets; ++i ) {
        int fd=self->socket_fd[i];
        if( fd!=-1 ) {
            if( FD_ISSET(fd,writable_set)) {
                self->writable(
                    self,
                    self->socket_context[i],
                    fd,
                    current_time_in_milliseconds);
            }
        }
    }

}


void us_socket_collection_tick( us_socket_collection_t *self, uint64_t current_time_in_milliseconds ) {
    int i;
    if( self->tick ) {
        self->do_early_tick = false;
        for( i=0; i<self->num_sockets; ++i ) {
            int fd = self->socket_fd[i];
            if( fd!=-1 ) {
                self->tick( self, self->socket_context[i], fd, current_time_in_milliseconds );
            }
        }
    }
}



int us_socket_collection_add_tcp_server(
    us_socket_collection_t *self,
    const char *local_addr_name,
    const char *local_port_name,
    void * context ) {
    int fd;

    fd = us_net_create_tcp_socket_host(local_addr_name, local_port_name, true );
    if( fd!=-1 ) {
        us_net_set_socket_nonblocking(fd);
        if( !us_socket_collection_add_fd(self, fd, context ) ) {
            closesocket(fd);
            fd=-1;
        }
    }
    return fd;
}

int us_socket_collection_add_tcp_client(
    us_socket_collection_t *self,
    const char *remote_addr_name,
    const char *remote_port_name,
    void * context ) {
    int fd=-1;

    // make sure we have room before trying anything
    if( self->num_sockets < US_SOCKET_COLLECTION_MAX_SOCKETS ) {
        // get the address of the remote
        struct addrinfo *remote_addr;
        remote_addr = us_net_get_addrinfo(remote_addr_name, remote_port_name, SOCK_STREAM, false );

        // create the tcp client socket for this kind of address
        fd = us_net_create_tcp_socket(remote_addr, false);

        // did it work?
        if( fd!=-1 ) {
            // yes, try initiate a connect
            int con_result;

            // we are doing a non blocking connect
            us_net_set_socket_nonblocking(fd);

            // try connect, ignore signals during this time
            do {
                con_result = connect(fd, remote_addr->ai_addr, remote_addr->ai_addrlen);
            } while( con_result==-1 && errno == EINTR );

            // If the connect succeeded or it is in progress, then track this fd
            if( con_result==0 || (con_result==-1 && errno==EINPROGRESS) ) {
                // Add the fd to the collection
                if( !us_socket_collection_add_fd(self, fd, context ) ) {
                    // unable to add the fd to the collection, close the socket
                    closesocket(fd);
                    fd=-1;
                }
            }
        }
    }
    return fd;
}


int us_socket_collection_add_udp(
    us_socket_collection_t *self,
    const char *local_addr_name,
    const char *local_port_name,
    void * context ) {
    struct addrinfo *local_addr;
    int fd;

    local_addr = us_net_get_addrinfo(local_addr_name, local_port_name, SOCK_DGRAM, true );
    fd = us_net_create_udp_socket(local_addr, true );
    if( fd!=-1 ) {
        us_net_set_socket_nonblocking(fd);
        if( !us_socket_collection_add_fd(self, fd, context ) ) {
            closesocket(fd);
            fd=-1;
        }
    }
    return fd;
}


int us_socket_collection_add_multicast_udp(
    us_socket_collection_t *self,
    const char *local_addr_name,
    const char *local_port_name,
    const char *multicast_addr_name,
    const char *multicast_port_name,
    const char *network_port_name,
    void * context ) {
    struct addrinfo *local_addr;
    struct addrinfo *multicast_addr;
    int fd;

    local_addr = us_net_get_addrinfo(local_addr_name, local_port_name, SOCK_DGRAM, true );
    multicast_addr = us_net_get_addrinfo(multicast_addr_name, multicast_port_name, SOCK_DGRAM, false );
    fd = us_net_create_multicast_rx_udp_socket(
                              local_addr,
                              multicast_addr,
                              network_port_name);
    if( fd!=-1 ) {
        us_net_set_socket_nonblocking(fd);
        if( !us_socket_collection_add_fd(self, fd, context ) ) {
            closesocket(fd);
            fd=-1;
        }
    }
    return fd;
}

#if defined(US_ENABLE_RAW_ETHERNET)
int us_socket_collection_add_rawnet(
    us_socket_collection_t *self,
    us_rawnet_context_t *rawnet_context
    ) {
    int fd = rawnet_context->m_fd;

    if( fd!=-1 ) {
        if( !us_socket_collection_add_fd(self, fd, rawnet_context ) ) {
            us_rawnet_close(rawnet_context);
            fd=-1;
        }
    }
    return fd;
}
#endif


void us_socket_collection_group_init( us_socket_collection_group_t *self ) {
    self->num_collections = 0;
}

void us_socket_collection_group_destroy( us_socket_collection_group_t *self ) {
    int i;
    for( i=0; i<self->num_collections; ++i ) {
        us_socket_collection_destroy(self->collection[i]);
    }
}

int us_socket_collection_group_count_sockets( us_socket_collection_group_t *self ) {
    int count=0;
    int i;
    for( i=0; i<self->num_collections; ++i ) {
        count += self->collection[i]->num_sockets;
    }
    return count;
}

bool us_socket_collection_group_add(
    us_socket_collection_group_t *self,
    us_socket_collection_t *c ) {
    bool r=false;
    if( self->num_collections < US_SOCKET_COLLECTION_GROUP_MAX_COLLECTIONS ) {
        self->collection[ self->num_collections ] = c;
        self->num_collections++;
        r=true;
    }
    return r;
}

void us_socket_collection_group_cleanup(
    us_socket_collection_group_t *self ) {
    int i;
    for( i=0; i<self->num_collections; ++i ) {
        us_socket_collection_cleanup(self->collection[i]);
    }
}

int us_socket_collection_group_fill_sets(
    us_socket_collection_group_t *self,
    fd_set *readable,
    fd_set *writable,
    int largest_fd ) {
    int i;
    for( i=0; i<self->num_collections; ++i ) {
        us_socket_collection_t *c = self->collection[i];
        largest_fd = us_socket_collection_fill_read_set(c, readable, largest_fd);
        largest_fd = us_socket_collection_fill_write_set(c, writable, largest_fd);
    }
    return largest_fd;
}

void us_socket_collection_group_tick(
    us_socket_collection_group_t *self,
    uint64_t current_time_in_ms )
{
    int i;
    for( i=0; i<self->num_collections; ++i ) {
        us_socket_collection_t *c = self->collection[i];
        us_socket_collection_tick(c,current_time_in_ms);
        us_socket_collection_cleanup(c);
    }
}

void us_socket_collection_group_handle_sets(
    us_socket_collection_group_t *self,
    fd_set const *readable,
    fd_set const *writable,
    uint64_t current_time_in_ms ) {
    int i;
    for( i=0; i<self->num_collections; ++i ) {
        us_socket_collection_t *c = self->collection[i];
        us_socket_collection_handle_readable_set(c, readable, current_time_in_ms);
        us_socket_collection_handle_writable_set(c, writable, current_time_in_ms);
        us_socket_collection_cleanup(c);
    }
}

bool us_socket_collection_group_wants_early_tick( us_socket_collection_group_t *self ) {
    bool r=false;
    int i;
    for( i=0; i<self->num_collections; ++i ) {
        us_socket_collection_t *c = self->collection[i];
        r|=c->do_early_tick;
    }
    return r;
}

static void tcp_client_close(
        struct us_socket_collection_s *self,
        int fd,
        void * context ) {
    (void)context;
    closesocket(fd);
    us_socket_collection_remove_fd(self, fd);
}


static ssize_t tcp_client_receive_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        void *buf,
        size_t buflen,
        struct sockaddr *from_addr,
        socklen_t *from_addrlen) {
    (void)self;
    (void)context;
    (void)from_addr;
    (void)from_addrlen;
    return recv(fd, buf, buflen, 0 );
}

static ssize_t tcp_client_send_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        struct sockaddr const *to_addr,
        socklen_t to_addrlen,
        uint8_t const *buf,
        size_t len ) {
    ssize_t r=0;
    (void)self;
    (void)context;
    (void)to_addr;
    (void)to_addrlen;

    r = send(fd, buf, len, 0);
    return r;
}

void us_socket_collection_init_tcp_client(
    us_socket_collection_t *self ) {
    us_socket_collection_init(self);
    self->receive_data = tcp_client_receive_data;
    self->send_data = tcp_client_send_data;
    self->close = tcp_client_close;
}


static void tcp_server_close(
        struct us_socket_collection_s *self,
        int fd,
        void * context ) {
    closesocket(fd);
    (void)context;
    us_socket_collection_remove_fd(self, fd);
}


static bool tcp_server_interested_in_reading(
        struct us_socket_collection_s *self,
        void * context,
        int fd
        ) {
    (void)self;
    (void)context;
    (void)fd;
    return true;
}

static ssize_t tcp_server_receive_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        void *buf,
        size_t buflen,
        struct sockaddr *from_addr,
        socklen_t *from_addrlen) {
    (void)self;
    (void)context;
    (void)fd;
    (void)buf;
    (void)buflen;
    (void)from_addr;
    (void)from_addrlen;
    return 0;
}


void us_socket_collection_init_tcp_server(
    us_socket_collection_t *self ) {
    us_socket_collection_init(self);
    self->interested_in_reading = tcp_server_interested_in_reading;
    self->receive_data = tcp_server_receive_data;
    self->send_data = 0;
    self->close = tcp_server_close;
}

static void udp_unicast_close(
        struct us_socket_collection_s *self,
        int fd,
        void * context ) {
    struct addrinfo *destaddr = (struct addrinfo *)context;
    if( destaddr ) {
        freeaddrinfo(destaddr);
    }
    closesocket(fd);
    us_socket_collection_remove_fd(self, fd);
}


static bool udp_unicast_interested_in_reading(
        struct us_socket_collection_s *self,
        void * context,
        int fd
        ) {
    (void)self;
    (void)context;
    (void)fd;
    return true;
}

static ssize_t udp_unicast_receive_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        void *buf,
        size_t buflen,
        struct sockaddr *from_addr,
        socklen_t *from_addrlen) {
    (void)self;
    (void)context;

    return recvfrom(fd, buf, buflen, 0, from_addr, from_addrlen);
}

static ssize_t udp_unicast_send_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        struct sockaddr const *to_addr,
        socklen_t to_addrlen,
        uint8_t const *buf,
        size_t len ) {
    struct addrinfo *destaddr = (struct addrinfo *)context;
    ssize_t r=0;
    (void)self;

    if( to_addr==0 || to_addrlen==0 ) {
        if( destaddr ) {
            to_addr = destaddr->ai_addr;
            to_addrlen = destaddr->ai_addrlen;
        }
    }

    if( to_addr==0 || to_addrlen==0 ) {
        r = send(fd, buf, len, 0);
    } else {
        r = sendto(fd, buf, len, 0, to_addr, to_addrlen);
    }
    return r;
}


void us_socket_collection_init_udp_unicast(
    us_socket_collection_t *self ) {
    us_socket_collection_init(self);
    self->interested_in_reading = udp_unicast_interested_in_reading;
    self->receive_data = udp_unicast_receive_data;
    self->send_data = udp_unicast_send_data;
    self->close = udp_unicast_close;
}



static void udp_multicast_close(
        struct us_socket_collection_s *self,
        int fd,
        void * context ) {
    struct addrinfo *destaddr = (struct addrinfo *)context;
    freeaddrinfo(destaddr);
    closesocket(fd);
    us_socket_collection_remove_fd(self, fd);
}


static bool udp_multicast_interested_in_reading(
        struct us_socket_collection_s *self,
        void * context,
        int fd
        ) {
    (void)self;
    (void)context;
    (void)fd;

    return true;
}

static ssize_t udp_multicast_receive_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        void *buf,
        size_t buflen,
        struct sockaddr *from_addr,
        socklen_t *from_addrlen) {
    (void)self;
    (void)context;

    return recvfrom(fd, buf, buflen, 0, from_addr, from_addrlen);
}

static ssize_t udp_multicast_send_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        struct sockaddr const *to_addr,
        socklen_t to_addrlen,
        uint8_t const *buf,
        size_t len ) {
    struct addrinfo *destaddr = (struct addrinfo *)context;
    ssize_t r=0;
    (void)self;
    if( to_addr==0 || to_addrlen==0 ) {
        if( destaddr ) {
            to_addr = destaddr->ai_addr;
            to_addrlen = destaddr->ai_addrlen;
        }
    }

    if( to_addr==0 || to_addrlen==0 ) {
        r = send(fd, buf, len, 0);
    } else {
        r = sendto(fd, buf, len, 0, to_addr, to_addrlen);
    }
    return r;
}


void us_socket_collection_init_udp_multicast(
    us_socket_collection_t *self ) {
    us_socket_collection_init(self);
    self->interested_in_reading = udp_multicast_interested_in_reading;
    self->receive_data = udp_multicast_receive_data;
    self->send_data = udp_multicast_send_data;
    self->close = udp_multicast_close;
}

#if defined(US_ENABLE_RAW_ETHERNET)

static void rawnet_close(
        struct us_socket_collection_s *self,
        int fd,
        void * context ) {
    us_rawnet_context_t *rawnet_context = (us_rawnet_context_t *)context;
    us_rawnet_close(rawnet_context);
    us_socket_collection_remove_fd(self, fd);
}

static bool rawnet_interested_in_reading(
        struct us_socket_collection_s *self,
        void * context,
        int fd
        ) {
    (void)self;
    (void)context;
    (void)fd;

    return true;
}

static ssize_t rawnet_receive_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        void *buf_,
        size_t buflen,
        struct sockaddr *from_addr,
        socklen_t *from_addrlen) {
    ssize_t r=0;
    us_rawnet_context_t *rawnet = (us_rawnet_context_t *)context;
    uint8_t * buf = (uint8_t *)buf_;
    uint8_t srcmac[6];
    uint8_t destmac[6];
    (void)self;
    (void)fd;
    r=us_rawnet_recv(
        rawnet,
        srcmac,
        destmac,
        buf,
        buflen);

    if (r>0 && from_addr!=0 && *from_addrlen>=sizeof(us_sockaddr_dl) ) {
		us_sockaddr_dl_set_mac(from_addr,srcmac);
        *from_addrlen = sizeof(us_sockaddr_dl);
    }

    return r;
}

static ssize_t rawnet_send_data(
        struct us_socket_collection_s *self,
        void * context,
        int fd,
        struct sockaddr const *to_addr,
        socklen_t to_addrlen,
        uint8_t const *buf_,
        size_t len ) {
    us_rawnet_context_t *rawnet = (us_rawnet_context_t *)context;
    uint8_t * buf = (uint8_t *)buf_;
    uint8_t const * destaddr = 0;

    (void)self;
    (void)fd;
    if( to_addr !=0 && to_addrlen>=sizeof(us_sockaddr_dl) ) {
        if( to_addr->sa_family == US_AF_LINK ) {
			destaddr = us_sockaddr_dl_get_mac( to_addr );
        }
    }

    return us_rawnet_send(
            rawnet,
            destaddr,
            buf,
            len);
}


void us_socket_collection_init_rawnet(
    us_socket_collection_t *self ) {
    us_socket_collection_init(self);
    self->interested_in_reading = rawnet_interested_in_reading;
    self->receive_data = rawnet_receive_data;
    self->send_data = rawnet_send_data;
    self->close = rawnet_close;
}
#endif


bool us_socket_collections_group_select(
    us_socket_collection_group_t *self,
    uint64_t cur_time,
    uint64_t max_sleep_time_in_ms ) {
    bool r=true;
    int s;
    fd_set readable;
    fd_set writable;
    int largest_fd=-1;
    struct timeval tv;

    FD_ZERO(&readable);
    FD_ZERO(&writable);

    largest_fd = us_socket_collection_group_fill_sets(
                                        self,
                                        &readable,
                                        &writable,
                                        largest_fd);

    tv.tv_sec = max_sleep_time_in_ms / 1000;
    tv.tv_usec = (max_sleep_time_in_ms % 1000) * 1000;

    if( us_socket_collection_group_wants_early_tick(self) ) {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
    }

    do {
        s=select(largest_fd+1, &readable, &writable, 0, &tv );
    } while( s<0 && (errno==EINTR || errno==EAGAIN) );

    if( s<0 ) {
        r=false;
    }

    if( s>0 ) {
        us_socket_collection_group_handle_sets(
            self,
            &readable,
            &writable,
            cur_time);
    }
    return r;
}








