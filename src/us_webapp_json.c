
#include "us_world.h"
#include "us_webapp_json.h"
#include "us_json.h"

int us_webapp_json_reply( us_http_response_header_t *header, us_buffer_t *buf, const us_json_t *m )
{
    bool r=true;
    r&=us_json_flatten_to_buffer( m, buf );
    r&=us_http_response_header_init_ok( header, 200, "application/json", us_buffer_readable_count( buf ),true);
    r&=us_http_response_header_set_no_cache( header );
    if ( r )
        return header->m_code;
    else
        return 500;
}



us_webapp_t *us_webapp_json_create(
    us_allocator_t *allocator,
    const char *path,
    bool (*update_data)( const us_buffer_t *request_data, us_json_t *data )
)
{
    us_webapp_json_t *self = us_new(allocator,us_webapp_json_t);
    if ( self )
    {
        us_webapp_init( &self->m_base, allocator );
        self->m_base.destroy = us_webapp_json_destroy;
        self->m_base.path_match = us_webapp_json_path_match;
        self->m_base.dispatch = us_webapp_json_dispatch;
        self->m_path = path;
        self->update_data = update_data;
    }
    return &self->m_base;
}

void us_webapp_json_destroy(
    us_webapp_t *self_
)
{
    us_webapp_json_t *self = (us_webapp_json_t *)self_;
    us_webapp_destroy( &self->m_base );
}

bool us_webapp_json_path_match(us_webapp_t *self_, const char *path )
{
    us_webapp_json_t *self = (us_webapp_json_t *)self_;
    if ( strcmp( path, self->m_path) ==0 )
        return true;
    else
        return false;
}

int us_webapp_json_dispatch(
    us_webapp_t *self_,
    const us_http_request_header_t *request_header,
    const us_buffer_t *request_content,
    us_http_response_header_t *response_header,
    us_buffer_t *response_content
)
{
    bool r=false;
    us_webapp_json_t *self = (us_webapp_json_t *)self_;
    if ( strcmp( request_header->m_method, "POST" )==0 )
    {
        r=self->update_data( request_content, self->m_json_data );
    }
    else
    {
        r=self->update_data( 0, self->m_json_data );
    }
    if ( r )
    {
        return us_webapp_json_reply( response_header, response_content, self->m_json_data );
    }
    return 500;
}


