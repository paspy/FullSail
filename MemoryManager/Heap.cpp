#include "stdafx.h"
#include "Heap.h"

#define MM_HEAP_LOCK if(synchronize)EnterCriticalSection(&criticalSection)
#define MM_HEAP_UNLOCK if(synchronize)LeaveCriticalSection(&criticalSection)

#define HIGH_BIT_32		(1 << 31)

namespace FSMM {

    void Heap::Destroy() {
        MM_HEAP_LOCK;
        if ( pool )
            free( pool );
        pool = nullptr;

        MM_HEAP_UNLOCK;
        DeleteCriticalSection( &criticalSection );
    }

    Heap::~Heap() {
        Destroy();
    }

    Header * Heap::FindBlock( unsigned int allocSize ) {
        if ( !freeListEntry )
            return nullptr;
        Header * hIter = freeListEntry;
        do {
            if ( hIter->size >= allocSize )
                return hIter;
            hIter = hIter->next;
        } while ( hIter != freeListEntry );
        return nullptr;
    }

    void Heap::Init( unsigned int poolSize, unsigned int _synchronize ) {
        synchronize = _synchronize;
        InitializeCriticalSection( &criticalSection );
        //MM_HEAP_LOCK;

        poolSizeTotal = poolSize;

        pool = (char *)malloc( poolSize );
        assert( pool );

        // If the target platform is 32-bit
        // we must make sure that our memory after the first header starts on a 16 byte boundary
        // On a 32 bit system in MSVC++ 2k8 malloc returns memory 8 byte aligned
        // Therefore we need 4 bytes of padding or 12 bytes of padding, since we want the memory after the header which is 12 bytes to begin on a 16 byte boundary.
        // Find the next address divisible by 16 and add 4 and place the header there.
        //firstHeader = (Header *)(((((unsigned int)pool)+15) & (~15))+4);
        // this padding stuff was causing problems with the renderer
        firstHeader = (Header*)pool;
        // Set the entry to the free list to be our current 1 free block
        freeListEntry = firstHeader;
        // Set up the cyclic linked list
        firstHeader->next = firstHeader->prev = firstHeader;
        // Find the correct size to pad the block so that the end of our block will be on a 16 byte boundary 
        // (shrink the block size to the next smallest number divisible by 16 by subtracting the size of the header and footer that will encapsulate it, and the pad space at the beginning which will be
        // either 4 or 12
        firstHeader->size = poolSize - sizeof( Footer ) - sizeof( Header );
        // Set up the footer
        lastFooter = reinterpret_cast<Footer*>( reinterpret_cast<char*>( firstHeader ) + sizeof( Header ) + firstHeader->size );
        lastFooter->size = firstHeader->size;

#ifdef _DEBUG
        memset( (void*)( firstHeader + 1 ), 0x0, firstHeader->size );
#endif

        //MM_HEAP_UNLOCK;
    }

    void* Heap::Allocate( unsigned int allocSize ) {
        //MM_HEAP_LOCK;
        Header* free_head = FindBlock( allocSize );
        Footer* free_foot = nullptr;
        Header* used_head = nullptr;
        Footer* used_foot = nullptr;
        void* data = nullptr;

        // TODO - comment out the solution version and write your own, you can ignore the synchronization code
        if ( AllocateSplit( free_head, &data, allocSize ) == false ) {
            AllocateNoSplit( free_head, &data );
        }

        //MM_HEAP_UNLOCK;
        return data;
    }

    void Heap::DeAllocate( void * data ) {
        //MM_HEAP_LOCK;
        bool merged_left = false;
        bool merged_right = false;

        merged_right = DeAllocateRight( data );
        merged_left = DeAllocateLeft( data );

        if ( merged_left != true ) {
            DeAllocateMiddle( data );
        }

        //MM_HEAP_UNLOCK;
    }


    bool Heap::AllocateSplit( Header* _head, void** _data, int _alloc_size ) {
        Footer* free_foot = nullptr;
        Header* used_head = nullptr;
        Footer* used_foot = reinterpret_cast<Footer*>( reinterpret_cast<char*>( _head ) + _head->size + sizeof( Header ) );
        bool do_split = false;

        if ( _head->size > _alloc_size + sizeof( Header ) + sizeof( Footer ) ) {
            // Set up used block.
            used_head = reinterpret_cast<Header*>( reinterpret_cast<char*>( used_foot ) - _alloc_size - sizeof( Header ) );
            used_foot->size = _alloc_size | HIGH_BIT_32;
            used_head->size = used_foot->size;

            // Set up the free block
            free_foot = reinterpret_cast<Footer*>( reinterpret_cast<char*>( used_head ) - sizeof( Footer ) );
            free_foot->size = _head->size - sizeof( Header ) - sizeof( Footer ) - _alloc_size;
            _head->size = free_foot->size;

            ( *_data ) = static_cast<void*>( used_head + 1 );
            do_split = true;
        }

        return do_split;
    }

