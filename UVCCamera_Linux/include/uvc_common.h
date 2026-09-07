#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#if WIN32
#ifdef USE_EXPORTDLL
#define CAMERASDK_API _declspec(dllexport)
#else
#define CAMERASDK_API _declspec(dllimport)
#endif
#else
#define CAMERASDK_API
#endif

const uint16_t kLinkId_ = 0x4c01;
const uint16_t kLoveneseId_ = 0x4c02;
const uint16_t kPUC2ProductHostId_ = 0x4c03;
const uint16_t kPUC2ProductFlowId_ = 0x4c04;
const uint16_t kPUC3ProductHostId_ = 0x4c05;
const uint16_t kPUC3ProductFlowId_ = 0x4c06;

class UVCCameraDevice;

struct ColorType {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct CameraControlInfo {
    long min;
    long max;
    long step;
    long def;
    long cur;
    long flags;
};

enum class CameraControlRelativeValue {
    Stop = 0x00,  // 停止
    ClockwiseMove = 0x01,  // 顺时针
    AntiClockwiseMove = 0xFF   // 逆时针
};

struct CameraControlRelativeInfo {
    long speed;  // 范围[1,10]
    CameraControlRelativeValue value;
};

struct HostPTZInfo {
    uint16_t ratio;
    uint16_t zoomX;
    uint16_t zoomY;
    uint8_t move_step;
    uint8_t ratio_step;
};

struct UVCPoint {
    float x;
    float y;
};

struct UVCRect {
    UVCPoint point;
    float width;
    float height;
};

struct PTZVerInfo {
    uint32_t app_version;
    uint8_t uuid[12];
    uint8_t hw_version;
    uint8_t imu_exist;
    uint8_t touch_key;
    uint8_t sn[14];
    uint8_t reserved[6];
};

#ifdef WIN32
typedef struct UVCCameraInfo {
    std::string friendly_name;
    std::string display_name;
    int video_device_index;
    uint16_t vendor_id;
    uint16_t product_id;
} UVCCameraInfo;
#else
typedef struct UVCCameraInfo {
    uint32_t location_id;
    uint16_t vendor_id;
    uint16_t product_id;
    std::string friendly_name;
    std::string display_name;
    int video_device_index;
} UVCCameraInfo;
#endif

typedef struct UVCMediaFormat {
    int width;
    int height;
    int framerate;
} UVCMediaFormat;

typedef struct TakePhotoResult {
    std::string jpg_file_name;
    std::string raw_file_name;
} TakePhotoResult;

typedef struct DeviceStatus {
    uint8_t cpu_temperature = 0;
    uint8_t sensor_temperature = 0;
    bool video_stream_is_opend = false;
    bool fireware_is_upgrading = false;
} DeviceStatus;

typedef struct DeviceInfo {
    std::string uuid;
    std::string sensorId;
    std::string hostFirmwareVer;
    bool codecIsLegal;
    int32_t hostHardwareVer;
    int32_t ptzFirmwareVer;
    int8_t ptzHardwareVer;
    int8_t deviceType;
} DeviceInfo;

enum class PowerLineFrequency {
    FREQUENCY_NotUse = 0x00,
    FREQUENCY_50HZ = 0x01,
    FREQUENCY_60HZ = 0x02,
    FREQUENCY_Auto = 0x03,
};

enum class VideoMode {
    Normal = 0x00,      // 正常模式
    AutoComposition = 0x01,      // 自动构图模式
    //Track                = 0x02,  // 跟踪模式
    //Zoom                 = 0x03,  // 放大模式
    Whiteboard = 0x04,  // 白板模式
    Craneshot = 0x05,  // 俯拍模式
    DeskView = 0x06,  // DeskView
    AutoFraming = 0x07,  // 多人自动构图
    SmartWhiteboardMode = 0x08,  // 智能白板
    //RegionalTack         = 0x09,  // 区域追踪
    SmartWhiteboardQuery = 0x0a  // 智能白板查找中
    //RegionalTackConfig   = 0x0b   // 区域追踪配置中
};

enum class FilterMode {
    Standard = 0x00,
    Reserved1 = 0x01,
    Reserved2 = 0x02,
    Portrait = 0x03,
    Bright = 0x04,
    Vintage1 = 0x05,
    Vintage2 = 0x06,
    Neon = 0x07,
    Clear = 0x08
};

enum class VideoModeStatus {
    Normal = 0x00,  // 正常；
    Dectecting = 0x01,  // 识别中
    Working = 0x02,  // 工作中
    LostObj = 0x03,  // 白板丢失目标
    EnterDeskView = 0x10,  //deskView dewarp成功
    DeskViewFailed = 0x11   //deskView dewarp失败
};

typedef struct ValueChangeMask {
    bool whiteBlance_param{ false };             // 0
    bool brightness_param{ false };              // 1
    bool contrast_param{ false };                // 2
    bool saturation_param{ false };              // 3
    bool sharpness_param{ false };               // 4
    bool ev_param{ false };                      // 5
    bool iso_param{ false };                     // 6
    bool expo_mode_param{ false };               // 8
    bool flicker_param{ false };                 // 9
    bool video_mirror_param{ false };            // 10
    bool af_param{ false };                      // 11
    bool mf_param{ false };                      // 12
    bool gesture_status_param{ false };          // 18
    bool layout_style_param{ false };            // 25
    bool track_speed_param{ false };             // 26
    bool enable_ai_param{ false };               // 30
    bool noise_cancel_param{ false };            // 31
    bool video_filp_param{ false };              // 38
    bool enable_vertical_param{ false };         // 44
    bool enable_lowspec_param{ false };          // 45
    bool enable_hdr_param{ false };              // 46
    bool preset_set_param{ false };              // 52
    bool preset_goto_param{ false };             // 53
    bool enable_uacmutex_param{ false };         // 56
    bool enable_extreme_privacy_param{ false };  // 57
    bool recovery_config_param{ false };         // 66
    bool horizontal_corrent_param{ false };      // 67
    bool enable_audio_volume_param{ false };     // 71
    bool preset_delete_param{ false };           // 72
    bool audio_wall_param{ false };              // 音墙
    bool audio_capture_mode_param{ false };      // 音频捕获模式
} ValueChangeMask;

typedef struct VideoModeAuxiliaryData {
    VideoModeStatus video_mode_status;
    std::vector<float> smartWhiteborad_extenddata;
    std::vector<uint8_t> whileborad_extenddata;
    // 接口文档中定义3610为无效值，zoom小于100为无效值，主机模式才有hostpitch.
    int32_t yaw = 3610;
    uint16_t host_x = -1;
    uint16_t host_y = -1;
    int32_t pitch = 3610;
    int32_t roll = 3610;
    int16_t zoom = 0;
    int16_t hostpitch = -450;
    int privacy_mode = 0;
    bool enable_privacy = false;
    int32_t default_track_mode = 0;  // 默认跟踪模式  0单人  1多人
    bool has_report_data = false;
    bool horizontal_correct_exceed = false;
    int rotate = 0;
    int ptz_check_result = 0;
    int vido_mode_report_flag = 0;
    bool res_is_changed = false;
    bool track_area_status_err = false;
    bool enable_set_preset = false;
    ValueChangeMask mask;
    bool audio_device_valid = true;
    bool uac_streaming = false;
} VideoModeAuxiliaryData;

/**
 * @brief 构图样式
 */
enum class CompositionStyle {
    None = 0x00,
    OnlyHead = 0x01,  // 人头像
    HalfBody = 0x02,  // 半身像
    FullBody = 0x03,  // 全身像
};

enum class AudioCaptureMode {
    Near = 0x00,  // 近场
    Far = 0x01,  // 远场
    LiveBroadcast = 0x02   // 直播
};

enum class AudioDirectionMode {
    Standard = 0x00,  // 标准模式
    Wide = 0x01,  // 广域模式
    Focus = 0x02,  // 聚焦模式
    Music = 0x03   // 音乐模式
};

/**
 * @brief 跟踪速度
 */
enum class TrackSpeed {
    Fast = 0x00,
    Normal = 0x01,
    Slow = 0x02,
};

/**
 * @brief 手势类型
 * @note 枚举值对应相机协议，不可重排。
 */
enum class Gesture {
    Gesture_Ok,    // OK手势（已废弃，相机不再支持）
    Gesture_Palm,  // Plam手势
    Gesture_L,     // L手势
    Gesture_V,     // V手势
    Gesture_Rock,  // Rock手势（已废弃，相机不再支持）
};
/**
 * @brief 图像模板操作
 */
enum class ImageOperation {
    ImageOperation_Renew = 1,  // 更新
    ImageOperation_Switch = 2,  // 切换
};

/**
 * @brief 拓展功能
 */
enum class ExtendFuction {
    AiZoom,                   // 智能Zoom开关
    AF,                       // 自动对焦开关
    HDR,                      // HDR开关
    Mirror,                   // 镜像翻转开关
    Ai,                       // Ai算法开关（手势总开关）
    VScreen,                  // 横竖屏开关
    EnableStartupSetting,     // 保存使能开关
    EnableSingleTapTracking,  // 掰轴跟踪开关
    EnableTracking,           // 触摸跟踪开关
    SmartAdjustment,          // 智能调整开关
    ForcedVertical,           // 强制竖拍开关
    ExtremePrivacy,           // 极致隐私开关
    MirrorVertically,         // 垂直镜像开关
    LowResolution,            // 低分辨率开关
    TrackForbiddenArea,       // 追踪禁区开关
    AudioSystemVolume,        // 音频系统音量开关
};

enum class GestureFunction {
    AutoComposition = 0x00,
    Track = 0x01,  // 跟踪模式
    Zoom = 0x02,  // 放大模式
    None = 0x03
};

enum class CalibrationMode { Normal = 0x00, Near = 0x01, Far = 0x02 };
enum class CalibrationResult { Idel = 0x00, Running = 0x01, Pass = 0x02, Failed = 0x03 };

typedef struct CalibrationStatus {
    CalibrationResult result;
    uint16_t far_code;
    uint16_t near_code;
    uint16_t far_default;
    uint16_t near_default;
} CalibrationStatus;

typedef struct MotorStatus {
    bool roll = false;
    bool pitch = false;
    bool yaw = false;
} MotorStatus;

//for debug
typedef struct DrawBoxInfo {
    float x;
    float y;
    float width;
    float height;
    uint16_t box_id;
} DrawBoxInfo;

enum class GimbalTrackingBoxType { PTZ_TRACK_NORMAL = 0x00, PTZ_TRACK_LOST = 0x01, PTZ_TRACK_EXIT = 0x02 };

typedef struct GimbalTrackingBoxInfo {
    GimbalTrackingBoxType type;
    int16_t x;
    int16_t y;
    uint8_t scale;
    uint8_t w;
    uint8_t h;
    uint8_t tracking_type;  // 0: head, 1: others
} GimbalTrackingBoxInfo;

typedef struct ImageTemplateInfo {
    uint8_t template_total_num;    // 模板总数(目前一个是40个)
    int8_t current_template_id;    // 当前的模板ID
    uint64_t template_use_status;  // 模板使用情况(同时位数进行统计 0.是没有被使用，1.可以被使用)
} ImageTemplateInfo;

typedef struct ExpouseCurve {
    std::vector<uint16_t> data;
    bool is_apply;
    bool enable;
} ExpouseCurve;

typedef struct PresetInfo {
    uint8_t preset_id;
    bool valid;
    std::string name;
} PresetInfo;

typedef struct LocalPresetInfo {
    uint8_t preset_id;
    VideoMode mode;
    bool valid;
    std::string name;
    float whiteboard_args[9];  // 当 mode 等于Whiteboard时才有效
    uint16_t zoom{ 100 };
    // 云台版本信息
    int32_t ptz_x;
    int32_t ptz_y;
    int32_t ptz_z;
    // 主机版本信息
    float host_x;  // 归一化到 0~1.0
    float host_y;  // 归一化到 0~1.0
    int16_t host_pitch;
} LocalPresetInfo;

typedef struct TrackAreaInfo {
    bool enable_tracking;    // 是否开启区域追踪
    bool enter_setting;      // 是否进入设置界面
    bool use_vertical_data;  // 是否使用垂直数据
    // 水平区域角度
    int16_t horizontal_pos_yaw_left;
    int16_t horizontal_pos_yaw_right;
    int16_t horizontal_pos_pitch_top;
    int16_t horizontal_pos_pitch_botton;
    // 垂直区域角度
    int16_t vertical_pos_yaw_left;
    int16_t vertical_pos_yaw_right;
    int16_t vertical_pos_pitch_top;
    int16_t vertical_pos_pitch_botton;

    // 0:表示正常
    // 1:掰轴识别目标在跟踪范围外
    // 2:palm手势识别目标在跟踪范围外
    uint8_t exception_state;
} TrackAreaInfo;

enum class DrawBoxType { Close = 0x00, DrawInCamera = 0x01, DrawInClient = 0x02 };

enum class DeviceWatcherEvent { USB_DEVICE_ADD, USB_DEVICE_LEFT };

#pragma pack(push, 1)
struct ForbiddenAreaInfo_s {
    uint8_t box_id : 7;  //box_id 从1开始，最大到6，如果是0则表示没有使用.在设置过程中，如果不使用，直接设置为0
    uint8_t enable : 1;
    uint16_t zoom_rate;           // 预留下  100~400
    int16_t gimbal_forbid_pos_x;  // -1800 +1800
    int16_t gimbal_forbid_pos_y;
    uint16_t roi_pos_x;  // 归一化到 0~10000 表示(0~1.0)
    uint16_t roi_pos_y;
    uint16_t roi_pos_w;
    uint16_t roi_pos_h;
};
#pragma pack(pop)
typedef ForbiddenAreaInfo_s ForbiddenAreaInfo;

/**
 * @brief 固件升级中的状态
 */
enum class FirmwareUpgradeStatus {
    OpenFileFailed = -2,
    UpgradeFailed = -1,
    Prepare,
    Uploading,
    UploadSuccess,
    Upgrading,
    UpgradeSuccess,
    NotifyReplugin,
    NotifyInstallDriver,
};

using USBSpeedTestCallBack = std::function<void(long speed, bool finisned)>;
using FirmwareUpgradeProgressCallBack = std::function<void(long process, FirmwareUpgradeStatus status)>;
using DeviceWatcherCallBack = std::function<void(DeviceWatcherEvent event, std::string device_name)>;
using AgeTestStreamDataCallBack = std::function<void()>;
