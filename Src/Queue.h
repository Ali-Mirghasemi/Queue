/**
 * @file Queue.h
 * @author Ali Mirghasemi (ali.mirghasemi1376@gmail.com)
 * @brief this library implement queue buffer with read & write operations
 * @version 0.5
 * @date 2021-09-01
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef _QUEUE_H_
#define _QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define QUEUE_VER_MAJOR    0
#define QUEUE_VER_MINOR    1
#define QUEUE_VER_FIX      0

/************************************************************************/
/*                            Configuration                             */
/************************************************************************/

/**
 * @brief enable set limit for write functions
 */
#define QUEUE_WRITE_LIMIT                  0
/**
 * @brief enable set limit for read operations
 */
#define QUEUE_READ_LIMIT                   0
/**
 * @brief enable write lock feature
 */
#define QUEUE_WRITE_LOCK                   0
/**
 * @brief enable read lock feature
 */
#define QUEUE_READ_LOCK                    0
/**
 * @brief enable get functions
 */
#define QUEUE_GET_FUNCTIONS                1
/**
 * @brief enable getAt functions
 */
#define QUEUE_GET_AT_FUNCTIONS             1
/**
 * @brief check len parameter in read/write functions
 */
#define QUEUE_CHECK_ZERO_LEN               1
/**
 * @brief based on maximum size of buffer that you use for queue
 * you can change type of len variables
 * must be signed type
 */
typedef int16_t Queue_LenType;

/************************************************************************/

#define __QUEUE_VER_STR(major, minor, fix)     #major "." #minor "." #fix
#define _QUEUE_VER_STR(major, minor, fix)      __QUEUE_VER_STR(major, minor, fix)
/**
 * @brief show queue version in string format
 */
#define QUEUE_VER_STR                          _QUEUE_VER_STR(QUEUE_VER_MAJOR, QUEUE_VER_MINOR, QUEUE_VER_FIX)
/**
 * @brief show queue version in integer format, ex: 0.2.0 -> 200
 */
#define QUEUE_VER                              ((QUEUE_VER_MAJOR * 10000UL) + (QUEUE_VER_MINOR * 100UL) + (QUEUE_VER_FIX))

/**
 * @brief use for disable limit
 */
#define QUEUE_NO_LIMIT                     -1

/**
 * @brief result of Queue functions
 */
typedef enum {
    Queue_Ok                = 0,    /**< everything is OK */
    Queue_NoSpace           = 1,    /**< there is no space for write operation */
    Queue_NoAvailable       = 2,    /**< there no available bytes for read operation */
    Queue_BufferFull        = 3,    /**< buffer full*/
    Queue_ZeroLen           = 4,   /**< len parameter is zero */
} Queue_Result;
/**
 * @brief Queue struct
 * contains everything need for handle queue
 */
typedef struct {
    uint8_t*                Buf;                    /**< pointer to buffer */
    Queue_LenType           Size;                   /**< size of buffer */
    Queue_LenType           WPos;                   /**< write position */
    Queue_LenType           RPos;                   /**< read position */
    Queue_LenType           ItemSize;               /**< length of each item */
#if QUEUE_WRITE_LIMIT
    Queue_LenType           WriteLimit;             /**< limit for write operation */
#endif 
#if QUEUE_READ_LIMIT
    Queue_LenType           ReadLimit;              /**< limit for read operation */
#endif
    uint8_t                 Overflow    : 1;        /**< overflow flag */
    uint8_t                 Order       : 1;        /**< byte order */
    uint8_t                 OrderFn     : 1;        /**< byte order function */
    uint8_t                 WriteLocked : 1;        /**< queue write locked */
    uint8_t                 ReadLocked  : 1;        /**< queue write locked */
    uint8_t                 Reserved    : 3;        /**< reserved */
} Queue;
/**
 * @brief Write or Read queue with custom functions as query
 * @param queue pointer to queue
 * @param val pointer to data
 * @param index index of data
 * @param len length of data
 * @return Queue_Result
 */
typedef Queue_Result (*Queue_QueryFn)(Queue* queue, void* val, Queue_LenType index, Queue_LenType len);

void Queue_init(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemLength);
void Queue_fromBuff(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemLength, Queue_LenType len);
void Queue_deinit(Queue* queue);

/*************** General APIs *************/

#if QUEUE_WRITE_LIMIT
    #define Queue_spaceRaw(QUEUE)                Queue_spaceLimitRaw((QUEUE))
#else
    #define Queue_spaceRaw(QUEUE)                Queue_spaceRealRaw((QUEUE))
#endif // QUEUE_WRITE_LIMIT

#if QUEUE_READ_LIMIT
    #define Queue_availableRaw(QUEUE)            Queue_availableLimitRaw((QUEUE))
#else
    #define Queue_availableRaw(QUEUE)            Queue_availableRealRaw((QUEUE))
#endif // QUEUE_READ_LIMIT

Queue_LenType Queue_availableRealRaw(Queue* queue);
Queue_LenType Queue_spaceRealRaw(Queue* queue);
Queue_LenType Queue_available(Queue* queue);
Queue_LenType Queue_space(Queue* queue);
uint8_t Queue_isEmpty(Queue* queue);
uint8_t Queue_isFull(Queue* queue);

