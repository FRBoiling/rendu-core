//
// Created by boil on 2026/1/15.
//

#include "common/ecs/registry_thread_safe.h"

BEGIN_NAMESPACE_ECS

    // ============================================================================
    // ReadWriteLock 实现
    // ============================================================================

    ReadWriteLock::ReadWriteLock() = default;

    void ReadWriteLock::lockRead()
    {
        m_mutex.lock_shared();
    }

    void ReadWriteLock::unlockRead()
    {
        m_mutex.unlock_shared();
    }

    void ReadWriteLock::lockWrite()
    {
        m_mutex.lock();
    }

    void ReadWriteLock::unlockWrite()
    {
        m_mutex.unlock();
    }

    // ============================================================================
    // ReadLockGuard 实现
    // ============================================================================

    ReadLockGuard::ReadLockGuard(ReadWriteLock& lock)
        : m_lock(lock)
    {
        m_lock.lockRead();
    }

    ReadLockGuard::~ReadLockGuard()
    {
        m_lock.unlockRead();
    }

    // ============================================================================
    // WriteLockGuard 实现
    // ============================================================================

    WriteLockGuard::WriteLockGuard(ReadWriteLock& lock)
        : m_lock(lock)
    {
        m_lock.lockWrite();
    }

    WriteLockGuard::~WriteLockGuard()
    {
        m_lock.unlockWrite();
    }

    // ============================================================================
    // RegistryThreadSafe 实现
    // ============================================================================

    RegistryThreadSafe::RegistryThreadSafe() = default;

    RegistryThreadSafe::~RegistryThreadSafe() = default;

    Entity RegistryThreadSafe::create()
    {
        WriteLockGuard lock(m_lock);
        return m_registry.create();
    }

    void RegistryThreadSafe::destroy(Entity entity)
    {
        WriteLockGuard lock(m_lock);
        m_registry.destroy(entity);
    }

    bool RegistryThreadSafe::valid(Entity entity) const
    {
        ReadLockGuard lock(m_lock);
        return m_registry.valid(entity);
    }


END_NAMESPACE_ECS

