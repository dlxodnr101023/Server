#pragma once
#include <fstream>
#include <string>

// .env 파일에서 특정 키의 값을 읽어오는 함수
inline std::string getEnvString() {
    std::ifstream file("/Users/taeuk/Server/.env");
    std::string line;
    std::string key = "DB_CONNECTION_STRING=";

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // 줄이 "DB_CONNECTION_STRING="으로 시작하는지 확인
            if (line.compare(0, key.length(), key) == 0) {
                // 키 뒷부분부터 끝까지 잘라서 반환
                return line.substr(key.length());
            }
        }
    }
    return ""; // 못 찾으면 빈 문자열
}