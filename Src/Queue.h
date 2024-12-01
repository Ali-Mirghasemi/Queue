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
 * @brief enable queue library
 */
#define QUEUE                           (1 && STREAM_WRITE && STREAM_READ)
/**
 * @brief Enable queue write operations
 */
#define QUEUE_WRITE                     (1 && STREAM_WRITE)
/**
 * @brief Enable queue write query
 */
#define QUEUE_WRITE_QUERY               (1 && QUEUE_WRITE)
/**
 * @brief Enable queue read operations
 */
#define QUEUE_READ                      (1 && STREAM_READ)
/**
 * @brief Enable queue read query
 */
#define QUEUE_READ_QUERY                (1 && QUEUE_READ)
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

#define Queue_Ok                                Stream_Ok,                 /**< everything is OK */
#define Queue_NoSpace                           Stream_NoSpace,            /**< there is no space for write operation */
#define Queue_NoAvailable                       Stream_NoAvailable,        /**< there no available bytes for read operation */
#define Queue_BufferFull                        Stream_BufferFull,         /**< buffer full*/
#define Queue_NoReceiveFn                       Stream_NoReceiveFn,        /**< no receive function set for IStream */
#define Queue_NoTransmitFn                      Stream_NoTransmitFn,       /**< no transmit function set for OStream */
#define Queue_NoReceive                         Stream_NoReceive,          /**< stream is not in receive mode */
#define Queue_NoTransmit                        Stream_NoTransmit,         /**< stream is not in transmit mode */
#define Queue_InReceive                         Stream_InReceive,          /**< stream is in receive mode */
#define Queue_InTransmit                        Stream_InTransmit,         /**< stream is in transmit mode */
#define Queue_ZeroLen                           Stream_ZeroLen,            /**< len parameter is zero */
#define Queue_ReceiveFailed                     Stream_ReceiveFailed,      /**< failed in receive */
#define Queue_TransmitFailed                    Stream_TransmitFailed,     /**< failed in transmit */
#define Queue_CustomError                       Stream_CustomError,        /**< can be used for custom errors */

typedef Stream_Result Queue_Result;
/**
 * @brief Queue struct
 * contains everything need for handle queue
 */
