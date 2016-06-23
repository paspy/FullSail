#ifndef HeapLog_h__
#define HeapLog_h__

#include "stdafx.h"
#define LEAK_LOG_LOCATION "../../Logs/LeakedMemory.txt"

namespace FSMM {
    struct MemBlockLogInfo {
        char fileName[512];
        int lineNum;
        int size;
        MemBlockLogInfo & operator = ( const MemBlockLogInfo &ref ) {
            memcpy( fileName, ref.fileName, strlen( ref.fileName ) + 1 );
            lineNum = ref.lineNum;
            size = ref.size;
            return *this;
        }
    };

    class HeapLog {
    private:
        typedef std::map<void*, MemBlockLogInfo, std::less<void*>, std::allocator<void*>> AllocatedBlocksType;

        std::map<int, int> alloc_sizes;

        unsigned int numberOfAllocations;
        unsigned int totalSizeAllocated;
        unsigned int smallest_allocation;
        unsigned int largest_allocation;

        AllocatedBlocksType allocatedBlocks;

        CRITICAL_SECTION criticalSection;

    public:
        HeapLog();
        ~HeapLog();
        void Add( void * block, MemBlockLogInfo & info );
        void Remove( void * block );
        void WriteLogFile( const char * heapName );
        void WriteMetricFile( const char * heapName );

        inline unsigned int GetTotalSizeAllocated() { return totalSizeAllocated; }
        inline unsigned int GetNumberOfAllocations() { return numberOfAllocations; }
        inline unsigned int GetSmallestAllocation() { return smallest_allocation; }
        inline unsigned int GetLargestAllocation() { return largest_allocation; }
    };
}

#endif // HeapLog_h__
