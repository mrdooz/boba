#include "protobuf_proto.hpp"
#include "protobuf_types.hpp"

namespace boba { namespace protobuf { 
  FileDescriptorSet FromProtocol(const ::google::protobuf::FileDescriptorSet& p)
  {
    FileDescriptorSet res;
    for (const auto& x : p.file() )
      res.file.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const FileDescriptorSet& v, ::google::protobuf::FileDescriptorSet* p)
  {
    for (const auto& x : v.file)
      ::boba::protobuf::ToProtocol(x, p->add_file());
  }

  FileDescriptorProto FromProtocol(const ::google::protobuf::FileDescriptorProto& p)
  {
    FileDescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    if (p.has_package())
      res.package = p.package();

    for (const auto& x : p.dependency() )
      res.dependency.push_back(x);

    for (const auto& x : p.public_dependency() )
      res.publicDependency.push_back(x);

    for (const auto& x : p.weak_dependency() )
      res.weakDependency.push_back(x);

    for (const auto& x : p.message_type() )
      res.messageType.push_back(::boba::protobuf::FromProtocol(x));

    for (const auto& x : p.enum_type() )
      res.enumType.push_back(::boba::protobuf::FromProtocol(x));

    for (const auto& x : p.service() )
      res.service.push_back(::boba::protobuf::FromProtocol(x));

    for (const auto& x : p.extension() )
      res.extension.push_back(::boba::protobuf::FromProtocol(x));

    if (p.has_options())
      res.options = ::boba::protobuf::FromProtocol(p.options());

    if (p.has_source_code_info())
      res.sourceCodeInfo = ::boba::protobuf::FromProtocol(p.source_code_info());

    return res;
  }

  void ToProtocol(const FileDescriptorProto& v, ::google::protobuf::FileDescriptorProto* p)
  {
    p->set_name(v.name);
    p->set_package(v.package);
    for (const auto& x : v.dependency)
      p->add_dependency(x);
    for (const auto& x : v.publicDependency)
      p->add_public_dependency(x);
    for (const auto& x : v.weakDependency)
      p->add_weak_dependency(x);
    for (const auto& x : v.messageType)
      ::boba::protobuf::ToProtocol(x, p->add_message_type());
    for (const auto& x : v.enumType)
      ::boba::protobuf::ToProtocol(x, p->add_enum_type());
    for (const auto& x : v.service)
      ::boba::protobuf::ToProtocol(x, p->add_service());
    for (const auto& x : v.extension)
      ::boba::protobuf::ToProtocol(x, p->add_extension());
    ::boba::protobuf::ToProtocol(v.options, p->mutable_options());
    ::boba::protobuf::ToProtocol(v.sourceCodeInfo, p->mutable_source_code_info());
  }

  DescriptorProto FromProtocol(const ::google::protobuf::DescriptorProto& p)
  {
    DescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    for (const auto& x : p.field() )
      res.field.push_back(::boba::protobuf::FromProtocol(x));

    for (const auto& x : p.extension() )
      res.extension.push_back(::boba::protobuf::FromProtocol(x));

    for (const auto& x : p.nested_type() )
      res.nestedType.push_back(::boba::protobuf::FromProtocol(x));

    for (const auto& x : p.enum_type() )
      res.enumType.push_back(::boba::protobuf::FromProtocol(x));

    for (const auto& x : p.extension_range() )
      res.extensionRange.push_back(::boba::protobuf::DescriptorProto::FromProtocol(x));

    for (const auto& x : p.oneof_decl() )
      res.oneofDecl.push_back(::boba::protobuf::FromProtocol(x));

    if (p.has_options())
      res.options = ::boba::protobuf::FromProtocol(p.options());

    return res;
  }

  void ToProtocol(const DescriptorProto& v, ::google::protobuf::DescriptorProto* p)
  {
    p->set_name(v.name);
    for (const auto& x : v.field)
      ::boba::protobuf::ToProtocol(x, p->add_field());
    for (const auto& x : v.extension)
      ::boba::protobuf::ToProtocol(x, p->add_extension());
    for (const auto& x : v.nestedType)
      ::boba::protobuf::ToProtocol(x, p->add_nested_type());
    for (const auto& x : v.enumType)
      ::boba::protobuf::ToProtocol(x, p->add_enum_type());
    for (const auto& x : v.extensionRange)
      ::boba::protobuf::DescriptorProto::ToProtocol(x, p->add_extension_range());
    for (const auto& x : v.oneofDecl)
      ::boba::protobuf::ToProtocol(x, p->add_oneof_decl());
    ::boba::protobuf::ToProtocol(v.options, p->mutable_options());
  }

