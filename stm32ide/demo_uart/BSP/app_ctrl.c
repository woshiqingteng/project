/**
 * @file app_ctrl.c
 * @brief Application Control Layer Implementation
 */

#include "app_ctrl.h"
#include "com_frame.h"
#include <string.h>

// Constants
#define APP_RSP_TIMEOUT_MS     100
#define APP_MAX_CHANNEL        63
#define APP_DATA_LEN           8

// Internal state
static struct {
    bool waitRsp;
    uint8_t expCmdId;
    uint8_t expRspData[APP_DATA_LEN];
    AppCmdCb currCb;
    uint32_t txStartTime;
    bool timeoutEnabled;
    
    // Channel tracking for batch operations
    AppOpParams trackedChannels;
} gAppState;

// Forward declarations
static void App_ResetState(void);
static void App_HandleTimeout(void);
static void App_FrameTxCb(bool success);
static void App_FrameRxCb(const uint8_t* frame);

// Data conversion functions
static void App_U16ToLe(uint16_t value, uint8_t* bytes) {
    bytes[0] = value & 0xFF;
    bytes[1] = (value >> 8) & 0xFF;
}

static void App_U64ToLe(uint64_t value, uint8_t* bytes) {
    for (int i = 0; i < 8; i++) {
        bytes[i] = (value >> (i * 8)) & 0xFF;
    }
}

static void App_BuildCfgData(const AppCfgParams* params, uint8_t* data) {
    data[0] = params->chId & 0x3F;
    data[1] = params->outCfg;
    data[2] = params->workMode;
    
    switch (params->workMode) {
        case APP_MODE_CTRL:
            memset(&data[3], 0xFF, 5);
            break;
        case APP_MODE_SYN:
            App_U16ToLe(params->highTime, &data[3]);
            App_U16ToLe(params->lowTime, &data[5]);
            data[7] = params->cycles;
            break;
        case APP_MODE_TRIG:
            App_U16ToLe(params->highTime, &data[3]);
            App_U16ToLe(params->lowTime, &data[5]);
            data[7] = 0xFF;
            break;
        case APP_MODE_STOP:
        default:
            memset(&data[3], 0xFF, 5);
            break;
    }
}

static void App_BuildOpData(const AppOpParams* params, uint8_t* data) {
    App_U64ToLe(params->chMask, data);
}

// Channel operation functions
static void App_SetChBit(AppOpParams* params, uint8_t channel) {
    if (channel <= APP_MAX_CHANNEL) {
        params->chMask |= (1ULL << channel);
    }
}

static void App_ClrChBit(AppOpParams* params, uint8_t channel) {
    if (channel <= APP_MAX_CHANNEL) {
        params->chMask &= ~(1ULL << channel);
    }
}

static bool App_IsChSet(const AppOpParams* params, uint8_t channel) {
    if (channel <= APP_MAX_CHANNEL) {
        return (params->chMask & (1ULL << channel)) != 0;
    }
    return false;
}

// State management functions
static void App_ResetState(void) {
    gAppState.waitRsp = false;
    gAppState.expCmdId = 0;
    gAppState.currCb = NULL;
    gAppState.timeoutEnabled = false;
    gAppState.txStartTime = 0;
    memset(gAppState.expRspData, 0, sizeof(gAppState.expRspData));
}

static void App_HandleTimeout(void) {
    if (gAppState.waitRsp && gAppState.timeoutEnabled) {
        uint32_t currTime = App_GetSysTime();
        if ((currTime - gAppState.txStartTime) >= APP_RSP_TIMEOUT_MS) {
            gAppState.waitRsp = false;
            gAppState.timeoutEnabled = false;
            
            if (gAppState.currCb) {
                gAppState.currCb(gAppState.expCmdId, false, NULL);
                gAppState.currCb = NULL;
            }
            
            App_ResetState();
        }
    }
}

// Callback functions
static void App_FrameTxCb(bool success) {
    if (!success) {
        gAppState.waitRsp = false;
        gAppState.timeoutEnabled = false;
        
        if (gAppState.currCb) {
            gAppState.currCb(gAppState.expCmdId, false, NULL);
            gAppState.currCb = NULL;
        }
        App_ResetState();
        return;
    }
    
    gAppState.txStartTime = App_GetSysTime();
    gAppState.timeoutEnabled = true;
}

static void App_FrameRxCb(const uint8_t* frame) {
    if (!gAppState.waitRsp) {
        return;
    }
    
    uint8_t rxCmdId = frame[1];
    const uint8_t* rxData = &frame[2];
    
    bool success = (rxCmdId == gAppState.expCmdId) && 
                  (memcmp(rxData, gAppState.expRspData, APP_DATA_LEN) == 0);
    
    gAppState.waitRsp = false;
    gAppState.timeoutEnabled = false;
    
    if (gAppState.currCb) {
        gAppState.currCb(rxCmdId, success, rxData);
        gAppState.currCb = NULL;
    }
    
    App_ResetState();
}

