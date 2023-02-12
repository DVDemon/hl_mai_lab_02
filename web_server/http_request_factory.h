#ifndef HTTPREQUESTFACTORY_H
#define HTTPREQUESTFACTORY_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
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

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "handlers/author_handler.h"
#include "handlers/message_handler.h"
#include "handlers/web_page_handler.h"

static bool startsWith(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

class HTTPRequestFactory : public HTTPRequestHandlerFactory
{
public:
    HTTPRequestFactory(const std::string &format) : _format(format)
    {
    }

    HTTPRequestHandler *createRequestHandler(
        const HTTPServerRequest &request)
    {
        static const std::string author = "/author";
        static const std::string message = "/message";
        static const std::string contacts = "/contacts";
        if (startsWith(request.getURI(), author))
            return new AuthorHandler(_format);
        if (startsWith(request.getURI(), message))
            return new MessageHandler(_format);
        if (startsWith(request.getURI(), contacts))
            return new MessageHandler(_format);
        return new WebPageHandler(_format);
        return 0;
    }

private:
    std::string _format;
};

#endif