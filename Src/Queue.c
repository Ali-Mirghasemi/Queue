#include "Queue.h"

#if QUEUE

#if STREAM_WRITE
    #error "Queue library require STREAM_WRITE to be enabled"
#endif

#if STREAM_READ
    #error "Queue library require STREAM_READ to be enabled"
#endif

/**
 * @brief initialize queue
 *
 * @param queue address of queue struct
 * @param buffer address of byte buffer
 * @param size size of buffer
 */
void Queue_init(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemSize) {
    Stream_init(&queue->Buffer, buffer, size);
    queue->ItemSize = itemSize;
}
/**
 * @brief initialize queue with a buffer that already have data in it
 * 
 * @param queue 
 * @param buffer 
 * @param size 
 */
void Queue_fromBuff(Queue* queue, void* buffer, Queue_LenType size, Queue_LenType itemSize, Queue_LenType len) {
    Stream_fromBuff(&queue->Buffer, buffer, size, itemSize);
    queue->ItemSize = itemSize;
}
/**
 * @brief write item into queue with custom query function
 * 
 * @param queue 
 * @param len 
 * @param query 
 * @return Queue_Result 
 */
Queue_Result Queue_writeQuery(Queue* queue, Queue_QueryFn query) {
    // check available space for write
    if (Queue_spaceRaw(queue) < queue->ItemSize) {
        return Queue_NoSpace;
    }
#if STREAM_WRITE_LIMIT
    if (Queue_isWriteLimited(queue)) {
        queue->Buffer.WriteLimit -= len;
    }
#endif

    Queue_Result res;
    
    // Call query function
    if ((res = query(queue, Queue_getWritePtr(queue), 0, 1)) == Queue_Ok) {
        // Move WPos
        Queue_moveWritePosRaw(queue, queue->ItemSize);
    }

    return res;
}
/**
 * @brief write item into queue with custom query function
 * 
 * @param queue 
 * @param len 
 * @param query 
 * @return Queue_Result 
 */
Queue_Result Queue_writeQueryArray(Queue* queue, Queue_LenType len, Queue_QueryFn query) {
    // check available space for write
#if STREAM_CHECK_ZERO_LEN
    if (len == 0) {
      return Queue_ZeroLen;
    }
#endif
    if (Queue_spaceRaw(queue) < len * queue->ItemSize) {
        return Queue_NoSpace;
    }
#if STREAM_WRITE_LIMIT
    if (Queue_isWriteLimited(queue)) {
        queue->Buffer.WriteLimit -= len;
    }
#endif

    Queue_Result res = Queue_Ok;
    Queue_LenType i = 0;

    while ((res = query(queue, Queue_getWritePtr(queue), i, len)) == Queue_Ok &&
            ++i < len
    ) {
        // Move WPos
        Queue_moveWritePosRaw(queue, queue->ItemSize);
    }

    return res;
}
/**
 * @brief read bytes from queue with custom query function
 * 
 * @param queue 
 * @param len 
 * @param query 
 * @return Queue_Result 
 */
Queue_Result Queue_readQuery(Queue* queue, Queue_QueryFn query) {
    // check available bytes for read
    if (Queue_availableRaw(queue) < queue->ItemSize) {
        return Queue_NoAvailable;
    }
#if STREAM_READ_LIMIT
    if (Queue_isReadLimited(queue)) {
        queue->Buffer.ReadLimit -= len;
    }
#endif

    Queue_Result res;
    
    // Call query function
    if ((res = query(queue, Queue_getReadPtr(queue), 0, 1)) == Queue_Ok) {
        // Move RPos
        Queue_moveReadPosRaw(queue, queue->ItemSize);
    }

    return res;
}

/**
 * @brief read bytes from queue with custom query function
 * 
 * @param queue 
 * @param len 
 * @param query 
 * @return Queue_Result 
 */
Queue_Result Queue_readQueryArray(Queue* queue, Queue_LenType len, Queue_QueryFn query) {
    // check available bytes for read
#if STREAM_CHECK_ZERO_LEN
    if (len == 0) {
        return Queue_ZeroLen;
    }
#endif
    if (Queue_availableRaw(queue) < len * queue->ItemSize) {
        return Queue_NoAvailable;
    }
#if STREAM_READ_LIMIT
    if (Queue_isReadLimited(queue)) {
        queue->ReadLimit -= len;
    }
#endif

    Queue_Result res = Queue_Ok;
    Queue_LenType i = 0;

    while ((res = query(queue, Queue_getReadPtr(queue), i, len)) == Queue_Ok &&
            ++i < len
    ) {
        // Move RPos
        Queue_moveReadPosRaw(queue, queue->ItemSize);
    }

    return res;
}

#endif // QUEUE
