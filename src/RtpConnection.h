﻿// PHZ
// 2018-6-8

#ifndef XOP_RTP_CONNECTION_H
#define XOP_RTP_CONNECTION_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include "rtp.h"
#include "media.h"
#include "xop/Socket.h"

namespace xop
{

class RtspConnection;

class RtpConnection : public std::enable_shared_from_this<RtpConnection>
{
public:
    RtpConnection(RtspConnection* rtspConnection);
    virtual ~RtpConnection();

    void setClockRate(MediaChannelId channelId, uint32_t clockRate)
    { _mediaChannelInfo[channelId].clockRate = clockRate; }

    void setPayloadType(MediaChannelId channelId, uint32_t payload)
    { _mediaChannelInfo[channelId].rtpHeader.payload = payload; }

    bool setupRtpOverTcp(MediaChannelId channelId, uint16_t rtpChannel, uint16_t rtcpChannel);
    bool setupRtpOverUdp(MediaChannelId channelId, uint16_t rtpPort, uint16_t rtcpPort);
    bool setupRtpOverMulticast(MediaChannelId channelId, int sockfd, std::string ip, uint16_t port);

    uint32_t getRtpSessionId() const
    { return (uint32_t)((size_t)(this)); }

    uint16_t getRtpPort(MediaChannelId channelId) const
    { return _localRtpPort[channelId]; }

    uint16_t getRtcpPort(MediaChannelId channelId) const
    { return _localRtcpPort[channelId]; }

    SOCKET getRtcpfd(MediaChannelId channelId)
    { return _rtcpfd[channelId]; }

    bool isMulticast() const
    { return _isMulticast; }

    bool isSetup(MediaChannelId channelId) const
    { return _mediaChannelInfo[channelId].isSetup; }

    std::string getMulticastIp(MediaChannelId channelId) const;

    void play();
    void record();
    void teardown();

    std::string getRtpInfo(const std::string& rtspUrl);

    void setFrameType(uint8_t frameType=0);
    void setRtpHeader(MediaChannelId channelId, RtpPacketPtr& rtpPkt, uint8_t last, uint32_t ts);
    void sendRtpPacket(MediaChannelId channelId, RtpPacketPtr& rtpPkt, uint32_t pktSize);

    bool isClosed() const
    { return _isClosed; }

private:
    friend class RtspConnection;
    friend class MediaSession;

    int sendRtpOverTcp(MediaChannelId channelId, RtpPacketPtr rtpPkt, uint32_t pktSize);
    int sendRtpOverUdp(MediaChannelId channelId, RtpPacketPtr& rtpPkt, uint32_t pktSize);

    RtspConnection* _rtspConnection;

    TransportMode _transportMode;
    bool _isMulticast = false;

    // server
    bool _isClosed = false, _isIDRFrame = false;

    uint8_t _frameType = 0;
    uint16_t _localRtpPort[MAX_MEDIA_CHANNEL];
    uint16_t _localRtcpPort[MAX_MEDIA_CHANNEL];
    SOCKET _rtpfd[MAX_MEDIA_CHANNEL], _rtcpfd[MAX_MEDIA_CHANNEL];

    // client
    struct sockaddr_in _peerAddr;
    struct sockaddr_in _peerRtpAddr[MAX_MEDIA_CHANNEL];
    struct sockaddr_in _peerRtcpAddr[MAX_MEDIA_CHANNEL];
    MediaChannelInfo _mediaChannelInfo[MAX_MEDIA_CHANNEL];
};

}

#endif
