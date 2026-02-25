#pragma once

/**
 * @file GvCameraAPI.h
 * @brief 구조광 3D 카메라 DLL 공개 사용자 인터페이스 헤더(Gv*** 공개 API).
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#ifdef GVCAMERA_EXPORTS
#define GV_PUBLIC_API __declspec(dllexport)
#else
#define GV_PUBLIC_API __declspec(dllimport)
#endif
#define GV_INTERNAL_API
#define GVCAMERA_API GV_INTERNAL_API

namespace gv {

using UserPtr = void*;

typedef uint32_t GvHandleID;

struct GV_PUBLIC_API GvHandle {
    GvHandle() : sid(0), gid(0) {}
    GvHandle(GvHandleID sid_, GvHandleID gid_) : sid(sid_), gid(gid_) {}
    GvHandle(const GvHandle& rhs) : sid(rhs.sid), gid(rhs.gid) {}
    GvHandle& operator=(const GvHandle& rhs) {
        sid = rhs.sid;
        gid = rhs.gid;
        return *this;
    }
    bool operator==(const GvHandle& rhs) const { return sid == rhs.sid && gid == rhs.gid; }
    bool operator!=(const GvHandle& rhs) const { return sid != rhs.sid || gid != rhs.gid; }

    GvHandleID sid;
    GvHandleID gid;
};

struct GV_PUBLIC_API GvSize {
    GvSize() : width(0), height(0) {}
    GvSize(int w, int h) : width(w), height(h) {}
    union {
        int32_t width, cols;
    };
    union {
        int32_t height, rows;
    };
};

inline bool operator==(const GvSize& lhs, const GvSize& rhs) {
    return lhs.width == rhs.width && lhs.height == rhs.height;
}
inline bool operator!=(const GvSize& lhs, const GvSize& rhs) {
    return !(lhs == rhs);
}

struct GV_PUBLIC_API GvROI {
    GvROI() : x(0), y(0), width(0), height(0) {}
    GvROI(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}
    int x;
    int y;
    int width;
    int height;
};

struct GV_PUBLIC_API GvROIRange {
    GvROIRange() : min_x(0), min_y(0), max_x(0), max_y(0) {}
    int min_x;
    int min_y;
    int max_x;
    int max_y;
};

enum GvCameraID {
    CameraID_Left = 0,
    CameraID_Right = 1,
    CameraID_Both = 2,
};

struct GV_PUBLIC_API GvRealtimeImageFrame {
    GvCameraID camera_id = CameraID_Left;
    const unsigned char* data = nullptr;
    int width = 0;
    int height = 0;
    int stride_bytes = 0;
    int channels = 0;
    uint64_t frame_id = 0;
    bool is_color = false;
};

struct GV_PUBLIC_API GvRealtimeImageFpsInfo {
    double camera_fps = 0.0;
    double throttled_fps = 0.0;
    uint64_t received_frames = 0;
    uint64_t callback_frames = 0;
    uint64_t throttled_frames = 0;
    uint64_t queue_dropped_frames = 0;
};

using GvRealtimeImageCallback = void(*)(const GvRealtimeImageFrame* frame, UserPtr user_data);

enum GvPortType {
    PortType_Unknown = 0,
    PortType_USB = 1,
    PortType_GIGE = 2,
};

enum GvNetworkType {
    NetworkType_UNKNOWN = 0,
    NetworkType_STATIC = 1,
    NetworkType_DHCP = 2,
};

enum GvNetworkDevice {
    NetworkDevice_Camera = 0,
    NetworkDevice_Projector = 1,
};

enum GvNetworkDeviceStatus {
    NetworkDeviceStatus_OK = 0,
    NetworkDeviceStatus_Error = 1,
};

enum GvProjectorColor {
    ProjectorColor_None = 0,
    ProjectorColor_Blue = 1,
    ProjectorColor_Red = 2,
    ProjectorColor_White = 3,
};

enum GvCaptureMode {
    CaptureMode_Fast = 1 << 0,
    CaptureMode_Normal = 1 << 1,
    CaptureMode_Ultra = 1 << 2,
    CaptureMode_Robust [[deprecated("Use scan_times instead.")]] = 1 << 3,
    CaptureMode_AntiInterReflection = 1 << 4,
    CaptureMode_SwingLineScan = 1 << 5,
    CaptureMode_FixedLineScan = 1 << 6,
    CaptureMode_All = CaptureMode_Fast | CaptureMode_Normal | CaptureMode_Ultra |
                      (1 << 3) | CaptureMode_AntiInterReflection |
                      CaptureMode_SwingLineScan | CaptureMode_FixedLineScan,
};

enum GvProtectiveCoverStatus {
    ProtectiveCover_Unknown = 0,
    ProtectiveCover_Open = 1,
    ProtectiveCover_Closed = 2,
};

enum GvCameraTempSelector {
    CameraTempSelector_Sensor = 0,
    CameraTempSelector_Board = 1,
};

enum GvBalanceSelector {
    BalanceSelector_R = 0,
    BalanceSelector_G = 1,
    BalanceSelector_B = 2,
};

struct GV_PUBLIC_API GvImageType {
    enum Enum {
        None = 0,
        Mono8 = 1,
        RGB8 = 2,
        BGR8 = 3,
    };
    static const char* ToString(GvImageType::Enum type);
    static size_t GetPixelSize(GvImageType::Enum type);
};

struct GV_PUBLIC_API GvImage {
    static GvImage Create(GvImageType::Enum it, const GvSize sz, unsigned char* data = nullptr, bool own_data = false);
    static GvImage CreateFromFile(const char* addr);
    static void Destroy(GvImage img, bool release = true);

    GvImage Clone() const;
    bool IsValid() const;
    GvSize GetSize() const;
    GvImageType::Enum GetType() const;
    unsigned char* GetDataPtr() const;
    const unsigned char* GetDataConstPtr() const;
    bool SaveImage(const char* addr) const;

    GvHandle m_handle;
};

struct GV_PUBLIC_API GvDepthMap {
    static GvDepthMap Create(const GvSize sz, double* data = nullptr, bool own_data = false);
    static void Destroy(GvDepthMap depthmap, bool release = true);

    bool IsValid();
    GvSize GetSize();
    /**
     * @brief Depth 버퍼 포인터를 반환한다.
     * @details 유효하지 않은 depth 픽셀은 `NaN` 값으로 표현된다.
     */
    double* GetDataPtr();
    /**
     * @brief Depth 버퍼 상수 포인터를 반환한다.
     * @details 유효하지 않은 depth 픽셀은 `NaN` 값으로 표현된다.
     */
    const double* GetDataConstPtr();
    bool SaveDepthMap(const char* address, bool is_m);

    GvHandle m_handle;
};

