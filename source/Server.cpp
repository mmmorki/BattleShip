#include "Server.h"

#include <iostream>

#include <QTcpSocket>

Server::Server(QObject* parent)
    : QTcpServer(parent)
{
    this->listen(QHostAddress::Any, 2323);
    std::cerr << "server listening on port 2323" << std::endl;
}

void Server::send(const int identifier, const int col, const int row)
{
    m_byteArray.clear();

    QDataStream out{ &m_byteArray, QIODevice::WriteOnly };
    out.setVersion(QDataStream::Qt_6_10);

    const int num{ identifier * 100 + col * 10 + row };
    out << quint16(0) << num;
    out.device()->seek(0);
    out << quint16(m_byteArray.size() - sizeof(quint16));

    m_client->write(m_byteArray);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    m_client = new QTcpSocket{};
    m_client->setSocketDescriptor(socketDescriptor);
    connect(m_client, &QTcpSocket::readyRead, this, &Server::readyReadSlot);
    connect(m_client, &QTcpSocket::disconnected, m_client, &QTcpSocket::deleteLater);
    std::cerr << "client connected" << std::endl;
    emit dataSignal(0, 0, 1);
}

bool Server::isReady() const
{
    return m_isReady;
}

void Server::getReady()
{
    m_isReady = true;
}

void Server::readyReadSlot()
{
    QDataStream in(m_client);
    in.setVersion(QDataStream::Qt_6_10);

    for (;;)
    {
        if (m_nextBlockSize == 0)
        {
            if (m_client->bytesAvailable() < 2)
                break;

            in >> m_nextBlockSize;
        }

        if (m_client->bytesAvailable() < m_nextBlockSize)
            break;

        int incomingNum{ 0 };
        in >> incomingNum;

        m_data[0] = incomingNum / 100;
        m_data[1] = incomingNum % 100 / 10;
        m_data[2] = incomingNum % 10;

        emit dataSignal(m_data[0], m_data[1], m_data[2]);

        m_nextBlockSize = 0;
    }
}