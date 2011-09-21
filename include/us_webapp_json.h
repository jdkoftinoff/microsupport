#ifndef US_WEBAPP_JSON_H
#define US_WEBAPP_JSON_H

#include "us_world.h"
#include "us_buffer.h"
#include "us_http.h"
#include "us_webapp.h"
#include "us_json.h"

#ifdef __cplusplus
extern "C" {
#endif
    
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

#ifdef __cplusplus
}
#endif

#endif
