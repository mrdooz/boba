xcopy /D /U /Y ..\..\protocol\*.proto
protoc --cpp_out=. --proto_path=/projects/protobuf-2.6.0/src --proto_path=. --include_imports --include_source_info -oeffects.desc *.proto
python codegen.py --namespace editor
python codegen.py --namespace boba --outdir ../../protocol --engine
rem protoc --cpp_out=. --proto_path=/projects/protobuf-2.5.0/src --proto_path=../../protocol/ ../../protocol/effect_settings*.proto ../../protocol/common.proto
pause