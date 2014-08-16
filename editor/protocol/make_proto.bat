copy /Y ..\..\protocol\effect_settings*.proto
copy /Y ..\..\protocol\common*.proto
protoc --cpp_out=. --proto_path=/projects/protobuf-2.5.0/src --proto_path=. --include_imports --include_source_info -oeffects.desc *.proto
python codegen.py
rem protoc --cpp_out=. --proto_path=/projects/protobuf-2.5.0/src --proto_path=../../protocol/ ../../protocol/effect_settings*.proto ../../protocol/common.proto
pause