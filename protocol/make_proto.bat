protoc --cpp_out=. --proto_path=/projects/protobuf-2.5.0/src --proto_path=. *.proto
protoc --plugin=protoc-gen-ANT=comptest.exe --ANT_out=. --proto_path=/projects/protobuf-2.5.0/src --proto_path=. *.proto
rem protoc --descriptor_set_out=desc.pb --include_imports --include_source_info --proto_path=/projects/protobuf-2.5.0/src --proto_path=. *.proto