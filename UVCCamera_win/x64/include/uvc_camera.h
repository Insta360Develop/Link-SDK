#pragma once

#include <map>
#include <memory>
#include <vector>
#include "uvc_common.h"

class UVCCameraControllerImp;
class UVCCameraExtendControllerImp;
class UVCCameraDeviceWatcherImp;

/**
 * @brief 接口主要是通过UVC指令控制Link相机,接口主要分为两部分，标准接口和拓展接口
 *        标准接口通过这个'UVCCameraController'这个接口去实现，里面包含Pan、Tilt和Roll以及白平衡、Foucs、快门等标准实现，详细请查看这个接口
 *        拓展接口通过这个'UVCCameraExtendController'这个接口去实现，里面实现Link的特色功能
 */

namespace uvc {
    /**
     * @brief 获取相机列表
     * @return list 相机信息列表，详细请查看'UVCCameraInfo'这个结构体
     */
    void CAMERASDK_API GetUVCCameraList(std::vector<UVCCameraInfo>& list);

    /**
     *@brief 这个类主要用于监控usb的热插拔事件,目前对只是针对Link相机进行检测，其他的相机没有作用
     *      这个只是针对mac os生效
     *      注意：Linux 平台也不支持热插拔监听
     */
    class CAMERASDK_API UVCCameraDeviceWatcher {
    public:
        UVCCameraDeviceWatcher();
        ~UVCCameraDeviceWatcher();

        /**
         * \brief 开始监听相机的USB插拔
         * \param callback 可以通过这个回调接口去动态查看相机的插播信息
         * \return 无
         */
        bool StartWatch(DeviceWatcherCallBack callback);

        /**
         * \brief 停止监听
         * \return 无
         */
        void StopWatch();

    private:
        std::shared_ptr<UVCCameraDeviceWatcherImp> imp_;
    };

    /**
     *@brief 用于UVC的标准命令
     */
    class CAMERASDK_API UVCCameraController {
    public:
        UVCCameraController(const UVCCameraInfo& camera);
        ~UVCCameraController();

    public:
        /**
         * @brief 获取相机支持的MeidaFormat列表,信息包含分辨率和帧率
         * @param list 支持的meidaFormat列表
         * @return 无
         */
        bool GetMediaFormatList(std::vector<UVCMediaFormat>& list);

        /**
            @brief 设置pan和tilt的Relative值
            @param pan_info 左右移动
            @param tilt_info 垂直移动
            @return true 操作成功 false 操作失败
            @note 当想停止移动的时候,将 'CameraControlRelativeInfo'的value设置为stop.
                  CameraControlRelativeInfo 这个结构体包含了移动的方向和速度。设置value的stop的时候
                  停止移动
        */
        bool SetPanTiltRelative(CameraControlRelativeInfo pan_info, CameraControlRelativeInfo tilt_info);

        /**
           @brief 获取pan和tilt的Relative值
           @param pan_info 左右移动
           @param tilt_info 垂直移动
           @return true 操作成功 false 操作失败
        */
        bool GetPanTiltRelative(CameraControlRelativeInfo& pan_info, CameraControlRelativeInfo& tilt_info);

        /**
            @brief 获取pan和tilt的绝对值的最大最小值、步长等信息
            @param pan_info
            @param tilt_info
            @return true 操作成功 false 操作失败
       */
        bool GetPanTiltAbsoluteRange(CameraControlInfo& pan_info, CameraControlInfo& tilt_info);

        /**
            @brief 设置pan和tilt的Absolute值
            @param pan_value
            @param tilt_value
            @return true 操作成功 false 操作失败
       */
        bool SetPanTiltAbsolute(long pan_value, long tilt_value);

        /**  only link1
             @brief 获取roll的绝对值的最大最小值、步长等信息
             @param info
             @return true 操作成功 false 操作失败
        */
        bool GetRollAbsoluteRange(CameraControlInfo& info);

        /** only link1
            @brief 获取roll的Absolute值
            @param value
            @return true 操作成功 false 操作失败
        */
        bool GetRollAbsolute(long& value);

        /**
            @brief 设置roll的Absolute值
            @param value
            @return true 操作成功 false 操作失败
        */
        bool SetRollAbsolute(long value);

