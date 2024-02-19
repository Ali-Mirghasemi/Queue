#include "Queue.h"
#include <string.h>

/**
 * @brief initialize queue
 *
 * @param queue address of queue struct
 * @param buffer address of byte buffer
 * @param size size of buffer
 */
void Queue_init(Queue* queue, uint8_t* buffer, Queue_LenType size, Queue_LenType itemLength) {
    queue->Buf = buffer;
    queue->Size = size;
    queue->Overflow = 0;
    queue->RPos = 0;
    queue->WPos = 0;
    queue->ItemSize = itemLength;
#if QUEUE_WRITE_LIMIT
    queue->WriteLimit = QUEUE_NO_LIMIT;
#endif // QUEUE_WRITE_LIMIT
#if QUEUE_READ_LIMIT
    queue->ReadLimit = QUEUE_NO_LIMIT;
#endif // QUEUE_READ_LIMIT
#if QUEUE_WRITE_LOCK
    queue->WriteLocked = 0;
#endif // QUEUE_WRITE_LOCK
#if QUEUE_READ_LOCK
    queue->ReadLocked = 0;
#endif // QUEUE_READ_LOCK
}
/**
 * @brief initialize queue with a buffer that already have data in it
 * 
 * @param queue 
 * @param buffer 
 * @param size 
 */
void Queue_fromBuff(Queue* queue, uint8_t* buffer, Queue_LenType size, Queue_LenType itemLength) {
    Queue_init(queue, buffer, size, itemLength);
    queue->WPos = size;
}
/**
 * @brief reset queue struct into default values
 *
 * @param queue address of queue
 */
void Queue_deinit(Queue* queue) {
    memset(queue, 0, sizeof(Queue));
}

/**
 * @brief return available bytes to read
 *
 * @param queue
 * @return Queue_LenType available bytes
 */
Queue_LenType Queue_availableRealRaw(Queue* queue) {
    return queue->Overflow ? queue->WPos + (queue->Size - queue->RPos) :
                                queue->WPos - queue->RPos;
}
/**
 * @brief return buffer space for write bytes
 *
 * @param queue
 * @return Queue_LenType space for write
 */
Queue_LenType Queue_spaceRealRaw(Queue* queue) {
    return queue->Overflow ? queue->RPos - queue->WPos :
                                queue->RPos + (queue->Size - queue->WPos);
}
/**
 * @brief return number of items available to read
 * 
 * @param queue 
 * @return Queue_LenType 
 */
Queue_LenType Queue_available(Queue* queue) {
#if QUEUE_READ_LIMIT
    return Queue_availableLimitRaw(queue) / queue->ItemSize;
#else
    return Queue_availableRealRaw(queue) / queue->ItemSize;
#endif
}
/**
 * @brief return number of items available to write
 * 
 * @param queue 
 * @return Queue_LenType 
 */
Queue_LenType Queue_space(Queue* queue) {
#if QUEUE_WRITE_LIMIT
    return Queue_spaceLimitRaw(queue) / queue->ItemSize;
#else
    return Queue_spaceRealRaw(queue) / queue->ItemSize;
#endif
}
/**
 * @brief check queue is empty
 * no bytes available to read
 * @param queue
 * @return uint8_t 0 -> Not Empty, 1-> it's empty
 */
uint8_t Queue_isEmpty(Queue* queue) {
    return queue->RPos == queue->WPos && !queue->Overflow;
}
/**
 * @brief check queue it's full
 * no bytes can write
 *
 * @param queue
 * @return uint8_t 0 -> Not Full, 1-> it's Full
 */
uint8_t Queue_isFull(Queue* queue) {
    return queue->RPos == queue->WPos && queue->Overflow;
}
/**
 * @brief reset queue
 *
 * @param queue
 */
void Queue_reset(Queue* queue) {
    queue->RPos = 0;
    queue->WPos = 0;
    queue->Overflow = 0;
}
/**
 * @brief clear buffer and reset queue
 *
 * @param queue
 */
