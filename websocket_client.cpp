#include "websocket_client.hpp"

#pragma comment(lib,"ws2_32.lib")

using namespace boba;
using namespace bristol;

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


//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
static int scan_websocket_frame(const struct WebbyBuffer *buf, struct WebbyWsFrame *frame)
{
  unsigned char flags = 0;
  unsigned int len = 0;
  unsigned int opcode = 0;
  unsigned char* data = (u8*)buf->data.data() + buf->readOfs;
  unsigned char* data_max = data + buf->writeOfs;
  int i;
  int len_bytes = 0;
  int mask_bytes = 0;
  unsigned char header0, header1;

  if (data_max - data < 2)
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

  frame->header_size = (unsigned char) (data - buf->data.data());
  frame->flags = flags;
  frame->opcode = (unsigned char) opcode;
  frame->payload_length = (int) len;
  return 0;
}

//------------------------------------------------------------------------------
WebsocketClient::WebsocketClient()
  : _readState(ReadState::ReadHeader)
  , _sockfd(0)
{
}

//------------------------------------------------------------------------------
bool WebsocketClient::Connect(const char* host, const char* serviceName)
{
  _host = host;
  _serviceName = serviceName;

  addrinfo hints;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // Connect to host
  struct addrinfo* res = nullptr;
  int r1 = getaddrinfo(host, serviceName, &hints, &res);
  if (r1 != 0 )
  {
    LOG_WARN(to_string("getaddrinfo err: %d", r1).c_str());
    return false;
  }

  _sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (connect(_sockfd, res->ai_addr, (int)res->ai_addrlen) < 0)
  {
    LOG_INFO(to_string("connect err: %d", errno).c_str());
    return false;
  }

  // Send websocket upgrade request
  char get[1024];
  sprintf(get, "GET /%s HTTP/1.1\r\n" \
    "Upgrade: websocket\r\n"\
    "Connection: Upgrade\r\n"\
    "Sec-WebSocket-Key: V0hZLUhBTE8tVEhBUg==\r\n"\
    "Sec-WebSocket-Version: 13\r\n"
    "Host: %s\r\n\r\n", "wstest", host);
  send(_sockfd, get, sizeof(get), 0);

  char buf[1024];
  int r = recv(_sockfd, buf, sizeof(buf), 0);
  if (r < 0)
  {
    LOG_INFO(to_string("recv: %d", errno).c_str());
    return false;
  }

  if (!strstr(buf, "Sec-WebSocket-Accept"))
  {
    LOG_WARN("Unable to upgrade");
    return false;
  }

  // Set non-blocking io
  u_long v = 1;
  ioctlsocket(_sockfd, FIONBIO, &v);

  if (_cbConnected)
    _cbConnected();

  return true;
}

//------------------------------------------------------------------------------
void WebsocketClient::Process()
{
  // try to reconnect
  if (!_sockfd)
  {
    Connect(_host.c_str(), _serviceName.c_str());
    return;
  }

  int res = recv(_sockfd, (char*)&_readBuffer.data[_readBuffer.writeOfs], (int)_readBuffer.data.size() - _readBuffer.writeOfs, 0);
  if (res != -1)
  {
    _readBuffer.writeOfs += res;

    // process all the complete frames
    while (true)
    {
      switch (_readState)
      {
        case ReadState::ReadHeader:
        {
          // check if we've read a full ws header
          if (scan_websocket_frame(&_readBuffer, &_curFrame) < 0)
          {
            if (_readBuffer.BufferFull())
            {
              // if at end of buffer, discard current frame, and reset the buffer
              LOG_INFO("Discarding frame due to full read buffer");
              _readBuffer.Reset();
            }
            goto DONE;
          }

          _readBuffer.readOfs += _curFrame.header_size;
          _payloadStart = _readBuffer.readOfs;

          // state processing the payload
          _readState = ReadState::ReadPayload;
        }

        case ReadState::ReadPayload:
        {
          int bytesInBuffer = _readBuffer.writeOfs - _readBuffer.readOfs;
          if (bytesInBuffer < _curFrame.payload_length)
          {
            if (_readBuffer.BufferFull())
            {
              // if at end of buffer, discard current frame, and reset the buffer
              LOG_INFO("Discarding frame due to full read buffer");
              _readBuffer.Reset();
            }
            goto DONE;
          }

          // got a full payload, so dispatch it
          if (_cbProcessPayload)
          {
            _cbProcessPayload(_readBuffer.data.data() + _payloadStart, _curFrame.payload_length);
          }
  
          // prepare for next frame
          _readBuffer.readOfs += _curFrame.payload_length;
          _readState = ReadState::ReadHeader;

          // if we've processed all the bytes in the buffer, reset the buffer pointers
          if (_readBuffer.readOfs == _readBuffer.writeOfs)
          {
            _readBuffer.readOfs = 0;
            _readBuffer.writeOfs = 0;
          }

        }
      }
    }
    DONE:;
  }
}

//------------------------------------------------------------------------------
void WebsocketClient::Disconnect()
{

}

//------------------------------------------------------------------------------
void WebsocketClient::SendWebsocketFrame(const u8* buf, int len)
{
  if (!_sockfd)
    return;

  u8 header[10];
  size_t headerLen = make_websocket_header(header, WEBBY_WS_OP_BINARY_FRAME, len, 1);
  send(_sockfd, (const char*)header, (int)headerLen, 0);
  send(_sockfd, (const char*)buf, len, 0);
}

//------------------------------------------------------------------------------
void WebsocketClient::SetCallbacks(
    const fnProcessPayload& cbPayload,
    const fnConnectedCallback& cbConnected)
{
  _cbProcessPayload = cbPayload;
  _cbConnected = cbConnected;
}
