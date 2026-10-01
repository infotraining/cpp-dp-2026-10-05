#ifndef ABSTRACT_FACTORY_HPP_
#define ABSTRACT_FACTORY_HPP_

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

struct DbResult
{
    // Implementation details for database result
};

using SqlCmd = std::string;
using SqlParams = std::vector<std::string>;

class DbConnection
{
public:
    virtual ~DbConnection() = default;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
};

class DbCommand
{
public:
    virtual ~DbCommand() = default;
    virtual DbResult execute(DbConnection& conn, const SqlCmd& cmd, const SqlParams& params) = 0;
};

class DbTransaction
{
public:
    virtual ~DbTransaction() = default;
    virtual void begin(DbConnection& conn) = 0;
    virtual void commit() = 0;
};

/////////////////////////////////////////////////////////
// Family of related products for Oracle database engine

class OracleConnection : public DbConnection
{
public:
    void connect() override
    {
        std::cout << "Connecting to Oracle database..." << std::endl;
    }

    void disconnect() override
    {
        std::cout << "Disconnecting from Oracle database..." << std::endl;
    }
};

class OracleCommand : public DbCommand
{
public:
    DbResult execute(DbConnection& conn, const SqlCmd& cmd, const SqlParams& params) override
    {
        // execute command on Oracle database and return result
        std::cout << "Executing command on Oracle database: " << cmd << std::endl;
        return DbResult{};
    }
};

class OracleTransaction : public DbTransaction
{
public:
    void begin(DbConnection& conn) override
    {
        // begin transaction on Oracle database
        std::cout << "Beginning transaction on Oracle database..." << std::endl;
    }

    void commit() override
    {
        // commit transaction on Oracle database
        std::cout << "Committing transaction on Oracle database..." << std::endl;
    }
};

/////////////////////////////////////////////////////////
// Family of related products for MySQL database engine
class MySqlConnection : public DbConnection
{
public:
    void connect() override
    {
        std::cout << "Connecting to MySQL database..." << std::endl;
    }

    void disconnect() override
    {
        std::cout << "Disconnecting from MySQL database..." << std::endl;
    }
};

class MySqlCommand : public DbCommand
{
public:
    DbResult execute(DbConnection& conn, const SqlCmd& cmd, const SqlParams& params) override
    {
        // execute command on MySQL database and return result
        std::cout << "Executing command on MySQL database: " << cmd << std::endl;
        return DbResult{};
    }
};

class MySqlTransaction : public DbTransaction
{
public:
    void begin(DbConnection& conn) override
    {
        // begin transaction on MySQL database
        std::cout << "Beginning transaction on MySQL database..." << std::endl;
    }

    void commit() override
    {
        // commit transaction on MySQL database
        std::cout << "Committing transaction on MySQL database..." << std::endl;
    }
};

/////////////////////////////////////////////////////////
// Abstract Factory interface for creating related products
class DbFactory
{
public:
    virtual ~DbFactory() = default;
    virtual std::unique_ptr<DbConnection> create_connection() = 0;
    virtual std::unique_ptr<DbCommand> create_command() = 0;
    virtual std::unique_ptr<DbTransaction> create_transaction() = 0;
};

// Concrete Factory for Oracle database engine
class OracleFactory : public DbFactory
{
public:
    std::unique_ptr<DbConnection> create_connection() override
    {
        return std::make_unique<OracleConnection>();
    }

    std::unique_ptr<DbCommand> create_command() override
    {
        return std::make_unique<OracleCommand>();
    }

    std::unique_ptr<DbTransaction> create_transaction() override
    {
        return std::make_unique<OracleTransaction>();
    }
};

// Concrete Factory for MySQL database engine
class MySqlFactory : public DbFactory
{
public:
    std::unique_ptr<DbConnection> create_connection() override
    {
        return std::make_unique<MySqlConnection>();
    }

    std::unique_ptr<DbCommand> create_command() override
    {
        return std::make_unique<MySqlCommand>();
    }

    std::unique_ptr<DbTransaction> create_transaction() override
    {
        return std::make_unique<MySqlTransaction>();
    }
};

#endif // ABSTRACT_FACTORY_HPP_