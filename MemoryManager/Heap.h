#ifndef Heap_h__
#define Heap_h__

#include "stdafx.h"

namespace FSMM {

    struct Header {
        unsigned int size;
        Header * prev;
        Header * next;
    };

    struct Footer {
        unsigned int size;
    };

    class Heap {
        friend class MemoryManager;
        char * pool;
        Header * freeListEntry;
        Header * firstHeader;
        Footer * lastFooter;
        unsigned int poolSizeTotal;
        unsigned int synchronize;
        CRITICAL_SECTION criticalSection;

    public:
        Heap() = default;
        ~Heap();
        inline unsigned int GetPoolSizeTotal() { return poolSizeTotal; }
        void Destroy();

    public:
        Header * FindBlock( unsigned int allocSize );
        void Init( unsigned int poolSize, unsigned int _synchronize = 0 );
        void * Allocate( unsigned int allocSize );
        void DeAllocate( void * data );

    private:
        bool AllocateSplit( Header* _head, void** _data, int _alloc_size );
        void AllocateNoSplit( Header* _head, void** _data );

        bool DeAllocateRight( void * data );
        bool DeAllocateLeft( void * data );
        void DeAllocateMiddle( void * data );
    };
}
#endif // Heap_h__
