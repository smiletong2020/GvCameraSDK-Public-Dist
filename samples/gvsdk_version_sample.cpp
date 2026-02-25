#include "GvCameraAPI.h"

#include <iostream>

// -----------------------------------------------------------------------------
// 샘플 목적
// - SDK DLL이 정상 로드되는지 가장 빠르게 확인합니다.
// - 카메라 연결 없이도 실행 가능합니다.
// -----------------------------------------------------------------------------
int main() {
    // SDK 버전 문자열을 얻습니다.
    const char* version = gv::GvGetVersion();
    if (!version) {
        std::cerr << "GvGetVersion returned null\n";
        std::cerr << "Check DLL path and dependent runtime DLLs.\n";
        return 1;
    }

    std::cout << "GvCameraSDK version: " << version << "\n";
    return 0;
}
