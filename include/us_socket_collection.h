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
#ifndef US_SOCKET_COLLECTION_H
#define US_SOCKET_COLLECTION_H

#ifndef US_WORLD_H
#include "us_world.h"
#endif
#include "us_net.h"
#if defined(US_ENABLE_RAW_ETHERNET)
#include "us_rawnet.h"
#include "us_rawnet_multi.h"
#endif
#include "us_time.h"


#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup us_socket_collection
 */
/*@{*/

#ifndef US_SOCKET_COLLECTION_MAX_SOCKETS
#define US_SOCKET_COLLECTION_MAX_SOCKETS (8)
#endif

/// us_socket_collection manages a collection of similiar sockets
/// A similiar socket means that the mechanism for sending data
/// and receiving data is the same. Handles a maximum
/// of US_SOCKET_COLLECTION_MAX_SOCKETS at a time, along with
/// a context pointer for each
typedef struct us_socket_collection_s {

    /// The list of context pointters for each socket
    void *socket_context[ US_SOCKET_COLLECTION_MAX_SOCKETS ];

    /// The list of file descriptors for each socket
    int socket_fd[ US_SOCKET_COLLECTION_MAX_SOCKETS ];

    /// The current number of active sockets
    int num_sockets;

    /// The flag to trigger an immediate tick again without waiting
    bool do_early_tick;

    /// The function to call in order to send data to a socket in this collection
    void (*send_data)(
            struct us_socket_collection_s *self,
            void * context,
            int fd,
            struct sockaddr const *to_addr,
            socklen_t to_addrlen,
            uint8_t const *buf,
            size_t len );

    /// The function to call in order to receive data from a socket in this collection
    ssize_t (*receive_data)(
            struct us_socket_collection_s *self,
            void * context,
            int fd,
            void *buf,
            size_t buflen,
            struct sockaddr *from_addr,
            socklen_t *from_addrlen);

    /// The function the collection calls in order to find out if a handler
    /// for a specific socket is interested in reading from the socket
    bool (*interested_in_reading)(
            struct us_socket_collection_s *self,
            void * context,
            int fd
            );

    /// The function the collection calls in order to find out if a handler
    /// for a specific socket is interested in writing to the socket
    bool (*interested_in_writing)(
            struct us_socket_collection_s *self,
            void * context,
            int fd
            );

    /// The function the collection calls when data is received on the socket
    void (*readable)(
            struct us_socket_collection_s *self,
            void * context,
            int fd,
            uint64_t current_time_in_milliseconds,
            struct sockaddr const *from_addr,
            socklen_t from_addrlen,
            uint8_t const *buf,
            ssize_t len );

    /// The function the collection calls when the socket is writable
    void (*writable)(
            struct us_socket_collection_s *self,
            void * context,
            int fd,
            uint64_t current_time_in_milliseconds);

    /// The function the collection calls when it is time to do a tick
    void (*tick)(
            struct us_socket_collection_s *self,
            void * context,
            int fd,
            uint64_t current_time_in_milliseconds
            );

} us_socket_collection_t;

/// Initialize a socket collection. It is up to the caller to set the
/// send_data, receive_data, interested_in_reading, interested_in_writing,
/// readable, writable, and tick function pointers
void us_socket_collection_init( us_socket_collection_t *self );

/// Destroy a socket collection by calling closesocket on each active socket handler
void us_socket_collection_destroy( us_socket_collection_t *self );

/// Cleanup any sockets scheduled to be removed
void us_socket_collection_cleanup( us_socket_collection_t *self );

/// Add a socket to the collection with the specified socket_context. Returns false if
/// the collection is full or if the file descriptor is invalid.
bool us_socket_collection_add_fd( us_socket_collection_t *self, int fd, void *socket_context );

/// Mark a socket to be removed in the cleanup stage
bool us_socket_collection_remove_fd( us_socket_collection_t *self, int fd );

/// Fill a fd_set based on the interested_in_reading status of each socket handler. Returns the
/// maximum socket handle.
int us_socket_collection_fill_read_set( us_socket_collection_t *self, fd_set *readable, int current_largest_fd );

/// Fill a fd_set based on the interested_in_writing status of each socket handler. Returns the
/// maximum socket handle.
int us_socket_collection_fill_write_set( us_socket_collection_t *self, fd_set *writable, int current_largest_fd );

/// call the tick function for each socket
void us_socket_collection_tick( us_socket_collection_t *self, uint64_t current_time_in_milliseconds );

/// Try to read from each readable socket and call the data_received function for the socket
void us_socket_collection_handle_readable_set(
    us_socket_collection_t *self,
    fd_set const *readable_set,
    uint64_t current_time_in_milliseconds );

/// Try to call the writable function for each file descriptor that is marked writable
void us_socket_collection_handle_writable_set(
    us_socket_collection_t *self,
    fd_set const *writable_set,
    uint64_t current_time_in_milliseconds );

/// Create a TCP server socket, bound to a local address on a specific network port
static inline int us_socket_collection_add_tcp_server(
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

/// Create a TCP client socket, trying to initiate a non-blocking connect to the specified remote address
static inline int us_socket_collection_add_tcp_client(
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


/// Create a UDP socket, bound to a local address on a specific network port
static inline int us_socket_collection_add_udp(
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


/// Create a UDP socket, bound to a local address on a specific network port, that also joins
/// the stated multicast address
static inline int us_socket_collection_add_multicast_udp(
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
/// Given a rawnet context, add it to the collection and set the context pointer to the rawnet_context
/// If unable to add it to the collection, it closes the rawnet socket and returns -1
static inline int us_socket_collection_add_rawnet(
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

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
