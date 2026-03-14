#!/bin/bash

PROTO_FILE="packet.proto"
OUT_CS="./gen/cs"
OUT_CPP="./gen/cpp"
PROTOC="$HOME/vcpkg/packages/protobuf_arm64-osx/tools/protobuf/protoc"

mkdir -p "$OUT_CS" "$OUT_CPP"

# protoc 확인
if [ ! -f "$PROTOC" ]; then
    echo "[ERROR] protoc 를 찾을 수 없습니다: $PROTOC"
    exit 1
fi

# C++ 생성
echo "[*] C++ 생성 중..."
"$PROTOC" \
    --cpp_out="$OUT_CPP" \
    "$PROTO_FILE"

if [ $? -ne 0 ]; then
    echo "[ERROR] C++ 생성 실패"
    exit 1
fi
echo "[OK] C++ -> $OUT_CPP"

# C# 생성 (grpc 플러그인 필요)
echo "[*] C# 생성 중..."

if command -v grpc_csharp_plugin &> /dev/null; then
    GRPC_PLUGIN=$(which grpc_csharp_plugin)
    "$PROTOC" \
        --csharp_out="$OUT_CS" \
        --grpc_out="$OUT_CS" \
        --plugin=protoc-gen-grpc="$GRPC_PLUGIN" \
        "$PROTO_FILE"
else
    echo "[WARN] grpc_csharp_plugin 없음 - 메시지 클래스만 생성합니다."
    "$PROTOC" \
        --csharp_out="$OUT_CS" \
        "$PROTO_FILE"
fi

if [ $? -ne 0 ]; then
    echo "[ERROR] C# 생성 실패"
    exit 1
fi
echo "[OK] C# -> $OUT_CS"

echo ""
echo "생성 완료:"
ls "$OUT_CPP"
ls "$OUT_CS"