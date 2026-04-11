#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Server final : public QTcpServer
{
    Q_OBJECT

public: signals:
    void dataSignal(const int identifier, const int row, const int col);

public slots:
    void readyReadSlot();

public:
    explicit Server(QObject* parent = nullptr);

    void send(const int identifier, const int col, const int row);

    void incomingConnection(qintptr socketDescriptor) override;

    [[nodiscard]] bool isReady() const;

    void getReady();

private:
    QTcpSocket* m_client{ nullptr };
    std::array<int, 3>  m_data{};
    QByteArray m_byteArray{};
    bool m_isReady{ false };
    quint16 m_nextBlockSize{ 0 };
};

#endif //SERVER_H