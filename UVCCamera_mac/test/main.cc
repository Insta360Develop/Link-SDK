// UVCCamera SDK 交互式功能演示
// 按功能类型分组：
//   [1xx] 标准控制器 —— 云台/对焦/变焦/曝光/画质/电源频率
//   [2xx] 设备信息
//   [3xx] 构图与追踪
//   [4xx] 手势
//   [5xx] 画质增强（曝光补偿/ISO/曲线/滤镜/补光灯）
//   [6xx] 音频
//   [8xx] 模式与隐私
//   [10xx] 追踪禁区与区域追踪
//   [11xx] 云台扩展与其他
//   [12xx] 固件升级
#include <uvc_camera.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <thread>
#include <vector>

using namespace uvc;

namespace {

// 扩展控制器（XU 通道）云台绝对值单位换算：三端统一为「1/360 度」，
// 不区分平台（业务侧亦按 value/360 折算，见 camera_ptz_param.cc）。demo 统一以「度」呈现。
constexpr long kExtPanTiltUnitsPerDegree = 3600;

// 打印布尔结果
void PrintResult(bool ok) {
    std::cout << (ok ? "success" : "failed") << std::endl;
}

// 读取一个整数（对非法输入做校验，清除错误状态并重新提示）
int ReadInt(const std::string &prompt) {
    int v = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) {
            // 丢弃本行剩余字符，避免像 "0.5" 的小数部分残留在缓冲区
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }
        // 进入失败状态：清除标志位并丢弃这一行非法输入
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "invalid input, please enter an integer" << std::endl;
    }
}

// 读取一个浮点数（对非法输入做校验，清除错误状态并重新提示）
double ReadDouble(const std::string &prompt) {
    double v = 0.0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) {
            // 丢弃本行剩余字符
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }
        // 进入失败状态：清除标志位并丢弃这一行非法输入
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "invalid input, please enter a number" << std::endl;
    }
}

// 打印控制项范围信息
void PrintRange(const char *name, const CameraControlInfo &info) {
    std::cout << name << " -> min:" << info.min << " max:" << info.max << " step:" << info.step << " def:" << info.def
              << " cur:" << info.cur << std::endl;
}