  FieldDescriptorProto FromProtocol(const ::google::protobuf::FieldDescriptorProto& p)
  {
    FieldDescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    if (p.has_number())
      res.number = p.number();

    res.label = (FieldDescriptorProto::Label)p.label();

    res.type = (FieldDescriptorProto::Type)p.type();

    if (p.has_type_name())
      res.typeName = p.type_name();

    if (p.has_extendee())
      res.extendee = p.extendee();

    if (p.has_default_value())
      res.defaultValue = p.default_value();

    if (p.has_oneof_index())
      res.oneofIndex = p.oneof_index();

    if (p.has_options())
      res.options = ::boba::protobuf::FromProtocol(p.options());

    return res;
  }

  void ToProtocol(const FieldDescriptorProto& v, ::google::protobuf::FieldDescriptorProto* p)
  {
    p->set_name(v.name);
    p->set_number(v.number);
    p->set_type_name(v.typeName);
    p->set_extendee(v.extendee);
    p->set_default_value(v.defaultValue);
    p->set_oneof_index(v.oneofIndex);
    ::boba::protobuf::ToProtocol(v.options, p->mutable_options());
  }

  OneofDescriptorProto FromProtocol(const ::google::protobuf::OneofDescriptorProto& p)
  {
    OneofDescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    return res;
  }

  void ToProtocol(const OneofDescriptorProto& v, ::google::protobuf::OneofDescriptorProto* p)
  {
    p->set_name(v.name);
  }

  EnumDescriptorProto FromProtocol(const ::google::protobuf::EnumDescriptorProto& p)
  {
    EnumDescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    for (const auto& x : p.value() )
      res.value.push_back(::boba::protobuf::FromProtocol(x));

    if (p.has_options())
      res.options = ::boba::protobuf::FromProtocol(p.options());

    return res;
  }

  void ToProtocol(const EnumDescriptorProto& v, ::google::protobuf::EnumDescriptorProto* p)
  {
    p->set_name(v.name);
    for (const auto& x : v.value)
      ::boba::protobuf::ToProtocol(x, p->add_value());
    ::boba::protobuf::ToProtocol(v.options, p->mutable_options());
  }

  EnumValueDescriptorProto FromProtocol(const ::google::protobuf::EnumValueDescriptorProto& p)
  {
    EnumValueDescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    if (p.has_number())
      res.number = p.number();

    if (p.has_options())
      res.options = ::boba::protobuf::FromProtocol(p.options());

    return res;
  }

  void ToProtocol(const EnumValueDescriptorProto& v, ::google::protobuf::EnumValueDescriptorProto* p)
  {
    p->set_name(v.name);
    p->set_number(v.number);
    ::boba::protobuf::ToProtocol(v.options, p->mutable_options());
  }

  ServiceDescriptorProto FromProtocol(const ::google::protobuf::ServiceDescriptorProto& p)
  {
    ServiceDescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    for (const auto& x : p.method() )
      res.method.push_back(::boba::protobuf::FromProtocol(x));

    if (p.has_options())
      res.options = ::boba::protobuf::FromProtocol(p.options());

    return res;
  }

  void ToProtocol(const ServiceDescriptorProto& v, ::google::protobuf::ServiceDescriptorProto* p)
  {
    p->set_name(v.name);
    for (const auto& x : v.method)
      ::boba::protobuf::ToProtocol(x, p->add_method());
    ::boba::protobuf::ToProtocol(v.options, p->mutable_options());
  }

  MethodDescriptorProto FromProtocol(const ::google::protobuf::MethodDescriptorProto& p)
  {
    MethodDescriptorProto res;
    if (p.has_name())
      res.name = p.name();

    if (p.has_input_type())
      res.inputType = p.input_type();

    if (p.has_output_type())
      res.outputType = p.output_type();

    if (p.has_options())
      res.options = ::boba::protobuf::FromProtocol(p.options());

    return res;
  }

  void ToProtocol(const MethodDescriptorProto& v, ::google::protobuf::MethodDescriptorProto* p)
  {
    p->set_name(v.name);
    p->set_input_type(v.inputType);
    p->set_output_type(v.outputType);
    ::boba::protobuf::ToProtocol(v.options, p->mutable_options());
  }

  FileOptions FromProtocol(const ::google::protobuf::FileOptions& p)
  {
    FileOptions res;
    if (p.has_java_package())
      res.javaPackage = p.java_package();

    if (p.has_java_outer_classname())
      res.javaOuterClassname = p.java_outer_classname();

    if (p.has_java_multiple_files())
      res.javaMultipleFiles = p.java_multiple_files();

    if (p.has_java_generate_equals_and_hash())
      res.javaGenerateEqualsAndHash = p.java_generate_equals_and_hash();

    if (p.has_java_string_check_utf8())
      res.javaStringCheckUtf8 = p.java_string_check_utf8();

    res.optimizeFor = (FileOptions::OptimizeMode)p.optimize_for();

    if (p.has_go_package())
      res.goPackage = p.go_package();

    if (p.has_cc_generic_services())
      res.ccGenericServices = p.cc_generic_services();

    if (p.has_java_generic_services())
      res.javaGenericServices = p.java_generic_services();

    if (p.has_py_generic_services())
      res.pyGenericServices = p.py_generic_services();

    if (p.has_deprecated())
      res.deprecated = p.deprecated();

    for (const auto& x : p.uninterpreted_option() )
      res.uninterpretedOption.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const FileOptions& v, ::google::protobuf::FileOptions* p)
  {
    p->set_java_package(v.javaPackage);
    p->set_java_outer_classname(v.javaOuterClassname);
    p->set_java_multiple_files(v.javaMultipleFiles);
    p->set_java_generate_equals_and_hash(v.javaGenerateEqualsAndHash);
    p->set_java_string_check_utf8(v.javaStringCheckUtf8);
    p->set_go_package(v.goPackage);
    p->set_cc_generic_services(v.ccGenericServices);
    p->set_java_generic_services(v.javaGenericServices);
    p->set_py_generic_services(v.pyGenericServices);
    p->set_deprecated(v.deprecated);
    for (const auto& x : v.uninterpretedOption)
      ::boba::protobuf::ToProtocol(x, p->add_uninterpreted_option());
  }

  MessageOptions FromProtocol(const ::google::protobuf::MessageOptions& p)
  {
    MessageOptions res;
    if (p.has_message_set_wire_format())
      res.messageSetWireFormat = p.message_set_wire_format();

    if (p.has_no_standard_descriptor_accessor())
      res.noStandardDescriptorAccessor = p.no_standard_descriptor_accessor();

    if (p.has_deprecated())
      res.deprecated = p.deprecated();

    for (const auto& x : p.uninterpreted_option() )
      res.uninterpretedOption.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const MessageOptions& v, ::google::protobuf::MessageOptions* p)
  {
    p->set_message_set_wire_format(v.messageSetWireFormat);
    p->set_no_standard_descriptor_accessor(v.noStandardDescriptorAccessor);
    p->set_deprecated(v.deprecated);
    for (const auto& x : v.uninterpretedOption)
      ::boba::protobuf::ToProtocol(x, p->add_uninterpreted_option());
  }

  FieldOptions FromProtocol(const ::google::protobuf::FieldOptions& p)
  {
    FieldOptions res;
    res.ctype = (FieldOptions::CType)p.ctype();

    if (p.has_packed())
      res.packed = p.packed();

    if (p.has_lazy())
      res.lazy = p.lazy();

    if (p.has_deprecated())
      res.deprecated = p.deprecated();

    if (p.has_experimental_map_key())
      res.experimentalMapKey = p.experimental_map_key();

    if (p.has_weak())
      res.weak = p.weak();

    for (const auto& x : p.uninterpreted_option() )
      res.uninterpretedOption.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const FieldOptions& v, ::google::protobuf::FieldOptions* p)
  {
    p->set_packed(v.packed);
    p->set_lazy(v.lazy);
    p->set_deprecated(v.deprecated);
    p->set_experimental_map_key(v.experimentalMapKey);
    p->set_weak(v.weak);
    for (const auto& x : v.uninterpretedOption)
      ::boba::protobuf::ToProtocol(x, p->add_uninterpreted_option());
  }