struct GV_PUBLIC_API GvConfidenceMap {
    static GvConfidenceMap Create(const GvSize sz, double* data = nullptr, bool own_data = false);
    static void Destroy(GvConfidenceMap confidencemap, bool release = true);

    bool IsValid();
    GvSize GetSize();
    double* GetDataPtr();
    const double* GetDataConstPtr();

    GvHandle m_handle;
};

struct GV_PUBLIC_API GvPointMapType {
    enum Enum {
        None = 0,
        PointsOnly = 1,
        PointsNormals = 2,
    };
    static const char* ToString(enum GvPointMapType::Enum e);
};

struct GV_PUBLIC_API GvPointMapUnit {
    /** @brief 포인트맵 좌표 저장 단위(기본 좌표계는 meter). */
    enum Enum {
        Meter = 0,
        Millimeter = 1,
    };
    static const char* ToString(GvPointMapUnit::Enum e);
};

struct GV_PUBLIC_API GvPointMap {
    static GvPointMap Create(GvPointMapType::Enum pmt, const GvSize size, double* data = nullptr, bool owndata = false);
    static void Destroy(GvPointMap pm, bool release = true);

    bool Save(const char* fileName, const GvPointMapUnit::Enum unit, const GvImage& texture);
    bool IsValid() const;
    GvSize GetSize() const;
    /**
     * @brief 포인트 버퍼 포인터를 반환한다.
     * @details 버퍼는 `[x0,y0,z0,x1,y1,z1,...]` 순서이며
     *          좌표 단위는 `meter`이다.
     *          유효하지 않은 포인트는 `(NaN, NaN, NaN)`으로 표현된다.
     */
    double* GetPointDataPtr();
    double* GetNormalDataPtr();
    /**
     * @brief 포인트 버퍼 상수 포인터를 반환한다.
     * @details 버퍼는 `[x0,y0,z0,x1,y1,z1,...]` 순서이며
     *          좌표 단위는 `meter`이다.
     *          유효하지 않은 포인트는 `(NaN, NaN, NaN)`으로 표현된다.
     */
    const double* GetPointDataConstPtr() const;
    const double* GetNormalDataConstPtr() const;
    /**
     * @brief xyz 분리 버퍼를 채운다.
     * @details 원본 좌표 단위는 `meter`이며 출력값은 `원본 * scale`이다.
     *          예: millimeter 변환은 `scale=1000.0`.
     *          유효하지 않은 포인트는 `x/y/z` 각각 `NaN`이 기록된다.
     */
    bool GetPointMapSeperated(double* x, double* y, double* z, const double scale);
    GvPointMap Clone() const;