void Queue_clear(Queue* queue) {
    Queue_reset(queue);
    memset(queue->Buf, 0, queue->Size);
}
/**
 * @brief return Write Pos
 *
 * @param queue
 * @return Queue_LenType it's between 0 ~ Size
 */
Queue_LenType Queue_getWritePos(Queue* queue) {
    return queue->WPos;
}
/**
 * @brief return Read Pos
 *
 * @param queue
 * @return Queue_LenType it's between 0 ~ Size
 */
Queue_LenType Queue_getReadPos(Queue* queue) {
    return queue->RPos;
}
/**
 * @brief return number of bytes that it's in row in the ram
 * use for transmit function in OStream
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_directAvailableRaw(Queue* queue) {
    return queue->Overflow ? queue->Size - queue->RPos :
                                queue->WPos - queue->RPos;
}
/**
 * @brief return number of bytes it's in row in the ram
 * use for receive function in IStream
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_directSpaceRaw(Queue* queue) {
    return queue->Overflow ? queue->RPos - queue->WPos :
                                queue->Size - queue->WPos;
}
/**
 * @brief return number of bytes that it's in row in the ram
 * use for transmit function in OStream
 * offset is RPos
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_directAvailableAtRaw(Queue* queue, Queue_LenType index) {
    Queue_LenType len = Queue_availableRealRaw(queue);
    Queue_LenType dirLen = Queue_directAvailableRaw(queue);
    if (len == dirLen) {
        return len - index;
    }
    else {
        return dirLen > index ? dirLen - index :
                                queue->WPos - (index - dirLen);
    }
}
/**
 * @brief return number of bytes it's in row in the ram
 * use for receive function in IStream
 * offset is WPos
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_directSpaceAtRaw(Queue* queue, Queue_LenType index) {
    Queue_LenType len = Queue_spaceRealRaw(queue);
    Queue_LenType dirLen = Queue_directSpaceRaw(queue);
    if (len == dirLen) {
        return len - index;
    }
    else {
        return dirLen > index ? dirLen - index :
                                queue->RPos - (index - dirLen);
    }
}
/**
 * @brief get ptr to start of WPos in ram
 *
 * @param queue
 * @return uint8_t*
 */
uint8_t* Queue_getWritePtr(Queue* queue) {
    return &queue->Buf[queue->WPos];
}
/**
 * @brief get ptr to start of RPos in ram
 *
 * @param queue
 * @return uint8_t*
 */
uint8_t* Queue_getReadPtr(Queue* queue) {
    return &queue->Buf[queue->RPos];
}
uint8_t* Queue_getWritePtrAt(Queue* queue, Queue_LenType index) {
    index += queue->WPos;

    if (index >= queue->Size) {
        index %= queue->Size;
    }

    return &queue->Buf[index];
}
uint8_t* Queue_getReadPtrAt(Queue* queue, Queue_LenType index) {
    index += queue->RPos;

    if (index >= queue->Size) {
        index %= queue->Size;
    }

    return &queue->Buf[index];
}
/**
 * @brief set buffer for queue and reset queue
 *
 * @param queue
 */
void Queue_setBuffer(Queue* queue, uint8_t* data, Queue_LenType size) {
    Queue_init(queue, data, size);
}
/**
 * @brief return byte buffer
 *
 * @param queue
 * @return uint8_t*
 */
uint8_t* Queue_getBuffer(Queue* queue) {
    return queue->Buf;
}
/**
 * @brief return size of queue buffer
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_getBufferSize(Queue* queue) {
    return queue->Size;
}
/**
 * @brief can use for ignore bytes, and move WPos index
 *
 * @param queue
 * @param steps
 * @return Queue_Result
 */
Queue_Result Queue_moveWritePos(Queue* queue, Queue_LenType steps) {
    if (Queue_spaceRaw(queue) < steps) {
        return Queue_NoSpace;
    }

    queue->WPos += steps;
    if (queue->WPos >= queue->Size) {
        queue->WPos %= queue->Size;
        queue->Overflow = 1;
    }

    return Queue_Ok;
}
/**
 * @brief can use for ignore bytes, and move RPos index
 *
 * @param queue
 * @param steps
 * @return Queue_Result
 */