Queue_LenType Queue_directAvailableRaw(Queue* queue);
Queue_LenType Queue_directSpaceRaw(Queue* queue);

Queue_LenType Queue_directAvailableAtRaw(Queue* queue, Queue_LenType index);
Queue_LenType Queue_directSpaceAtRaw(Queue* queue, Queue_LenType index);

void* Queue_getWritePtr(Queue* queue);
void* Queue_getReadPtr(Queue* queue);
void* Queue_getWritePtrAt(Queue* queue, Queue_LenType index);
void* Queue_getReadPtrAt(Queue* queue, Queue_LenType index);

void Queue_reset(Queue* queue);
void Queue_clear(Queue* queue);

uint8_t* Queue_getBuffer(Queue* queue);

void Queue_setBuffer(Queue* queue, void* data, Queue_LenType size, Queue_LenType itemSize);
Queue_LenType Queue_getBufferSize(Queue* queue);

Queue_LenType Queue_getWritePos(Queue* queue);
Queue_LenType Queue_getReadPos(Queue* queue);

Queue_Result Queue_moveWritePosRaw(Queue* queue, Queue_LenType steps);
Queue_Result Queue_moveReadPosRaw(Queue* queue, Queue_LenType steps);

void Queue_flipWrite(Queue* queue, Queue_LenType len);
void Queue_flipRead(Queue* queue, Queue_LenType len);

#if QUEUE_WRITE_LIMIT
    void       Queue_setWriteLimit(Queue* queue, Queue_LenType len);
    uint8_t    Queue_isWriteLimited(Queue* queue);
    Queue_LenType Queue_spaceLimitRaw(Queue* queue);
    Queue_LenType Queue_getWriteLimitRaw(Queue* queue);
    Queue_LenType Queue_spaceLimit(Queue* queue);
    Queue_LenType Queue_getWriteLimit(Queue* queue);
#endif // QUEUE_WRITE_LIMIT

#if QUEUE_READ_LIMIT
    void       Queue_setReadLimit(Queue* queue, Queue_LenType len);
    uint8_t    Queue_isReadLimited(Queue* queue);
    Queue_LenType Queue_availableLimitRaw(Queue* queue);
    Queue_LenType Queue_getReadLimitRaw(Queue* queue);
    Queue_LenType Queue_availableLimit(Queue* queue);
    Queue_LenType Queue_getReadLimit(Queue* queue);
#endif // QUEUE_READ_LIMIT

#if QUEUE_WRITE_LOCK
    Queue_Result    Queue_lockWrite(Queue* queue, Queue* lock, Queue_LenType len);
    void            Queue_unlockWrite(Queue* queue, Queue* lock);
    void            Queue_unlockWriteIgnore(Queue* queue);
    Queue_LenType   Queue_lockWriteLenRaw(Queue* queue, Queue* lock);
    Queue_LenType   Queue_lockWriteLen(Queue* queue, Queue* lock);
#endif // QUEUE_WRITE_LOCK

#if QUEUE_READ_LOCK
    Queue_Result    Queue_lockRead(Queue* queue, Queue* lock, Queue_LenType len);
    void            Queue_unlockRead(Queue* queue, Queue* lock);
    void            Queue_unlockReadIgnore(Queue* queue);
    Queue_LenType   Queue_lockReadLenRaw(Queue* queue, Queue* lock);
    Queue_LenType   Queue_lockReadLen(Queue* queue, Queue* lock);
#endif // QUEUE_READ_LOCK

/**************** Write APIs **************/
Queue_Result Queue_write(Queue* queue, void* val);
Queue_Result Queue_writeArray(Queue* queue, void* val, Queue_LenType len);
Queue_Result Queue_writeQueue(Queue* out, Queue* in, Queue_LenType len);
Queue_Result Queue_writeQuery(Queue* queue, Queue_QueryFn query);
Queue_Result Queue_writeQueryArray(Queue* queue, Queue_LenType len, Queue_QueryFn query);

/**************** Read APIs **************/
Queue_Result Queue_read(Queue* queue, void* val);
Queue_Result Queue_readArray(Queue* queue, void* val, Queue_LenType len);
Queue_Result Queue_readQueue(Queue* in, Queue* out, Queue_LenType len);
Queue_Result Queue_readQuery(Queue* queue, Queue_QueryFn query);
Queue_Result Queue_readQueryArray(Queue* queue, Queue_LenType len, Queue_QueryFn query);

#if QUEUE_GET_AT_FUNCTIONS && QUEUE_GET_FUNCTIONS
    Queue_Result Queue_get(Queue* queue, void* val);
    Queue_Result Queue_getArray(Queue* queue, void* val, Queue_LenType len);
#endif // QUEUE_GET_FUNCTIONS

#if QUEUE_GET_AT_FUNCTIONS
    Queue_Result Queue_getAt(Queue* queue, Queue_LenType index, void* val);
    Queue_Result Queue_getArrayAt(Queue* queue, Queue_LenType index, void* val, Queue_LenType len);
#endif // QUEUE_GET_AT_FUNCTIONS

#ifdef __cplusplus
};
#endif

#endif /* _QUEUE_H_ */