  EnumOptions FromProtocol(const ::google::protobuf::EnumOptions& p)
  {
    EnumOptions res;
    if (p.has_allow_alias())
      res.allowAlias = p.allow_alias();

    if (p.has_deprecated())
      res.deprecated = p.deprecated();

    for (const auto& x : p.uninterpreted_option() )
      res.uninterpretedOption.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const EnumOptions& v, ::google::protobuf::EnumOptions* p)
  {
    p->set_allow_alias(v.allowAlias);
    p->set_deprecated(v.deprecated);
    for (const auto& x : v.uninterpretedOption)
      ::boba::protobuf::ToProtocol(x, p->add_uninterpreted_option());
  }

  EnumValueOptions FromProtocol(const ::google::protobuf::EnumValueOptions& p)
  {
    EnumValueOptions res;
    if (p.has_deprecated())
      res.deprecated = p.deprecated();

    for (const auto& x : p.uninterpreted_option() )
      res.uninterpretedOption.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const EnumValueOptions& v, ::google::protobuf::EnumValueOptions* p)
  {
    p->set_deprecated(v.deprecated);
    for (const auto& x : v.uninterpretedOption)
      ::boba::protobuf::ToProtocol(x, p->add_uninterpreted_option());
  }

  ServiceOptions FromProtocol(const ::google::protobuf::ServiceOptions& p)
  {
    ServiceOptions res;
    if (p.has_deprecated())
      res.deprecated = p.deprecated();

    for (const auto& x : p.uninterpreted_option() )
      res.uninterpretedOption.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const ServiceOptions& v, ::google::protobuf::ServiceOptions* p)
  {
    p->set_deprecated(v.deprecated);
    for (const auto& x : v.uninterpretedOption)
      ::boba::protobuf::ToProtocol(x, p->add_uninterpreted_option());
  }

  MethodOptions FromProtocol(const ::google::protobuf::MethodOptions& p)
  {
    MethodOptions res;
    if (p.has_deprecated())
      res.deprecated = p.deprecated();

    for (const auto& x : p.uninterpreted_option() )
      res.uninterpretedOption.push_back(::boba::protobuf::FromProtocol(x));

    return res;
  }

  void ToProtocol(const MethodOptions& v, ::google::protobuf::MethodOptions* p)
  {
    p->set_deprecated(v.deprecated);
    for (const auto& x : v.uninterpretedOption)
      ::boba::protobuf::ToProtocol(x, p->add_uninterpreted_option());
  }

  UninterpretedOption FromProtocol(const ::google::protobuf::UninterpretedOption& p)
  {
    UninterpretedOption res;
    for (const auto& x : p.name() )
      res.name.push_back(::boba::protobuf::UninterpretedOption::FromProtocol(x));

    if (p.has_identifier_value())
      res.identifierValue = p.identifier_value();

    if (p.has_positive_int_value())
      res.positiveIntValue = p.positive_int_value();

    if (p.has_negative_int_value())
      res.negativeIntValue = p.negative_int_value();

    if (p.has_double_value())
      res.doubleValue = p.double_value();

    res.stringValue.resize(p.string_value().size());
    memcpy(res.stringValue.data(), p.string_value().data(), p.string_value().size());

    if (p.has_aggregate_value())
      res.aggregateValue = p.aggregate_value();

    return res;
  }

  void ToProtocol(const UninterpretedOption& v, ::google::protobuf::UninterpretedOption* p)
  {
    for (const auto& x : v.name)
      ::boba::protobuf::UninterpretedOption::ToProtocol(x, p->add_name());
    p->set_identifier_value(v.identifierValue);
    p->set_positive_int_value(v.positiveIntValue);
    p->set_negative_int_value(v.negativeIntValue);
    p->set_double_value(v.doubleValue);
    p->set_aggregate_value(v.aggregateValue);
  }

  SourceCodeInfo FromProtocol(const ::google::protobuf::SourceCodeInfo& p)
  {
    SourceCodeInfo res;
    for (const auto& x : p.location() )
      res.location.push_back(::boba::protobuf::SourceCodeInfo::FromProtocol(x));

    return res;
  }

  void ToProtocol(const SourceCodeInfo& v, ::google::protobuf::SourceCodeInfo* p)
  {
    for (const auto& x : v.location)
      ::boba::protobuf::SourceCodeInfo::ToProtocol(x, p->add_location());
  }

	
} } 