Queue_Result Queue_moveReadPos(Queue* queue, Queue_LenType steps) {
    if (Queue_availableRaw(queue) < steps) {
        return Queue_NoAvailable;
    }

    queue->RPos += steps;
    if (queue->RPos >= queue->Size) {
        queue->RPos %= queue->Size;
        queue->Overflow = 0;
    }

    return Queue_Ok;
}
/**
 * @brief flip space size for write
 *
 * @param queue
 * @param len
 */
void Queue_flipWrite(Queue* queue, Queue_LenType len) {
    queue->RPos = queue->WPos + len;
    if (queue->RPos >= queue->Size) {
        queue->RPos %= queue->Size;
        queue->Overflow = 0;
    }
    else {
        queue->Overflow = 1;
    }
}
/**
 * @brief flip available size for read
 *
 * @param queue
 * @param len
 */
void Queue_flipRead(Queue* queue, Queue_LenType len) {
    queue->WPos = queue->RPos + len;
    if (queue->WPos >= queue->Size) {
        queue->WPos %= queue->Size;
        queue->Overflow = 1;
    }
    else {
        queue->Overflow = 0;
    }
}
#if QUEUE_WRITE_LIMIT
/**
 * @brief set limit for write operations, you can't set limit greater than space
 *
 * @param queue
 * @param len
 */
void       Queue_setWriteLimit(Queue* queue, Queue_LenType len) {
    Queue_LenType space = Queue_spaceRealRaw(queue);
    len *= queue->ItemSize;
    if (space < len) {
        len = space;
    }
    queue->WriteLimit = len;
}
/**
 * @brief return write operation is limited or not
 *
 * @param queue
 * @return uint8_t true means limited
 */
uint8_t    Queue_isWriteLimited(Queue* queue) {
    return queue->WriteLimit >= 0;
}
/**
 * @brief return space available for write bytes respect to write limit
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_spaceLimitRaw(Queue* queue) {
    return queue->WriteLimit >= 0 ? queue->WriteLimit : Queue_spaceRealRaw(queue);
}
Queue_LenType Queue_getWriteLimitRaw(Queue* queue) {
    return queue->WriteLimit;
}
Queue_LenType Queue_spaceLimit(Queue* queue) {
    return Queue_spaceLimitRaw(queue) / queue->ItemSize;
}
Queue_LenType Queue_getWriteLimit(Queue* queue) {
    return Queue_getWriteLimitRaw(queue) / queue->ItemSize;
}
#endif // QUEUE_WRITE_LIMIT
#if QUEUE_READ_LIMIT
/**
 * @brief set limit for read operations, you can't set limit greater than available
 *
 * @param queue
 * @param len
 */
void Queue_setReadLimit(Queue* queue, Queue_LenType len) {
    Queue_LenType available = Queue_availableRealRaw(queue);
    len *= queue->ItemSize;
    if (available < len) {
        len = available;
    }
    queue->ReadLimit = len;
}
/**
 * @brief return read operations is limited or not
 *
 * @param queue
 * @return uint8_t true means it's limited
 */
uint8_t Queue_isReadLimited(Queue* queue) {
    return queue->ReadLimit >= 0;
}
/**
 * @brief retruna available bytes for read respect to read limit
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_availableLimitRaw(Queue* queue) {
    return queue->ReadLimit >= 0 ? queue->ReadLimit : Queue_availableRealRaw(queue);
}
/**
 * @brief return read limit
 *
 * @param queue
 * @return Queue_LenType
 */
Queue_LenType Queue_getReadLimitRaw(Queue* queue) {
    return queue->ReadLimit;
}
Queue_LenType Queue_availableLimit(Queue* queue) {
    return Queue_availableLimitRaw(queue) / queue->ItemSize;
}
Queue_LenType Queue_getReadLimit(Queue* queue) {
    return Queue_getReadLimitRaw(queue) / queue->ItemSize;
}
#endif // QUEUE_READ_LIMIT
#if QUEUE_CURSOR
/**
 * @brief fill cursor object based on given queue
 *
 * @param queue
 * @param cursor
 */
