protoc --cpp_out=. --proto_path=/projects/protobuf-2.5.0/src --proto_path=. *.proto
protoc --cpp_out=. --proto_path=/projects/protobuf-2.5.0/src --proto_path=../../protocol/ ../../protocol/effect_settings*.proto ../../protocol/common.proto
pause