        /**
             @brief 获取Focus的绝对值的最大最小值、步长等信息
             @param info
             @return true 操作成功 false 操作失败
        */
        bool GetFocusAbsoluteRange(CameraControlInfo& info);

        /**
            @brief 获取Focus的Absolute值
            @param value
            @return true 操作成功 false 操作失败
       */
        bool GetFocusAbsolute(long& value);

        /**
            @brief 设置Focus的Absolute值
            @param value
            @return true 操作成功 false 操作失败
        */
        bool SetFocusAbsolute(long value);

        /**
            @brief 获取自动曝光模式的设置
            @param enble true开启自动曝光 false为关闭自动曝光
            @return true 操作成功 false 操作失败
        */
        bool GetAutoFocusStatus(bool& enable);

        /**
            @brief 自动曝光模式的设置
            @param enble true开启自动曝光 false为关闭自动曝光
            @return true 操作成功 false 操作失败
        */
        bool EnableAutoFocus(bool enable);

        /**
            @brief 设置PowerLineFrequency ，抗闪烁
            @param value 详见'PowerLineFrequency'
            @return true 操作成功 false 操作失败
        */
        bool SetPowerLineFrequency(PowerLineFrequency value);

        /**
             @brief 获取PowerLineFrequency
             @param value 详见'PowerLineFrequency'
             @return true 操作成功 false 操作失败
        */
        bool GetPowerLineFrequency(PowerLineFrequency& value);

        /**
             @brief 获取Zoom的绝对值的最大最小值、步长等信息
             @param info
             @return true 操作成功 false 操作失败
        */
        bool GetZoomAbsoluteRange(CameraControlInfo& info);

        /**
            @brief 获取Zoom的Absolute值
            @param value
            @return true 操作成功 false 操作失败
        */
        bool GetZoomAbsolute(long& value);

        /**
            @brief 设置Zoom的Absolute值
            @param value
            @return true 操作成功 false 操作失败
        */
        bool SetZoomAbsolute(long value);

        /**
            @brief 获取快门速度的列表
            @param list 速度列表
            @return true 操作成功 false 操作失败
        */
        bool GetShutterSpeedList(std::vector<long>& list);

        /**
            @brief 设置快门速度
            @param value 快门速度
            @return true 操作成功 false 操作失败
        */
        bool SetShutterSpeed(long value);

        /**
            @brief 获取当前快门速度
            @param value 快门速度
            @return true 操作成功 false 操作失败
        */
        bool GetShutterSpeed(long& value);

        /**
            @brief 自动曝光的设置
            @param enable true为开启 false为关闭
            @return true 操作成功 false 操作失败
        */
        bool EnableAutoExposure(bool enable);

        /**
            @brief 获取自动曝光的设置
            @param enable true为开启 false为关闭
            @return true 操作成功 false 操作失败
       */
        bool GetAutoExposureStatus(bool& enable);

        /**
             @brief 获取Brightness的最大最小值、步长等信息
             @param info
             @return true 操作成功 false 操作失败
        */
        bool GetBrightnessRange(CameraControlInfo& info);

        /**
            @brief 设置Brightness
            @param value
            @return true 操作成功 false 操作失败
        */
        bool SetBrightnessValue(int16_t value);

        /**
            @brief 获取当前的Brightness
            @param value
            @return true 操作成功 false 操作失败
       */
        bool GetBrightnessValue(int16_t& value);

        /**
             @brief 获取Contrast的最大最小值、步长等信息
             @param info
             @return true 操作成功 false 操作失败
        */
        bool GetContrastRange(CameraControlInfo& info);

        /**
             @brief 设置Contrast
             @param value
             @return true 操作成功 false 操作失败
        */
        bool SetContrastValue(uint16_t value);

        /**
            @brief 获取当前的Contrast
            @param value
            @return true 操作成功 false 操作失败
        */
        bool GetContrastValue(uint16_t& value);

        /**
            @brief 获取Saturation的最大最小值、步长等信息
            @param info
            @return true 操作成功 false 操作失败
        */
        bool GetSaturationRange(CameraControlInfo& pan_info);

