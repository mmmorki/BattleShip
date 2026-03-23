#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

class PlayerField;
class OpponentField;

class QGridLayout;
class QStackedWidget;
class QVBoxLayout;
class QPushButton;
class QLabel;
class QHBoxLayout;
class QMessageBox;

class MainWindow final : public QMainWindow
{
    Q_OBJECT

private slots:
    void shotsAreOverSlot();
    void allShipsAreDestroyedSlot();

public:
    enum class Page
    {
        Main,
        ChooseHostOrClient,
        Prepare,
        Game,
    };

    enum class GameVariant
    {
        None,
        Local,
        Online,
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
};

#endif //MAINWINDOW_H