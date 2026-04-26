#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
class QLineEdit;
class ThemeMusicManager;

//Класс окна приложения.
class MainWindow final : public QMainWindow
{
    Q_OBJECT

private slots:
    //Слоты для сигналов завершения ходов игрока.
    void shotsAreOverSlot();
    void allShipsAreDestroyedSlot();

    //Слоты для режима игры по сети.
    void dataSlot(const int identifier, const int row, const int col);
    void playerClickCellOnlineSlot(const int row, const int col) const;
    void playerChangeShipVariantSlot(const int ID) const;
    void playerChangeOrientationSlot(const int ID) const;
    void playerClickCellOnlineSlotOpponent(const int row, const int col) const;

public:
    //Перечисление для страниц приложения.
    enum class Page
    {
        Main,
        ChooseHostOrClient,
        ChooseAddress,
        Connecting,
        Prepare,
        Game,
    };

    //Перечисление для вариантов игры.
    enum class GameVariant
    {
        None,
        Local,
        Server,
        Client,
    };

    //Перечисление для ходов игроков.
    enum class Turn
    {
        None,
        Player1,
        Player2,
    };

    //Конструктор.
    explicit MainWindow();

    //Деструктор по умолчанию.
    ~MainWindow() override = default;

    //Функция настройки звуков приложения.
    void setupSounds() const;

    //Функция настройки m_mainPage.
    void setupMainPage() const;

    //Функция настройки m_hostOrClientPage.
    void setupHostOrClientPage() const;

    //Функция настройки m_chooseAddressPage.
    void setupChooseAddressPage() const;

    //Функция настройки m_connectingPage.
    void setupConnectingPage() const;

    //Функция настройки m_preparePage.
    void setupPreparePage() const;

    //Функция настройки m_gamePage.
    void setupGamePage() const;

    //Функция начала боя после завершения расстановки кораблей.
    void startGame();

    //Функция завершения боя.
    void endGame();

private:
    //Поля перечислений типа игры и хода.
    GameVariant m_gameVariant{ GameVariant::None };
    Turn m_turn{ Turn::None };

    //Поля сокетов клиента и хоста для игры по сети.
    Client* m_client{ nullptr };
    Server* m_server{ nullptr };

    //Поля для звуковых эффектов и фоновой музыки.
    QSoundEffect* m_defeat{ nullptr };
    QSoundEffect* m_victory{ nullptr };
    QSoundEffect* m_menuClick{ nullptr };
    ThemeMusicManager* m_themeMusicManager{ nullptr };

    //Поля, необходимые для окна приложения.
    QWidget* m_central{ nullptr };
    QGridLayout* m_centralLayout{ nullptr };
    QStackedWidget* m_centralStack{ nullptr };

    //Поля, необходимые для главной страницы приложения.
    QWidget* m_mainPage{ nullptr };
    QVBoxLayout* m_mainPageLayout{ nullptr };
    QPushButton* m_chooseLocalBtn{ nullptr };
    QPushButton* m_chooseOnlineBtn{ nullptr };

    //Поля, необходимые для страницы выбора способа игры по сети.
    QWidget* m_hostOrClientPage{ nullptr };
    QVBoxLayout* m_hostOrClientPageLayout{ nullptr };
    QPushButton* m_chooseHostBtn{ nullptr };
    QPushButton* m_chooseClientBtn{ nullptr };
    QPushButton* m_fromHostOrClientToMainBtn{ nullptr };

    //Поля, необходимые для страницы ввода ip-адреса хоста при игре как клиент.
    QWidget* m_chooseAddressPage{ nullptr };
    QLineEdit* m_chooseAddressLine{ nullptr };
    QLabel* m_chooseAddressLabel{ nullptr };
    QPushButton* m_chooseAddressBtn{ nullptr };
    QVBoxLayout* m_chooseAddressLayout{ nullptr };

    //Поля, необходимые для страницы ожидания подключения к оппоненту.
    QWidget* m_connectingPage{ nullptr };
    QGridLayout* m_connectingPageLayout{ nullptr };
    QLabel* m_connectingLabel{ nullptr };

    //Поля, необходимые для страницы расстановки кораблей.
    QWidget* m_preparePage{ nullptr };
    QGridLayout* m_preparePageLayout{ nullptr };
    PlayerField* m_firstPlayerField{ nullptr };
    PlayerField* m_secondPlayerField{ nullptr };
    QPushButton* m_readyBtn{ nullptr };
    QLabel* m_readyLabel{ nullptr };

    //Поля, необходимые для страницы боя.
    QWidget* m_gamePage{ nullptr };
    QGridLayout* m_gamePageLayout{ nullptr };
    OpponentField* m_firstPlayerHiddenField{ nullptr };
    OpponentField* m_secondPlayerHiddenField{ nullptr };
    QLabel* m_whoseTurnLabel{ nullptr };
};

#endif //MAINWINDOW_H