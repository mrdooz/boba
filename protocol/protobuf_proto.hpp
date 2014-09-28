#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "google/protobuf/descriptor.pb.h"
#include "proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace boba { namespace protobuf { 
  struct FileDescriptorSet;
  struct FileDescriptorProto;
  struct DescriptorProto;
  struct FieldDescriptorProto;
  struct EnumDescriptorProto;
  struct EnumValueDescriptorProto;
  struct ServiceDescriptorProto;
  struct MethodDescriptorProto;
  struct FileOptions;
  struct MessageOptions;
  struct FieldOptions;
  struct EnumOptions;
  struct EnumValueOptions;
  struct ServiceOptions;
  struct MethodOptions;
  struct UninterpretedOption;
  struct SourceCodeInfo;

  FileDescriptorSet FromProtocol(const ::google::protobuf::FileDescriptorSet& p);
  void ToProtocol(const FileDescriptorSet& v, ::google::protobuf::FileDescriptorSet* p);
  FileDescriptorProto FromProtocol(const ::google::protobuf::FileDescriptorProto& p);
  void ToProtocol(const FileDescriptorProto& v, ::google::protobuf::FileDescriptorProto* p);
  DescriptorProto FromProtocol(const ::google::protobuf::DescriptorProto& p);
  void ToProtocol(const DescriptorProto& v, ::google::protobuf::DescriptorProto* p);
  FieldDescriptorProto FromProtocol(const ::google::protobuf::FieldDescriptorProto& p);
  void ToProtocol(const FieldDescriptorProto& v, ::google::protobuf::FieldDescriptorProto* p);
  EnumDescriptorProto FromProtocol(const ::google::protobuf::EnumDescriptorProto& p);
  void ToProtocol(const EnumDescriptorProto& v, ::google::protobuf::EnumDescriptorProto* p);
  EnumValueDescriptorProto FromProtocol(const ::google::protobuf::EnumValueDescriptorProto& p);
  void ToProtocol(const EnumValueDescriptorProto& v, ::google::protobuf::EnumValueDescriptorProto* p);
  ServiceDescriptorProto FromProtocol(const ::google::protobuf::ServiceDescriptorProto& p);
  void ToProtocol(const ServiceDescriptorProto& v, ::google::protobuf::ServiceDescriptorProto* p);
  MethodDescriptorProto FromProtocol(const ::google::protobuf::MethodDescriptorProto& p);
  void ToProtocol(const MethodDescriptorProto& v, ::google::protobuf::MethodDescriptorProto* p);
  FileOptions FromProtocol(const ::google::protobuf::FileOptions& p);
  void ToProtocol(const FileOptions& v, ::google::protobuf::FileOptions* p);
  MessageOptions FromProtocol(const ::google::protobuf::MessageOptions& p);
  void ToProtocol(const MessageOptions& v, ::google::protobuf::MessageOptions* p);
  FieldOptions FromProtocol(const ::google::protobuf::FieldOptions& p);
  void ToProtocol(const FieldOptions& v, ::google::protobuf::FieldOptions* p);
  EnumOptions FromProtocol(const ::google::protobuf::EnumOptions& p);
  void ToProtocol(const EnumOptions& v, ::google::protobuf::EnumOptions* p);
  EnumValueOptions FromProtocol(const ::google::protobuf::EnumValueOptions& p);
  void ToProtocol(const EnumValueOptions& v, ::google::protobuf::EnumValueOptions* p);
  ServiceOptions FromProtocol(const ::google::protobuf::ServiceOptions& p);
  void ToProtocol(const ServiceOptions& v, ::google::protobuf::ServiceOptions* p);
  MethodOptions FromProtocol(const ::google::protobuf::MethodOptions& p);
  void ToProtocol(const MethodOptions& v, ::google::protobuf::MethodOptions* p);
  UninterpretedOption FromProtocol(const ::google::protobuf::UninterpretedOption& p);
  void ToProtocol(const UninterpretedOption& v, ::google::protobuf::UninterpretedOption* p);
  SourceCodeInfo FromProtocol(const ::google::protobuf::SourceCodeInfo& p);
  void ToProtocol(const SourceCodeInfo& v, ::google::protobuf::SourceCodeInfo* p);
} } 