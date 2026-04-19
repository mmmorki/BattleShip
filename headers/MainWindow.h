#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

class PlayerField;
class OpponentField;
class Server;
class Client;

class QGridLayout;
class QStackedWidget;
class QVBoxLayout;
class QPushButton;
class QLabel;
class QHBoxLayout;
class QMessageBox;
class QTcpSocket;
class QSoundEffect;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

private slots:
    void shotsAreOverSlot();
    void allShipsAreDestroyedSlot();
    void dataSlot(const int identifier, const int col, const int row);
    //Слоты для онлайн режима
    void playerClickCellOnlineSlot(const int row, const int col);
    void playerChangeShipVariantSlot(const int ID);
    void playerChangeOrientationSlot(const int ID);
    void playerClickCellOnlineSlotOpponent(const int row, const int col);

public:
    enum class Page
    {
        Main,
        ChooseHostOrClient,
        Prepare,
        Game,
        Connecting,
    };

    enum class GameVariant
    {
        None,
        Local,
        Server,
        Client,
    };

    enum class Turn
    {
        None,
        Player1,
        Player2,
    };

    explicit MainWindow();

    ~MainWindow() override = default;

    void startGame();

private:
    GameVariant m_gameVariant{ GameVariant::None };
    Turn m_turn{ Turn::None };
    QWidget* m_central{ nullptr };
    QGridLayout* m_centralLayout{ nullptr };
    QStackedWidget* m_centralStack{ nullptr };
    QWidget* m_mainPage{ nullptr };
    QVBoxLayout* m_mainPageLayout{ nullptr };
    QPushButton* m_chooseLocalBtn{ nullptr };
    QPushButton* m_chooseOnlineBtn{ nullptr };
    QWidget* m_hostOrClientPage{ nullptr };
    QVBoxLayout* m_hostOrClientPageLayout{ nullptr };
    QPushButton* m_chooseHostBtn{ nullptr };
    QPushButton* m_chooseClientBtn{ nullptr };
    PlayerField* m_firstPlayerField{ nullptr };
    OpponentField* m_firstPlayerHiddenField{ nullptr };
    PlayerField* m_secondPlayerField{ nullptr };
    OpponentField* m_secondPlayerHiddenField{ nullptr };
    QPushButton* m_fromHostOrOnlineToMainBtn{ nullptr };
    QWidget* m_preparePage{ nullptr };
    QGridLayout* m_preparePageLayout{ nullptr };
    QPushButton* m_readyBtn{ nullptr };
    QWidget* m_gamePage{ nullptr };
    QGridLayout* m_gamePageLayout{ nullptr };
    Client* m_client{ nullptr };
    Server* m_server{ nullptr };
    QLabel* m_readyLabel{ nullptr };
    QWidget* m_connectingPage{ nullptr };
    QGridLayout* m_connectingPageLayout{ nullptr };
    QLabel* m_connectingLabel{ nullptr };
    QLabel* m_whoseTurnLabel{ nullptr };
    QSoundEffect* m_defeat{ nullptr };
    QSoundEffect* m_victory{ nullptr };
};

#endif //MAINWINDOW_H