void Queue_getCursor(Queue* queue, Queue_Cursor* cursor) {
    cursor->WPos = queue->WPos;
    cursor->RPos = queue->RPos;
}
/**
 * @brief return read len from cursor pos
 *
 * @param queue
 * @param cursor
 * @return Queue_LenType
 */
Queue_LenType Queue_getReadLen(Queue* queue, Queue_Cursor* cursor) {
    return cursor->RPos >= queue->RPos ? cursor->RPos - queue->RPos :
                                            (queue->Size - cursor->RPos) + queue->RPos;
}
/**
 * @brief return write len from cursor pos
 *
 * @param queue
 * @param cursor
 * @return Queue_LenType
 */
Queue_LenType Queue_getWriteLen(Queue* queue, Queue_Cursor* cursor) {
    return cursor->WPos >= queue->WPos ? cursor->WPos - queue->WPos :
                                            (queue->Size - cursor->WPos) + queue->WPos;
}
#endif // QUEUE_CURSOR
/**
 * @brief write item into queue
 *
 * @param queue
 * @param val byte array
 * @param len len of array
 * @return Queue_Result
 */
Queue_Result Queue_write(Queue* queue, void* val) {
    if (Queue_spaceRaw(queue) < queue->ItemSize) {
        return Queue_NoSpace;
    }
#if QUEUE_WRITE_LIMIT
    if (Queue_isWriteLimited(queue)) {
        queue->WriteLimit -= queue->ItemSize;
    }
#endif

    memcpy(&queue->Buf[queue->WPos], val, queue->ItemSize);
    // move WPos
    queue->WPos += queue->ItemSize;
    if (queue->WPos >= queue->Size) {
        queue->WPos = 0;
        queue->Overflow = 1;
    }
    return Queue_Ok;
}
Queue_Result Queue_writeArray(Queue* queue, void* val, Queue_LenType len) {
#if QUEUE_CHECK_ZERO_LEN
    if (len == 0) {
      return Queue_ZeroLen;
    }
#endif
    len *= queue->ItemSize;
    if (Queue_spaceRaw(queue) < len) {
        return Queue_NoSpace;
    }
#if QUEUE_WRITE_LIMIT
    if (Queue_isWriteLimited(queue)) {
        queue->WriteLimit -= queue->ItemSize;
    }
#endif

    memcpy(&queue->Buf[queue->WPos], val, len);
    // move WPos
    queue->WPos += len;
    if (queue->WPos >= queue->Size) {
        queue->WPos = 0;
        queue->Overflow = 1;
    }

    return Queue_Ok;
}
/**
 * @brief directly read from a queue and write to another
 *
 * @param queue
 * @param in
 * @param len
 * @return Queue_Result
 */
Queue_Result Queue_writeQueue(Queue* out, Queue* in, Queue_LenType len) {
    // check available space for write
#if QUEUE_CHECK_ZERO_LEN
    if (len == 0) {
      return Queue_ZeroLen;
    }
#endif
    len *= in->ItemSize;
    if (Queue_spaceRaw(out) < len) {
        return Queue_NoSpace;
    }
    // check available bytes for read
    if (Queue_availableRaw(in) < len) {
        return Queue_NoAvailable;
    }
#if QUEUE_WRITE_LIMIT
    if (Queue_isWriteLimited(out)) {
        out->WriteLimit -= len;
    }
#endif

    if (out->WPos + len >= out->Size) {
        Queue_LenType tmpLen;
        tmpLen = out->Size - out->WPos;
        len -= tmpLen;
        Queue_readArray(in, &out->Buf[out->WPos], tmpLen);
        // move WPos
        out->WPos = (out->WPos + tmpLen) % out->Size;
        out->Overflow = 1;
    }
    if (len > 0) {
        Queue_readArray(in, &out->Buf[out->WPos], len);
        // move WPos
        out->WPos = (out->WPos + len) % out->Size;
    }

    return Queue_Ok;
}
/**
 * @brief read bytes form queue, if possible
 *
 * @param queue
 * @param val
 * @param len
 * @return Queue_Result
 */
