#include <stdint.h>
#include "descriptor.pb.h"

typedef uint32_t u32;
typedef uint64_t u64;

//------------------------------------------------------------------------------
template <typename T>
bool LoadBinaryProto(const char* filename, T* msg)
{
  vector<char> buf;
  FILE* f = fopen(filename, "rb");
  if (!f)
    return false;

  fseek(f, 0, SEEK_END);
  u64 len = ftell(f);
  fseek(f, 0, SEEK_SET);

  buf.resize((size_t)len);
  fread(buf.data(), 1, (size_t)len, f);

  return msg->ParseFromArray(buf.data(), (size_t)len);
}

//------------------------------------------------------------------------------
int main(int argc, char** argv)
{

  using namespace google::protobuf;

  FileDescriptorSet s;
  if (!LoadBinaryProto("/projects/boba/protocol/desc.pb", &s))
    return 1;

  for (const FileDescriptorProto& fileProto : s.file())
  {
    for (const DescriptorProto& descProto : fileProto.message_type())
    {
      printf("name: %s\n", descProto.name().c_str());

      for (const FieldDescriptorProto& fieldProto : descProto.field())
      {
        printf("  field: %s, %d\n", fieldProto.name().c_str(), fieldProto.type());
      }
    }

  }

  return 0;
}