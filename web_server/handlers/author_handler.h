#ifndef AUTHORHANDLER_H
#define AUTHORHANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../database/author.h"
#include <map>

class AuthorHandler : public HTTPRequestHandler
{
private:
    bool check_name(const std::string &name, std::string &reason)
    {
        if (name.length() < 3)
        {
            reason = "Name must be at leas 3 signs";
            return false;
        }

        if (name.find(' ') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        if (name.find('\t') != std::string::npos)
        {
            reason = "Name can't contain spaces";
            return false;
        }

        return true;
    };

    bool check_email(const std::string &email, std::string &reason)
    {
        if (email.find('@') == std::string::npos)
        {
            reason = "Email must contain @";
            return false;
        }

        if (email.find(' ') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        if (email.find('\t') != std::string::npos)
        {
            reason = "EMail can't contain spaces";
            return false;
        }

        return true;
    };

public:
    AuthorHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        std::ostream &ostr = response.send();

        if (form.has("id"))
        {
            long id = atol(form.get("id").c_str());
            try
            {
                database::Author result = database::Author::read_by_id(id);
                Poco::JSON::Stringifier::stringify(result.toJSON(), ostr);
                return;
            }
            catch (...)
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                std::ostream &ostr = response.send();
                ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                response.send();
            }
        }
        else if (form.has("search"))
        {
            try
            {
                std::string fn = form.get("first_name");
                std::string ln = form.get("last_name");
                auto results = database::Author::search(fn, ln);
                Poco::JSON::Array arr;
                for (auto s : results)
                    arr.add(s.toJSON());
                Poco::JSON::Stringifier::stringify(arr, ostr);
            }
            catch (...)
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                std::ostream &ostr = response.send();
                ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                response.send();
            }
            return;
        }
        if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
        {
            if (form.has("first_name"))
                if (form.has("last_name"))
                    if (form.has("email"))
                        if (form.has("title"))
                        {
                            database::Author author;
                            author.first_name() = form.get("first_name");
                            author.last_name() = form.get("last_name");
                            author.email() = form.get("email");
                            author.title() = form.get("title");

                            bool check_result = true;
                            std::string message;
                            std::string reason;

                            if (!check_name(author.get_first_name(), reason))
                            {
                                check_result = false;
                                message += reason;
                                message += "<br>";
                            }

                            if (!check_name(author.get_last_name(), reason))
                            {
                                check_result = false;
                                message += reason;
                                message += "<br>";
                            }

                            if (!check_email(author.get_email(), reason))
                            {
                                check_result = false;
                                message += reason;
                                message += "<br>";
                            }

                            if (check_result)
                            {
                                try
                                {
                                    author.save_to_mysql();
                                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                                    response.setChunkedTransferEncoding(true);
                                    response.setContentType("application/json");
                                    std::ostream &ostr = response.send();
                                    ostr << author.get_id();
                                    return;
                                }
                                catch (...)
                                {
                                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                                    std::ostream &ostr = response.send();
                                    ostr << "database error";
                                    response.send();
                                    return;
                                }
                            }
                            else
                            {
                                response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                                std::ostream &ostr = response.send();
                                ostr << message;
                                response.send();
                                return;
                            }
                        }
        }

        auto results = database::Author::read_all();
        Poco::JSON::Array arr;
        for (auto s : results)
            arr.add(s.toJSON());
        Poco::JSON::Stringifier::stringify(arr, ostr);
    }

private:
    std::string _format;
};
#endif // !AUTHORHANDLER_H