Queue_Result Queue_read(Queue* queue, void* val) {
    if (Queue_availableRaw(queue) < queue->ItemSize) {
        return Queue_NoAvailable;
    }
#if QUEUE_READ_LIMIT
    if (Queue_isReadLimited(queue)) {
        queue->ReadLimit -= queue->ItemSize;
    }
#endif

    memcpy(val, &queue->Buf[queue->RPos], queue->ItemSize);
    // move RPos
    queue->RPos += queue->ItemSize;
    if (queue->RPos >= queue->Size) {
        queue->RPos = 0;
        queue->Overflow = 0;
    }
    
    return Queue_Ok;
}
Queue_Result Queue_readArray(Queue* queue, void* val, Queue_LenType len) {
#if QUEUE_CHECK_ZERO_LEN
    if (len == 0) {
      return Queue_ZeroLen;
    }
#endif
    len *= queue->ItemSize;
    if (Queue_availableRaw(queue) < queue->ItemSize) {
        return Queue_NoAvailable;
    }
#if QUEUE_READ_LIMIT
    if (Queue_isReadLimited(queue)) {
        queue->ReadLimit -= queue->ItemSize;
    }
#endif

    memcpy(val, &queue->Buf[queue->RPos], len);
    // move RPos
    queue->RPos += len;
    if (queue->RPos >= queue->Size) {
        queue->RPos = 0;
        queue->Overflow = 0;
    }
    
    return Queue_Ok;
}
Queue_Result Queue_readQueue(Queue* in, Queue* out, Queue_LenType len) {
#if QUEUE_CHECK_ZERO_LEN
    if (len == 0) {
      return Queue_ZeroLen;
    }
#endif
    len *= in->ItemSize;
    if (Queue_availableRaw(in) < len) {
        return Queue_NoAvailable;
    }
    if (Queue_spaceRaw(out) < len) {
        return Queue_NoSpace;
    }
#if QUEUE_READ_LIMIT
    if (Queue_isReadLimited(in)) {
        in->ReadLimit -= len;
    }
#endif

    if (in->RPos + len >= in->Size) {
        Queue_LenType tmpLen;

        tmpLen = in->Size - in->RPos;
        len -= tmpLen;
        Queue_writeArray(out, &in->Buf[in->RPos], tmpLen);
        // move RPos
        in->RPos = (in->RPos + tmpLen) % in->Size;
        in->Overflow = 0;
    }
    if (len > 0) {
        Queue_writeArray(out, &in->Buf[in->RPos], len);
        // move RPos
        in->RPos = (in->RPos + len) % in->Size;
    }

    return Queue_Ok;
}
#if QUEUE_GET_AT_FUNCTIONS && QUEUE_GET_FUNCTIONS

Queue_Result Queue_get(Queue* queue, void* val) {
    return Queue_getAt(queue, 0, val, len);
}
Queue_Result Queue_getArray(Queue* queue, void* val, Queue_LenType len) {
    return Queue_getArrayAt(queue, 0, val, len);
}
#endif // QUEUE_GET_FUNCTIONS

#if QUEUE_GET_AT_FUNCTIONS

Queue_Result Queue_getAt(Queue* queue, Queue_LenType index, void* val) {
    index *= queue->ItemSize;
    if (Queue_availableRaw(queue) - index < 0) {
        return Queue_NoAvailable;
    }

    index = (queue->RPos + index) % queue->Size;
    memcpy(val, &queue->Buf[index], queue->ItemSize);

    return Queue_Ok;
}
Queue_Result Queue_getArrayAt(Queue* queue, Queue_LenType index, void* val, Queue_LenType len) {
    len *= queue->ItemSize;
    if (Queue_availableRaw(queue) - index - index < 0) {
        return Queue_NoAvailable;
    }

    index = (queue->RPos + index) % queue->Size;
    memcpy(val, &queue->Buf[index], len);

    return Queue_Ok;
}
#endif // QUEUE_GET_AT_FUNCTIONS
#if QUEUE_WRITE_LOCK
/**
 * @brief lock the queue for writing
 *
 * @param queue
 * @return Queue_Result
 */
