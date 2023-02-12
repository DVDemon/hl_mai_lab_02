#ifndef AUTHOR_H
#define AUTHOR_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"

namespace database
{
    class Author{
        private:
            long _id;
            std::string _first_name;
            std::string _last_name;
            std::string _email;
            std::string _title;

        public:

            static Author fromJSON(const std::string & str);
            static void preload(const std::string & str);

            long               get_id() const;
            const std::string &get_first_name() const;
            const std::string &get_last_name() const;
            const std::string &get_email() const;
            const std::string &get_title() const;

            long&        id();
            std::string &first_name();
            std::string &last_name();
            std::string &email();
            std::string &title();

            static void init();
            static Author read_by_id(long id);
            static std::vector<Author> read_all();
            static std::vector<Author> search(std::string first_name,std::string last_name);
            void save_to_mysql();

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif