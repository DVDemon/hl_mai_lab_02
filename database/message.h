#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database
{
    class Message{
        private:
            long _id;
            long _id_from;
            long _id_to;
            std::string _message;

        public:
            static Message fromJSON(const std::string & str);

            long               get_id() const;
            long               get_id_from() const;
            long               get_id_to() const;
            const std::string &get_message() const;

            long&        id();
            long&        id_from();
            long&        id_to();
            std::string &message();

            static void init();
            static std::vector<Message> read_all(long from,long to);
            static std::vector<long> all_contact(long id);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif