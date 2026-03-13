#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Field.h"

#include <QMainWindow>
#include <QWidget>
#include <QStackedWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>

class MainWindow final : public QMainWindow
{
public:
    enum class Page
    {
        Main,
        ChooseHostOrClient,
        Game,
        ChangePlayer,
    };

    explicit MainWindow();

    ~MainWindow() override = default;

private:
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
    Field* m_playerField{ nullptr };
    Field* m_opponentField{ nullptr };
    QWidget* m_gamePage{ nullptr };
    QHBoxLayout* m_gamePageLayout{ nullptr };
    QWidget* m_changePlayerPage{ nullptr };
    QHBoxLayout* m_changePlayerPageLayout{ nullptr };
    QPushButton* m_changePlayerBtn{ nullptr };
};

#endif //MAINWINDOW_H