    GvHandle m_handle;
};

struct GV_PUBLIC_API GvCorrespondMap {
    static GvCorrespondMap Create(const GvSize sz, double* data = nullptr, bool own_data = false);
    static void Destroy(GvCorrespondMap correspondMap, bool release = true);

    bool IsValid();
    GvSize GetSize();
    double* GetDataPtr();
    const double* GetDataConstPtr();

    GvHandle m_handle;
};

struct GV_PUBLIC_API GvDeviceInfo {
    char name[64];
    char sn[64];
    char factroydate[32];
    char port[32];
    GvPortType type;
    GvCameraID cameraid;
    int boardmodel;
    bool support_stereo;
    GvProjectorColor support_color;
    int workingdist_near_mm;
    int workingdist_far_mm;
    char firmware_version[64];
    GvCaptureMode support_capture_mode;
    bool support_single;
    bool support_protective_cover;
};

struct GV_PUBLIC_API GvCustomTransformOptions {
    float rotation[9]{};
    float translation[3]{};
};

struct GV_PUBLIC_API GvSingle {
    struct GV_PUBLIC_API GvCaptureOptions {
        GvCaptureOptions() = default;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        bool calc_normal = false;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        bool transform_to_camera = false;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        unsigned int calc_normal_radius = 0;
        int projector_brightness = 0;
        int light_contrast_threshold = 0;
        int phase_filter_range = 0;
        int exposure_time_2d = 50;
        int exposure_time_3d = 50;
        float gain_2d = 1.0f;
        float gain_3d = 1.0f;
        int hdr_exposure_times = 0;
        int hdr_scan_times[3] = {0, 0, 0};
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        int hdr_projector_brightness[3] = {0, 0, 0};
        float gamma_2d = 1.0f;
        float gamma_3d = 1.0f;
        bool use_projector_capturing_2d_image = false;
        int bilateral_filter_kernal_size = 0;
        float bilateral_filter_depth_sigma = 0.0f;
        float bilateral_filter_space_sigma = 0.0f;
        /** @brief 현재 플래그 자체는 자동 적용 스위치로 동작하지 않습니다. GetAutoNoiseRemovalSetting()으로 추천값을 받아 noise_removal_*를 설정해 사용하세요. */
        bool use_auto_noise_removal = false;
        /** @brief Outlier rejection: minimum neighbor count inside `noise_removal_distance` radius. 0 disables filter. */
        int noise_removal_point_number = 0;
        /** @brief Outlier rejection radius in point-cloud metric space (typically mm). <=0 disables filter. */
        float noise_removal_distance = 0.0f;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        float downsample_distance = 0.0f;
        float confidence_threshold = 0.0f;
        float truncate_z_min = -99999.0f;
        float truncate_z_max = 99999.0f;
        float reflection_filter_threshold = 0.0f;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        float smooth_sigma = 0.0f;
        GvCaptureMode capture_mode = CaptureMode_Normal;
        GvROI roi{};
        int hdr_exposuretime_content[3] = {0, 0, 0};
        float hdr_gain_3d[3] = {0.0f, 0.0f, 0.0f};
        int scan_times = 0;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        GvProjectorColor projector_color = ProjectorColor_None;
        int line_scanner_scan_time_ms = 0;
        int line_scanner_exposure_time_us = 0;
        int line_scanner_min_distance = 0;
        int line_scanner_max_distance = 0;
        uint16_t line_scanner_laser_position = 0;
        bool correspond2d = false;
    };

