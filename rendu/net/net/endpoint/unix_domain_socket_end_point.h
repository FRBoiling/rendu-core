/*
* Created by boil on 2024/2/18.
*/

#ifndef RENDU_NET_NET_ENDPOINT_UNIX_DOMAIN_SOCKET_END_POINT_H_
#define RENDU_NET_NET_ENDPOINT_UNIX_DOMAIN_SOCKET_END_POINT_H_

#include "end_point.h"

NET_NAMESPACE_BEGIN

class UnixDomainSocketEndPoint : public EndPoint {
private:
  string _path;
  byte* _encodedPath;
  static int s_nativePathOffset;
  static int s_nativePathLength;
  static int s_nativeAddressSize;

public:
  explicit UnixDomainSocketEndPoint(string path)
      : UnixDomainSocketEndPoint(path, nullptr) {
  }
  UnixDomainSocketEndPoint(std::span<byte> socketAddress) {};

private:
  UnixDomainSocketEndPoint(string path, string boundFileName) {};


public:
  SocketAddress *Serialize() override{
    //FIXME：
    return nullptr;
  };

  EndPoint *Create(SocketAddress *socketAddress) override{
      //FIXME：
      return nullptr;
  };
  AddressFamily GetAddressFamily() override{
      //FIXME：
      return AddressFamily::Unix;
  };
  string ToString() const override{
      //FIXME：
      return "UnixDomainSocketEndPoint";
  };

  UnixDomainSocketEndPoint* CreateBoundEndPoint(){
      //FIXME：
      return new UnixDomainSocketEndPoint(GetBoundFileName());
  };

  UnixDomainSocketEndPoint* CreateUnboundEndPoint(){
      //FIXME：
      return new UnixDomainSocketEndPoint(GetBoundFileName());
  };

  string GetBoundFileName(){return "";}

private:
  static bool IsAbstract(string path){
      //FIXME：
      return false;
  }
  static bool IsAbstract(byte* encodedPath){
      //FIXME：
      return false;

  }

  SocketAddress* CreateSocketAddressForSerialize(){
      //FIXME：
      return nullptr;
  }
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_ENDPOINT_UNIX_DOMAIN_SOCKET_END_POINT_H_
