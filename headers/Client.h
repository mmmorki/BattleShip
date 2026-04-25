#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

class QTcpSocket;
class QTimer;

class Client final : public QObject
{
    Q_OBJECT

public: signals:
    void dataSignal(const int identifier, const int row, const int col);

public slots:
    void readyReadSlot();

public:
    explicit Client(const QString& hostAddress, QObject* parent = nullptr);

    void send(const int identifier, const int row, const int col);

    [[nodiscard]] bool isReady() const;

    void getReady();

private:
    QTcpSocket* m_socket{ nullptr };
    std::array<int, 3> m_data{};
    QByteArray m_byteArray{};
    bool m_isReady{ false };
    quint16 m_nextBlockSize{ 0 };
    QTimer* m_connectTimer{ nullptr };
};

#endif //CLIENT_H