typedef struct {
    StreamBuffer            Buffer;                     /**< Queue buffer */
    Queue_LenType           ItemSize;                 /**< length of each item */
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
void                Queue_init(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemSize);
void                Queue_fromBuff(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemSize, Queue_LenType len);
#define             Queue_deinit(QUEUE)                                     memset((QUEUE), 0, sizeof(Queue))

#define             Queue_space(QUEUE)                                      (Queue_spaceRaw(QUEUE) / ((QUEUE)->ItemSize))
#define             Queue_available(QUEUE)                                  (Queue_availableRaw(QUEUE) / ((QUEUE)->ItemSize))

#define             Queue_isEmpty(QUEUE)                                    Stream_isEmpty(&((QUEUE)->Buffer))
#define             Queue_isFull(QUEUE)                                     Stream_isFull(&((QUEUE)->Buffer))

#define             Queue_directAvailable(QUEUE)                            (Queue_directAvailableRaw(QUEUE) / ((QUEUE)->ItemSize))
#define             Queue_directSpace(QUEUE)                                (Queue_directSpaceRaw(QUEUE) / ((QUEUE)->ItemSize))

#define             Queue_directAvailableAt(QUEUE, IDX)                     (Queue_directAvailableAtRaw(QUEUE, IDX) / ((QUEUE)->ItemSize))
#define             Queue_directSpaceAt(QUEUE, IDX)                         (Queue_directSpaceAtRaw(QUEUE, IDX) / ((QUEUE)->ItemSize))

#define             Queue_getWritePtr(QUEUE)                                ((void*) Stream_getWritePtr(&((QUEUE)->Buffer)))
#define             Queue_getReadPtr(QUEUE)                                 ((void*) Stream_getReadPtr(&((QUEUE)->Buffer)))
#define             Queue_getWritePtrAt(QUEUE, IDX)                         ((void*) Stream_getWritePtrAt(&((QUEUE)->Buffer), (IDX) * ((QUEUE)->ItemSize)))
#define             Queue_getReadPtrAt(QUEUE, IDX)                          ((void*) Stream_getReadPtrAt(&((QUEUE)->Buffer), (IDX) * ((QUEUE)->ItemSize)))

#define             Queue_reset(QUEUE)                                      Stream_reset(&((QUEUE)->Buffer))
#define             Queue_clear(QUEUE)                                      Stream_clear(&((QUEUE)->Buffer))

#define             Queue_setBuffer(QUEUE, BUF)                             Stream_getBuffer(&((QUEUE)->Buffer), (BUF))
#define             Queue_getBuffer(QUEUE)                                  ((void*) Stream_getBuffer(&((QUEUE)->Buffer)))
#define             Queue_getBufferSize(QUEUE)                              (Queue_getBufferSizeRaw((QUEUE)) / ((QUEUE)->ItemSize))

#define             Queue_getWritePos(QUEUE)                                (Queue_getWritePosRaw(QUEUE) / ((QUEUE)->ItemSize))
#define             Queue_getReadPos(QUEUE)                                 (Queue_getReadPosRaw(QUEUE) / ((QUEUE)->ItemSize))

#define             Queue_moveWritePos(QUEUE, STEPS)                        Queue_moveReadPosRaw(QUEUE, STEPS * ((QUEUE)->ItemSize))
#define             Queue_moveReadPos(QUEUE, STEPS)                         Queue_moveWritePosRaw(QUEUE, STEPS * ((QUEUE)->ItemSize))

#define             Queue_flipWrite(QUEUE, LEN)                             Queue_flipWriteRaw(QUEUE, (LEN) * ((QUEUE)->ItemSize))
#define             Queue_flipRead(QUEUE, LEN)                              Queue_flipReadRaw(QUEUE, (LEN) * ((QUEUE)->ItemSize))


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
    
    #define         Queue_spaceLimit(QUEUE)                                 (Queue_spaceLimitRaw(QUEUE) / ((QUEUE)->ItemSize))
    #define         Queue_getWriteLimit(QUEUE)                              (Queue_getWriteLimitRaw(QUEUE) / ((QUEUE)->ItemSize))
    #define         Queue_setWriteLimit(QUEUE, LEN)                         Queue_setWriteLimitRaw(QUEUE, (LEN) * ((QUEUE)->ItemSize))
    #define         Queue_isWriteLimited(QUEUE)                             Stream_isWriteLimited(&((QUEUE)->Buffer))
#endif // QUEUE_WRITE_LIMIT

// -------------------------- Read Limit APIs ----------------------------
#if QUEUE_READ_LIMIT
    #define         Queue_availableLimitRaw(QUEUE)                          Stream_availableLimit(&((QUEUE)->Buffer))
    #define         Queue_getReadLimitRaw(QUEUE)                            Stream_getReadLimit(&((QUEUE)->Buffer))
    #define         Queue_setReadLimitRaw(QUEUE, LEN)                       Stream_setReadLimit(&((QUEUE)->Buffer), (LEN))
    
    #define         Queue_availableLimit(QUEUE)                             (Queue_availableLimitRaw(QUEUE) / ((QUEUE)->ItemSize))
    #define         Queue_getReadLimit(QUEUE)                               (Queue_getReadLimitRaw(QUEUE) / ((QUEUE)->ItemSize))
    #define         Queue_setReadLimit(QUEUE, LEN)                          Queue_setReadLimitRaw(QUEUE, (LEN) * ((QUEUE)->ItemSize))
    #define         Queue_isReadLimited(QUEUE)                              Stream_isReadLimited(&((QUEUE)->Buffer))
#endif // QUEUE_READ_LIMIT

// -------------------------- Write Lock APIs ----------------------------
#if QUEUE_WRITE_LOCK
    #define         Queue_lockWrite(QUEUE, LOCK, LEN)                       Stream_lockWriteCustom(&(QUEUE)->Buffer, LOCK, LEN, sizeof(Queue))
    #define         Queue_unlockWrite(QUEUE, LOCK)                          Stream_unlockWrite(&(QUEUE)->Buffer, LOCK)
    #define         Queue_unlockWriteIgnore(QUEUE)                          Stream_unlockWriteIgnore(&(QUEUE)->Buffer)
    #define         Queue_lockWriteLenRaw(QUEUE, LOCK)                      Stream_lockWriteLen(&(QUEUE)->Buffer, LOCK)
    #define         Queue_lockWriteLen(QUEUE, LOCK)                         (Queue_lockWriteLenRaw(QUEUE, LOCK) / (QUEUE)->ItemSize)
#endif // QUEUE_WRITE_LOCK

#if QUEUE_READ_LOCK
    #define         Queue_lockRead(QUEUE, LOCK, LEN)                        Stream_lockReadCustom(&(QUEUE)->Buffer, LOCK, LEN, sizeof(Queue))
    #define         Queue_unlockRead(QUEUE, LOCK)                           Stream_unlockRead(&(QUEUE)->Buffer, LOCK)
    #define         Queue_unlockReadIgnore(QUEUE)                           Stream_unlockReadIgnore(&(QUEUE)->Buffer)
    #define         Queue_lockReadLenRaw(QUEUE, LOCK)                       Stream_lockReadLen(&(QUEUE)->Buffer, lock)
    #define         Queue_lockReadLen(QUEUE, LOCK)                          (Queue_lockReadLenRaw(QUEUE, LOCK) / (QUEUE)->ItemSize)
#endif // QUEUE_READ_LOCK

/**************** Write APIs **************/
#define             Queue_write(QUEUE, VAL)                                 Stream_writeBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (QUEUE)->ItemSize)
#if STREAM_WRITE_ARRAY
    #define         Queue_writeArray(QUEUE, VAL, LEN)                       Stream_writeBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (LEN) * (QUEUE)->ItemSize)
#endif // STREAM_WRITE_ARRAY
#if STREAM_WRITE_STREAM
    #define         Queue_writeQueue(OUT, IN, LEN)                          Stream_writeStream(&(OUT)->Buffer, &(IN)->Buffer, (LEN) * (OUT)->ItemSize)
#endif // STREAM_WRITE_STREAM
Queue_Result        Queue_writeQuery(Queue* queue, Queue_QueryFn query);
Queue_Result        Queue_writeQueryArray(Queue* queue, Queue_LenType len, Queue_QueryFn query);

/**************** Read APIs **************/
#define             Queue_read(QUEUE, VAL)                                  Stream_readBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (QUEUE)->ItemSize)
#if STREAM_READ_ARRAY
    #define         Queue_readArray(QUEUE, VAL, LEN)                        Stream_readBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (LEN) * (QUEUE)->ItemSize)
#endif // STREAM_READ_ARRAY
#if STREAM_READ_STREAM
    #define         Queue_readQueue(IN, OUT, LEN)                           Stream_readStream(&(IN)->Buffer, &(OUT)->Buffer, (LEN) * (IN)->ItemSize)
#endif // STREAM_READ_STREAM
Queue_Result        Queue_readQuery(Queue* queue, Queue_QueryFn query);
Queue_Result        Queue_readQueryArray(Queue* queue, Queue_LenType len, Queue_QueryFn query);

#if STREAM_GET_AT_FUNCTIONS
    #define         Queue_getAt(QUEUE, IDX, VAL)                                Stream_getBytesAt(&(QUEUE)->Buffer, (IDX), (uint8_t*) (VAL), (QUEUE)->ItemSize)
    #define         Queue_getArray(QUEUE, IDX, VAL, LEN)                        Stream_getBytesAt(&(QUEUE)->Buffer, (IDX), (uint8_t*) (VAL), (LEN) * (QUEUE)->ItemSize)
#endif // QUEUE_GET_FUNCTIONS

#if STREAM_GET_FUNCTIONS
    #define         Queue_get(QUEUE, VAL)                                       Stream_getBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (QUEUE)->ItemSize)
    #define         Queue_getArray(QUEUE, VAL, LEN)                             Stream_getBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (LEN) * (QUEUE)->ItemSize)
#endif // QUEUE_GET_FUNCTIONS

#if STREAM_SET_AT_FUNCTIONS
    #define         Queue_setAt(QUEUE, IDX, VAL)                                Stream_setBytesAt(&(QUEUE)->Buffer, (IDX), (uint8_t*) (VAL), (QUEUE)->ItemSize)
    #define         Queue_setArray(QUEUE, IDX, VAL, LEN)                        Stream_setBytesAt(&(QUEUE)->Buffer, (IDX), (uint8_t*) (VAL), (LEN) * (QUEUE)->ItemSize)
#endif // QUEUE_GET_FUNCTIONS

#if STREAM_SET_FUNCTIONS
    #define         Queue_set(QUEUE, VAL)                                       Stream_setBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (QUEUE)->ItemSize)
    #define         Queue_setArray(QUEUE, VAL, LEN)                             Stream_setBytes(&(QUEUE)->Buffer, (uint8_t*) (VAL), (LEN) * (QUEUE)->ItemSize)
#endif // QUEUE_GET_FUNCTIONS

#ifdef __cplusplus
};
#endif

#endif /* _QUEUE_H_ */
