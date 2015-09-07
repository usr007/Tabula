#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Button.h"
#include "Form.h"
#include "Input.h"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

io_service ioservice;
tcp::endpoint tcp_endpoint {tcp::v4(), 1967};
tcp::acceptor tcp_acceptor {ioservice, tcp_endpoint};

void do_write(tcp::socket& tcp_socket,yield_context yield) {
    Form form {"test.js"};
    Button button {"Syltetøj"};
    Input field {"text"};
    string content = "";
    content += "Blåbærsyltetøj er <b>godt</b><br>";
    content += form.getOpeningElement();
    content += button.getElement();
    content += field.getElement();
    content += form.getClosingElement();
    const string cr = "\r\n";
    const string OK = "200 OK";
    const string text = "text/html;";
    const string charset = "charset=utf-8";

    string data = "";
    data += "HTTP/1.1 " + OK + cr;
    data += "Server: Tabula content server/0.0.1" + cr;
    data += "Content-Length: " + to_string(content.length()) + cr;
    data += "Connnection: close" + cr;
    data += "Content-Type: " + text + " " + charset + cr + cr;
    data += content;
    async_write(tcp_socket, buffer(data), yield);
    tcp_socket.shutdown(tcp::socket::shutdown_send);
}

void do_accept(yield_context yield) {
    for (int i = 0; i < 5; ++i) {
        tcp::socket *tcp_socket = new tcp::socket {ioservice};
        tcp_acceptor.async_accept(*tcp_socket, yield);
        spawn(ioservice, [&] (yield_context yieldContext) { do_write(*tcp_socket, yieldContext); });
    }
}

int main() {
    cout << "Hello, World!" << endl;
    tcp_acceptor.listen();
    spawn(ioservice, do_accept);
    ioservice.run();
    return 0;
}
