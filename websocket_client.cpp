#include "websocket_client.hpp"

#pragma comment(lib,"ws2_32.lib")

using namespace boba;

enum
{
  WEBBY_WS_OP_CONTINUATION    = 0,
  WEBBY_WS_OP_TEXT_FRAME      = 1,
  WEBBY_WS_OP_BINARY_FRAME    = 2,
  WEBBY_WS_OP_CLOSE           = 8,
  WEBBY_WS_OP_PING            = 9,
  WEBBY_WS_OP_PONG            = 10
};

enum
{
  WEBBY_WSF_FIN               = 1 << 0,
  WEBBY_WSF_MASKED            = 1 << 1
};

struct WebbyWsFrame
{
  unsigned char flags;
  unsigned char opcode;
  unsigned char header_size;
  unsigned char padding_;
  unsigned char mask_key[4];
  int           payload_length;
};

struct WebbyBuffer
{
  int used;
  int max;
  unsigned char* data;
};

static size_t make_websocket_header(unsigned char buffer[10], unsigned char opcode, int payload_len, int fin)
{
  buffer[0] = (fin ? 0x80 : 0x00) | opcode;

  if (payload_len < 126)
  {
    buffer[1] = (unsigned char) (payload_len & 0x7f);
    return 2;
  }
  else if (payload_len < 65536)
  {
    buffer[1] = 126;
    buffer[2] = (unsigned char) (payload_len >> 8);
    buffer[3] = (unsigned char) payload_len;
    return 4;
  }
  else
  {
    buffer[1] = 127;
    /* Ignore high 32-bits. I didn't want to require 64-bit types and typdef hell in the API. */
    buffer[2] = buffer[3] = buffer[4] = buffer[5] = 0;
    buffer[6] = (unsigned char) (payload_len >> 24);
    buffer[7] = (unsigned char) (payload_len >> 16);
    buffer[8] = (unsigned char) (payload_len >> 8);
    buffer[9] = (unsigned char) payload_len;
    return 10;
  }
}


static int scan_websocket_frame(const struct WebbyBuffer *buf, struct WebbyWsFrame *frame)
{
  unsigned char flags = 0;
  unsigned int len = 0;
  unsigned int opcode = 0;
  unsigned char* data = buf->data;
  unsigned char* data_max = data + buf->used;
  int i;
  int len_bytes = 0;
  int mask_bytes = 0;
  unsigned char header0, header1;

  if (buf->used < 2)
    return -1;

  header0 = *data++;
  header1 = *data++;

  if (header0 & 0x80)
  {
    flags |= WEBBY_WSF_FIN;
  }

  if (header1 & 0x80)
  {
    flags |= WEBBY_WSF_MASKED;
    mask_bytes = 4;
  }

  opcode = header0 & 0xf;
  len = header1 & 0x7f;

  if (len == 126)
    len_bytes = 2;
  else if (len == 127)
    len_bytes = 8;

  if (data + len_bytes + mask_bytes > data_max)
    return -1;

  /* Read big endian length from length bytes (if greater than 125) */
  len = len_bytes == 0 ? len : 0;
  for (i = 0; i < len_bytes; ++i)
  {
    /* This will totally overflow for 64-bit values. I don't care.
     * If you're transmitting more than 4 GB of data using Webby,
     * seek help. */
    len <<= 8;
    len |= *data++;
  }

  /* Read mask word if present */
  for (i = 0; i < mask_bytes; ++i)
  {
    frame->mask_key[i] = *data++;
  }

  frame->header_size = (unsigned char) (data - buf->data);
  frame->flags = flags;
  frame->opcode = (unsigned char) opcode;
  frame->payload_length = (int) len;
  return 0;
}

//------------------------------------------------------------------------------
bool WebsocketClient::Connect(const char* host, const char* serviceName)
{
  addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  struct addrinfo* res;
  if (getaddrinfo(host, serviceName, &hints, &res) < 0) {
    printf("getaddrinfo err: %d", errno);
  }

  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
    printf("connect err: %d, %d", errno, EWOULDBLOCK);
  }

  //fcntl(sockfd, F_SETFL, O_NONBLOCK);

  char get[1024];
  sprintf(get, "GET /%s HTTP/1.1\r\n" \
    "Upgrade: websocket\r\n"\
    "Connection: Upgrade\r\n"\
    "Sec-WebSocket-Key: V0hZLUhBTE8tVEhBUg==\r\n"\
    "Sec-WebSocket-Version: 13\r\n"
    "Host: %s\r\n\r\n", "wstest", host);
  send(sockfd, get, sizeof(get), 0);

  char buf[1024];
  int r = recv(sockfd, buf, sizeof(buf), 0);
  if (r < 0) {
    printf("recv: %d, %d", errno, EWOULDBLOCK);
  }
  printf("%s\n", buf);

  if (!strstr(buf, "Sec-WebSocket-Accept")) {
    printf("Unable to upgrade");
    return 2;
  }

  return true;
}

//------------------------------------------------------------------------------
void WebsocketClient::Process()
{

}

//------------------------------------------------------------------------------
void WebsocketClient::Disconnect()
{

}
