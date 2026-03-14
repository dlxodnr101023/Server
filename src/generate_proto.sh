PROTO_FILE="packet.proto"
OUT_CS="./gen/cs"
OUT_CPP="./gen/cpp"
PROTOC="$HOME/vcpkg/packages/protobuf_arm64-osx/tools/protobuf/protoc"

mkdir -p "$OUT_CS" "$OUT_CPP"

if [ ! -f "$PROTOC" ]; then
    echo "Error: protoc not found at $PROTOC"
    exit 1
fi

echo "Building C++ files..."
if "$PROTOC" --cpp_out="$OUT_CPP" "$PROTO_FILE"; then
    echo "[OK] C++ -> $OUT_CPP"
else
    echo "Error: Failed to build C++ files"
    exit 1
fi

echo "Building C# files..."

GRPC_PLUGIN=$(command -v grpc_csharp_plugin)

if [ -n "$GRPC_PLUGIN" ]; then
    echo "Found gRPC plugin: $GRPC_PLUGIN"
    "$PROTOC" \
        --csharp_out="$OUT_CS" \
        --grpc_out="$OUT_CS" \
        --plugin=protoc-gen-grpc="$GRPC_PLUGIN" \
        "$PROTO_FILE"
else
    echo "Warning: grpc_csharp_plugin not found. Building standard C# only."
    "$PROTOC" --csharp_out="$OUT_CS" "$PROTO_FILE"
fi

if [ $? -eq 0 ]; then
    echo "[OK] C# -> $OUT_CS"
else
    echo "Error: Failed to build C# files"
    exit 1
fi

echo -e "\nBuild Success!"
ls -R "$OUT_CPP" "$OUT_CS"