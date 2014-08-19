#pragma once

namespace boba
{
  enum class ReadState
  {
    ReadHeader,
    ReadPayload,
    FullFrame,
  };

  struct WebbyBuffer
  {
    WebbyBuffer() { Reset(); data.resize(64 * 1024); }
    void Reset() { readOfs = 0; writeOfs = 0; }
    bool BufferFull() { return writeOfs == data.size(); }
    int readOfs;
    int writeOfs;
    vector<u8> data;
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


  struct WebsocketClient
  {
    typedef function<void(const u8* payload, u32 size)> fnProcessPayload;
    typedef function<void()> fnConnectedCallback;

    WebsocketClient();

    void SetCallbacks(
      const fnProcessPayload& cbPayload,
      const fnConnectedCallback& cbConnected);
    bool Connect(const char* host, const char* serviceName);
    void Process();
    void Disconnect();

    int SendWebsocketFrame(const u8* buf, int len);
    void SetBlockingIo(bool blocking);

    fnProcessPayload _cbProcessPayload;
    fnConnectedCallback _cbConnected;

    ReadState _readState;
    WebbyBuffer _readBuffer;
    SOCKET _sockfd;
    WebbyWsFrame _curFrame;
    u32 _payloadStart;
    string _host;
    string _serviceName;
    TimeStamp _lastPing;
    TimeStamp _lastReconnect;
    bool _connected;
    struct addrinfo* _addrinfo;
  };
}