    struct GV_PUBLIC_API GvCollectionCallBackInfo {
        GvImage image;
    };

    struct GV_PUBLIC_API GvCalculationCallBackInfo {
        GvImage image;
        GvPointMap pointmap;
        GvDepthMap depthmap;
        GvConfidenceMap confidencemap;
    };

    using CollectionCallBackPtr = void(*)(const GvCollectionCallBackInfo&, const GvCaptureOptions&, UserPtr);
    using CalculationCallBackPtr = void(*)(const GvCalculationCallBackInfo&, const GvCaptureOptions&, UserPtr);

    static GvSingle Create(int deviceIndex, enum GvCameraID camid);
    static void Destroy(GvSingle& x);

    bool IsValid();
    bool Open();
    void Close();
    bool IsOpen();
    bool IsPhysicallyConnected();
    bool OpenProtectiveCover();
    bool OpenProtectiveCoverAsync();
    bool CloseProtectiveCover();
    bool CloseProtectiveCoverAsync();
    bool ResetProtectiveCover();
    bool GetProtectiveCoverStatus(GvProtectiveCoverStatus&);
    bool SetCollectionCallBack(GvSingle::CollectionCallBackPtr cb, UserPtr ctx);
    bool SetCalculationCallBack(GvSingle::CalculationCallBackPtr cb, UserPtr ctx);
    bool Capture(const GvCaptureOptions& opts);
    bool Capture();
    bool Capture2D(const GvCaptureOptions& opts);
    bool Capture2D();
    bool SetBandwidth(float);
    bool GetBandwidth(float& percent);
    bool SetCustomTransformation(const GvCustomTransformOptions&);
    bool GetCustomTransformation(GvCustomTransformOptions&);
    bool SaveEncodedImagesData(const char*);
    bool GenerateEncodedMap();
    GvImage GetEncodeMap();
    GvImage GetEncodedRawImage(uint16_t);
    GvImage GetImage();
    GvImage GetRawImage(uint16_t, const bool);
    bool GetRawImage(GvImage& img, uint16_t);
    /**
     * @brief 최근 3D 캡처 결과 depth map을 반환한다.
     * @details 유효하지 않은 depth 픽셀은 `NaN`이다.
     */
    GvDepthMap GetDepthMap();
    /**
     * @brief 최근 3D 캡처 결과 point map을 반환한다.
     * @details 좌표 단위는 `meter`이며 유효하지 않은 포인트는 `(NaN, NaN, NaN)`이다.
     */
    GvPointMap GetPointMap();
    GvConfidenceMap GetConfidenceMap();
    bool GetExtrinsicMatrix(float*);
    bool GetIntrinsicParameters(float* instrinsic_matrix, float* distortion);
    bool GetProjectorTemperature(float&);
    bool GetCameraTemperature(GvCameraTempSelector, float&);
    bool SetBalanceRatio(GvBalanceSelector, float);
    bool GetBalanceRatio(GvBalanceSelector, float*);
    bool GetBalanceRange(GvBalanceSelector, float* min_value, float* max_value);
    bool AutoWhiteBalance(int, const GvCaptureOptions&, const ::gv::GvROI&);
    bool GetExposureTimeRange(int* min_value, int* max_value);
    bool GetGainRange(float* min_value, float* max_value);
    bool GetGammaRange(float* min_value, float* max_value);
    bool SaveCaptureOptionParameters(const GvSingle::GvCaptureOptions& opts);
    bool LoadCaptureOptionParameters(GvSingle::GvCaptureOptions& opts);
    bool GetAutoCaptureSetting(GvCaptureOptions& opts, const GvROI&);
    bool GetAutoHdrCaptureSetting(GvCaptureOptions& opts, const GvROI&);
    bool GetAutoNoiseRemovalSetting(GvCaptureOptions& opts);
    bool LoadSettingFromFile(const char*);
    bool SaveSettingToFile(const char*);
    bool CheckRoi(GvROI roi);
    GvROI AutoAdjustRoi(GvROI roi = GvROI());
    bool GetRoiRange(GvROIRange&);
    bool GetCameraResolution(GvSize& resolution);

