#include "GvCameraAPI.h"

#include <iostream>

// -----------------------------------------------------------------------------
// 샘플 목적
// - 검색된 모든 장치에 대해 자동 IP 재할당을 시도합니다.
//
// 주의
// - 실제 장치의 네트워크 설정을 바꿀 수 있으므로 테스트 환경에서 먼저 실행하세요.
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

    std::cout << "Before auto IP assign:\n";
    for (int i = 0; i < count; ++i) {
        gv::GvDeviceInfo info{};
        if (!gv::GvSystemGetDeviceInfo(i, &info)) {
            std::cerr << "GvSystemGetDeviceInfo failed(index=" << i
                      << "): " << gv::GvGetLastErrorMessage() << "\n";
            gv::GvSystemShutdown();
            return 1;
        }
        std::cout << "  [" << i << "] " << info.name << " SN=" << info.sn
                  << " IP=" << info.port << "\n";
    }

    // [3] 장치별 자동 IP 할당 시도
    int success = 0;
    int failed = 0;
    for (int i = 0; i < count; ++i) {
        std::cout << "  -> GvSystemAssignNewIpAuto(" << i << ")\n";
        if (!gv::GvSystemAssignNewIpAuto(i)) {
            ++failed;
            std::cout << "     failed: " << gv::GvGetLastErrorMessage() << "\n";
            continue;
        }
        ++success;
    }

    std::cout << "After auto IP assign:\n";
    for (int i = 0; i < count; ++i) {
        gv::GvDeviceInfo info{};
        if (!gv::GvSystemGetDeviceInfo(i, &info)) {
            std::cerr << "GvSystemGetDeviceInfo failed(index=" << i
                      << "): " << gv::GvGetLastErrorMessage() << "\n";
            gv::GvSystemShutdown();
            return 1;
        }
        std::cout << "  [" << i << "] " << info.name << " SN=" << info.sn
                  << " IP=" << info.port << "\n";
    }

    std::cout << "Assign result: success=" << success << ", failed=" << failed << "\n";

    // [4] SDK 시스템 종료
    gv::GvSystemShutdown();
    return failed == 0 ? 0 : 1;
}