void PrintMenu() {
    std::cout << "\n================ UVCCamera Demo ================\n";
    std::cout << "0   : exit   1    : print menu\n";
    std::cout << "--- [1xx] Standard Controller---\n";
    std::cout << "101 : Get MediaFormat List                                           102 : Get PanTilt Relative (Link1)\n";
    std::cout << "103 : Set PanTilt Relative(Link1)                                    104 : Get PanTilt Absolute Range (Link1)\n";
    std::cout << "105 : Set PanTilt Absolute(Link1)                                    106 : Roll (range/get/set) (Link1)\n";
    std::cout << "107 : Focus (range/get/set)                                          108 : Auto Focus (get/set)\n";
    std::cout << "109 : Zoom (range/get/set)                                           110 : Auto Exposure (get/set)\n";
    std::cout << "111 : Shutter Speed (list/get/set)112 : anti-flicker (get/set)\n";
    std::cout << "113 : Brightness (range/get/set)                                     114 : Contrast (range/get/set)\n";
    std::cout << "115 : Saturation (range/get/set)                                     116 : Sharpness (range/get/set)\n";
    std::cout << "117 : WhiteBalance (range/get/set)118 : WhiteBalance Mode (get/set)\n";
    std::cout << "--- [2xx] Device Info ---\n";
    std::cout << "201 : Get Device Info                                                202 : Get Serial Number\n";
    std::cout << "203 : Get PTZ Version Info                                           204 : Get Camera Type / PID\n";
    std::cout << "205 : Get Device Status                                              206 : Get Camera Play Resolution\n";
    std::cout << "--- [3xx] Composition & Tracking ---\n";
    std::cout << "301 : Set Video Mode                                                 302 : Get Video Mode\n";
    std::cout << "303 : Set Composition Style                                          304 : Get Composition Style\n";
    std::cout << "305 : Set Track Speed                                                306 : Get Track Speed\n";
    std::cout << "307 : Get Track Obj List(link 1)                                     308 : Get New Track Obj List\n";
    std::cout << "309 : Set Track Obj                                                  312 : Set Default Track Mode\n";
    std::cout << "313 : Get Default Track Mode\n";
    std::cout << "--- [4xx] Gesture ---\n";
    std::cout << "401 : Enable All Gesture                                             402 : Enable Gesture Work\n";
    std::cout << "403 : Get Gesture Status\n";
    std::cout << "--- [5xx] Image Quality ---\n";
    std::cout << "501 : Set Exposure Compensation                                      502 : Get Exposure Compensation\n";
    std::cout << "503 : Set ISO                                                        504 : Get ISO\n";
    std::cout << "505 : Set Filter                                                     506 : Get Filter\n";
    std::cout << "507 : Get FillLight Param                                            508 : Enable Extend Func\n";
    std::cout << "509 : Get Extend Func Status\n";
    std::cout << "--- [6xx] Audio ---\n";
    std::cout << "601 : Enable Audio NoiseReduction                                    602 : Get Audio Noise Status\n";
    std::cout << "603 : Set Audio Capture Mode(link2)                                  604 : Get Audio Capture Mode(link2)\n"; // link2
    std::cout << "605 : Set Audio Direction Mode(link2 pro)                            606 : Get Audio Direction Mode(link2 pro)\n"; // link2 pro
    std::cout << "--- [8xx] Mode & Privacy ---\n";
    std::cout << "801 : Switch Storage Mode                                            802 : Switch Msc ReadOnly\n";
    std::cout << "803 : Switch UVC Normal Mode                                         804 : Set Privacy Mode\n";
    std::cout << "805 : Get Privacy Mode                                               806 : Enable Vertical Screen\n";
    std::cout << "807 : Enable Low Resolution                                          808 : Enable Horizontal Correct\n";
    std::cout << "809 : Get Horizontal Correct                                         810 : Load Factory Default\n";
    std::cout << "--- [10xx] Track Forbidden & Area ---\n";
    std::cout << "1001: Enter Forbidden Setting                                        1002: Exit Forbidden Setting\n";
    std::cout << "1003: Get Track Forbidden Area                                       1004: Get Track Area Info\n";
    std::cout << "--- [11xx] PTZ Extend & Others ---\n";
    std::cout << "1101: Ext Get PanTilt Relative (Link1/Link2/Pro)                     1102: Ext Set PanTilt Relative (Link1/Link2/Pro)\n";
    std::cout << "1103: Ext Get PanTilt Absolute (Link1/Link2/Pro)                     1104: Ext Set PanTilt Absolute (Link1/Link2/Pro)\n";
    std::cout << "1105: Get Real Zoom                                                  1107: Get Host PTZ\n";
    std::cout << "1108: Set Host PTZ\n";
    std::cout << "--- [12xx] Firmware Upgrade ---\n";
    std::cout << "1201: Firmware Upgrade (will exit)\n";
    std::cout << "================================================\n";
}

// 中文菜单
void PrintMenuCN() {
    std::cout << "\n================ UVCCamera 功能演示 ================\n";
    std::cout << "0   : exit   1    : print menu\n";
    std::cout << "--- [1xx] 标准控制器 ---\n";
    std::cout << "101 : 获取媒体格式列表                          102 : 获取云台相对值(Link1)\n";
    std::cout << "103 : 设置云台相对值 (Link1)                    104 : 获取云台绝对范围(Link1)\n";
    std::cout << "105 : 设置云台绝对位置(Link1)                   106 : roll（范围/读取/设置）(Link1)\n";
    std::cout << "107 : 对焦（范围/读取/设置）                    108 : 自动对焦（读取/设置）\n";
    std::cout << "109 : zoom（范围/读取/设置）                    110 : 自动曝光（读取/设置）\n";
    std::cout << "111 : 快门速度（列表/读取/设置）                112 : 抗闪烁（读取/设置）\n";
    std::cout << "113 : 亮度（范围/读取/设置）                    114 : 对比度（范围/读取/设置）\n";
    std::cout << "115 : 饱和度（范围/读取/设置）                  116 : 锐度（范围/读取/设置）\n";
    std::cout << "117 : 色温（范围/读取/设置）                    118 : 白平衡模式（读取/设置）\n";
    std::cout << "--- [2xx] 设备信息 ---\n";
    std::cout << "201 : 获取设备信息                              202 : 获取序列号\n";
    std::cout << "203 : 获取云台版本信息                          204 : 获取相机类型 / PID\n";
    std::cout << "205 : 获取设备状态                              206 : 获取相机播放分辨率\n";
    std::cout << "--- [3xx] 构图与追踪 ---\n";
    std::cout << "301 : 设置视频模式                              302 : 获取视频模式\n";
    std::cout << "303 : 设置构图风格                              304 : 获取构图风格\n";
    std::cout << "305 : 设置追踪速度                              306 : 获取追踪速度\n";
    std::cout << "307 : 获取追踪对象列表(link 1)                  308 : 获取新追踪对象列表\n";
    std::cout << "309 : 设置追踪对象                              312 : 设置默认追踪模式\n";
    std::cout << "313 : 获取默认追踪模式\n";
    std::cout << "--- [4xx] 手势 ---\n";
    std::cout << "401 : 启用全部手势                              402 : 启用手势功能\n";
    std::cout << "403 : 获取手势状态\n";
    std::cout << "--- [5xx] 画质增强 ---\n";
    std::cout << "501 : 设置曝光补偿                              502 : 获取曝光补偿\n";
    std::cout << "503 : 设置 ISO                                  504 : 获取 ISO\n";
    std::cout << "505 : 设置滤镜                                  506 : 获取滤镜\n";
    std::cout << "507 : 获取补光灯参数                            508 : 启用扩展功能(hdr/镜像等等，具体看结构体)\n";
    std::cout << "509 : 获取扩展功能状态\n";
    std::cout << "--- [6xx] 音频 ---\n";
    std::cout << "601 : 启用音频降噪                              602 : 获取音频降噪状态\n";
    std::cout << "603 : 设置音频采集模式(link2)                   604 : 获取音频采集模式(link2)\n";  // link2
    std::cout << "605 : 设置音频方向模式(link2 pro)               606 : 获取音频方向模式(link2 pro)\n";  // link2 pro
    std::cout << "--- [8xx] 模式与隐私 ---\n";
    std::cout << "801 : 切换存储模式                              802 : 切换 Msc 只读\n";
    std::cout << "803 : 切换普通UVC模式                           804 : 设置隐私模式\n";
    std::cout << "805 : 获取隐私模式                              806 : 启用竖屏\n";
    std::cout << "807 : 启用低分辨率                              808 : 启用水平校正\n";
    std::cout << "809 : 获取水平校正状态                          810 : 恢复出厂设置\n";
    std::cout << "--- [10xx] 追踪禁区与区域追踪 ---\n";
    std::cout << "1001: 进入禁区设置                              1002: 退出禁区设置\n";
    std::cout << "1003: 获取追踪禁区                              1004: 获取追踪区域信息\n";
    std::cout << "--- [11xx] 云台扩展与其他 ---\n";
    std::cout << "1101: 扩展-获取云台相对值 (Link1/Link2/Pro)     1102: 扩展-设置云台相对值 (Link1/Link2/Pro)\n";
    std::cout << "1103: 扩展-获取云台绝对值 (Link1/Link2/Pro)     1104: 扩展-设置云台绝对值 (Link1/Link2/Pro)\n";
    std::cout << "1105: 获取实时变焦值                            1107: 获取主机版 PTZ\n";
    std::cout << "1108: 设置主机版 PTZ\n";
    std::cout << "--- [12xx] 固件升级 ---\n";
    std::cout << "1201: 固件升级（将退出程序）\n";
    std::cout << "================================================\n";
}
    
}  // namespace

int main(int argc, char *argv[]) {
    std::vector<UVCCameraInfo> list;
    GetUVCCameraList(list);
    if (list.empty()) {
        std::cout << "not find uvc camera device" << std::endl;
        return -1;
    }

    UVCCameraInfo info = list.at(0);
    UVCCameraController camera(info);
    UVCCameraExtendController ext(info);
    std::cout << "Succeed to open camera: " << info.friendly_name << std::endl;
    PrintMenuCN();
    while (true) {
        int option = ReadInt("please enter index: ");
        if (option == 0) {
            break;
        }        
        else if (option == 1) {
            PrintMenuCN();
            continue;
        }
        switch (option) {
        // ---------------- [1xx] 标准控制器 ----------------
        case 101: {
            std::vector<UVCMediaFormat> formats;
            camera.GetMediaFormatList(formats);
            for (const auto &f : formats) {
                std::cout << f.width << "x" << f.height << "@" << f.framerate << std::endl;
            }
            break;
        }
        case 102: {
            CameraControlRelativeInfo pan, tilt;
            if (camera.GetPanTiltRelative(pan, tilt)) {
                std::cout << "pan value:" << static_cast<int>(pan.value) << " speed:" << pan.speed << std::endl;
                std::cout << "tilt value:" << static_cast<int>(tilt.value) << " speed:" << tilt.speed << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 103: {
            CameraControlRelativeInfo pan, tilt;
            pan.value  = static_cast<CameraControlRelativeValue>(ReadInt("pan value (0 stop/1 cw/255 ccw): "));
            pan.speed  = ReadInt("pan speed (1-10): ");
            tilt.value = static_cast<CameraControlRelativeValue>(ReadInt("tilt value (0/1/255): "));
            tilt.speed = ReadInt("tilt speed (1-10): ");
            PrintResult(camera.SetPanTiltRelative(pan, tilt));
            break;
        }
        case 104: {
            CameraControlInfo pan, tilt;
            if (camera.GetPanTiltAbsoluteRange(pan, tilt)) {
                PrintRange("pan", pan);
                PrintRange("tilt", tilt);
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 105: {
            long pan  = ReadInt("pan value: ");
            long tilt = ReadInt("tilt value: ");
            PrintResult(camera.SetPanTiltAbsolute(pan, tilt));
            break;
        }
        case 106: {
            CameraControlInfo r;
            if (camera.GetRollAbsoluteRange(r))
                PrintRange("roll", r);
            long v = 0;
            camera.GetRollAbsolute(v);
            std::cout << "cur roll: " << v << std::endl;
            PrintResult(camera.SetRollAbsolute(ReadInt("set roll: ")));
            break;
        }
        case 107: {
            CameraControlInfo r;
            if (camera.GetFocusAbsoluteRange(r))
                PrintRange("focus", r);
            long v = 0;
            camera.GetFocusAbsolute(v);
            std::cout << "cur focus: " << v << std::endl;
            PrintResult(camera.SetFocusAbsolute(ReadInt("set focus: ")));
            break;
        }
        case 108: {
            bool enable = false;
            camera.GetAutoFocusStatus(enable);
            std::cout << "auto focus: " << enable << std::endl;
            PrintResult(camera.EnableAutoFocus(ReadInt("enable auto focus (0/1): ") != 0));
            break;
        }
        case 109: {
            CameraControlInfo r;
            if (camera.GetZoomAbsoluteRange(r))
                PrintRange("zoom", r);
            long v = 0;
            camera.GetZoomAbsolute(v);
            std::cout << "cur zoom: " << v << std::endl;
            PrintResult(camera.SetZoomAbsolute(ReadInt("set zoom: ")));
            break;
        }
        case 110: {
            bool enable = false;
            ext.GetAutoExposureStatus(enable);
            std::cout << "auto exposure: " << enable << std::endl;
            PrintResult(ext.EnableAutoExposure(ReadInt("enable auto exposure (0/1): ") != 0));
            break;
        }
        case 111: {
            std::vector<long> speeds;
            camera.GetShutterSpeedList(speeds);
            std::cout << "shutter list: ";
            for (long s : speeds)
                std::cout << s << " ";
            std::cout << std::endl;
            long cur = 0;
            camera.GetShutterSpeed(cur);
            std::cout << "cur shutter: " << cur << std::endl;
            PrintResult(camera.SetShutterSpeed(ReadInt("set shutter: ")));
            break;
        }
        case 112: {
            PowerLineFrequency f;
            camera.GetPowerLineFrequency(f);
            std::cout << "cur power line freq: " << static_cast<int>(f) << std::endl;
            PrintResult(camera.SetPowerLineFrequency(static_cast<PowerLineFrequency>(ReadInt("set (0 none/1 50Hz/2 60Hz/3 auto): "))));
            break;
        }
        case 113: {
            CameraControlInfo r;
            if (camera.GetBrightnessRange(r))
                PrintRange("brightness", r);
            int16_t v = 0;
            camera.GetBrightnessValue(v);
            std::cout << "cur brightness: " << v << std::endl;
            PrintResult(camera.SetBrightnessValue(static_cast<int16_t>(ReadInt("set brightness: "))));
            break;
        }
        case 114: {
            CameraControlInfo r;
            if (camera.GetContrastRange(r))
                PrintRange("contrast", r);
            uint16_t v = 0;
            camera.GetContrastValue(v);
            std::cout << "cur contrast: " << v << std::endl;
            PrintResult(camera.SetContrastValue(static_cast<uint16_t>(ReadInt("set contrast: "))));
            break;
        }
        case 115: {
            CameraControlInfo r;
            if (camera.GetSaturationRange(r))
                PrintRange("saturation", r);
            uint16_t v = 0;
            camera.GetSaturationValue(v);
            std::cout << "cur saturation: " << v << std::endl;
            PrintResult(camera.SetSaturationValue(static_cast<uint16_t>(ReadInt("set saturation: "))));
            break;
        }
        case 116: {
            CameraControlInfo r;
            if (camera.GetSharpnessRange(r))
                PrintRange("sharpness", r);
            uint16_t v = 0;
            camera.GetSharpnessValue(v);
            std::cout << "cur sharpness: " << v << std::endl;
            PrintResult(camera.SetSharpnessValue(static_cast<uint16_t>(ReadInt("set sharpness: "))));
            break;
        }
        case 117: {
            CameraControlInfo r;
            if (camera.GetWhitebalanceTemperatureRange(r))
                PrintRange("whitebalance", r);
            uint16_t v = 0;
            camera.GetWhitebalanceTemperatureValue(v);
            std::cout << "cur whitebalance: " << v << std::endl;
            PrintResult(camera.SetWhitebalanceTemperatureValue(static_cast<uint16_t>(ReadInt("set whitebalance: "))));
            break;
        }
        case 118: {
            uint8_t mode = 0;
            camera.GetWhitebalanceTemperatureMode(mode);
            std::cout << "cur wb mode (1 auto/0 manual): " << static_cast<int>(mode) << std::endl;
            PrintResult(camera.SetWhitebalanceTemperatureMode(static_cast<uint8_t>(ReadInt("set wb mode (0/1): "))));
            break;
        }
        // ---------------- [2xx] 设备信息 ----------------
        case 201: {
            DeviceInfo di;
            if (ext.GetDeviceInfo(di)) {
                std::cout << "uuid: " << di.uuid << "\nsensorId: " << di.sensorId << "\nhostFirmwareVer: " << di.hostFirmwareVer
                          << "\nhostHardwareVer: " << di.hostHardwareVer << "\nptzFirmwareVer: " << di.ptzFirmwareVer
                          << "\nptzHardwareVer: " << static_cast<int>(di.ptzHardwareVer) << "\ncodecIsLegal: " << di.codecIsLegal
                          << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 202: {
            std::string sn;
            if (ext.GetSerialNumber(sn))
                std::cout << "serial: " << sn << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 203: {
            PTZVerInfo pi;
            if (ext.GetPTZVersionInfo(pi)) {
                std::cout << "app_version: " << pi.app_version << " hw_version: " << static_cast<int>(pi.hw_version)
                          << " imu_exist: " << static_cast<int>(pi.imu_exist) << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 204: {
            std::string type;
            ext.GetCameraType(type);
            std::cout << "camera type: " << type << " PID: 0x" << std::hex << ext.GetPID() << std::dec << std::endl;
            break;
        }
        case 205: {
            DeviceStatus st;
            if (ext.GetDeviceStatus(st)) {
                std::cout << "cpu_temp: " << static_cast<int>(st.cpu_temperature)
                          << " sensor_temp: " << static_cast<int>(st.sensor_temperature) << " stream_open: " << st.video_stream_is_opend
                          << " upgrading: " << st.fireware_is_upgrading << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 206: {
            uint32_t w = 0, h = 0;
            uint16_t fps = 0;
            if (ext.GetCameraPlayRes(w, h, fps)) {
                std::cout << "play res: " << w << "x" << h << "@" << fps << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        // ---------------- [3xx] 构图与追踪 ----------------
        case 301: {
            VideoModeAuxiliaryData data;
            int mode = ReadInt("video mode (0 normal/1 auto/4 whiteboard/5 crane/6 deskview/7 framing/8 smartWB): ");
            PrintResult(ext.SetVideoMode(static_cast<VideoMode>(mode), data));
            break;
        }
        case 302: {
            VideoMode mode;
            VideoModeAuxiliaryData data;
            if (ext.GetVideoMode(mode, data)) {
                std::cout << "mode: " << static_cast<int>(mode) << " status: " << static_cast<int>(data.video_mode_status) << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 303:
            PrintResult(ext.SetCompositionStyle(static_cast<CompositionStyle>(ReadInt("style (1 head/2 half/3 full): "))));
            break;
        case 304: {
            CompositionStyle s;
            if (ext.GetCompositionStyle(s))
                std::cout << "style: " << static_cast<int>(s) << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 305:
            PrintResult(ext.SetTrackSpeed(static_cast<TrackSpeed>(ReadInt("speed (0 fast/1 normal/2 slow): "))));
            break;
        case 306: {
            TrackSpeed s;
            if (ext.GetTrackSpeed(s))
                std::cout << "track speed: " << static_cast<int>(s) << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 307: {
            std::vector<UVCRect> objs;
            if (ext.GetTrackObjLists(objs)) {
                for (const auto &o : objs)
                    std::cout << "x:" << o.point.x << " y:" << o.point.y << " w:" << o.width << " h:" << o.height << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 308: {
            std::vector<UVCRect> objs;
            if (ext.GetNewTrackObjLists(objs)) {
                for (const auto &o : objs)
                    std::cout << "x:" << o.point.x << " y:" << o.point.y << " w:" << o.width << " h:" << o.height << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 309: {
            UVCRect rect{};
            rect.point.x = ReadInt("x*100 (0-100): ") / 100.0f;
            rect.point.y = ReadInt("y*100 (0-100): ") / 100.0f;
            PrintResult(ext.SetTrackObj(rect));
            break;
        }
        case 312:
            PrintResult(ext.SetDefaultTrackMode(ReadInt("track mode (0 single/1 multi): ")));
            break;
        case 313: {
            int m = 0;
            if (ext.GetDefaultTrackMode(m))
                std::cout << "default track mode: " << m << std::endl;
            else
                PrintResult(false);
            break;
        }
        // ---------------- [4xx] 手势 ----------------
        case 401:
            PrintResult(ext.EnableAllGesture(ReadInt("enable all gesture (0/1): ") != 0));
            break;
        case 402: {
            int g   = ReadInt("gesture (1 Palm/2 L/3 V): ");
            bool en = ReadInt("enable (0/1): ") != 0;
            PrintResult(ext.EnableGestureWork(static_cast<Gesture>(g), en));
            break;
        }
        case 403: {
            std::map<Gesture, bool> status;
            if (ext.GetGestureStatus(status)) {
                for (const auto &kv : status) {
                    // Gesture_Ok / Gesture_Rock 已废弃，不展示
                    if (kv.first == Gesture::Gesture_Ok || kv.first == Gesture::Gesture_Rock) {
                        continue;
                    }
                    std::cout << "gesture " << static_cast<int>(kv.first) << " -> " << kv.second << std::endl;
                }
            }
            else {
                PrintResult(false);
            }
            break;
        }
        // ---------------- [5xx] 画质增强 ----------------
        case 501: {
            float v = 0;
            // EV 单位，业务范围 [-3.0, +3.0]，步进 0.30（SDK 不校验）
            std::cout << "exposure compensation (EV, -3.0~3.0 step 0.30): ";
            std::cin >> v;
            PrintResult(ext.SetExposureCompensation(v));
            break;
        }
        case 502: {
            float v = 0;
            if (ext.GetExposureCompensation(v))
                std::cout << "exposure comp: " << v << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 503:
            // 业务档位: 100/125/160/200/250/320/400/500/640/800/1000/1250/1600/2000/2500/3200（SDK 不校验）
            PrintResult(ext.SetISOValue(static_cast<uint16_t>(ReadInt("ISO value (100~3200): "))));
            break;
        case 504: {
            uint16_t v = 0;
            if (ext.GetISOValue(v))
                std::cout << "ISO: " << v << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 505:
            PrintResult(ext.SetFilter(
                static_cast<FilterMode>(ReadInt("filter (0 std/3 portrait/4 bright/5 vintage1/6 vintage2/7 neon/8 clear): "))));
            break;
        case 506: {
            FilterMode f;
            if (ext.GetFilter(f))
                std::cout << "filter: " << static_cast<int>(f) << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 507: {
            int wb = 0, bright = 0;
            if (ext.GetFillLightParam(wb, bright))
                std::cout << "fill light wb:" << wb << " brightness:" << bright << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 508: {
            int f   = ReadInt("func (0 AiZoom/1 AF/2 HDR/3 Mirror/4 Ai/5 VScreen ...): ");
            bool en = ReadInt("enable (0/1): ") != 0;
            PrintResult(ext.EnableExtendFuncWork(static_cast<ExtendFuction>(f), en));
            break;
        }
        case 509: {
            std::map<ExtendFuction, bool> status;
            if (ext.GetExtendFuncStatus(status)) {
                for (const auto &kv : status)
                    std::cout << "func " << static_cast<int>(kv.first) << " -> " << kv.second << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        // ---------------- [6xx] 音频 ----------------
        case 601:
            PrintResult(ext.EnableAudioNoiseReduction(ReadInt("enable noise reduction (0/1): ") != 0));
            break;
        case 602: {
            bool en = false;
            if (ext.GetAudioNoiseStatus(en))
                std::cout << "noise reduction: " << en << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 603:
            PrintResult(ext.SetAudioCaptureMode(static_cast<AudioCaptureMode>(ReadInt("capture mode (0 near/1 far/2 live): "))));
            break;
        case 604:
            std::cout << "audio capture mode: " << static_cast<int>(ext.GetAudioCaptureMode()) << std::endl;
            break;
        case 605:
            PrintResult(
                ext.SetAudioDirectionMode(static_cast<AudioDirectionMode>(ReadInt("direction mode (0 std/1 wide/2 focus/3 music): "))));
            break;
        case 606:
            std::cout << "audio direction mode: " << static_cast<int>(ext.GetAudioDirectionMode()) << std::endl;
            break;
        // ---------------- [8xx] 模式与隐私 ----------------
        case 801:
            PrintResult(ext.SwitchStorageMode());
            break;
        case 802:
            PrintResult(ext.SwitchMscReadOnlyClass());
            break;
        case 803:
            PrintResult(ext.SwitchNormalMode());
            break;
        case 804:
            PrintResult(ext.SetPrivacyMode(ReadInt("enable privacy (0/1): ") != 0));
            break;
        case 805: {
            int mode = 0;
            bool en  = false;
            if (ext.GetPrivacyMode(mode, en))
                std::cout << "privacy mode:" << mode << " enable:" << en << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 806:
            PrintResult(ext.EnableVerticalScreen(ReadInt("vertical screen (0 horiz/1 vert): ") != 0));
            break;
        case 807:
            PrintResult(ext.EnableLowResolution(ReadInt("low resolution (0/1): ") != 0));
            break;
        case 808:
            PrintResult(ext.EnableHorizontalCorrection(ReadInt("horizontal correction (0/1): ") != 0));
            break;
        case 809: {
            bool en = false;
            if (ext.GetEnableHorizontalCorrection(en))
                std::cout << "horizontal correction: " << en << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 810:
            PrintResult(ext.LoadFactoryDefault());
            break;
        // ---------------- [10xx] 追踪禁区与区域追踪 ----------------
        case 1001:
            PrintResult(ext.EnterTrackForbiddenAreaSetting());
            break;
        case 1002:
            PrintResult(ext.ExitTrackForbiddenAreaSetting());
            break;
        case 1003: {
            std::vector<ForbiddenAreaInfo> infos;
            if (ext.GetTrackForbiddenArea(infos)) {
                std::cout << "forbidden area count: " << infos.size() << std::endl;
                for (const auto &a : infos)
                    std::cout << "box_id:" << static_cast<int>(a.box_id) << " enable:" << static_cast<int>(a.enable) << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 1004: {
            TrackAreaInfo ta;
            if (ext.GetTrackAreaInfo(ta)) {
                std::cout << "enable_tracking:" << ta.enable_tracking << " enter_setting:" << ta.enter_setting
                          << " exception_state:" << static_cast<int>(ta.exception_state) << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        // ---------------- [11xx] 云台扩展与其他 ----------------
        case 1101: {
            CameraControlRelativeInfo pan, tilt;
            if (ext.GetPanTiltRelative(pan, tilt)) {
                std::cout << "pan value:" << static_cast<int>(pan.value) << " speed:" << pan.speed << std::endl;
                std::cout << "tilt value:" << static_cast<int>(tilt.value) << " speed:" << tilt.speed << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 1102: {
            CameraControlRelativeInfo pan, tilt;
            pan.value  = static_cast<CameraControlRelativeValue>(ReadInt("pan value (0/1/255): "));
            pan.speed  = ReadInt("pan speed: ");
            tilt.value = static_cast<CameraControlRelativeValue>(ReadInt("tilt value (0/1/255): "));
            tilt.speed = ReadInt("tilt speed: ");
            PrintResult(ext.SetPanTiltRelative(pan, tilt));
            break;
        }
        case 1103: {
            int32_t pan = 0, tilt = 0;
            if (ext.GetPanTiltAbsoluteValue(pan, tilt))
                // 1/360 度 → 度
                std::cout << "pan(degree):" << pan / kExtPanTiltUnitsPerDegree
                          << " tilt(degree):" << tilt / kExtPanTiltUnitsPerDegree << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 1104: {
            int32_t pan  = ReadInt("pan value (degree): ");
            int32_t tilt = ReadInt("tilt value (degree): ");
            // 度 → 1/360 度下发
            PrintResult(ext.SetPanTiltAbsolute(pan * kExtPanTiltUnitsPerDegree, tilt * kExtPanTiltUnitsPerDegree));
            break;
        }
        case 1105: {
            uint16_t zoom = 0;
            if (ext.GetRealZoomValue(zoom))
                std::cout << "real zoom: " << zoom << std::endl;
            else
                PrintResult(false);
            break;
        }
        case 1107: {
            HostPTZInfo host_ptz{};
            constexpr uint16_t kRatioMin = 0x0000;
            constexpr uint16_t kRatioMax = 0xffff;
            if (ext.GetHostPTZ(host_ptz)) {
                const double host_x = static_cast<double>(host_ptz.zoomX - 0) / (kRatioMax - kRatioMin);
                const double host_y = static_cast<double>(host_ptz.zoomY - 0) / (kRatioMax - kRatioMin);
                std::cout << "get host ptz x: " << host_x << std::endl;
                std::cout << "get host ptz y: " << host_y << std::endl;
            }
            else {
                PrintResult(false);
            }
            break;
        }
        case 1108: {
            HostPTZInfo host_ptz{};
            constexpr uint16_t kRatioMin = 0x0000;
            constexpr uint16_t kRatioMax = 0xffff;
            host_ptz.ratio      = static_cast<uint16_t>(ReadInt("zoom(100-400): "));
            host_ptz.zoomX      = static_cast<uint16_t>((ReadDouble("host ptz x(0-1): ") - 0) * (kRatioMax - kRatioMin));
            host_ptz.zoomY      = static_cast<uint16_t>((ReadDouble("host ptz y(0-1): ") - 0) * (kRatioMax - kRatioMin));
            host_ptz.move_step  = 20;
            host_ptz.ratio_step = 20;
            PrintResult(ext.SetHostPTZ(host_ptz));
            break;
        }
        // ---------------- [12xx] 固件升级 ----------------
        case 1201: {
            std::string path;
            std::cout << "firmware file path: ";
            std::cin >> path;
            bool started = UVCCameraExtendController::FirmwareUpgrade(info, path, [](long progress, FirmwareUpgradeStatus status) {
                std::cout << "progress:" << progress << " status:" << static_cast<int>(status) << std::endl;
            });
            std::cout << (started ? "upgrade finished" : "upgrade failed / unsupported") << std::endl;
            return 0;
        }
        default:
            std::cout << "unknown option" << std::endl;
            break;
        }
    }
    return 0;
}
