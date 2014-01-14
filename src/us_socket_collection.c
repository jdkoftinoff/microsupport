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
    self->num_sockets = 0;
    self->do_early_tick = false;
    self->send_data = 0;
    self->interested_in_reading = 0;
    self->interested_in_writing = 0;
    self->readable = 0;
    self->writable = 0;
    self->tick = 0;
}

void us_socket_collection_destroy( us_socket_collection_t *self ) {
    int i;
    for( i=0;i<self->num_sockets; ++i ) {
        int fd = self->socket_fd[i];
        closesocket(fd);
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




