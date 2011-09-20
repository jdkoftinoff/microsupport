#ifndef US_WEBAPP_JSON_HPP
#define US_WEBAPP_JSON_HPP

#include "us_world.hpp"
#include "us_buffer.hpp"
#include "us_http.hpp"
#include "us_webapp.hpp"
#include "us_json.hpp"


    int us_webapp_json_reply( us_http_response_header_t *header, us_buffer_t *buf, const us_json_t *m );

    typedef struct us_webapp_json_s
    {
        us_webapp_t m_base;
        const char *m_path;
        us_json_t *m_json_data;
        bool (*update_data)( const us_buffer_t *request_data, us_json_t *data );
    } us_webapp_json_t;


    us_webapp_t *us_webapp_json_create(
        us_allocator_t *allocator,
        const char *path,
        bool (*update_data)( const us_buffer_t *request_data, us_json_t *data )
    );

    void us_webapp_json_destroy(
        us_webapp_t *self
    );

    bool us_webapp_json_path_match(us_webapp_t *self, const char *path );

    int us_webapp_json_dispatch(
        us_webapp_t *self,
        const us_http_request_header_t *request_header,
        const us_buffer_t *request_content,
        us_http_response_header_t *response_header,
        us_buffer_t *response_content
    );


#endif
