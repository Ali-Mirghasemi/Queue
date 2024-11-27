/**
 * @file Queue.h
 * @author Ali Mirghasemi (ali.mirghasemi1376@gmail.com)
 * @brief this library implement queue buffer with read & write operations, it's based n Stream library
 * @version 0.2
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

#include "StreamBuffer.h"

#define QUEUE_VER_MAJOR    0
#define QUEUE_VER_MINOR    2
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
typedef Stream_LenType Queue_LenType;

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
#define QUEUE_NO_LIMIT                          STREAM_NO_LIMIT

/**
 * @brief result of Queue functions
 */
typedef enum {
    Queue_Ok               = Stream_Ok,                 /**< everything is OK */
    Queue_NoSpace          = Stream_NoSpace,            /**< there is no space for write operation */
    Queue_NoAvailable      = Stream_NoAvailable,        /**< there no available bytes for read operation */
    Queue_BufferFull       = Stream_BufferFull,         /**< buffer full*/
    Queue_NoReceiveFn      = Stream_NoReceiveFn,        /**< no receive function set for IStream */
    Queue_NoTransmitFn     = Stream_NoTransmitFn,       /**< no transmit function set for OStream */
    Queue_NoReceive        = Stream_NoReceive,          /**< stream is not in receive mode */
    Queue_NoTransmit       = Stream_NoTransmit,         /**< stream is not in transmit mode */
    Queue_InReceive        = Stream_InReceive,          /**< stream is in receive mode */
    Queue_InTransmit       = Stream_InTransmit,         /**< stream is in transmit mode */
    Queue_ZeroLen          = Stream_ZeroLen,            /**< len parameter is zero */
    Queue_ReceiveFailed    = Stream_ReceiveFailed,      /**< failed in receive */
    Queue_TransmitFailed   = Stream_TransmitFailed,     /**< failed in transmit */
    Queue_CustomError      = Stream_CustomError,        /**< can be used for custom errors */
} Queue_Result;
/**
 * @brief Queue struct
 * contains everything need for handle queue
 */
