#include "GvCameraAPI.h"

#include <cstdint>
#include <fstream>
#include <iostream>

namespace {

// 저장 파일명 (실행 폴더 기준)
constexpr const char* kOutput3DBinPath = "gvsdk_capture3d_pointmap.bin";

void writeInt32BE(std::ofstream& ofs, int value) {
    const std::uint32_t v = static_cast<std::uint32_t>(value);
    const unsigned char buf[4] = {
        static_cast<unsigned char>((v >> 24) & 0xFF),
        static_cast<unsigned char>((v >> 16) & 0xFF),
        static_cast<unsigned char>((v >> 8) & 0xFF),
        static_cast<unsigned char>(v & 0xFF)
    };
    ofs.write(reinterpret_cast<const char*>(buf), sizeof(buf));
}

bool isSupportedTextureType(const gv::GvImageType::Enum type) {
    return type == gv::GvImageType::Mono8 ||
           type == gv::GvImageType::RGB8 ||
           type == gv::GvImageType::BGR8;
}

// PointMap을 간단한 바이너리로 저장합니다.
// 형식:
// - int32(be) width
// - int32(be) height
// - 반복(width*height): float x(mm), float y(mm), float z(mm), rgb(3byte)
//
// 주의:
// - 본 SDK의 포인트 단위는 mm 이므로 별도 단위 변환 없이 그대로 저장합니다.
bool savePointMapBin(const gv::GvPointMap& pointMap, const gv::GvImage& texture, const char* path) {
    if (path == nullptr || !pointMap.IsValid() || !texture.IsValid()) {
        return false;
    }

    const gv::GvSize size = pointMap.GetSize();
    if (size.width <= 0 || size.height <= 0) {
        return false;
    }

    const double* points = pointMap.GetPointDataConstPtr();
    if (points == nullptr) {
        return false;
    }

    const gv::GvSize textureSize = texture.GetSize();
    if (textureSize.width != size.width || textureSize.height != size.height) {
        return false;
    }
    const gv::GvImageType::Enum textureType = texture.GetType();
    if (!isSupportedTextureType(textureType)) {
        return false;
    }
    const unsigned char* textureData = texture.GetDataConstPtr();
    if (textureData == nullptr) {
        return false;
    }

    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open()) {
        return false;
    }

    writeInt32BE(ofs, size.width);
    writeInt32BE(ofs, size.height);

    const std::size_t count = static_cast<std::size_t>(size.width) * static_cast<std::size_t>(size.height);
    for (std::size_t i = 0; i < count; ++i) {
        const std::size_t base = i * 3;
        const float x = static_cast<float>(points[base]);
        const float y = static_cast<float>(points[base + 1]);
        const float z = static_cast<float>(points[base + 2]);
        ofs.write(reinterpret_cast<const char*>(&x), sizeof(float));
        ofs.write(reinterpret_cast<const char*>(&y), sizeof(float));
        ofs.write(reinterpret_cast<const char*>(&z), sizeof(float));

        unsigned char rgb[3]{0, 0, 0};
        if (textureType == gv::GvImageType::Mono8) {
            const unsigned char v = textureData[i];
            rgb[0] = v;
            rgb[1] = v;
            rgb[2] = v;
        } else if (textureType == gv::GvImageType::RGB8) {
            const std::size_t tbase = i * 3;
            rgb[0] = textureData[tbase];
            rgb[1] = textureData[tbase + 1];
            rgb[2] = textureData[tbase + 2];
        } else if (textureType == gv::GvImageType::BGR8) {
            const std::size_t tbase = i * 3;
            rgb[0] = textureData[tbase + 2];
            rgb[1] = textureData[tbase + 1];
            rgb[2] = textureData[tbase];
        }
        ofs.write(reinterpret_cast<const char*>(rgb), sizeof(rgb));
    }

    return ofs.good();
}

}  // namespace

// -----------------------------------------------------------------------------
// 샘플 목적
// - Single 카메라에서 3D 데이터를 1회 캡처합니다.
// - 결과를 BIN 파일로 저장합니다.
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

    // [5] 3D 캡처 옵션 설정
    // - 기본값과 동일해도 샘플에서는 명시적으로 넣어 튜닝 포인트를 보여줍니다.
    gv::GvSingle::GvCaptureOptions captureOpts;
    captureOpts.capture_mode = gv::CaptureMode_Normal;
    captureOpts.exposure_time_2d = 70;
    captureOpts.gain_2d = 5.0f;
    captureOpts.use_projector_capturing_2d_image = true;
    captureOpts.exposure_time_3d = 20;
    captureOpts.gain_3d = 1.0f;
    captureOpts.gamma_3d = 1.0f;
    captureOpts.noise_removal_point_number = 40;
    captureOpts.noise_removal_distance = 3.0f;

    bool ok = cam.Capture(captureOpts);
    if (ok) {
        gv::GvPointMap pointMap = cam.GetPointMap();
        if (pointMap.IsValid()) {
            const gv::GvImage texture = cam.GetImage();
            if (!savePointMapBin(pointMap, texture, kOutput3DBinPath)) {
                std::cerr << "PointMap bin save failed\n";
                ok = false;
            }
        } else {
            std::cerr << "Capture3D succeeded but pointmap is invalid\n";
            ok = false;
        }
    }

    // [6] 자원 정리
    cam.Close();
    gv::GvSingle::Destroy(cam);
    gv::GvSystemShutdown();

    if (!ok) {
        std::cerr << "Capture failed: " << gv::GvGetLastErrorMessage() << "\n";
        return 1;
    }
    return 0;
}
