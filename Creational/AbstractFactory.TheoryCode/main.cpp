#include "abstract_factory.hpp"
#include <iostream>

using namespace std;

void strongly_coupled_db_client()
{
    // client code that is strongly coupled to specific database engine (e.g. Oracle)
    OracleConnection conn;
    conn.connect();

    OracleCommand cmd;
    DbResult result = cmd.execute(conn, "SELECT * FROM users", {});

    OracleTransaction db_transaction;
    db_transaction.begin(conn);
    DbResult result2 = cmd.execute(conn, "UPDATE users SET name='John' WHERE id=1", {});
    db_transaction.commit();

    conn.disconnect();
}

void decoupled_db_client(DbFactory& db_factory)
{
    // client code that is decoupled from specific database engine
    auto conn = db_factory.create_connection();
    conn->connect();

    auto cmd = db_factory.create_command();
    DbResult result = cmd->execute(*conn, "SELECT * FROM users", {});

    auto db_transaction = db_factory.create_transaction();
    db_transaction->begin(*conn);
    DbResult result2 = cmd->execute(*conn, "UPDATE users SET name='John' WHERE id=1", {});
    db_transaction->commit();

    conn->disconnect();
}

int main()
{    
    strongly_coupled_db_client();

    std::cout << "-----------------------------" << std::endl;

    OracleFactory oracle_factory;
    decoupled_db_client(oracle_factory);

    std::cout << "-----------------------------" << std::endl;

    MySqlFactory mysql_factory;
    decoupled_db_client(mysql_factory);
}