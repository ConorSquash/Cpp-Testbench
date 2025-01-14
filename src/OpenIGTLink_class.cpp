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

    m_port = port;
    m_fps = fps;

    interval = (int)(1000.0 / fps);


    transMsg = igtl::TransformMessage::New();
    transMsg->SetDeviceName("Tracker");

    serverSocket = igtl::ServerSocket::New();
    r = serverSocket->CreateServer(port);

    // Waiting for Connection
    socket = serverSocket->WaitForConnection(1000);

}

void OpenIGTLink::SendIGTMessage(vector <double> pando)
{



    if (socket.IsNotNull()) // if client connected
    {
        
            CreateMatrix(pando);

            transMsg->SetDeviceName("Tracker");

            transMsg->SetMatrix(m_matrix);

            transMsg->Pack();

            socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());

            //igtl::Sleep(interval); // wait
        
    }


}



void OpenIGTLink::CreateMatrix(vector <double> pando)
{
    //  First column
    m_matrix[0][0] = -cos(pando[4]) * cos(pando[3]);
    m_matrix[1][0] = -sin(pando[4]) * cos(pando[3]);
    m_matrix[2][0] = -sin(pando[3]);
    m_matrix[3][0] = 0;

    //  Second column
    m_matrix[0][1] = sin(pando[4]);
    m_matrix[1][1] = -cos(pando[4]);
    m_matrix[2][1] = 0;
    m_matrix[3][1] = 0;

    //  Third column
    m_matrix[0][2] = -cos(pando[4]) * sin(pando[3]);
    m_matrix[1][2] = -sin(pando[4]) * sin(pando[3]);
    m_matrix[2][2] = cos(pando[3]);
    m_matrix[3][2] = 0;

    // Fourth column
    m_matrix[0][3] = -pando[0] * 1000;
    m_matrix[1][3] = -pando[1] * 1000;
    m_matrix[2][3] = pando[2] * 1000;
    m_matrix[3][3] = 1;

    //igtl::PrintMatrix(m_matrix);

}




