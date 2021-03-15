#ifndef OPENIGTLINK_CLASS_H
#define OPENIGTLINK_CLASS_H


using namespace std;


//PUT CLASS DECLARATION HERE

class OpenIGTLink
{
private:

    int    port;
    double fps;
    int    interval;

    igtl::TransformMessage::Pointer transMsg;
    igtl::ServerSocket::Pointer serverSocket;

    int r;

    igtl::Socket::Pointer socket;

    igtl::Matrix4x4 matrix;


public:

    OpenIGTLink(int argc, char* argv[]);

	void GetRandomTestMatrix(igtl::Matrix4x4& matrix);


};

#endif