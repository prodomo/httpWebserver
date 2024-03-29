// #include <map>
// #include <string.h>
// #include <unordered_map>
// #include <vector>

// enum class StatusCode {
//     unknown = 0,
//     status_continue = 100,
//     status_switching_protocols,
//     status_ok = 200,
//     status_created,
//     status_accepted,
//     status_non_authoritative_information,
//     status_no_content,
//     status_reset_content,
//     status_partial_content,
//     status_multiple_choices = 300,
//     status_moved_permanently,
//     status_found,
//     status_see_other,
//     status_not_modified,
//     status_use_proxy,
//     status_temporary_redirect = 307,
//     status_error_bad_request = 400,
//     status_error_unauthorized,
//     status_error_payment_required,
//     status_error_forbidden,
//     status_error_not_found,
//     status_error_method_not_allowed,
//     status_error_not_acceptable,
//     status_error_proxy_authentication_required,
//     status_error_request_timeout,
//     status_error_conflict,
//     status_error_gone,
//     status_error_length_required,
//     status_error_precondition_failed,
//     status_error_payload_too_large,
//     status_error_uri_too_long,
//     status_error_unsupported_media_type,
//     status_error_range_not_satisfiable,
//     status_error_expectation_failed,
//     status_error_internal_server_error = 500,
//     status_error_not_implemented,
//     status_error_bad_gateway,
//     status_error_service_unavailable,
//     status_error_gateway_timeout,
//     status_error_http_version_not_supported,
//   };

// static const map<StatusCode, string> status_code_strings = {
//     {StatusCode::unknown, ""},
//     {StatusCode::status_continue, "100 Continue"},
//     {StatusCode::status_switching_protocols, "101 Switching Protocols"},
//     {StatusCode::status_ok, "200 OK"},
//     {StatusCode::status_created, "201 Created"},
//     {StatusCode::status_accepted, "202 Accepted"},
//     {StatusCode::status_non_authoritative_information, "203 Non-Authoritative Information"},
//     {StatusCode::status_no_content, "204 No Content"},
//     {StatusCode::status_reset_content, "205 Reset Content"},
//     {StatusCode::status_partial_content, "206 Partial Content"},
//     {StatusCode::status_multiple_choices, "300 Multiple Choices"},
//     {StatusCode::status_moved_permanently, "301 Moved Permanently"},
//     {StatusCode::status_found, "302 Found"},
//     {StatusCode::status_see_other, "303 See Other"},
//     {StatusCode::status_not_modified, "304 Not Modified"},
//     {StatusCode::status_use_proxy, "305 Use Proxy"},
//     {StatusCode::status_temporary_redirect, "307 Temporary Redirect"},
//     {StatusCode::status_error_bad_request, "400 Bad Request"},
//     {StatusCode::status_error_unauthorized, "401 Unauthorized"},
//     {StatusCode::status_error_payment_required, "402 Payment Required"},
//     {StatusCode::status_error_forbidden, "403 Forbidden"},
//     {StatusCode::status_error_not_found, "404 Not Found"},
//     {StatusCode::status_error_method_not_allowed, "405 Method Not Allowed"},
//     {StatusCode::status_error_not_acceptable, "406 Not Acceptable"},
//     {StatusCode::status_error_proxy_authentication_required, "407 Proxy Authentication Required"},
//     {StatusCode::status_error_request_timeout, "408 Request Timeout"},
//     {StatusCode::status_error_conflict, "409 Conflict"},
//     {StatusCode::status_error_gone, "410 Gone"},
//     {StatusCode::status_error_length_required, "411 Length Required"},
//     {StatusCode::status_error_precondition_failed, "412 Precondition Failed"},
//     {StatusCode::status_error_payload_too_large, "413 Payload Too Large"},
//     {StatusCode::status_error_uri_too_long, "414 URI Too Long"},
//     {StatusCode::status_error_unsupported_media_type, "415 Unsupported Media Type"},
//     {StatusCode::status_error_range_not_satisfiable, "416 Range Not Satisfiable"},
//     {StatusCode::status_error_expectation_failed, "417 Expectation Failed"},
//     {StatusCode::status_error_internal_server_error, "500 Internal Server Error"},
//     {StatusCode::status_error_not_implemented, "501 Not Implemented"},
//     {StatusCode::status_error_bad_gateway, "502 Bad Gateway"},
//     {StatusCode::status_error_service_unavailable, "503 Service Unavailable"},
//     {StatusCode::status_error_gateway_timeout, "504 Gateway Timeout"},
//     {StatusCode::status_error_http_version_not_supported, "505 HTTP Version Not Supported"}};