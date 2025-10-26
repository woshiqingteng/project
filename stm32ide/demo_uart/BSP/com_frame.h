/**
 * @file com_frame.h
 * @brief Communication Frame Layer Header
 */

#ifndef COM_FRAME_H
#define COM_FRAME_H

#include <stdint.h>
#include <stdbool.h>

// =============================================
// Callback Type Definitions
// =============================================
typedef void (*ComFrameRxCb)(const uint8_t* frame);
typedef void (*ComFrameTxCb)(bool success);

// =============================================
// Communication Interface Structure
// =============================================
typedef struct {
    bool (*init)(void);
    bool (*sendCmd)(uint8_t cmdId, const uint8_t* cmdData, uint8_t dataLen, ComFrameTxCb callback);
    void (*setRxCb)(ComFrameRxCb callback);
    bool (*isTxBusy)(void);
} ComFrame;

// =============================================
// Public Interface Functions
// =============================================
const ComFrame* ComFrame_GetInstance(void);

#endif