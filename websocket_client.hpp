#pragma once

namespace boba
{
  struct WebsocketClient
  {
    bool Connect(const char* host, const char* serviceName);
    void Process();
    void Disconnect();
  };
}