typedef struct {
    StreamBuffer            Buffer;                     /**< Queue buffer */
    Queue_LenType           ItemLength;                 /**< length of each item */
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

// -------------------------- General APIs ----------------------------
void                Queue_init(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemLength);
void                Queue_fromBuff(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemLength, Queue_LenType len);
void                Queue_deinit(Queue* queue);

#define             Queue_space(QUEUE)                                      (Queue_spaceRaw(QUEUE) / ((QUEUE)->ItemLength))
#define             Queue_available(QUEUE)                                  (Queue_availableRaw(QUEUE) / ((QUEUE)->ItemLength))

#define             Queue_isEmpty(QUEUE)                                    Stream_isEmpty(&((QUEUE)->Buffer))
#define             Queue_isFull(QUEUE)                                     Stream_isFull(&((QUEUE)->Buffer))

#define             Queue_directAvailable(QUEUE)                            (Queue_directAvailableRaw(QUEUE) / ((QUEUE)->ItemLength))
#define             Queue_directSpace(QUEUE)                                (Queue_directSpaceRaw(QUEUE) / ((QUEUE)->ItemLength))

#define             Queue_directAvailableAt(QUEUE, IDX)                     (Queue_directAvailableAtRaw(QUEUE, IDX) / ((QUEUE)->ItemLength))
#define             Queue_directSpaceAt(QUEUE, IDX)                         (Queue_directSpaceAtRaw(QUEUE, IDX) / ((QUEUE)->ItemLength))

#define             Queue_getWritePtr(QUEUE)                                ((void*) Stream_getWritePtr(&((QUEUE)->Buffer)))
#define             Queue_getReadPtr(QUEUE)                                 ((void*) Stream_getReadPtr(&((QUEUE)->Buffer)))
#define             Queue_getWritePtrAt(QUEUE, IDX)                         ((void*) Stream_getWritePtrAt(&((QUEUE)->Buffer), (IDX) * ((QUEUE)->ItemLength)))
#define             Queue_getReadPtrAt(QUEUE, IDX)                          ((void*) Stream_getReadPtrAt(&((QUEUE)->Buffer), (IDX) * ((QUEUE)->ItemLength)))

#define             Queue_reset(QUEUE)                                      Stream_reset(&((QUEUE)->Buffer))
#define             Queue_clear(QUEUE)                                      Stream_clear(&((QUEUE)->Buffer))

#define             Queue_setBuffer(QUEUE, BUF)                             Stream_getBuffer(&((QUEUE)->Buffer), (BUF))
#define             Queue_getBuffer(QUEUE)                                  ((void*) Stream_getBuffer(&((QUEUE)->Buffer)))
#define             Queue_getBufferSize(QUEUE)                              (Queue_getBufferSizeRaw((QUEUE)) / ((QUEUE)->ItemLength))

#define             Queue_getWritePos(QUEUE)                                (Queue_getWritePosRaw(QUEUE) / ((QUEUE)->ItemLength))
#define             Queue_getReadPos(QUEUE)                                 (Queue_getReadPosRaw(QUEUE) / ((QUEUE)->ItemLength))

#define             Queue_moveWritePos(QUEUE, STEPS)                        Queue_moveReadPosRaw(QUEUE, STEPS * ((QUEUE)->ItemLength))
#define             Queue_moveReadPos(QUEUE, STEPS)                         Queue_moveWritePosRaw(QUEUE, STEPS * ((QUEUE)->ItemLength))

#define             Queue_flipWrite(QUEUE, LEN)                             Queue_flipWriteRaw(QUEUE, (LEN) * ((QUEUE)->ItemLength))
#define             Queue_flipRead(QUEUE, LEN)                              Queue_flipReadRaw(QUEUE, (LEN) * ((QUEUE)->ItemLength))


// -------------------------- Raw APIs ----------------------------
#define             Queue_spaceRaw(QUEUE)                                   Stream_space(&((QUEUE)->Buffer))
#define             Queue_availableRaw(QUEUE)                               Stream_available(&((QUEUE)->Buffer))

#define             Queue_directAvailableRaw(QUEUE)                         Stream_directAvailable(&((QUEUE)->Buffer))
#define             Queue_directSpaceRaw(QUEUE)                             Stream_directSpace(&((QUEUE)->Buffer))

#define             Queue_directAvailableAtRaw(QUEUE, IDX)                  Stream_directAvailableAt(&((QUEUE)->Buffer), (IDX))
#define             Queue_directSpaceAtRaw(QUEUE, IDX)                      Stream_directSpaceAt(&((QUEUE)->Buffer), (IDX))

#define             Queue_getWritePosRaw(QUEUE)                             Stream_getWritePos(&((QUEUE)->Buffer))
#define             Queue_getReadPosRaw(QUEUE)                              Stream_getReadPos(&((QUEUE)->Buffer))

#define             Queue_moveWritePosRaw(QUEUE, STEPS)                     Stream_moveWritePos(&((QUEUE)->Buffer), (STEPS))
#define             Queue_moveReadPosRaw(QUEUE, STEPS)                      Stream_moveReadPos(&((QUEUE)->Buffer), (STEPS))

#define             Queue_flipWriteRaw(QUEUE, LEN)                          Stream_flipWrite(&((QUEUE)->Buffer), (LEN))
#define             Queue_flipReadRaw(QUEUE, LEN)                           Stream_flipRead(&((QUEUE)->Buffer), (LEN))

#define             Queue_getBufferSizeRaw(QUEUE)                           Stream_getBufferSize(&((QUEUE)->Buffer))

// -------------------------- Write Limit APIs ----------------------------
#if QUEUE_WRITE_LIMIT
    #define         Queue_spaceLimitRaw(QUEUE)                              Stream_spaceLimit(&((QUEUE)->Buffer))
    #define         Queue_getWriteLimitRaw(QUEUE)                           Stream_getWriteLimit(&((QUEUE)->Buffer))
    #define         Queue_setWriteLimitRaw(QUEUE, LEN)                      Stream_setWriteLimit(&((QUEUE)->Buffer), (LEN))
    
    #define         Queue_spaceLimit(QUEUE)                                 (Queue_spaceLimitRaw(QUEUE) / ((QUEUE)->ItemLength))
    #define         Queue_getWriteLimit(QUEUE)                              (Queue_getWriteLimitRaw(QUEUE) / ((QUEUE)->ItemLength))
    #define         Queue_setWriteLimit(QUEUE, LEN)                         Queue_setWriteLimitRaw(QUEUE, (LEN) * ((QUEUE)->ItemLength))
    #define         Queue_isWriteLimited(QUEUE)                             Stream_isWriteLimited(&((QUEUE)->Buffer))
#endif // QUEUE_WRITE_LIMIT

// -------------------------- Read Limit APIs ----------------------------
#if QUEUE_READ_LIMIT
    #define         Queue_availableLimitRaw(QUEUE)                          Stream_availableLimit(&((QUEUE)->Buffer))
    #define         Queue_getReadLimitRaw(QUEUE)                            Stream_getReadLimit(&((QUEUE)->Buffer))
    #define         Queue_setReadLimitRaw(QUEUE, LEN)                       Stream_setReadLimit(&((QUEUE)->Buffer), (LEN))
    
    #define         Queue_availableLimit(QUEUE)                             (Queue_availableLimitRaw(QUEUE) / ((QUEUE)->ItemLength))
    #define         Queue_getReadLimit(QUEUE)                               (Queue_getReadLimitRaw(QUEUE) / ((QUEUE)->ItemLength))
    #define         Queue_setReadLimit(QUEUE, LEN)                          Queue_setReadLimitRaw(QUEUE, (LEN) * ((QUEUE)->ItemLength))
    #define         Queue_isReadLimited(QUEUE)                              Stream_isReadLimited(&((QUEUE)->Buffer))
#endif // QUEUE_READ_LIMIT

// -------------------------- Write Lock APIs ----------------------------
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
