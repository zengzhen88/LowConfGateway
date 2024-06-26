#include "RingBuffer.h"
#include <string.h>

#define min(a, b)  (((a) < (b)) ? (a) : (b))

/**
 * @brief  Determine whether some value is a power of two.
 * @param  [in] x: The number to be confirmed.
 * @retval true:   Yes.
 * @retval false:  No.
 * @note   Where zero is not considered a power of two.
 */
static bool is_power_of_2(uint32_t x) {
    return (x != 0) && ((x & (x - 1)) == 0);
}

/**
 * @brief  Round the given value up to nearest power of two.
 * @param  [in] x: The number to be converted.
 * @return The power of two.
 */
static uint32_t roundup_pow_of_two(uint32_t x) {
    uint32_t b = 0;

    for(int i = 0; i < 32; i++) {
        b = 1UL << i;

        if(x <= b) {
            break;
        }
    }

    return b;
}

/**
 * @brief  Allocates a new FIFO and its internal buffer.
 * @param  [in] size: The size of the internal buffer to be allocated.
 * @note   The size will be rounded-up to a power of 2.
 * @return RingBuffer pointer.
 */
RingBuffer *RingBufferInit(uint32_t size) {
    RingBuffer *fifo = (RingBuffer *)malloc(sizeof(RingBuffer));
    if(fifo != NULL) {
        if(is_power_of_2(size) != true) {
            if(size > 0x80000000UL) {
                free(fifo);
                return NULL;
            }

            size = roundup_pow_of_two(size);
        }

        fifo->buffer = (uint8_t *)malloc(size);
        if(fifo->buffer == NULL) {
            free(fifo);
            return NULL;
        }

        fifo->size  = size;
        fifo->in    = fifo->out = 0;
        fifo->len   = 0;
    }

    return fifo;
}

/**
 * @brief  Frees the FIFO.
 * @param  [in] fifo: The fifo to be freed.
 * @return None.
 */
void RingBufferDestory(RingBuffer *fifo)
{
    free(fifo->buffer);
    free(fifo);
}

/**
 * @brief  Puts some data into the FIFO.
 * @param  [in] fifo: The fifo to be used.
 * @param  [in] in:   The data to be added.
 * @param  [in] len:  The length of the data to be added.
 * @return The number of bytes copied.
 * @note   This function copies at most @len bytes from the @in into
 *         the FIFO depending on the free space, and returns the number
 *         of bytes copied.
 */
uint32_t RingBufferWrite(RingBuffer *fifo, const void *in, uint32_t len)
{
    len = min(len, RingBufferAvail(fifo));

    /* First put the data starting from fifo->in to buffer end. */
    uint32_t l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), in, l);

    /* Then put the rest (if any) at the beginning of the buffer. */
    memcpy(fifo->buffer, (uint8_t *)in + l, len - l);

    fifo->in    += len;
    fifo->len   += len;

    return len;
}

/**
 * @brief  Gets some data from the FIFO.
 * @param  [in] fifo: The fifo to be used.
 * @param  [in] out:  Where the data must be copied.
 * @param  [in] len:  The size of the destination buffer.
 * @return The number of copied bytes.
 * @note   This function copies at most @len bytes from the FIFO into
 *         the @out and returns the number of copied bytes.
 */
uint32_t RingBufferRead(RingBuffer *fifo, void *out, uint32_t len)
{
    len = min(len, RingBufferLen(fifo));

    /* First get the data from fifo->out until the end of the buffer. */
    uint32_t l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(out, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

    /* Then get the rest (if any) from the beginning of the buffer. */
    memcpy((uint8_t *)out + l, fifo->buffer, len - l);

    fifo->out += len;
    fifo->len -= len;

    return len;
}

uint32_t RingBufferPreRead(RingBuffer *fifo, void *out, uint32_t len)
{
    len = min(len, RingBufferLen(fifo));

    /* First get the data from fifo->out until the end of the buffer. */
    uint32_t l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(out, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

    /* Then get the rest (if any) from the beginning of the buffer. */
    memcpy((uint8_t *)out + l, fifo->buffer, len - l);

    // fifo->out += len;

    return len;
}

void RingBufferMatchPreRead(RingBuffer *fifo, uint32_t len) {
    len = min(len, RingBufferLen(fifo));

    fifo->out += len;
    fifo->len -= len;
}
