/**
 * @file drv_uart.c
 * @brief UART Driver Layer Implementation for STM32F429
 */

#include "drv_uart.h"
#include <string.h>

// =============================================
// Internal State Definition
// =============================================
static struct {
    DrvUartRxCb rxCb;
    DrvUartTxCompleteCb txCompleteCb;
    DrvUartTxReqCb txReqCb;
    
    uint8_t txBuf[12];
    uint16_t txLen;
    uint16_t txIdx;
    bool txBusy;
    
    UART_HandleTypeDef* huart;
    uint8_t rxByte;  // Single byte reception buffer
} gDrvUart;

// =============================================
// Callback Setup Functions
// =============================================
static void DrvUart_SetRxCb(DrvUartRxCb callback) {
    gDrvUart.rxCb = callback;
}

static void DrvUart_SetTxCompleteCb(DrvUartTxCompleteCb callback) {
    gDrvUart.txCompleteCb = callback;
}

static void DrvUart_SetTxReqCb(DrvUartTxReqCb callback) {
    gDrvUart.txReqCb = callback;
}

// =============================================
// Transmission Control Functions
// =============================================
static bool DrvUart_StartTx(void) {
    if (gDrvUart.txBusy) return false;
    if (!gDrvUart.txReqCb) return false;
    
    gDrvUart.txIdx = 0;
    gDrvUart.txLen = 0;
    
    if (gDrvUart.txReqCb(gDrvUart.txBuf, &gDrvUart.txLen)) {
        if (gDrvUart.txLen > 0) {
            gDrvUart.txBusy = true;
            // Start interrupt-based transmission
            HAL_UART_Transmit_IT(gDrvUart.huart, gDrvUart.txBuf, gDrvUart.txLen);
            return true;
        }
    }
    
    return false;
}

static bool DrvUart_IsTxBusy(void) {
    return gDrvUart.txBusy;
}

// =============================================
// Initialization Function
// =============================================
static void DrvUart_Init(UART_HandleTypeDef* huart) {
    gDrvUart.rxCb = NULL;
    gDrvUart.txCompleteCb = NULL;
    gDrvUart.txReqCb = NULL;
    gDrvUart.txLen = 0;
    gDrvUart.txIdx = 0;
    gDrvUart.txBusy = false;
    gDrvUart.huart = huart;
    
    // Start receiving in interrupt mode
    HAL_UART_Receive_IT(huart, &gDrvUart.rxByte, 1);
}

// =============================================
// UART Callback Functions (Called from HAL)
// =============================================
void DrvUart_TxCompleteCallback(UART_HandleTypeDef* huart) {
    if (huart == gDrvUart.huart) {
        gDrvUart.txBusy = false;
        if (gDrvUart.txCompleteCb) {
            gDrvUart.txCompleteCb();
        }
    }
}

void DrvUart_RxCompleteCallback(UART_HandleTypeDef* huart) {
    if (huart == gDrvUart.huart && gDrvUart.rxCb) {
        gDrvUart.rxCb(gDrvUart.rxByte);
        // Restart reception for next byte
        HAL_UART_Receive_IT(huart, &gDrvUart.rxByte, 1);
    }
}

// =============================================
// Driver Instance and Public Interface
// =============================================
static const DrvUart gDrvUartInstance = {
    .setRxCb = DrvUart_SetRxCb,
    .setTxCompleteCb = DrvUart_SetTxCompleteCb,
    .setTxReqCb = DrvUart_SetTxReqCb,
    .startTx = DrvUart_StartTx,
    .isTxBusy = DrvUart_IsTxBusy,
    .init = DrvUart_Init
};

const DrvUart* DrvUart_GetInstance(void) {
    return &gDrvUartInstance;
}
