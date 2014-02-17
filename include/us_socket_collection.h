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

/** \addtogroup us_socket_collection us_socket_collection_t Socket Collections
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

    /// The current number of active sockets
    int num_sockets;

    /// The flag to trigger an immediate tick again without waiting
    bool do_early_tick;

    /// The user defined flag
    int user_flag;

    /// The user defined context
    void *user_context;

    /// The list of context pointters for each socket
    void *socket_context[ US_SOCKET_COLLECTION_MAX_SOCKETS ];


    /// The function to call to destroy this collection
    void (*destroy)(
            struct us_socket_collection_s *self);


    /// The function that is called in order to close a socket in this
    /// collection and free associated information
    void (*close)(
            struct us_socket_collection_s *self,
            int fd,
            void * context);

    /// The function to call in order to send data to a socket in this collection
    ssize_t (*send_data)(
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
            uint64_t current_time_in_milliseconds,
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

    /// The list of file descriptors for each socket
    int socket_fd[ US_SOCKET_COLLECTION_MAX_SOCKETS ];


} us_socket_collection_t;

/// Initialize a socket collection. It is up to the caller to set the
/// send_data, receive_data, interested_in_reading, interested_in_writing,
/// readable, writable, and tick function pointers and user_context
void us_socket_collection_init( us_socket_collection_t *self );

/// Destroy a socket collection by calling closesocket on each active socket handler
void us_socket_collection_destroy( us_socket_collection_t *self );

/// Close a plain socket in this collection, with no additional info
void us_socket_collection_close_plain_socket( us_socket_collection_t *self, int fd, void *socket_context );

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
int us_socket_collection_add_tcp_server(
    us_socket_collection_t *self,
    const char *local_addr_name,
    const char *local_port_name,
    void * context );

/// Create a TCP client socket, trying to initiate a non-blocking connect to the specified remote address
int us_socket_collection_add_tcp_client(
    us_socket_collection_t *self,
    const char *remote_addr_name,
    const char *remote_port_name,
    void * context );

/// Create a UDP socket, bound to a local address on a specific network port
int us_socket_collection_add_udp(
    us_socket_collection_t *self,
    const char *local_addr_name,
    const char *local_port_name,
    void * context );

/// Create a UDP socket, bound to a local address on a specific network port, that also joins
/// the stated multicast address
int us_socket_collection_add_multicast_udp(
    us_socket_collection_t *self,
    const char *local_addr_name,
    const char *local_port_name,
    const char *multicast_addr_name,
    const char *multicast_port_name,
    const char *network_port_name,
    void * context );

#if defined(US_ENABLE_RAW_ETHERNET)
/// Given a rawnet context, add it to the collection and set the context pointer to the rawnet_context
/// If unable to add it to the collection, it closes the rawnet socket and returns -1
int us_socket_collection_add_rawnet(
    us_socket_collection_t *self,
    us_rawnet_context_t *rawnet_context
    );
#endif

/*@}*/


/** \addtogroup us_socket_collection us_socket_collection_t Socket Collection Groups
 */
/*@{*/

#ifndef US_SOCKET_COLLECTION_GROUP_MAX_COLLECTIONS
# define US_SOCKET_COLLECTION_GROUP_MAX_COLLECTIONS (8)
#endif

/// The us_socket_collection_group manages a group of socket collections
typedef struct us_socket_collection_group_s {
    int num_collections;
    us_socket_collection_t *collection[US_SOCKET_COLLECTION_GROUP_MAX_COLLECTIONS];
} us_socket_collection_group_t;

/// Initialize the socket collection group
void us_socket_collection_group_init( us_socket_collection_group_t *self );

/// Destroy the socket collection group and all of the socket collections within
void us_socket_collection_group_destroy( us_socket_collection_group_t *self );

/// Count the total number of active sockets managed by all collections
int us_socket_collection_group_count_sockets( us_socket_collection_group_t *self );

/// Add a reference to the specified socket collection to the group.
/// Returns false if the collection group is full
bool us_socket_collection_group_add(
    us_socket_collection_group_t *self,
    us_socket_collection_t *c );

/// Clean up all closed sockets in all socket collections
void us_socket_collection_group_cleanup(
    us_socket_collection_group_t *self );

/// Fill the readable and writable fd_sets based on the requirements of all the sockets
/// in all the socket collections.  Returns the largest file descriptor of them all
int us_socket_collection_group_fill_sets(
    us_socket_collection_group_t *self,
    fd_set *readable,
    fd_set *writable,
    int largest_fd );

/// Send a tick message to all of the socket collections for each socket
/// Automatically calls cleanup() afterwards
void us_socket_collection_group_tick(
    us_socket_collection_group_t *self,
    uint64_t current_time_in_ms );

/// Handle any readable or writable sockets within the contained socket groups.
/// Automatically calls cleanup() aftwards
void us_socket_collection_group_handle_sets(
    us_socket_collection_group_t *self,
    fd_set const *readable,
    fd_set const *writable,
    uint64_t current_time_in_ms );

/// Scan through all socket collections and find out if an early tick is requested by any
bool us_socket_collection_group_wants_early_tick( us_socket_collection_group_t *self );

/*@}*/

/// Initialize a socket collection used for managing tcp server sockets
void us_socket_collection_init_tcp_server(
    us_socket_collection_t *self );

/// Initialize a socket collection used for managing tcp client sockets
void us_socket_collection_init_tcp_client(
    us_socket_collection_t *self );

/// Initialize a socket collection used for managing udp unicast sockets
void us_socket_collection_init_udp_unicast(
    us_socket_collection_t *self );

/// Initialize a socket collection used for managing udp multicast sockets
void us_socket_collection_init_udp_multicast(
    us_socket_collection_t *self );

/// Initialize a socket collection used for managing rawnet sockets
void us_socket_collection_init_rawnet(
    us_socket_collection_t *self );

#if defined(US_ENABLE_RAW_ETHERNET)

/// A subclass of socket_collection for managing a rawnet_multi object. Adds the rawnet_multi pointer.
typedef struct us_socket_collection_rawnet_multi_s {
    us_socket_collection_t base;
    us_rawnet_multi_t *rawnet_multi;
} us_socket_collection_rawnet_multi_t;

/// Initialize a socket collection of rawnet sockets based on the rawnet_multi object
void us_socket_collection_init_rawnet_multi(
        us_socket_collection_rawnet_multi_t *self,
        us_rawnet_multi_t *rawnet_multi );
#endif

/// Scan through all active sockets in all groups and call select()
bool us_socket_collections_group_select(
    us_socket_collection_group_t *self,
    uint64_t cur_time,
    uint64_t max_sleep_time_in_ms );

#ifdef __cplusplus
}
#endif

#endif
