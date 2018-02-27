#include "diskcache.h"

#include <sqlite3.h>
#include <stdexcept>

#define DATABASE_DISK_FILE "dns.cache"

class SQLDatabase : public DiskCacheInterface
{
	sqlite3 *db = nullptr;

	// Migrate database statements
	void MigrateDatabase()
	{
		//
	}

	bool IsDatabaseSetup() const noexcept
	{
		return false;
	}

public:
	SQLDatabase()
	{
		int ret = sqlite3_open(DATABASE_DISK_FILE, &db);
		if (ret) {
			throw std::runtime_error{ sqlite3_errmsg(db) };
		}

		// If database was not created, do it now
		if (IsDatabaseSetup()) {
			MigrateDatabase();
		}
	}

    // Commit DNS answer to disk
    void Commit() {}

    // Request DNS answer from disk
    void Request() {}

	~SQLDatabase()
	{
		// Close database if it was open
		if (db) {
			sqlite3_close(db);
		}
	}
};
