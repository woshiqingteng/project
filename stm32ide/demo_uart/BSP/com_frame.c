/**
 * @file com_frame.c
 * @brief Communication Frame Layer Implementation
 */

#include "com_frame.h"
#include "drv_uart.h"
#include <string.h>

// =============================================
// Constant Definitions
// =============================================
#define FRAME_HEADER_TX    0x5A
#define FRAME_HEADER_RX    0xA5
#define FRAME_TAIL_TX      0x55
#define FRAME_TAIL_RX      0xAA
#define FRAME_DATA_LEN     8
#define FRAME_TOTAL_LEN    12

// =============================================
// Internal State Definition
// =============================================
static struct {
    uint8_t rxBuf[32];
    uint8_t rxIdx;
    bool rxSynced;
    ComFrameRxCb rxCb;
    
    uint8_t txFrame[FRAME_TOTAL_LEN];
    ComFrameTxCb txCb;
    bool txPending;
    
    uint8_t lastTxCmdId;
    uint8_t lastTxData[FRAME_DATA_LEN];
} gComFrame;

// =============================================
// Frame Processing Functions
// =============================================
static uint8_t ComFrame_CalcChecksum(uint8_t cmdId, const uint8_t* data) {
    uint8_t sum = cmdId;
    for (int i = 0; i < FRAME_DATA_LEN; i++) {
        sum ^= data[i];
    }
    return sum;
}

static void ComFrame_BuildCmd(uint8_t* frame, uint8_t cmdId, const uint8_t* cmdData, uint8_t dataLen) {
    frame[0] = FRAME_HEADER_TX;
    frame[1] = cmdId;
    
    if (cmdData && dataLen > 0) {
        memcpy(&frame[2], cmdData, dataLen);
        if (dataLen < FRAME_DATA_LEN) {
            memset(&frame[2 + dataLen], 0, FRAME_DATA_LEN - dataLen);
        }
    } else {
        memset(&frame[2], 0, FRAME_DATA_LEN);
    }
    
    frame[10] = ComFrame_CalcChecksum(cmdId, &frame[2]);
    frame[11] = FRAME_TAIL_TX;
    
    gComFrame.lastTxCmdId = cmdId;
    memcpy(gComFrame.lastTxData, &frame[2], FRAME_DATA_LEN);
}

static bool ComFrame_ValidateRx(const uint8_t* frame) {
    if (frame[0] != FRAME_HEADER_RX || frame[11] != FRAME_TAIL_RX) {
        return false;
    }
    
    uint8_t calcSum = ComFrame_CalcChecksum(frame[1], &frame[2]);
    if (calcSum != frame[10]) {
        return false;
    }
    
    if (frame[1] != gComFrame.lastTxCmdId) {
        return false;
    }
    
    if (memcmp(&frame[2], gComFrame.lastTxData, FRAME_DATA_LEN) != 0) {
        return false;
    }
    
    return true;
}

// =============================================
// Driver Callback Functions
// =============================================
static bool ComFrame_TxReqCb(uint8_t* data, uint16_t* length) {
    if (gComFrame.txPending) {
        memcpy(data, gComFrame.txFrame, FRAME_TOTAL_LEN);
        *length = FRAME_TOTAL_LEN;
        gComFrame.txPending = false;
        return true;
    }
    
    *length = 0;
    return false;
}

static void ComFrame_TxCompleteCb(void) {
    if (gComFrame.txCb) {
        gComFrame.txCb(true);
        gComFrame.txCb = NULL;
    }
}

static void ComFrame_RxByteCb(uint8_t byte) {
    if (!gComFrame.rxSynced) {
        if (byte == FRAME_HEADER_RX) {
            gComFrame.rxBuf[0] = byte;
            gComFrame.rxIdx = 1;
            gComFrame.rxSynced = true;
        }
        return;
    }
    
    if (gComFrame.rxIdx < sizeof(gComFrame.rxBuf)) {
        gComFrame.rxBuf[gComFrame.rxIdx++] = byte;
    }
    
    if (gComFrame.rxIdx >= FRAME_TOTAL_LEN) {
        if (ComFrame_ValidateRx(gComFrame.rxBuf)) {
            if (gComFrame.rxCb) {
                gComFrame.rxCb(gComFrame.rxBuf);
            }
        }
        
        gComFrame.rxSynced = false;
        gComFrame.rxIdx = 0;
    }
}

// =============================================
// Public Interface Implementation
// =============================================
static bool ComFrame_Init(void) {
    const DrvUart* uart = DrvUart_GetInstance();
    
    uart->setRxCb(ComFrame_RxByteCb);
    uart->setTxCompleteCb(ComFrame_TxCompleteCb);
    uart->setTxReqCb(ComFrame_TxReqCb);
    
    gComFrame.rxIdx = 0;
    gComFrame.rxSynced = false;
    gComFrame.rxCb = NULL;
    gComFrame.txCb = NULL;
    gComFrame.txPending = false;
    gComFrame.lastTxCmdId = 0;
    memset(gComFrame.lastTxData, 0, sizeof(gComFrame.lastTxData));
    
    return true;
}

static bool ComFrame_SendCmd(uint8_t cmdId, const uint8_t* cmdData, uint8_t dataLen, ComFrameTxCb callback) {
    const DrvUart* uart = DrvUart_GetInstance();
    
    if (uart->isTxBusy() || !cmdData || dataLen > FRAME_DATA_LEN) {
        if (callback) callback(false);
        return false;
    }
    
    ComFrame_BuildCmd(gComFrame.txFrame, cmdId, cmdData, dataLen);
    
    gComFrame.txCb = callback;
    gComFrame.txPending = true;
    
    if (uart->startTx()) {
        return true;
    } else {
        gComFrame.txPending = false;
        if (callback) callback(false);
        return false;
    }
}

static void ComFrame_SetRxCb(ComFrameRxCb callback) {
    gComFrame.rxCb = callback;
}

static bool ComFrame_IsTxBusy(void) {
    const DrvUart* uart = DrvUart_GetInstance();
    return uart->isTxBusy() || gComFrame.txPending;
}

// =============================================
// Communication Instance and Public Interface
// =============================================
static const ComFrame gComFrameInstance = {
    .init = ComFrame_Init,
    .sendCmd = ComFrame_SendCmd,
    .setRxCb = ComFrame_SetRxCb,
    .isTxBusy = ComFrame_IsTxBusy
};

const ComFrame* ComFrame_GetInstance(void) {
    return &gComFrameInstance;
}