    void Heap::AllocateNoSplit( Header* _head, void** _data ) {
        Footer* used_foot = reinterpret_cast<Footer*>( reinterpret_cast<char*>( _head ) + _head->size + sizeof( Header ) );

        _head->size |= HIGH_BIT_32;
        used_foot->size = _head->size;

        freeListEntry = _head->next;
        if ( _head->next == _head ) {
            freeListEntry = nullptr;
        } else {
            _head->next->prev = _head->prev;
            _head->prev->next = _head->next;
        }

        ( *_data ) = static_cast<void*>( _head + 1 );
    }

    bool Heap::DeAllocateRight( void * data ) {
        bool did_merge = false;
        Header* headToFree = reinterpret_cast<Header*>( reinterpret_cast<char*>( data ) - sizeof( Header ) );
        headToFree->size &= ~HIGH_BIT_32;
        Footer* footToFree = reinterpret_cast<Footer*>( reinterpret_cast<char*>( headToFree ) + headToFree->size + sizeof( Header ) );
        footToFree->size &= ~HIGH_BIT_32;

        // Find out if we can look right
        if ( footToFree != lastFooter ) {
            // Find out if we can merge right
            Header* rightHeader = reinterpret_cast<Header*>( reinterpret_cast<char*>( footToFree ) + sizeof( Footer ) );
            if ( ( rightHeader->size & HIGH_BIT_32 ) == 0 ) {
                did_merge = true;
                footToFree = reinterpret_cast<Footer*>( reinterpret_cast<char*>( rightHeader ) + rightHeader->size + sizeof( Header ) );
                footToFree->size += headToFree->size + sizeof( Header ) + sizeof( Footer );
                headToFree->size = footToFree->size;

                // Remove the right block from the free list
                freeListEntry = rightHeader->next;
                if ( freeListEntry->next == freeListEntry ) {
                    freeListEntry = nullptr;
                } else {
                    rightHeader->next->prev = rightHeader->prev;
                    rightHeader->prev->next = rightHeader->next;
                }
            }
        }
        return did_merge;
    }

    bool Heap::DeAllocateLeft( void * data ) {
        bool did_merge = false;
        Header* headToFree = reinterpret_cast<Header*>( reinterpret_cast<char*>( data ) - sizeof( Header ) );
        headToFree->size &= ~HIGH_BIT_32;
        Footer* footToFree = reinterpret_cast<Footer*>( reinterpret_cast<char*>( headToFree ) + headToFree->size + sizeof( Header ) );
        footToFree->size &= ~HIGH_BIT_32;

        // Find out if we can look left
        if ( headToFree != firstHeader ) {
            // Find out if we can merge left
            Footer* leftFooter = reinterpret_cast<Footer*>( reinterpret_cast<char*>( headToFree ) - sizeof( Footer ) );
            if ( ( leftFooter->size & HIGH_BIT_32 ) == 0 ) {
                Header* leftHeader = reinterpret_cast<Header*>( reinterpret_cast<char*>( leftFooter ) - leftFooter->size - sizeof( Header ) );
                leftHeader->size += footToFree->size + sizeof( Header ) + sizeof( Footer );
                footToFree->size = leftHeader->size;

                did_merge = true;
            }
        }

        return did_merge;
    }

    void Heap::DeAllocateMiddle( void * data ) {
        Header* headToFree = reinterpret_cast<Header*>( reinterpret_cast<char*>( data ) - sizeof( Header ) );
        headToFree->size &= ~HIGH_BIT_32;
        Footer* footToFree = reinterpret_cast<Footer*>( reinterpret_cast<char*>( headToFree ) + headToFree->size + sizeof( Header ) );
        footToFree->size &= ~HIGH_BIT_32;

        // Not merged left so I need to add headToFree to the list.
        // If there is at least one free block in this pool, add the block.
        if ( freeListEntry ) {
            freeListEntry->next->prev = headToFree;
            headToFree->next = freeListEntry->next;
            headToFree->prev = freeListEntry;
            freeListEntry->next = headToFree;
        } else // There were no free blocks in this memory pool, therefore this block will be the entry to the cyclic linked list of free memory for this pool.
        {
            freeListEntry = headToFree;
            freeListEntry->next = freeListEntry;
            freeListEntry->prev = freeListEntry;
        }
    }

}