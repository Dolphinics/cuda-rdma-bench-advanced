#ifndef __RDMA_BENCH_TRANSFER_H__
#define __RDMA_BENCH_TRANSFER_H__

#include <memory>
#include <vector>
#include <cstddef>
#include <sisci_types.h>
#include "segment.h"

class Transfer;
struct TransferImpl;


/* Convenience type for a transfer pointer */
typedef std::shared_ptr<Transfer> TransferPtr;


/* Convenience type for DMA vectors */
typedef std::vector<dis_dma_vec_t> DmaVector; 


/* Actual transfer representation */
class Transfer
{
    public:
        const uint adapter;
        const uint remoteNodeId;
        const uint remoteSegmentId;
        const size_t remoteSegmentSize;
        const uint localSegmentId;
        const size_t localSegmentSize;
        const uint flags;

        static TransferPtr create(const SegmentPtr localSegment, uint remoteNodeId, uint remoteSegmentId, uint adapter, uint flags);

        void addVectorEntry(size_t localOffset, size_t remoteOffset, size_t size);

        void addVectorEntry(const dis_dma_vec_t& entry);

        sci_local_segment_t getLocalSegment() const;

        sci_remote_segment_t getRemoteSegment() const;

        const DmaVector& getDmaVector() const;

        sci_dma_queue_t getDmaQueue() const;

        const SegmentPtr getLocalSegmentPtr() const;

    private:
        std::shared_ptr<TransferImpl> impl;
        Transfer(std::shared_ptr<TransferImpl> impl);
};


/* Convenience type for a collection of transfers */
typedef std::vector<TransferPtr> TransferList;


#endif
