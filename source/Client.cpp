#include "Client.h"

#include <QTcpSocket>

Client::Client(QObject* parent)
    : QObject(parent)
    , m_socket{ new QTcpSocket{ this } }
{
    m_socket->connectToHost("127.0.0.1", 2323);

    connect(m_socket, &QTcpSocket::readyRead, this, &Client::readyReadSlot);
    connect(m_socket, &QTcpSocket::disconnected, m_socket, &QTcpSocket::deleteLater);
}

void Client::send(const int identifier, const int row, const int col)
{
    m_byteArray.clear();

    QDataStream out{ &m_byteArray, QIODevice::WriteOnly };
    out.setVersion(QDataStream::Qt_6_10);

    const int num{ identifier * 100 + row * 10 + col };
    out << quint16(0) << num;
    out.device()->seek(0);
    out << quint16(m_byteArray.size() - sizeof(quint16));

    m_socket->write(m_byteArray);
}

bool Client::isReady() const
{
    return m_isReady;
}

void Client::getReady()
{
    m_isReady = true;
}

void Client::readyReadSlot()
{
    QDataStream in{ m_socket };
    in.setVersion(QDataStream::Qt_6_10);

    while (true)
    {
        if (m_nextBlockSize == 0)
        {
            if (m_socket->bytesAvailable() < 2)
                break;

            in >> m_nextBlockSize;
        }

        if (m_socket->bytesAvailable() < m_nextBlockSize)
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