
typedef struct Response{
    char* status_code;
    char* content_type;
    char* response;
}Response;
typedef struct Request Request;

Response * handleApi(Request * request);