    GvHandle m_handle;
};

struct GV_PUBLIC_API GvStereo {
    struct GV_PUBLIC_API GvCaptureOptions {
        GvCaptureOptions() = default;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        bool calc_normal = false;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        bool transform_to_camera = false;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        unsigned int calc_normal_radius = 0;
        int projector_brightness = 0;
        int light_contrast_threshold = 0;
        int phase_filter_range = 0;
        int exposure_time_2d = 50;
        int exposure_time_3d = 50;
        float gain_2d = 1.0f;
        float gain_3d = 1.0f;
        int hdr_exposure_times = 0;
        int hdr_scan_times[3] = {0, 0, 0};
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        int hdr_projector_brightness[3] = {0, 0, 0};
        float gamma_2d = 1.0f;
        float gamma_3d = 1.0f;
        bool use_projector_capturing_2d_image = false;
        int bilateral_filter_kernal_size = 0;
        float bilateral_filter_depth_sigma = 0.0f;
        float bilateral_filter_space_sigma = 0.0f;
        /** @brief 현재 플래그 자체는 자동 적용 스위치로 동작하지 않습니다. GetAutoNoiseRemovalSetting()으로 추천값을 받아 noise_removal_*를 설정해 사용하세요. */
        bool use_auto_noise_removal = false;
        /** @brief Outlier rejection: minimum neighbor count inside `noise_removal_distance` radius. 0 disables filter. */
        int noise_removal_point_number = 0;
        /** @brief Outlier rejection radius in point-cloud metric space (typically mm). <=0 disables filter. */
        float noise_removal_distance = 0.0f;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        float downsample_distance = 0.0f;
        float confidence_threshold = 0.0f;
        float truncate_z_min = -99999.0f;
        float truncate_z_max = 99999.0f;
        float reflection_filter_threshold = 0.0f;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        float smooth_sigma = 0.0f;
        GvCaptureMode capture_mode = CaptureMode_Normal;
        GvROI roi{};
        int hdr_exposuretime_content[3] = {0, 0, 0};
        float hdr_gain_3d[3] = {0.0f, 0.0f, 0.0f};
        int scan_times = 0;
        /** @brief 현재 공개 캡처 경로에서 미동작(no-op)입니다. */
        GvProjectorColor projector_color = ProjectorColor_None;
        int line_scanner_scan_time_ms = 0;
        int line_scanner_exposure_time_us = 0;
        int line_scanner_min_distance = 0;
        int line_scanner_max_distance = 0;
        uint16_t line_scanner_laser_position = 0;
        bool correspond2d = false;
    };

    struct GV_PUBLIC_API GvCollectionCallBackInfo {
        GvImage image_l;
        GvImage image_r;
    };

    struct GV_PUBLIC_API GvCalculationCallBackInfo {
        GvImage image_l;
        GvImage image_r;
        GvPointMap pointmap;
        GvDepthMap depthmap;
        GvConfidenceMap confidencemap;
    };

