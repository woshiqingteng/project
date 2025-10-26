/**
 * @file app_ctrl.h
 * @brief Application Control Layer Header
 */

#ifndef APP_CTRL_H
#define APP_CTRL_H

#include <stdint.h>
#include <stdbool.h>

// Type Definitions
typedef void (*AppCmdCb)(uint8_t cmdId, bool success, const uint8_t* rspData);

// Command Definitions
typedef enum {
    APP_CMD_SET_CFG    = 0x11,
    APP_CMD_SYN_OP     = 0x14,
    APP_CMD_TRIG_OP    = 0x18,
    APP_CMD_CTRL_OP    = 0x20
} AppCmdId;

typedef enum {
    APP_FIU_NO_LOAD_NO_FAULT = 0x00,
    APP_FIU_LOAD_NO_FAULT    = 0x01,
    APP_FIU_NO_LOAD_STP      = 0x02,
    APP_FIU_NO_LOAD_STG      = 0x03,
    APP_FIU_LOAD_STP         = 0x04,
    APP_FIU_LOAD_STG         = 0x05,
    APP_FIU_CAN_STP          = 0x06,
    APP_FIU_CAN_STG          = 0x07,
    APP_FIU_CAN_SHORT        = 0x08,
    APP_FIU_LIN_STP          = 0x09,
    APP_FIU_LIN_STG          = 0x0A
} AppFiuCfg;

typedef enum {
    APP_MODE_STOP     = 0x00,
    APP_MODE_CTRL     = 0x01,
    APP_MODE_SYN      = 0x02,
    APP_MODE_TRIG     = 0x03
} AppWorkMode;

// Data Structure Definitions
typedef struct {
    uint8_t chId;
    AppFiuCfg outCfg;
    AppWorkMode workMode;
    uint16_t highTime;
    uint16_t lowTime;
    uint8_t cycles;
} AppCfgParams;

typedef struct {
    uint64_t chMask;
} AppOpParams;

// Application Interface Structure
typedef struct {
    bool (*init)(void);
    bool (*sendCmd)(uint8_t cmdId, const uint8_t* cmdData, uint8_t dataLen, AppCmdCb callback);
    bool (*setCfg)(const AppCfgParams* params, AppCmdCb callback);
    bool (*synOp)(const AppOpParams* params, AppCmdCb callback);
    bool (*trigOp)(const AppOpParams* params, AppCmdCb callback);
    bool (*ctrlOp)(const AppOpParams* params, AppCmdCb callback);
    void (*setChBit)(AppOpParams* params, uint8_t channel);
    void (*clrChBit)(AppOpParams* params, uint8_t channel);
    bool (*isChSet)(const AppOpParams* params, uint8_t channel);
    bool (*isTxBusy)(void);
    void (*process)(void);
} AppCtrl;

// Public Interface Functions
const AppCtrl* AppCtrl_GetInstance(void);
extern uint32_t App_GetSysTime(void);

#endif