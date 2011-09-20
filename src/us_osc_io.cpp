#include "us_world.hpp"

#include "us_osc_io.hpp"

#if 0
/* TODO: */
us_osc_io_packet_t *us_osc_io_packet_create(
    us_allocator_t *allocator,
    int max_buf_size
)
{
    us_osc_io_packet_t *self;
    self = us_new( allocator, us_osc_io_packet_t );
    us_osc_io_packet_init( self, allocator, max_buf_size );
    return self;
}

bool us_osc_io_packet_init(
    us_osc_io_packet_t *self,
    us_allocator_t *allocator,
    int max_buf_size
)
{
    bool r=true;
    self->destroy = us_osc_io_packet_destroy;
    self->m_allocator = allocator;
    self->m_incoming_buffer = us_buffer_create(allocator, max_buf_size);
    self->m_outgoing_buffer = us_buffer_create(allocator, max_buf_size);
    if ( !self->m_incoming_buffer || !self->m_outgoing_buffer )
    {
        r=false;
    }
    return r;
}

bool us_osc_io_packet_get_msg(
    us_osc_io_packet_t *self,
    us_allocator_t *allocator,
    us_osc_msg_t **msg
)
{
}


bool us_osc_io_packet_get_msg_bundle(
    us_osc_io_packet_t *self,
    us_allocator_t *allocator,
    us_osc_msg_bundle_t **bundle
)
{
}


bool us_osc_io_packet_put_msg(
    us_osc_io_packet_t *self,
    us_osc_msg_t *msg
)
{
}


bool us_osc_io_packet_put_bundle(
    us_osc_io_packet_t *self,
    us_osc_msg_bundle_t *bundle
)
{
}



us_osc_io_slip_stream_t *us_osc_io_slip_stream_create(
    us_allocator_t *allocator,
    int max_buf_size
)
{
}


bool us_osc_io_packet_slip_stream_init(
    us_osc_io_slip_stream_t *self,
    us_allocator_t *allocator,
    int max_buf_size
)
{
}


bool us_osc_io_slip_stream_get_msg(
    us_osc_io_slip_stream_t *self,
    us_allocator_t *allocator,
    us_osc_msg_t **msg
)
{
}


bool us_osc_io_slip_stream_get_msg_bundle(
    us_osc_io_slip_stream_t *self,
    us_allocator_t *allocator,
    us_osc_msg_bundle_t **bundle
)
{
}


bool us_osc_io_slip_stream_put_msg(
    us_osc_io_slip_stream_t *self,
    us_osc_msg_t *msg
)
{
}


bool us_osc_io_slip_stream_put_bundle(
    us_osc_io_slip_stream_t *self,
    us_osc_msg_bundle_t *bundle
)
{
}


us_osc_io_packet_stream_t *us_osc_io_packet_stream_create(
    us_allocator_t *allocator,
    int max_buf_size
)
{
}


bool us_osc_io_packet_packet_stream_init(
    us_osc_io_packet_stream_t *self,
    us_allocator_t *allocator,
    int max_buf_size
)
{
}



bool us_osc_io_packet_stream_get_msg(
    us_osc_io_packet_stream_t *self,
    us_allocator_t *allocator,
    us_osc_msg_t **msg
)
{
}


bool us_osc_io_packet_stream_get_msg_bundle(
    us_osc_io_packet_stream_t *self,
    us_allocator_t *allocator,
    us_osc_msg_bundle_t **bundle
)
{
}


bool us_osc_io_packet_stream_put_msg(
    us_osc_io_packet_stream_t *self,
    us_osc_msg_t *msg
)
{
}


bool us_osc_io_packet_stream_put_bundle(
    us_osc_io_packet_stream_t *self,
    us_osc_msg_bundle_t *bundle
)
{
}

#endif