        /**
            @brief 设置Saturation
            @param value
            @return true 操作成功 false 操作失败
        */
        bool SetSaturationValue(uint16_t value);

        /**
            @brief 获取当前的Saturation
            @param value
            @return true 操作成功 false 操作失败
        */
        bool GetSaturationValue(uint16_t& value);

        /**
            @brief 获取Sharpness的最大最小值、步长等信息
            @param info
            @return true 操作成功 false 操作失败
        */
        bool GetSharpnessRange(CameraControlInfo& pan_info);

        /**
            @brief 设置Sharpness
            @param value
            @return true 操作成功 false 操作失败
       */
        bool SetSharpnessValue(uint16_t value);

        /**
            @brief 获取当前的Sharpness
            @param value
            @return true 操作成功 false 操作失败
        */
        bool GetSharpnessValue(uint16_t& value);

        /**
            @brief 获取白平衡的最大最小值、步长等信息
            @param info
            @return true 操作成功 false 操作失败
        */
        bool GetWhitebalanceTemperatureRange(CameraControlInfo& pan_info);

        /**
            @brief 设置白平衡的值
            @param value
            @return true 操作成功 false 操作失败
        */
        bool SetWhitebalanceTemperatureValue(uint16_t value);

        /**
           @brief 获取当前的白平衡的值
           @param value
           @return true 操作成功 false 操作失败
        */
        bool GetWhitebalanceTemperatureValue(uint16_t& value);

        /**
            @brief 设置白平衡模式
            @param value 1--> auto 0 -->matual
            @return true 操作成功 false 操作失败
        */
        bool SetWhitebalanceTemperatureMode(uint8_t mode);

        /**
            @brief 获取白平衡模式
            @param value 1--> auto 0 -->matual
            @return true 操作成功 false 操作失败
        */
        bool GetWhitebalanceTemperatureMode(uint8_t& mode);

    public:
        std::shared_ptr<UVCCameraControllerImp> imp_;
    };

    /**
        @brief 这个类主要用于UVC的拓展命令，用于与UVCCamera进行拓展指令的交互
    */
    class CAMERASDK_API UVCCameraExtendController {
    public:
        UVCCameraExtendController(const UVCCameraInfo& camera);
        ~UVCCameraExtendController();

    public:
        /**
            @brief 获取设备信息，比如sensorId,uuid等
            @param info  执行成功后,信息保存在其中
            @return true 成功 false 失败
        */
        bool GetDeviceInfo(DeviceInfo& info);
        /**
            @brief 获取序列号
            @param serialNo 执行成功后,信息保存在其中
            @return true 成功 false 失败
        */
        bool GetSerialNumber(std::string& serialNo);

        /**
            @brief 恢复到出厂设置
            @return true 成功 false 失败
        */
        bool LoadFactoryDefault();

        /**
            @brief 固件升级
            @param file_path 固件文件的路径
            @param callback 用于实时获取当前升级的状态，根据回调的状态可以做一些操作，比如进行驱动升级
            @return true 升级成功 false 升级失败
            @note Linux 平台不支持固件升级，调用会通过 callback 返回失败并返回 false。
        */
        static bool FirmwareUpgrade(const UVCCameraInfo& uvc_camera_info,
            const std::string& file_path,
            FirmwareUpgradeProgressCallBack callback);

        /**
            @brief 设置构图模式
            @param mode 构图模式，详见'VideoMode'枚举类型
            @param extenddata 这个数据主要用于白板模式
            @return true 成功 false 失败
        */
        bool SetVideoMode(VideoMode mode, const VideoModeAuxiliaryData& extenddata);

        // 切换到正常模式, 会重启相机
        bool SwitchNormalMode();
        /**
             @brief 获取当前构图模式
             @param mode 获取到模式
             @param extenddata 如果是白板模式，要把此数据保存起来, 再次设置白板的时候，需要将此数据设置给相机
             @return true 成功 false 失败
        */
        bool GetVideoMode(VideoMode& mode, VideoModeAuxiliaryData& extenddata);

        /**
            @brief 设置构图样式
            @param style 构图样式 ，详见'CompositionStyle'枚举类型
            @return true 成功 false 失败
        */
        bool SetCompositionStyle(CompositionStyle style);

