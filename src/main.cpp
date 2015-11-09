
#include <opendnp3/decoder/Decoder.h>
#include <opendnp3/LogLevels.h>

#include <openpal/logging/LogRoot.h>
#include <openpal/container/Buffer.h>

#include <cgicc/Cgicc.h>
#include <cgicc/HTTPHTMLHeader.h>
#include <cgicc/HTMLClasses.h>
#include <stdexcept>

#include "HexData.h"
#include "DecodeHandler.h"

using namespace std;
using namespace cgicc;
using namespace openpal;
using namespace opendnp3;

enum class Mode : uint8_t
{
    Link,
    Transport,
    App
};

Mode GetMode(Cgicc& cgi);
std::string GetHex(Cgicc& cgi);

int main(int argc, char* argv[])
{
    // Set up the HTML document
    cout << HTTPHTMLHeader() << endl;
    cout << html() << head(title("decoded output")) << endl;
    cout << body() << endl;

    try
    {
        Cgicc cgi;
        const auto MODE = GetMode(cgi);
        const auto HEX = GetHex(cgi);

        DecodeHandler handler;
        openpal::LogRoot log(&handler, "decoder", LogFilters(~0));
        Decoder decoder(handler, log.GetLogger());

        HexData hex(HEX, true);

        switch(MODE)
        {
            case(Mode::Link):
                decoder.DecodeLPDU(hex.GetSlice());
                break;
            case(Mode::Transport):
                decoder.DecodeTPDU(hex.GetSlice());
                break;
            default:
                decoder.DecodeAPDU(hex.GetSlice());
                break;
        }
    }
    catch(const std::exception& e)
    {
        cout << "<pre class=\"error\">" << "An exception occured: " << "</pre>" << endl;
        cout << "<pre class=\"error\">" << e.what() << "</pre>" << endl;
    }

    // Close the HTML document
    cout << body() << html();

    return 0;
}

Mode GetMode(Cgicc& cgi)
{
    auto inputMode = cgi.getElement("mode");
    if(inputMode == cgi.getElements().end())
    {
        throw std::invalid_argument("The decoder mode was not specified");
    }

    const auto VALUE = **inputMode;

    if(VALUE == "app") return Mode::App;
    if(VALUE == "transport") return Mode::Transport;
    return Mode::Link;
}

std::string GetHex(Cgicc& cgi)
{
    auto hex = cgi.getElement("hex");
    if(hex == cgi.getElements().end())
    {
        throw std::invalid_argument("The input hex was not specified");
    }

    return **hex;
}