    using CollectionCallBackPtr = void(*)(const GvCollectionCallBackInfo&, const GvCaptureOptions&, UserPtr);
    using CalculationCallBackPtr = void(*)(const GvCalculationCallBackInfo&, const GvCaptureOptions&, UserPtr);

    static GvStereo Create(int deviceIndex);
    static void Destroy(GvStereo& x);

    bool IsValid();
    bool Open();
    void Close();
    bool IsOpen();
    bool IsPhysicallyConnected();
    bool OpenProtectiveCover();
    bool OpenProtectiveCoverAsync();
    bool CloseProtectiveCover();
    bool CloseProtectiveCoverAsync();
    bool ResetProtectiveCover();
    bool GetProtectiveCoverStatus(GvProtectiveCoverStatus&);
    bool SetCollectionCallBack(GvStereo::CollectionCallBackPtr cb, UserPtr ctx);
    bool SetCalculationCallBack(GvStereo::CalculationCallBackPtr cb, UserPtr ctx);
    bool Capture(const GvCaptureOptions& opts);
    bool Capture();
    bool StartFixedLineScan(const GvCaptureOptions&);
    bool StartFixedLineScan();
    GvPointMap GetFixedLineScanPointMap();
    bool GetFixedLineScanPointMap(GvPointMap&);
    bool StopFixedLineScan();
    bool Capture2D(const GvCameraID cid, const GvCaptureOptions&);
    bool Capture2D(const GvCameraID cid);
    bool SetBandwidth(float);
    bool GetBandwidth(float& percent);
    bool SetCustomTransformation(const GvCustomTransformOptions&);
    bool GetCustomTransformation(GvCustomTransformOptions&);
    /**
     * @brief 최근 3D 캡처 결과 point map을 반환한다.
     * @details 좌표 단위는 `meter`이며 유효하지 않은 포인트는 `(NaN, NaN, NaN)`이다.
     */
    GvPointMap GetPointMap();
    GvImage GetImage(const GvCameraID cid);
    /**
     * @brief 최근 3D 캡처 결과 depth map을 반환한다.
     * @details 유효하지 않은 depth 픽셀은 `NaN`이다.
     */
    GvDepthMap GetDepthMap();
    GvConfidenceMap GetConfidenceMap();
    GvCorrespondMap GetCorrespondMap();
    bool GetExtrinsicMatrix(const GvCameraID, float*);
    bool GetIntrinsicParameters(const GvCameraID, float*, float*);
    bool GetProjectorTemperature(float&);
    bool GetCameraTemperature(const GvCameraID, GvCameraTempSelector, float&);
    bool GetExposureTimeRange(int* min_value, int* max_value);
    bool GetGainRange(float* min_value, float* max_value);
    bool GetGammaRange(float* min_value, float* max_value);
    bool SaveEncodedImagesData(const char*);
    GvImage GetRawImage(const GvCameraID, const int);
    bool GetRawImage(GvImage& img, const GvCameraID, const int);
    bool GenerateEncodedMap();
    GvImage GetEncodeMap();
    GvImage GetEncodedRawImage(uint16_t, const int);
    bool SaveCaptureOptionParameters(const GvStereo::GvCaptureOptions& opts);
    bool LoadCaptureOptionParameters(GvStereo::GvCaptureOptions& opts);
    bool GetAutoCaptureSetting(GvCaptureOptions& opts, const GvROI&);
    bool GetAutoHdrCaptureSetting(GvCaptureOptions& opts, const GvROI&);
    bool GetAutoNoiseRemovalSetting(GvCaptureOptions& opts);
    bool LoadSettingFromFile(const char*);
    bool SaveSettingToFile(const char*);
    bool CheckRoi(GvROI roi);
    GvROI AutoAdjustRoi(GvROI roi = GvROI());
    bool GetRoiRange(GvROIRange&);
    bool GetCameraResolution(GvSize& resolution);

    GvHandle m_handle;
};

/**
 * @brief 마지막 에러 코드를 조회한다.
 * @return 마지막 에러 코드.
 */
