#include "message.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>
#include <algorithm>
#include <future>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;
namespace database
{
    Message Message::fromJSON(const std::string &str)
    {
        Message msg;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        msg.id() = object->getValue<long>("id");
        msg.id_from() = object->getValue<long>("id_from");
        msg.id_to() = object->getValue<long>("id_to");
        msg.message() = object->getValue<std::string>("message");

        return msg;
    }

    long Message::get_id() const
    {
        return _id;
    }

    long Message::get_id_from() const
    {
        return _id_from;
    }

    long Message::get_id_to() const
    {
        return _id_to;
    }

    const std::string &Message::get_message() const
    {
        return _message;
    }

    long &Message::id()
    {
        return _id;
    }

    long &Message::id_from()
    {
        return _id_from;
    }

    long &Message::id_to()
    {
        return _id_to;
    }

    std::string &Message::message()
    {
        return _message;
    }

    void Message::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            for (auto &hint : database::Database::get_all_hints())
            {
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS Message (id INT NOT NULL AUTO_INCREMENT,"
                        << "id_from INT NOT NULL,"
                        << "id_to INT NOT NULL,"
                        << "message VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,"
                        << "PRIMARY KEY (id),KEY ft (id_from,id_to));"
                        << hint,
                        now;

             std::cout << create_stmt.toString() << std::endl;
            }
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    std::vector<long> Message::all_contact(long id)
    {
        std::vector<long> result;
        // get all hints for shards
        std::vector<std::string> hints = database::Database::get_all_hints();

        std::vector<std::future<std::vector<long>>> futures;

        // map phase in parallel
        for (const std::string &hint : hints)
        {
            auto handle = std::async(std::launch::async, [id, hint]() -> std::vector<long>
                                     {
                                         std::vector<long> result;
                                         Poco::Data::Session session = database::Database::get().create_session();
                                         Statement select(session);
                                         std::string select_str = "SELECT id_from, id_to, message FROM Message WHERE id_from=";
                                         select_str += std::to_string(id);
                                         select_str += " OR id_to=";
                                         select_str += std::to_string(id);
                                         select_str += hint;
                                         select << select_str;

                                         select.execute();
                                         Poco::Data::RecordSet record_set(select);

                                         bool more = record_set.moveFirst();
                                         while (more)
                                         {
                                             long id_from = record_set[0].convert<long>();
                                             long id_to = record_set[1].convert<long>();
                                             if (id_from != id)
                                                 result.push_back(id_from);
                                             if (id_to != id)
                                                 result.push_back(id_to);
                                             more = record_set.moveNext();
                                         }
                                         return result; });

            futures.emplace_back(std::move(handle));
        }

        // reduce phase
        // get values
        for (std::future<std::vector<long>> &res : futures)
        {
            std::vector<long> v = res.get();
            std::copy(std::begin(v),
                      std::end(v),
                      std::back_inserter(result));
        }

        // sort
        std::vector<long> result_distincted;
        std::sort(std::begin(result), std::end(result));
        long old = -1;

        // deduplicate
        std::copy_if(std::begin(result), std::end(result), std::back_inserter(result_distincted),
                     [&old](long x)
                     {
                         if (x != old)
                         {
                             old = x;
                             return true;
                         }
                         else
                             return false;
                     });

        return result_distincted;
    }

    std::vector<Message> Message::read_all(long from, long to)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Message> result;
            Message a;
            std::string sharding_hint = database::Database::sharding_hint(from, to);
            std::string select_str = "SELECT id, id_from, id_to, message FROM Message WHERE id_from=? AND id_to=? ";
            select_str += sharding_hint;
            std::cout << select_str << std::endl;

            select << select_str,
                into(a._id),
                into(a._id_from),
                into(a._id_to),
                into(a._message),
                use(from),
                use(to),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Message::save_to_mysql()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);
            std::string sharding_hint = database::Database::sharding_hint(_id_from, _id_to);

            std::string select_str = "INSERT INTO Message (id_from,id_to,message) VALUES(?, ?, ?) ";
            select_str += sharding_hint;
            std::cout << select_str << std::endl;

            insert << select_str,
                use(_id_from),
                use(_id_to),
                use(_message),
                now;

            Poco::Data::Statement select(session);
            std::string query =  "SELECT LAST_INSERT_ID() "+sharding_hint;
            select << query,
                into(_id),
                range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Message::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("id_from", _id_from);
        root->set("id_to", _id_to);
        root->set("message", _message);
        return root;
    }
}