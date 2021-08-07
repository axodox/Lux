#include "pch.h"
#include "UdpLightController.h"
#include "DataStream.h"

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Networking;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

namespace Lux::Controllers
{
  const Configuration::UdpLightSettings& UdpLightController::Options() const
  {
    return _options;
  }
  void UdpLightController::Options(const Configuration::UdpLightSettings& options)
  {
    _options = options;
    Connect();
  }

  void UdpLightController::Push(const std::vector<Graphics::rgb>& colors)
  {
    if (!_stream) return;

    _buffer.resize(colors.size() * sizeof(Graphics::rgb));
    auto buffer = _buffer.data();
    for (auto& color : colors)
    {
      *buffer++ = color.g;
      *buffer++ = color.r;
      *buffer++ = color.b;
    }

    _stream->write(uint32_t(0x21C64BE5)); //protocol id
    _stream->write(_messageId++); //message id
    _stream->write(uint16_t(1)); //flags: passthrough
    _stream->write(uint16_t(colors.size())); //light count
    _stream->write(_buffer.size(), _buffer.data()); //lights
    _stream->flush();
  }

  Configuration::LightControllerKind UdpLightController::Type() const
  {
    return Configuration::LightControllerKind::Udp;
  }

  void UdpLightController::Connect()
  {
    _stream.reset();
    _messageId = 0;

    try
    { 
      _socket = {};
      _socket.Control().QualityOfService(SocketQualityOfService::LowLatency);
      EndpointPair endpoint(nullptr, L"", HostName(_options.Host), to_hstring(_options.Port));
      _socket.ConnectAsync(endpoint).get();
      
      auto stream = _socket.OutputStream();
      DataWriter writer{ stream };
      _stream = make_unique<Serialization::data_stream>(writer);

      IsConnected(true);
    }
    catch (...)
    {
      IsConnected(false);
    }
  }
}