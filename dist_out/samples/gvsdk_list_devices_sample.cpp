#include "GvCameraAPI.h"

#include <iostream>

// -----------------------------------------------------------------------------
// 샘플 목적
// - SDK 시스템 초기화 후, 현재 검색되는 카메라 개수를 출력합니다.
//
// 초보자 팁
// - 이 샘플은 "카메라가 보이는지"만 빠르게 확인할 때 사용하세요.
// -----------------------------------------------------------------------------
int main() {
    // [1] SDK 시스템 시작
    if (!gv::GvSystemInit()) {
        std::cerr << "GvSystemInit failed: " << gv::GvGetLastErrorMessage() << "\n";
        return 1;
    }

    // [2] 장치 개수 조회
    const int count = gv::GvSystemGetDeviceCount();
    if (count < 0) {
        std::cerr << "GvSystemGetDeviceCount failed: " << gv::GvGetLastErrorMessage() << "\n";
        gv::GvSystemShutdown();
        return 1;
    }

    std::cout << "Devices found: " << count << "\n";
    if (count == 0) {
        std::cout << "No camera detected. Check power/network/driver settings.\n";
    }

    // [3] SDK 시스템 종료
    gv::GvSystemShutdown();
    return 0;
}
