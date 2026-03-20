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
public:
    enum class Page
    {
        Main,
        ChooseHostOrClient,
        FirstPlayerGame,
        SecondPlayerGame,
        ChangePlayer,
    };

    enum class GameVariant
    {
        None,
        Local,
        Online,
    };

    explicit MainWindow();

    ~MainWindow() override = default;

private:
    GameVariant m_gameVariant{ GameVariant::None };
    Page m_currentPlayerPage{ Page::FirstPlayerGame };
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
    QWidget* m_firstPlayerGamePage{ nullptr };
    QGridLayout* m_firstPlayerGamePageLayout{ nullptr };
    PlayerField* m_secondPlayerField{ nullptr };
    OpponentField* m_secondPlayerHiddenField{ nullptr };
    QWidget* m_secondPlayerGamePage{ nullptr };
    QGridLayout* m_secondPlayerGamePageLayout{ nullptr };
    QWidget* m_changePlayerPage{ nullptr };
    QHBoxLayout* m_changePlayerPageLayout{ nullptr };
    QPushButton* m_changePlayerBtn{ nullptr };
    QPushButton* m_fromHostOrOnlineToMainBtn{ nullptr };
    QLabel* m_opponentIsReadyLabel{ nullptr };
    QPushButton* m_firstPlayerReadyBtn{ nullptr };
    QPushButton* m_secondPlayerReadyBtn{ nullptr };
};

#endif //MAINWINDOW_H