#ifndef __YUERIN_RINGBUFFER_H
#define __YUERIN_RINGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    uint8_t *buffer;
    uint32_t size;
    uint32_t len;
    uint32_t in;
    uint32_t out;
} RingBuffer;

RingBuffer *RingBufferInit(uint32_t size);
void RingBufferDestory(RingBuffer *fifo);
uint32_t RingBufferWrite(RingBuffer *fifo, const void *in, uint32_t len);
uint32_t RingBufferRead(RingBuffer *fifo, void *out, uint32_t len);
uint32_t RingBufferPreRead(RingBuffer *fifo, void *out, uint32_t len);
void RingBufferMatchPreRead(RingBuffer *fifo, uint32_t len);


/**
 * @brief  Removes the entire FIFO contents.
 * @param  [in] fifo: The fifo to be emptied.
 * @return None.
 */
static inline void RingBufferReset(RingBuffer *fifo) {
    fifo->in = fifo->out = 0;
}

/**
 * @brief  Returns the size of the FIFO in bytes.
 * @param  [in] fifo: The fifo to be used.
 * @return The size of the FIFO.
 */
static inline uint32_t RingBufferSize(RingBuffer *fifo) {
    return fifo->size;
}

/**
 * @brief  Returns the number of used bytes in the FIFO.
 * @param  [in] fifo: The fifo to be used.
 * @return The number of used bytes.
 */
static inline uint32_t RingBufferLen(RingBuffer *fifo) {
    return fifo->len;//fifo->in - fifo->out;
}

/**
 * @brief  Returns the number of bytes available in the FIFO.
 * @param  [in] fifo: The fifo to be used.
 * @return The number of bytes available.
 */
static inline uint32_t RingBufferAvail(RingBuffer *fifo) {
    return RingBufferSize(fifo) - RingBufferLen(fifo);
}

/**
 * @brief  Is the FIFO empty?
 * @param  [in] fifo: The fifo to be used.
 * @retval true:      Yes.
 * @retval false:     No.
 */
static inline bool RingBufferIsEmpty(RingBuffer *fifo) {
    return RingBufferLen(fifo) == 0;
}

/**
 * @brief  Is the FIFO full?
 * @param  [in] fifo: The fifo to be used.
 * @retval true:      Yes.
 * @retval false:     No.
 */
static inline bool RingBufferIsFull(RingBuffer *fifo) {
    return RingBufferAvail(fifo) == 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __YUERIN_RINGBUFFER_H */
