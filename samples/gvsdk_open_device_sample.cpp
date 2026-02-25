#include "GvCameraAPI.h"

#include <iostream>

// -----------------------------------------------------------------------------
// 샘플 목적
// - 첫 번째 장치(인덱스 0)의 기본 정보를 읽어 출력합니다.
// - 실제 캡처 전에 장치 검색/정보조회 흐름을 이해하는 데 도움이 됩니다.
// -----------------------------------------------------------------------------
int main() {
    // [1] SDK 시스템 시작
    if (!gv::GvSystemInit()) {
        std::cerr << "GvSystemInit failed: " << gv::GvGetLastErrorMessage() << "\n";
        return 1;
    }

    // [2] 장치 검색
    const int count = gv::GvSystemGetDeviceCount();
    if (count <= 0) {
        std::cerr << "No devices found: " << gv::GvGetLastErrorMessage() << "\n";
        gv::GvSystemShutdown();
        return 1;
    }

    // [3] 첫 번째 장치 정보 읽기
    gv::GvDeviceInfo info{};
    if (!gv::GvSystemGetDeviceInfo(0, &info)) {
        std::cerr << "GvSystemGetDeviceInfo failed: " << gv::GvGetLastErrorMessage() << "\n";
        gv::GvSystemShutdown();
        return 1;
    }

    // [4] 확인용 출력
    std::cout << "Connected device:\n";
    std::cout << "  name: " << info.name << "\n";
    std::cout << "  sn:   " << info.sn << "\n";
    std::cout << "  ip:   " << info.port << "\n";
    std::cout << "  support_single: " << (info.support_single ? "yes" : "no") << "\n";
    std::cout << "  support_stereo: " << (info.support_stereo ? "yes" : "no") << "\n";

    // [5] SDK 시스템 종료
    gv::GvSystemShutdown();
    return 0;
}