// Channel tracking functions
static void App_TrackChannel(uint8_t channel) {
    if (channel <= APP_MAX_CHANNEL) {
        gAppState.trackedChannels.chMask |= (1ULL << channel);
    }
}

static void App_ClearTrackedChannels(void) {
    gAppState.trackedChannels.chMask = 0;
}

static uint64_t App_GetTrackedChannels(void) {
    return gAppState.trackedChannels.chMask;
}

// Public interface implementation
static bool AppCtrl_Init(void) {
    const ComFrame* frame = ComFrame_GetInstance();
    
    if (!frame->init()) {
        return false;
    }
    
    frame->setRxCb(App_FrameRxCb);
    App_ResetState();
    App_ClearTrackedChannels();
    
    return true;
}

static bool AppCtrl_SendCmd(uint8_t cmdId, const uint8_t* cmdData, uint8_t dataLen, AppCmdCb callback) {
    const ComFrame* frame = ComFrame_GetInstance();
    
    // Check if communication layer is busy
    if (frame->isTxBusy()) {
        if (callback) callback(cmdId, false, NULL);
        return false;
    }
    
    gAppState.expCmdId = cmdId;
    if (cmdData) {
        memcpy(gAppState.expRspData, cmdData, APP_DATA_LEN);
    } else {
        memset(gAppState.expRspData, 0, APP_DATA_LEN);
    }
    gAppState.currCb = callback;
    gAppState.waitRsp = true;
    
    return frame->sendCmd(cmdId, cmdData, dataLen, App_FrameTxCb);
}

static bool AppCtrl_SetCfg(const AppCfgParams* params, AppCmdCb callback) {
    if (!params || params->chId > APP_MAX_CHANNEL || 
        params->outCfg > APP_FIU_LIN_STG || 
        params->workMode > APP_MODE_TRIG) {
        if (callback) callback(APP_CMD_SET_CFG, false, NULL);
        return false;
    }
    
    uint8_t cmdData[APP_DATA_LEN];
    App_BuildCfgData(params, cmdData);
    
    // Track the channel for future operations
    App_TrackChannel(params->chId);
    
    return AppCtrl_SendCmd(APP_CMD_SET_CFG, cmdData, sizeof(cmdData), callback);
}

static bool AppCtrl_SynOp(const AppOpParams* params, AppCmdCb callback) {
    AppOpParams opParams;
    
    // If no params provided, use tracked channels
    if (params == NULL) {
        opParams.chMask = App_GetTrackedChannels();
        if (opParams.chMask == 0) {
            if (callback) callback(APP_CMD_SYN_OP, false, NULL);
            return false;
        }
        params = &opParams;
    }
    
    uint8_t cmdData[APP_DATA_LEN];
    App_BuildOpData(params, cmdData);
    
    return AppCtrl_SendCmd(APP_CMD_SYN_OP, cmdData, sizeof(cmdData), callback);
}

static bool AppCtrl_TrigOp(const AppOpParams* params, AppCmdCb callback) {
    if (!params) {
        if (callback) callback(APP_CMD_TRIG_OP, false, NULL);
        return false;
    }
    
    uint8_t cmdData[APP_DATA_LEN];
    App_BuildOpData(params, cmdData);
    
    return AppCtrl_SendCmd(APP_CMD_TRIG_OP, cmdData, sizeof(cmdData), callback);
}

static bool AppCtrl_CtrlOp(const AppOpParams* params, AppCmdCb callback) {
    AppOpParams opParams;
    
    // If no params provided, use tracked channels
    if (params == NULL) {
        opParams.chMask = App_GetTrackedChannels();
        if (opParams.chMask == 0) {
            if (callback) callback(APP_CMD_CTRL_OP, false, NULL);
            return false;
        }
        params = &opParams;
    }
    
    uint8_t cmdData[APP_DATA_LEN];
    App_BuildOpData(params, cmdData);
    
    return AppCtrl_SendCmd(APP_CMD_CTRL_OP, cmdData, sizeof(cmdData), callback);
}

static bool AppCtrl_IsTxBusy(void) {
    const ComFrame* frame = ComFrame_GetInstance();
    return frame->isTxBusy() || gAppState.waitRsp;
}

static void AppCtrl_Process(void) {
    App_HandleTimeout();
}

// Application instance
static const AppCtrl gAppCtrlInstance = {
    .init = AppCtrl_Init,
    .sendCmd = AppCtrl_SendCmd,
    .setCfg = AppCtrl_SetCfg,
    .synOp = AppCtrl_SynOp,
    .trigOp = AppCtrl_TrigOp,
    .ctrlOp = AppCtrl_CtrlOp,
    .setChBit = App_SetChBit,
    .clrChBit = App_ClrChBit,
    .isChSet = App_IsChSet,
    .isTxBusy = AppCtrl_IsTxBusy,
    .process = AppCtrl_Process
};

const AppCtrl* AppCtrl_GetInstance(void) {
    return &gAppCtrlInstance;
}