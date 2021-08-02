#pragma once

enum class MyOpCode {
    StartLiveView = 0x9201,
    EndLiveView = 0x9202,
    StartBulbCapture = 0x9207,
    DeviceReady = 0x90C8,
    EndBulbCapture = 0x920C,
    ChangeAfArea = 0x9205,
    AutoFocus = 0x90C1,
    Capture = 0x90C0,
    CaptureInSDRAM = 0x90CB,
    OpenSession = 0x1002,
    ResetDevice = 0x1010,
    GetNumObjects = 0x1006,
    GetObject = 0x1009,
    GetThumb = 0x100A,
    DeleteObject = 0x100B,
    SetObjectProtection = 0x1012,
    MoveObject = 0x1019,
    CopyObject = 0x101A,
    InitiateCapture = 0x100E,
    InitiateOpenCapture = 0x101C,
    TerminateOpenCapture = 0x1018,
    PowerDown = 0x1013,
    SelfTest = 0x1011,
    CloseSession = 0x1003,
    GetDeviceInfo = 0x1001,
    GetObjectInfo = 0x1008,
    GetObjectPropValue = 0x9803,
    GetDevicePropValue = 0x1015,
    SetDevicePropValue = 0x1016,
    ResetDevicePropValue = 0x1017,
    GetStorageInfo = 0x1005,
    FormatStore = 0x100F,
    GetStorageIDs = 0x1004,
    GetObjectHandles = 0x1007,
    SendObjectInfo = 0x100C,
    SendObject = 0x100D,
    CheckEvent = 0x90C7,
    GetLiveViewImg = 0x9203,
    MfDrive = 0x9204,
};