GV_PUBLIC_API int GvGetLastError();
/**
 * @brief SDK 버전 문자열을 조회한다.
 * @return 널 종료 UTF-8 버전 문자열 포인터.
 */
GV_PUBLIC_API const char* GvGetVersion();
/**
 * @brief 마지막 에러 메시지를 조회한다.
 * @return 널 종료 UTF-8 에러 메시지 문자열 포인터.
 */
GV_PUBLIC_API const char* GvGetLastErrorMessage();

/**
 * @brief SDK 시스템을 초기화한다.
 * @details 다른 공개 API 호출 전에 먼저 호출해야 한다.
 * @return 성공 시 true.
 */
GV_PUBLIC_API bool GvSystemInit();
/**
 * @brief SDK 시스템 초기화 여부를 조회한다.
 * @return 초기화된 상태면 true.
 */
GV_PUBLIC_API bool GvSystemIsInited();
/**
 * @brief SDK 시스템을 종료한다.
 * @details `GvSystemInit()`으로 초기화한 리소스를 정리한다.
 */
GV_PUBLIC_API void GvSystemShutdown();

/**
 * @brief 실시간 이미지 콜백을 등록한다.
 * @details 선택한 카메라 측면(Left/Right/Both)에 콜백을 연결한다.
 *          콜백 경로는 Core 3D 캡처 흐름과 독립적으로 동작한다.
 *          `nullptr`를 전달하면 선택한 측면의 콜백을 해제한다.
 * @param camid 카메라 측면(Left/Right/Both).
 * @param cb 실시간 이미지 콜백 함수 포인터.
 * @param user_data 콜백에 전달할 사용자 컨텍스트 포인터.
 * @return 성공 시 true.
 */
GV_PUBLIC_API bool GvSetRealtimeImageCallback(enum GvCameraID camid, GvRealtimeImageCallback cb, UserPtr user_data);
/**
 * @brief 실시간 콜백 스로틀 FPS를 설정한다.
 * @param camid 카메라 측면(Left/Right/Both).
 * @param fps 목표 FPS. `0`이면 스로틀을 비활성화한다.
 *            유한한 값이며 `0` 이상이어야 한다.
 * @return 성공 시 true.
 */
GV_PUBLIC_API bool GvSetRealtimeImageThrottleFps(enum GvCameraID camid, double fps);
/**
 * @brief 실시간 이미지 FPS 통계를 조회한다.
 * @details `camera_fps`는 입력 프레임의 frame-id/시간으로 추정한 FPS이고,
 *          `throttled_fps`는 실제 콜백 디스패치 기준 FPS이다.
 * @param camid 카메라 측면(Left/Right).
 * @param info 조회 결과를 저장할 출력 포인터.
 * @return 성공 시 true.
 */
GV_PUBLIC_API bool GvGetRealtimeImageFpsInfo(enum GvCameraID camid, GvRealtimeImageFpsInfo* info);

/**
 * @brief 현재 검색 가능한 장치 개수를 조회한다.
 * @return 성공 시 장치 개수(0 이상), 실패 시 음수.
 */
GV_PUBLIC_API int GvSystemGetDeviceCount();
/**
 * @brief 장치 인덱스로 장치 정보를 조회한다.
 * @param deviceIndex 장치 인덱스.
 * @param pinfo 장치 정보를 저장할 출력 포인터.
 * @return 성공 시 true.
 */
GV_PUBLIC_API bool GvSystemGetDeviceInfo(int deviceIndex, GvDeviceInfo* pinfo);
/**
 * @brief Force-IP 완전 자동 재할당 API.
 * @details deviceIndex만 전달하면 SDK가 NIC preflight 정보를 기반으로
 *          인터페이스를 자동 선택해 IP 재할당을 수행한다.
 * @param deviceIndex 장치 인덱스(`GvSystemGetDeviceInfo()`와 동일 기준).
 * @return 성공 시 true.
 */
GV_PUBLIC_API bool GvSystemAssignNewIpAuto(int deviceIndex);

}  // namespace gv

