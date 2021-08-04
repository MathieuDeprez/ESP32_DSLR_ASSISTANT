#pragma once

enum class MyRpCode {
    HardwareError = 0xA001,
    OutOfFocus = 0xA002,
    ChangeCameraModeFailed = 0xA003,
    InvalidStatus = 0xA004,
    SetPropertyNotSupported = 0xA005,
    WbResetError = 0xA006,
    DustReferenceError = 0xA007,
    ShutterSpeedBulb = 0xA008,
    MirrorUpSequence = 0xA009,
    CameraModeNotAdjustFNumber = 0xA00A,
    NotLiveView = 0xA00B,
    MfDriveStepEnd = 0xA00C,
    MfDriveStepInsufficiency = 0xA00E,
    AdvancedTransferCancel = 0xA022,
};