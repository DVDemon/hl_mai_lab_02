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

#include "database/user.h"
#include "database/message.h"

#include "web_server/handlers/user_handler.h"
#include "web_server/handlers/message_handler.h"
#include "web_server/handlers/web_page_handler.h"

class HTTPRequestFactory : public HTTPRequestHandlerFactory
{
public:
    HTTPRequestFactory(const std::string &format) : _format(format)
    {
    }

    HTTPRequestHandler *createRequestHandler(
        const HTTPServerRequest &request)
    {

        if (hasSubstr(request.getURI(),"/user") ||
            hasSubstr(request.getURI(),"/search") ||
            hasSubstr(request.getURI(),"/auth")) 
            return new UserHandler(_format);

        if (hasSubstr(request.getURI(), "/message")||
            hasSubstr(request.getURI(), "/contacts"))
            return new MessageHandler(_format);
        
        return new WebPageHandler(_format);
    }

private:
    std::string _format;
};

class HTTPWebServer : public Poco::Util::ServerApplication
{

protected:

    int main([[maybe_unused]] const std::vector<std::string> &args)
    {
            database::User::init();    
            database::Message::init();            
            ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8080));
            HTTPServer srv(new HTTPRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
            srv.start();
            waitForTerminationRequest();
            srv.stop();
        return Application::EXIT_OK;
    }

};

int main(int argc, char *argv[])
{
    HTTPWebServer app;
    return app.run(argc, argv);
}