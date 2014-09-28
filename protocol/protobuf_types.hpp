#pragma once


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
    
  struct FileDescriptorSet
  {

    vector<FileDescriptorProto> file;
  };
  struct FileDescriptorProto
  {

    string name;
    string package;
    vector<string> dependency;
    vector<s32> publicDependency;
    vector<s32> weakDependency;
    vector<DescriptorProto> messageType;
    vector<EnumDescriptorProto> enumType;
    vector<ServiceDescriptorProto> service;
    vector<FieldDescriptorProto> extension;
    FileOptions options;
    SourceCodeInfo sourceCodeInfo;
  };
  struct DescriptorProto
  {

    string name;
    vector<FieldDescriptorProto> field;
    vector<FieldDescriptorProto> extension;
    vector<DescriptorProto> nestedType;
    vector<EnumDescriptorProto> enumType;
    vector<::boba::protobuf::DescriptorProto::ExtensionRange> extensionRange;
    MessageOptions options;
  };
  struct FieldDescriptorProto
  {
    enum class Type
    {
      TYPE_DOUBLE = 1,
      TYPE_FLOAT = 2,
      TYPE_INT64 = 3,
      TYPE_UINT64 = 4,
      TYPE_INT32 = 5,
      TYPE_FIXED64 = 6,
      TYPE_FIXED32 = 7,
      TYPE_BOOL = 8,
      TYPE_STRING = 9,
      TYPE_GROUP = 10,
      TYPE_MESSAGE = 11,
      TYPE_BYTES = 12,
      TYPE_UINT32 = 13,
      TYPE_ENUM = 14,
      TYPE_SFIXED32 = 15,
      TYPE_SFIXED64 = 16,
      TYPE_SINT32 = 17,
      TYPE_SINT64 = 18,
    };
    enum class Label
    {
      LABEL_OPTIONAL = 1,
      LABEL_REQUIRED = 2,
      LABEL_REPEATED = 3,
    };

    string name;
    s32 number;
    FieldDescriptorProto::Label label;
    FieldDescriptorProto::Type type;
    string typeName;
    string extendee;
    string defaultValue;
    FieldOptions options;
  };
  struct EnumDescriptorProto
  {

    string name;
    vector<EnumValueDescriptorProto> value;
    EnumOptions options;
  };
  struct EnumValueDescriptorProto
  {

    string name;
    s32 number;
    EnumValueOptions options;
  };
  struct ServiceDescriptorProto
  {

    string name;
    vector<MethodDescriptorProto> method;
    ServiceOptions options;
  };
  struct MethodDescriptorProto
  {

    string name;
    string inputType;
    string outputType;
    MethodOptions options;
  };
  struct FileOptions
  {
    enum class OptimizeMode
    {
      SPEED = 1,
      CODE_SIZE = 2,
      LITE_RUNTIME = 3,
    };

    string javaPackage;
    string javaOuterClassname;
    bool javaMultipleFiles = false;
    bool javaGenerateEqualsAndHash = false;
    FileOptions::OptimizeMode optimizeFor = OptimizeMode::SPEED;
    string goPackage;
    bool ccGenericServices = false;
    bool javaGenericServices = false;
    bool pyGenericServices = false;
    vector<UninterpretedOption> uninterpretedOption;
  };
  struct MessageOptions
  {

    bool messageSetWireFormat = false;
    bool noStandardDescriptorAccessor = false;
    vector<UninterpretedOption> uninterpretedOption;
  };
  struct FieldOptions
  {
    enum class CType
    {
      STRING = 0,
      CORD = 1,
      STRING_PIECE = 2,
    };

    FieldOptions::CType ctype = CType::STRING;
    bool packed;
    bool lazy = false;
    bool deprecated = false;
    string experimentalMapKey;
    bool weak = false;
    vector<UninterpretedOption> uninterpretedOption;
  };
  struct EnumOptions
  {

    bool allowAlias = true;
    vector<UninterpretedOption> uninterpretedOption;
  };
  struct EnumValueOptions
  {

    vector<UninterpretedOption> uninterpretedOption;
  };
  struct ServiceOptions
  {

    vector<UninterpretedOption> uninterpretedOption;
  };
  struct MethodOptions
  {

    vector<UninterpretedOption> uninterpretedOption;
  };
  struct UninterpretedOption
  {

    vector<::boba::protobuf::UninterpretedOption::NamePart> name;
    string identifierValue;
    u64 positiveIntValue;
    s64 negativeIntValue;
    double doubleValue;
    vector<uint8_t> stringValue;
    string aggregateValue;
  };
  struct SourceCodeInfo
  {

    vector<::boba::protobuf::SourceCodeInfo::Location> location;
  };
  
} } 