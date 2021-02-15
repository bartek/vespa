// Copyright 2017 Yahoo Holdings. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "indexflushtarget.h"
#include <vespa/vespalib/util/size_literals.h>

#include <vespa/log/log.h>
LOG_SETUP(".searchcorespi.index.indexflushtarget");

namespace searchcorespi::index {

IndexFlushTarget::IndexFlushTarget(IndexMaintainer &indexMaintainer, IndexMaintainer::FlushStats flushStats)
    : IFlushTarget("memoryindex.flush", Type::FLUSH, Component::INDEX),
      _indexMaintainer(indexMaintainer),
      _flushStats(flushStats),
      _numFrozenMemoryIndexes(indexMaintainer.getNumFrozenMemoryIndexes()),
      _maxFrozenMemoryIndexes(indexMaintainer.getMaxFrozenMemoryIndexes()),
      _lastStats()
{
    _lastStats.setPathElementsToLog(7);
}

IndexFlushTarget::IndexFlushTarget(IndexMaintainer &indexMaintainer)
    : IndexFlushTarget(indexMaintainer, indexMaintainer.getFlushStats())
{}

IndexFlushTarget::~IndexFlushTarget() = default;

IFlushTarget::MemoryGain
IndexFlushTarget::getApproxMemoryGain() const
{
    return MemoryGain(_flushStats.memory_before_bytes, _flushStats.memory_after_bytes);
}

IFlushTarget::DiskGain
IndexFlushTarget::getApproxDiskGain() const
{
    return DiskGain(0, 0);
}

bool
IndexFlushTarget::needUrgentFlush() const
{
    // Due to limitation of 16G address space of single datastore
    // TODO: Even better if urgency was decided by memory index itself.
    constexpr int64_t G =1_Gi;
    bool urgent = (_numFrozenMemoryIndexes > _maxFrozenMemoryIndexes) ||
                  (getApproxMemoryGain().gain() > 16*G);
    SerialNum flushedSerial = _indexMaintainer.getFlushedSerialNum();
    LOG(debug, "Num frozen: %u Memory gain: %ld Urgent: %d, flushedSerial=%" PRIu64,
        _numFrozenMemoryIndexes, getApproxMemoryGain().gain(), static_cast<int>(urgent), flushedSerial);
    return urgent;
}

IFlushTarget::Time
IndexFlushTarget::getLastFlushTime() const
{
    return _indexMaintainer.getLastFlushTime();
}

IFlushTarget::SerialNum
IndexFlushTarget::getFlushedSerialNum() const
{
    return _indexMaintainer.getFlushedSerialNum();
}

IFlushTarget::Task::UP
IndexFlushTarget::initFlush(SerialNum serialNum, std::shared_ptr<search::IFlushToken>)
{
    // the target must live until this task is done (handled by flush engine).
    return _indexMaintainer.initFlush(serialNum, &_lastStats);
}

uint64_t
IndexFlushTarget::getApproxBytesToWriteToDisk() const
{
    MemoryGain gain(_flushStats.memory_before_bytes, _flushStats.memory_after_bytes);
    if (gain.getAfter() < gain.getBefore()) {
        return gain.getBefore() - gain.getAfter();
    } else {
        return 0;
    }
}

}
