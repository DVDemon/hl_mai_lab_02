#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

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

#include "../../database/message.h"

class MessageHandler : public HTTPRequestHandler
{
public:
    MessageHandler(const std::string &format) : _format(format)
    {
    }

    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());

        if (form.has("id_from") && form.has("id_to")  && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
        {
            long id_from = atol(form.get("id_from").c_str());
            long id_to = atol(form.get("id_to").c_str());
            try
            {
                Poco::JSON::Array arr;
                auto results = database::Message::read_all(id_from, id_to);
                for (auto s : results)
                    arr.add(s.toJSON());
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                std::ostream &ostr = response.send();

                Poco::JSON::Stringifier::stringify(arr, ostr);
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
        else if (form.has("contact_id") && (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET))
        {
            long id = atol(form.get("contact_id").c_str());
            try
            {
                std::vector<long> results = database::Message::all_contact(id);
                if (!results.empty())
                {
                    std::string result_str = "[";
                    for (size_t i = 0; i < results.size(); ++i)
                    {
                        result_str += "{ \"id\" :";
                        result_str += std::to_string(results[i]);
                        result_str += "}";
                        if (i != results.size() - 1)
                            result_str += ",";
                    }
                    result_str += "]";
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();

                    ostr << result_str;
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    std::ostream &ostr = response.send();
                    ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                    response.send();
                }
            }
            catch (...)
            {
                response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                std::ostream &ostr = response.send();
                ostr << "{ \"result\": false , \"reason\": \"not found\" }";
                response.send();
            }
        }
        else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
        {
            if (form.has("id_from"))
                if (form.has("id_to"))
                    if (form.has("message"))
                    {
                        database::Message message;
                        message.id_from() = atol(form.get("id_from").c_str());
                        message.id_to() = atol(form.get("id_to").c_str());
                        message.message() = form.get("message");

                        try
                        {
                            message.save_to_mysql();
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            std::ostream &ostr = response.send();
                            ostr << message.get_id();
                            return;
                        }
                        catch (...)
                        {
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                            std::ostream &ostr = response.send();
                            ostr << "{ \"result\": false , \"reason\": \"exception\" }";
                            response.send();
                        }
                    }
        }
    }

private:
    std::string _format;
};
#endif