        /**
            @brief 获取当前构图样式
            @param style 获取到的样式 ，详见'CompositionStyle'枚举类型
            @return true 成功 false 失败
        */
        bool GetCompositionStyle(CompositionStyle& style);

        /**
            @brief 设置跟踪速度
            @param speed 跟踪速度 ，详见'TrackSpeed'枚举类型
            @return true 成功 false 失败
        */
        bool SetTrackSpeed(TrackSpeed speed);

        /**
            @brief 获取当前跟踪速度
            @param speed 此时跟踪速度 ，详见'TrackSpeed'枚举类型
            @return true 成功 false 失败
        */
        bool GetTrackSpeed(TrackSpeed& speed);

        /**
            @brief 设置音频降噪是否开启
            @param enable true为开启状态 false为关闭状态
            @return true 成功 false 失败
        */
        bool EnableAudioNoiseReduction(bool enable);

        /**
            @brief 获取是否当前音频降噪开启
            @param enable true为开启状态 false为关闭状态
            @return true 成功 false 失败
        */
        bool GetAudioNoiseStatus(bool& enable);

        /**
            @brief 开启和关闭某个手势
            @param gesture 手势类型, 详见'Gesture'结构体
            @param enable true为开启 false为关闭
            @return true 操作成功 false 操作失败
        */
        bool EnableGestureWork(Gesture gesture, bool enable);

        /**
           @brief 手势使能总开关
           @param enable true为所用手势功能开启 false为所用手势功能关闭
           @return true 操作成功 false 操作失败
           @note 这个接口是个总的开关，对各个手势的开关状态不进行设置
       */
        bool EnableAllGesture(bool enable);

        /**
            @brief 获取手势开启状态
            @enable_status 各个手势功能的开启状态, true为开启 false为关闭
            @return true 操作成功 false 操作失败
            @note 这个对于'EnableAllGesture'的操作没有影响
        */
        bool GetGestureStatus(std::map<Gesture, bool>& enable_status);

        /**
            @brief 绑定功能和手势样式
            @param gesture 手势样式
            @param func 手势对应的功能
            @return true 操作成功 false 操作失败
            @note 详见 'Gesture'和'GestureFunction'结构体
        */
        bool GestureBinding(Gesture gesture, GestureFunction func);

        /**
            @brief 获取手势样式和手势功能对应的map
            @param bind_status 获取样式和功能的绑定状态
            @return true 操作成功 false 操作失败
            @note 详见 'Gesture'和'GestureFunction'结构体
        */
        bool GetGestureBindingMap(std::map<Gesture, GestureFunction>& bind_status);

        /**
            @brief 获取追踪目标的列表(最多10个人头)
            @param objs 当前检测到人头位置列表
            @return true 操作成功 false 操作失败
        */
        bool GetTrackObjLists(std::vector<UVCRect>& objs);

        /**
            @brief 获取追踪目标的列表(最多10个人头) 新接口
            @param objs 当前检测到人头位置列表
            @return true 操作成功 false 操作失败
        */
        bool GetNewTrackObjLists(std::vector<UVCRect>& objs);

        /**
            @brief 设置追踪目标
            @param point 要追踪目标的点，比如获取到人头位置的框中
            @return true 操作成功 false 操作失败
        */
        bool SetTrackObj(const UVCRect& rect);

        /**
            @brief 设置曝光补偿
            @param value 单位 EV。业务实际范围 [-3.0, +3.0]，步进 0.30（来源为主程序 UI 约定，
                         SDK 不做范围校验，超出固件支持范围可能被相机钳制或拒绝）。
                         注意：HDR 开启时该功能通常被禁用（Link2 Pro 除外）。
            @return true 操作成功 false 操作失败
        */
        bool SetExposureCompensation(float value);

        /**
            @brief 获取当前曝光补偿
            @param value 当前曝光补偿值
            @return true 操作成功 false 操作失败
        */
        bool GetExposureCompensation(float& value);

        /**
            @brief 设置构图bias
            @param x 横向偏移量
            @param y 纵向偏移量
            @return true 操作成功 false 操作失败
        */
        bool SetCompositionBias(int16_t x, int16_t y);

        /**
            @brief 获取构图bias
            @param x 横向偏移量
            @param y 纵向偏移量
            @return true 操作成功 false 操作失败
        */
        bool GetCompositionBias(int16_t& x, int16_t& y);

        /**
            @brief 获取云台版本信息
            @param info 云台信息
            @return true 操作成功 false 操作失败
        */
        bool GetPTZVersionInfo(PTZVerInfo& info);

        /**
            @brief 设置ISO
            @param value ISO 值。业务使用离散档位（共16档，来源为主程序约定）：
                         100/125/160/200/250/320/400/500/640/800/1000/1250/1600/2000/2500/3200。
                         SDK 不做范围校验，直接下发原始 uint16_t，超出固件支持范围可能被拒绝。
            @return true 操作成功 false 操作失败
        */
        bool SetISOValue(uint16_t value);

        /**
            @brief 获取ISO
            @param value
            @return true 操作成功 false 操作失败
        */
        bool GetISOValue(uint16_t& value);

        /**
            @brief 拓展功能的开启
            @param func 详见 'ExtendFuction'这个里面有拓展的功能
            @param enable true为开启 false为关闭
            @return true 操作成功 false 操作失败
        */
        bool EnableExtendFuncWork(ExtendFuction func, bool enable);

        /**
            @brief 获取拓展功能的状态，开启还是关闭
            @param enable_status 详见 'ExtendFuction'这个里面有拓展的功能,对应的bool值(true为开启，false为关闭)
            @return true 操作成功 false 操作失败
        */
        bool GetExtendFuncStatus(std::map<ExtendFuction, bool>& enable_status);

        /**
            @brief 获取当前播放分辨率
            @param width和height 当前播放的分辨率
            @param framerate 当前播放的帧率
            @return true 操作成功 false 操作失败
        */
        bool GetCameraPlayRes(uint32_t& wdith, uint32_t& height, uint16_t& framerate);

        /**
            @brief 自动曝光模式的设置
            @param enble true开启自动曝光 false为关闭自动曝光
            @return true 操作成功 false 操作失败
        */
        bool EnableAutoExposure(bool enable);

        /**
            @brief 获取自动曝光模式的设置
            @param enble true开启自动曝光 false为关闭自动曝光
            @return true 操作成功 false 操作失败
        */
        bool GetAutoExposureStatus(bool& enable);

        /**
            @brief 设置画框类型
            @param type 详见'DrawBoxType'结构体
            @return true 操作成功 false 操作失败
        */
        bool SetDrawBoxType(DrawBoxType type);

        /**
            @brief 获取画框类型以及相关数据
            @param type 框类型
            @param infos 框的位置和其他信息
        */
        bool GetDrawBoxStatus(DrawBoxType& type, std::vector<DrawBoxInfo>& infos);

        /**
            @brief 设置曝光曲线
            @param curve 曝光曲线数据, 详见'ExpouseCurve'结构体
            @return true 操作成功 false 操作失败
        */
        bool SetExpouseCurve(const ExpouseCurve& curve);

        /**
            @brief 获取当前的曝光曲线
            @param curve 曝光曲线数据, 详见'ExpouseCurve'结构体
            @return true 操作成功 false 操作失败
        */
        bool GetExpouseCurve(ExpouseCurve& curve);

        /**
            @brief 横竖屏开关
            @param enable false为开启横屏,true为开启竖屏
            @return true 操作成功 false 操作失败
        */
        bool EnableVerticalScreen(bool enable);

        /**
        @brief 低分辨率开关
        @param enable false为开启低分辨率,true为开启低分辨率
        @return true 操作成功 false 操作失败
        */
        bool EnableLowResolution(bool enable);
        /**
            @brief 设置pan和tilt的Relative值
            @param pan_info 左右移动
            @param tilt_info 垂直移动
            @return true 操作成功 false 操作失败
            @note 当想停止移动的时候,将 'CameraControlRelativeInfo'的value设置为stop.
                  CameraControlRelativeInfo 这个结构体包含了移动的方向和速度。设置value的stop的时候
                  停止移动
        */
        bool SetPanTiltRelative(CameraControlRelativeInfo pan_info, CameraControlRelativeInfo tilt_info);

        /**
            @brief 获取pan和tilt的Relative值
            @param pan_info 左右移动
            @param tilt_info 垂直移动
            @return true 操作成功 false 操作失败
        */
        bool GetPanTiltRelative(CameraControlRelativeInfo& pan_info, CameraControlRelativeInfo& tilt_info);

        /**
            @brief 获取pan和tilt的Absolute值
            @param pan_value
            @param tilt_value
            @return true 操作成功 false 操作失败
            //范围（系数为3600时）：pan_value（-145 ~ 145），tilt_value（-45~ 90）
        */
        bool GetPanTiltAbsoluteValue(int32_t& pan_value, int32_t& tilt_value);

        /**
            @brief 设置pan和tilt的Absolute值
            @param pan_value
            @param tilt_value
            @return true 操作成功 false 操作失败
        */
        bool SetPanTiltAbsolute(int32_t pan_value, int32_t tilt_value);

        // 云台追踪接口
        bool SetPtzTrackingBox(const GimbalTrackingBoxInfo& info);

        // 图像模板接口
        bool GetImageTemplateInfo(ImageTemplateInfo& info);
        bool SetImageTemplate(int8_t template_id, ImageOperation operation);
        bool GetImageTemplateName(int8_t template_id, std::string& name);
        bool SetImageTemplateName(int8_t template_id, const std::string& name);
        bool DeleteTemplate(int8_t template_id);

        // 滤镜接口
        bool SetFilter(FilterMode filterId);
        bool GetFilter(FilterMode& filterId);

        // 时间戳接口
        bool SetTimestamp(uint64_t time_stamp);

        bool SetIQSwitch(int i, bool enable);

        bool GetFillLightParam(int& white_blance, int& brigtness);

        // 隐私模式接口
        bool SetPrivacyMode(bool enable);
        bool GetPrivacyMode(int& mode, bool& enable);

        // 通知恢复出厂设置
        bool RestoreFactorySet(bool enable);

        // 获取相机的类型
        bool GetCameraType(std::string& camera_type) const;

        // 发送HostPTZInfo
        bool SetHostPTZ(HostPTZInfo& host_ptz_info);

        // 获得HostPTZInfo
        bool GetHostPTZ(HostPTZInfo& host_ptz_info);

        // 强制记忆智能白板
        bool ForceSetSmatrWhiteBoard(int value);

        // 获取设备的PID
        uint16_t GetPID() const;

        AudioCaptureMode GetAudioCaptureMode(); //link 2 音频模式
        bool SetAudioCaptureMode(AudioCaptureMode mode);

        AudioDirectionMode GetAudioDirectionMode(); //link 2 pro 音频模式
        bool SetAudioDirectionMode(AudioDirectionMode mode);

        // 追踪禁区接口
        // 进入追踪禁区设置窗口之前要调用
        bool EnterTrackForbiddenAreaSetting();

        // 退出追踪禁区设置窗口之后要调用
        bool ExitTrackForbiddenAreaSetting();

        // 设置追踪禁区
        bool SetTrackForbiddenArea(const std::vector<ForbiddenAreaInfo>& infos);

        // 获取追踪禁区信息
        bool GetTrackForbiddenArea(std::vector<ForbiddenAreaInfo>& infos);

        // 开启或者关闭追踪禁区功能
        bool EnableTrackForbiddenArea(bool& enable);

        // 设置默认追踪模式
        bool SetDefaultTrackMode(int track_mode);
        bool GetDefaultTrackMode(int& track_mode);
        /**
            @brief 水平矫正
            @param enble true开启水平矫正 false为水平矫正
            @return true 操作成功 false 操作失败
        */
        bool EnableHorizontalCorrection(bool enable);
        /**
            @brief 获取 水平矫正 设置
            @param enble true开启水平矫正 false为水平矫正
            @return true 操作成功 false 操作失败
        */
        bool GetEnableHorizontalCorrection(bool& enable);

        // 获取设备状态，比如主板温度，sensor温度
        bool GetDeviceStatus(DeviceStatus& status);

