#include "GvCameraAPI.h"

#include <iostream>

namespace {

// 저장 파일명 (실행 폴더 기준)
constexpr const char* kOutput2DPath = "gvsdk_capture2d_left.png";

}  // namespace

// -----------------------------------------------------------------------------
// 샘플 목적
// - Single 카메라에서 2D 이미지를 1회 캡처하고 PNG 파일로 저장합니다.
//
// 참고
// - 현재 배포 SDK 기준으로 Stereo 경로는 사용하지 않습니다.
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

    // [3] 첫 번째 장치 정보 확인
    gv::GvDeviceInfo info{};
    if (!gv::GvSystemGetDeviceInfo(0, &info)) {
        std::cerr << "GvSystemGetDeviceInfo failed: " << gv::GvGetLastErrorMessage() << "\n";
        gv::GvSystemShutdown();
        return 1;
    }

    std::cout << "Selected device:\n";
    std::cout << "  name: " << info.name << "\n";
    std::cout << "  sn:   " << info.sn << "\n";
    std::cout << "  ip:   " << info.port << "\n";
    std::cout << "  mode: 2d (single only)\n";

    if (!info.support_single) {
        std::cerr << "This sample supports Single camera only.\n";
        gv::GvSystemShutdown();
        return 1;
    }

    // [4] Single 카메라 생성 및 연결
    gv::GvSingle cam = gv::GvSingle::Create(0, gv::CameraID_Left);
    if (!cam.IsValid()) {
        std::cerr << "GvSingle::Create failed\n";
        gv::GvSystemShutdown();
        return 1;
    }

    if (!cam.Open()) {
        std::cerr << "GvSingle::Open failed: " << gv::GvGetLastErrorMessage() << "\n";
        gv::GvSingle::Destroy(cam);
        gv::GvSystemShutdown();
        return 1;
    }

    // [5] 2D 캡처 옵션 설정
    // - 기본값과 동일해도 샘플에서는 명시적으로 넣어 튜닝 포인트를 보여줍니다.
    gv::GvSingle::GvCaptureOptions captureOpts;
    captureOpts.capture_mode = gv::CaptureMode_Normal;
    captureOpts.exposure_time_2d = 50;
    captureOpts.gain_2d = 1.0f;
    captureOpts.gamma_2d = 1.0f;
    captureOpts.use_projector_capturing_2d_image = false;

    std::cout << "Capture2D options:\n";
    std::cout << "  exposure_time_2d: " << captureOpts.exposure_time_2d << "\n";
    std::cout << "  gain_2d:          " << captureOpts.gain_2d << "\n";
    std::cout << "  gamma_2d:         " << captureOpts.gamma_2d << "\n";
    std::cout << "  capture_mode:     " << static_cast<int>(captureOpts.capture_mode)
              << " (CaptureMode_Normal)\n";

    // [6] 2D 캡처
    bool ok = cam.Capture2D(captureOpts);
    if (ok) {
        gv::GvImage image = cam.GetImage();
        if (image.IsValid()) {
            const gv::GvSize size = image.GetSize();
            std::cout << "Capture2D OK: " << size.width << "x" << size.height
                      << " (" << gv::GvImageType::ToString(image.GetType()) << ")\n";
            if (image.SaveImage(kOutput2DPath)) {
                std::cout << "Saved: " << kOutput2DPath << "\n";
            } else {
                std::cerr << "SaveImage failed: " << gv::GvGetLastErrorMessage() << "\n";
                ok = false;
            }
        } else {
            std::cerr << "Capture2D succeeded but image is invalid\n";
            ok = false;
        }
    }

    // [7] 자원 정리
    cam.Close();
    gv::GvSingle::Destroy(cam);
    gv::GvSystemShutdown();

    if (!ok) {
        std::cerr << "Capture failed: " << gv::GvGetLastErrorMessage() << "\n";
        return 1;
    }
    return 0;
}
