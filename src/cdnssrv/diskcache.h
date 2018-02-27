#pragma once

struct DiskCacheInterface
{
    // Commit DNS answer to disk
    virtual void Commit() = 0;

    // Request DNS answer from disk
    virtual void Request() = 0;
};