Queue_Result Queue_lockWrite(Queue* queue, Queue* lock, Queue_LenType len) {
    Queue_LenType space = Queue_spaceRaw(queue);
    len *= queue->ItemSize;
    if (space >= len && !queue->WriteLocked) {
        memcpy(lock, queue, sizeof(Queue));
        Queue_flipWrite(lock, len);
        queue->WriteLocked = 1;
        return Queue_Ok;
    }
    else {
        return Queue_NoSpace;
    }
}
/**
 * @brief unlock the queue for writing
 *
 * @param queue
 * @return Queue_Result
 */
void Queue_unlockWrite(Queue* queue, Queue* lock) {
    if (queue->WriteLocked) {
        Queue_moveWritePos(queue, Queue_lockWriteLenRaw(queue, lock));
        queue->WriteLocked = 0;
    }
}
/**
 * @brief unlock queue for write with ignore changes
 * 
 * @param queue 
 */
void Queue_unlockWriteIgnore(Queue* queue) {
    if (queue->WriteLocked) {
        queue->WriteLocked = 0;
    }
}
/**
 * @brief return number of byte write in lock
 * 
 * @param queue 
 * @param lock 
 */
Queue_LenType Queue_lockWriteLenRaw(Queue* queue, Queue* lock) {
    if (queue->WPos != lock->WPos) {
        // some data wrote
        if (queue->WPos < lock->WPos) {
            return  lock->WPos - queue->WPos;
        }
        else {
            return (queue->Size - queue->WPos) + lock->WPos;
        }
    }
    else if (queue->RPos == lock->RPos && 
        queue->Overflow == 0 &&
        lock->Overflow) {
        
        return queue->Size;
    }
    else {
        return 0;
    }
}
Queue_LenType   Queue_lockWriteLen(Queue* queue, Queue* lock) {
    return Queue_lockWriteLenRaw(queue, lock) / queue->ItemSize;
}
#endif // QUEUE_WRITE_LOCK

#if QUEUE_READ_LOCK
/**
 * @brief lock the queue for reading
 *
 * @param queue
 * @return Queue_Result
 */
Queue_Result Queue_lockRead(Queue* queue, Queue* lock, Queue_LenType len) {
    Queue_LenType available = Queue_availableRaw(queue);
    len *= queue->ItemSize;
    if (available >= len && !queue->ReadLocked) {
        memcpy(lock, queue, sizeof(Queue));
        Queue_flipRead(lock, len);
        queue->ReadLocked = 1;
        return Queue_Ok;
    }
    else {
        return Queue_NoAvailable;
    }
}
/**
 * @brief unlock the queue for reading
 *
 * @param queue
 * @return Queue_Result
 */
void Queue_unlockRead(Queue* queue, Queue* lock) {
    if (queue->ReadLocked) {
        Queue_moveReadPos(queue, Queue_lockReadLenRaw(queue, lock));
        queue->ReadLocked = 0;
    }
}
/**
 * @brief return number of bytes that read
 * 
 * @param queue 
 * @param lock 
 * @return Queue_LenType 
 */
Queue_LenType Queue_lockReadLenRaw(Queue* queue, Queue* lock) {
    if (queue->RPos != lock->RPos) {
        // some data read
        if (queue->RPos < lock->RPos) {
            return lock->RPos - queue->RPos;
        }
        else {
            return (queue->Size - queue->RPos) + lock->RPos;
        }
    }
    else if (queue->WPos == lock->WPos && 
        queue->Overflow != 0 &&
        !lock->Overflow) {

        return queue->Size;
    }
    else {
        return 0;
    }
}
Queue_LenType   Queue_lockReadLen(Queue* queue, Queue* lock) {
    return Queue_lockReadLenRaw(queue, lock) / queue->ItemSize;
}
/**
 * @brief unlock queue for read with ignore changes
 * 
 * @param queue 
 */
void Queue_unlockReadIgnore(Queue* queue) {
    if (queue->ReadLocked) {
        queue->ReadLocked = 0;
    }
}
#endif // QUEUE_READ_LOCK