        // 只读U盘模式
        bool SwitchMscReadOnlyClass();

        // 切换到U盘模式
        bool SwitchStorageMode();

        // 获取实时 zoom
        bool GetRealZoomValue(uint16_t& zoom);

        bool GetUDiskOpenCount(uint32_t& count);

        bool SetUDiskOpenCount(uint32_t count);

        // 透传接口
        bool SetData(int selector, unsigned char* data, int data_size, int unit_id = 0);

        bool GetReportData(std::vector<std::string>& data_vec);

        // 预设位相关接口
        bool SetCurrentPreset(int preset_id);

        bool GetCurrentPreset(int& preset_id);

        bool SetPresetStatus(const PresetInfo& preset_info);

        bool GetPresetStatus(PresetInfo& preset_info);

        bool GetPresetSupportNum(int& num);

        bool GotoPreset(int preset_id);

        bool DeletePreset(int preset_id);

        bool SyncLocalPresetToCamera(const LocalPresetInfo& local_preset_info);

        // 切换到simpleclass mode
        void EnableSimpleClassMode(bool enable);

        // 区域追踪接口
        bool GetTrackAreaInfo(TrackAreaInfo& info);

        bool SetTrackAreaInfo(const TrackAreaInfo& info);

        bool GetPresetInfoFromCamera(LocalPresetInfo& local_preset_info);

        bool SyncPresetInfoToCamera(const LocalPresetInfo& local_preset_info);

        bool GetCurrentPresetId(int& preset_id);

        bool SetCurrentPresetId(int preset_id);

#ifdef PRODECT_TEST_MODE

        // 设置序列号
        bool SetSerialNumber(const std::string& serialNo);

        // 切换到拍照模式
        bool SwitchPhotoMode(bool& bPhotoMode);

        // 用于相机拍照(用于产测)，会生成jpg和raw两种格式的文件
        TakePhotoResult TakePhoto(const std::string& jpg_save_path, const std::string& dng_save_path);

        // 下载日记文件
        // 注意：Linux 平台不支持日志下载（未依赖 libusb），调用会返回 -1。
        int DownLoadLogFile(const std::string& savePath);

        // 获取和设置音频设备的license
        bool GetAudioDeviceLicense(std::string& license);
        bool SetAudioDeviceLicense(const std::string& license);

        // 开始和停止老化测试
        bool StartAgingTest(AgeTestStreamDataCallBack callback);
        bool StopAgingTest();

        // 设置和获取pubkey
        bool SetPubkey(const std::string& key);
        bool GetPubkey(std::string& key);

        // 开始和停止云台老化测试
        bool StartPTZAgingTest(bool& imu_ok, MotorStatus& motor_status, bool& vailed, bool agetest, uint32_t ng_times);
        bool StopPTZAgingTest();

        // 弹出U盘
        bool SwitchUVCMode(const std::string& volumeName);

        // 录制音频
        bool StartAudioRecord(const std::string& save_path);
        bool StopAudioRecord();

        // 执行机内脚本
        bool ExecInMachineScript();

        // 获取执行脚本结果
        bool GetScriptExecResult(int32_t& res);

        // 指示灯测试
        bool IndicatorLightTest(ColorType type);

        // 开始触控板测试
        bool StartTouchPadTest();
        bool GetTouchPadTestResult(int& res);

        // 开始测试USB的读取速度,
        //test_duration 测试时长
        bool StartUSBSpeedTest(int test_duration, USBSpeedTestCallBack callback);

        // 停止测试USB的读取速度
        //speed 单位为 bytes/s
        bool StopUSBSpeedTest(long& speed);

        // 设置标定模式
        bool SetCalibrationMode(CalibrationMode mode);

        // 获取标定状态
        bool GetCalibrationStatus(CalibrationMode& cur_mode, CalibrationStatus& status);

        // 清除uuid
        bool ClearUUID();

        // 切换到simpleclass mode
        void EnableSimpleClassMode(bool enable);

        bool RunDataCheck(int8_t& res);
#endif

    public:
        std::shared_ptr<UVCCameraExtendControllerImp> imp_;
    };
}  // namespace uvc
