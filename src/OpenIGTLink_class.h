#ifndef OPENIGTLINK_CLASS_H
#define OPENIGTLINK_CLASS_H


#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"

using namespace std;


//PUT CLASS DECLARATION HERE

class OpenIGTLink
{
private:

    int    m_port;
    double m_fps;
    int    interval;

    igtl::TransformMessage::Pointer transMsg;
    igtl::ServerSocket::Pointer serverSocket;

    int r;

    igtl::Socket::Pointer socket;

    igtl::Matrix4x4 matrix;

    float m_position[3];
    float m_orientation[4];

    igtl::Matrix4x4 m_matrix;

    void GetRandomTestMatrix(igtl::Matrix4x4& matrix);

    void OpenIGTLink::CreateMatrix(vector <double> pando);



public:

    OpenIGTLink(int port, float fps);



    void SendMessage(vector<double> pando);


    //int CloseSocket();


};

#endif