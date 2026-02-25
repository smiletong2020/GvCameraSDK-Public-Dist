# GvCameraSDK 릴리즈 노트

## 2026-02-11
- 연결 API 권장 정책을 명확화했다.
  - Primary: `connectGvCamera()` / `disconnectGvCamera()`
  - Specialized: `connectGvDevice()` / `disconnectGvDevice()`
  - 개별 2D 좌/우 연결은 엔지니어링 API 전용으로 분리
- 엔지니어링 API 추가:
  - `GvEngSearchCameraDevices()`, `GvEngGetCameraDeviceList()`
  - `GvEngConnectCamera2D(index)`, `GvEngDisconnectCamera2D(index)`
- 공개 API 조정:
  - 공개 API 경로에서 개별 좌/우 연결 경로를 제거하고 `connectGvCamera` 중심으로 정리
  - 엔지니어링 API와 중복되는 시스템 제어 API를 공용 헤더에서 제거
    - 제거: `GvSystemSetPreferredInterface/Subnet`
    - 제거: `GvSystemGetDeviceIpList`, `GvSystemAssignNewIp`
    - 제거: `GvSystemBuildNicPreflightReport`, `GvSystemFixIp`, `GvSystemAssignNewIpWithTemporaryInterface`
  - 공용 API 카메라 용어를 `GvCamera3D`로 병행 표기
    - 추가: `using GvCamera3D = GvCamera`
    - `GvX1::GetCamera`, `GvX2::GetCamera` 반환 타입을 `GvCamera3D`로 표기
- 문서 반영:
  - `docs/GvCameraSDK-Hardware-Test-Plan.md`
  - `docs/GV_CAMERA_API_MATRIX.md`
- 샘플 반영:
  - `samples/gveng_capture2d_softtrigger_sample.cpp`에서 기본 연결 시나리오를 `GvEngConnectCamera3D()` 중심으로 정리
  - 엔지니어링 API 이름을 3D 의미가 드러나도록 조정
    - 추가: `GvEngConnectCamera3D()`, `GvEngDisconnectCamera3D()`
    - 정리: 기존 호환 래퍼(`GvEngConnect3DCamera`, `GvEngDisconnect3DCamera`, `GvEngConnectCamera`, `GvEngDisconnectCamera`) 제거
  - `samples/gvsdk_fix_ip_sample.cpp`를 엔지니어링 API 기반으로 전환



## Firmware Compatibility (Template)
- SDK Version:
- Minimum Supported FW:
- Recommended FW:
- Tested FW:
- Feature Gates:
  - Extended code-domain payload: FW >= 1.0.8
- Notes:
