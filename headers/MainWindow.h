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

    explicit MainWindow()
        : m_central{ new QWidget{ this } }
        , m_centralLayout{ new QGridLayout{ m_central } }
        , m_centralStack{ new QStackedWidget{ this } }
        , m_mainPage{ new QWidget{ this } }
        , m_mainPageLayout{ new QVBoxLayout{ m_mainPage } }
        , m_chooseLocalBtn{ new QPushButton{ "Локальная игра", this } }
        , m_chooseOnlineBtn{ new QPushButton{ "Игра по сети", this } }
        , m_hostOrClientPage{ new QWidget{ this } }
        , m_hostOrClientPageLayout{ new QVBoxLayout{ m_hostOrClientPage } }
        , m_chooseHostBtn{ new QPushButton{ "Создать игру", this } }
        , m_chooseClientBtn{ new QPushButton{ "Подключиться", this } }
        , m_playerField{ new Field{ Field::Owner::Player, this } }
        , m_opponentField{ new Field{ Field::Owner::Opponent, this } }
        , m_gamePage{ new QWidget{ this } }
        , m_gamePageLayout{ new QHBoxLayout{ m_gamePage } }
        , m_changePlayerPage{ new QWidget{ this } }
        , m_changePlayerPageLayout{ new QHBoxLayout{ this } }
        , m_changePlayerBtn{ new QPushButton{ "Игрок сменился", this } }
    {
        //Настройка окна
        setFixedSize(1000, 500);
        setWindowTitle("Морской бой");
        setCentralWidget(m_central);
        m_central->setLayout(m_centralLayout);
        m_centralLayout->addWidget(m_centralStack);

        //Настройка m_mainPage
        m_centralStack->insertWidget(0, m_mainPage);
        m_mainPage->setLayout(m_mainPageLayout);
        m_mainPageLayout->insertWidget(0, m_chooseLocalBtn);
        m_mainPageLayout->insertWidget(1, m_chooseOnlineBtn);

        //Настройка m_hostOrClientPage
        m_centralStack->insertWidget(1, m_hostOrClientPage);
        m_hostOrClientPage->setLayout(m_hostOrClientPageLayout);
        m_hostOrClientPageLayout->insertWidget(0, m_chooseHostBtn);
        m_hostOrClientPageLayout->insertWidget(1, m_chooseClientBtn);

        //Настройка m_gamePage
        m_centralStack->insertWidget(2, m_gamePage);
        m_gamePage->setLayout(m_gamePageLayout);
        m_gamePageLayout->insertWidget(0, m_playerField);
        m_gamePageLayout->insertWidget(1, m_opponentField);

        //Настройка m_changePlayerPage
        m_centralStack->insertWidget(3, m_changePlayerPage);
        m_changePlayerPage->setLayout(m_changePlayerPageLayout);
        m_changePlayerPageLayout->insertWidget(0, m_changePlayerBtn);

        auto changePlayerBtnLambda{
            [this]() {

            }
        };

        auto chooseLocalBtnLambda{
            [this]() {
                m_centralStack->setCurrentIndex(static_cast<int>(Page::Game));
                setWindowTitle("Морской бой -> Локальная игра -> Player1");
            }
        };

        QObject::connect(m_chooseLocalBtn, &QPushButton::clicked, chooseLocalBtnLambda);

        auto chooseOnlineBtnLambda{
            [this]() {
                m_centralStack->setCurrentIndex(
                    static_cast<int>(Page::ChooseHostOrClient));
                setWindowTitle("Морской бой -> Игра по сети");
            }
        };

        QObject::connect(m_chooseOnlineBtn, &QPushButton::clicked, chooseOnlineBtnLambda);
    }

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