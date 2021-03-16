/*=========================================================================

  Program:   OpenIGTLink -- Example for Tracker Server Program
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <vector>

#include "igtlOSUtil.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"
#include "OpenIGTLink_class.h"



OpenIGTLink::OpenIGTLink(int port, float fps)
{
    //port = atoi(argv[1]);
    //fps = atof(argv[2]);
    m_port = port;
    m_fps = fps;

    interval = (int)(1000.0 / fps);


    //igtl::TransformMessage::Pointer transMsg;
    transMsg = igtl::TransformMessage::New();
    transMsg->SetDeviceName("Tracker");

    //igtl::ServerSocket::Pointer serverSocket;
    serverSocket = igtl::ServerSocket::New();
    r = serverSocket->CreateServer(port);

    //igtl::Socket::Pointer socket;

   

    //while (1)
    //{
        //------------------------------------------------------------
        // Waiting for Connection
        //socket = serverSocket->WaitForConnection(1000);

        //if (socket.IsNotNull()) // if client connected
        //{
        //    //------------------------------------------------------------
        //    // loop
        //    for (int i = 0; i < 10; i++)
        //    {
        //        //igtl::Matrix4x4 matrix;
        //        GetRandomTestMatrix(matrix);
        //        transMsg->SetDeviceName("Tracker");
        //        transMsg->SetMatrix(matrix);
        //        transMsg->Pack();
        //        socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
        //        igtl::Sleep(interval); // wait
        //    }
        //}


    //}

    //------------------------------------------------------------
    // Close connection (The example code never reachs to this section ...)

    //socket->CloseSocket();

}

void OpenIGTLink::SendMessage(vector <double> pando)
{

    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);

    if (socket.IsNotNull()) // if client connected
    {
        
        for (int i = 0; i < 10; i++)
        {
            //igtl::Matrix4x4 matrix;
            //GetRandomTestMatrix(matrix);
            CreateMatrix(pando);

            transMsg->SetDeviceName("Tracker");
            //transMsg->SetMatrix(matrix);
            transMsg->SetMatrix(m_matrix);

            transMsg->Pack();

            socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());

            igtl::Sleep(interval); // wait
        }
    }


}



void OpenIGTLink::CreateMatrix(vector <double> pando)
{
    //  First column
    m_matrix[0][0] = cos(pando[4]) * cos(pando[3]);
    m_matrix[1][0] = sin(pando[4]) * cos(pando[3]);
    m_matrix[2][0] = -sin(pando[3]);
    m_matrix[3][0] = 0;

    //  Second column
    m_matrix[0][1] = -sin(pando[4]);
    m_matrix[1][1] = cos(pando[4]);
    m_matrix[2][1] = 0;
    m_matrix[3][1] = 0;

    //  Third column
    m_matrix[0][2] = cos(pando[4]) * sin(pando[3]);
    m_matrix[1][2] = sin(pando[4]) * sin(pando[3]);
    m_matrix[2][2] = cos(pando[3]);
    m_matrix[3][2] = 0;

    // Fourth column
    m_matrix[0][3] = pando[0];
    m_matrix[1][3] = pando[1];
    m_matrix[2][3] = pando[2];
    m_matrix[3][3] = 1;

    igtl::PrintMatrix(m_matrix);

}



void OpenIGTLink::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
{
    float position[3];
    float orientation[4];

    // random position
    static float phi = 0.0;
    position[0] = 50.0 * cos(phi);
    position[1] = 50.0 * sin(phi);
    position[2] = 50.0 * cos(phi);
    phi = phi + 0.2;

    // random orientation
    static float theta = 0.0;
    orientation[0] = 0.0;
    orientation[1] = 0.6666666666 * cos(theta);
    orientation[2] = 0.577350269189626;
    orientation[3] = 0.6666666666 * sin(theta);
    theta = theta + 0.1;

    //igtl::Matrix4x4 matrix;
    igtl::QuaternionToMatrix(orientation, matrix);

    matrix[0][3] = position[0];
    matrix[1][3] = position[1];
    matrix[2][3] = position[2];

    igtl::PrintMatrix(matrix);
}

//int OpenIGTLink::CloseSocket()
//{
//    cout << "-> CLOSING SOCKET" << endl;
//    socket->CloseSocket();
//
//    return 0;
//}


