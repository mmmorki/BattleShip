#include "MainWindow.h"
#include "PlayerField.h"
#include "OpponentField.h"

#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

void MainWindow::shotsAreOverSlot()
{

}

MainWindow::MainWindow()
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
    , m_firstPlayerField{ new PlayerField{ this } }
    , m_firstPlayerHiddenField{ new OpponentField{ this } }
    , m_secondPlayerField{ new PlayerField{ this } }
    , m_secondPlayerHiddenField{ new OpponentField{ this } }
    , m_fromHostOrOnlineToMainBtn{ new QPushButton{ "Назад", this } }
    , m_preparePage{ new QWidget{ this } }
    , m_preparePageLayout{ new QGridLayout{ m_preparePage } }
    , m_readyBtn{ new QPushButton{ "Готов", this } }
    , m_gamePage{ new QWidget{ this } }
    , m_gamePageLayout{ new QGridLayout{ m_gamePage } }
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
    m_hostOrClientPageLayout->insertWidget(2, m_fromHostOrOnlineToMainBtn);

    //Настройка m_preparePage
    m_centralStack->insertWidget(2, m_preparePage);
    m_preparePage->setLayout(m_preparePageLayout);
    m_preparePageLayout->addWidget(m_firstPlayerField, 0, 0, 1, 1);
    m_preparePageLayout->addWidget(m_secondPlayerField, 0, 1, 1, 1);
    m_preparePageLayout->addWidget(m_readyBtn, 1, 0, 1, 2);
    QSizePolicy firstFieldPolicy{ m_firstPlayerField->sizePolicy() };
    firstFieldPolicy.setRetainSizeWhenHidden(true);
    m_firstPlayerField->setSizePolicy(firstFieldPolicy);
    QSizePolicy secondFieldPolicy{ m_secondPlayerField->sizePolicy() };
    secondFieldPolicy.setRetainSizeWhenHidden(true);
    m_secondPlayerField->setSizePolicy(secondFieldPolicy);

    //Настройка m_gamePage
    m_centralStack->insertWidget(3, m_gamePage);
    m_gamePage->setLayout(m_gamePageLayout);
    m_gamePageLayout->addWidget(m_firstPlayerHiddenField, 0, 0, 1, 1);
    m_gamePageLayout->addWidget(m_secondPlayerHiddenField, 0, 1, 1, 1);


    //Подключения кнопки выбора локального режима игры и лямбда для неё
    auto chooseLocalBtnLambda{
        [this] {
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Prepare));
            setWindowTitle("Морской бой -> Локальная игра -> Расстановка");
            m_gameVariant = GameVariant::Local;

            m_firstPlayerField->startPrepare();
            m_firstPlayerHiddenField->startPrepare();
            m_secondPlayerField->startPrepare();
            m_secondPlayerHiddenField->startPrepare();

            m_turn = Turn::Player1;
            m_secondPlayerField->hide();
        }
    };
    connect(m_chooseLocalBtn, &QPushButton::clicked, chooseLocalBtnLambda);

    //Подключение кнопки выбора режима игры по сети и лямбда для неё
    auto chooseOnlineBtnLambda{
        [this] {
            m_centralStack->setCurrentIndex(
                static_cast<int>(Page::ChooseHostOrClient));
            setWindowTitle("Морской бой -> Игра по сети");
            m_gameVariant = GameVariant::Online;
        }
    };
    connect(m_chooseOnlineBtn, &QPushButton::clicked, chooseOnlineBtnLambda);

    //Подключение кнопки возврата в главное меню из страницы выбора типа игры по
    //сети и лямбда для неё
    auto fromHostOrOnlineToMainBtnLambda{
        [this] {
            m_centralStack->setCurrentIndex(
                static_cast<int>(Page::Main));
            setWindowTitle("Морской бой");
            m_gameVariant = GameVariant::None;
        }
    };
    connect(m_fromHostOrOnlineToMainBtn, &QPushButton::clicked,
        fromHostOrOnlineToMainBtnLambda);

    //Подключение кнопки завершения расстановки кораблей и лямбда для неё
    auto readyBtnLambda{
        [this] {
            switch (m_turn)
            {
            case Turn::Player1:
            {
                if (m_firstPlayerField->allShipsCreated())
                {
                    m_firstPlayerField->hide();
                    m_secondPlayerField->show();
                    m_turn = Turn::Player2;
                    return;
                }

                QMessageBox::warning(this, " ", "Расставьте все корабли");
                return;
            }
            case Turn::Player2:
            {
                if (m_secondPlayerField->allShipsCreated())
                {
                    m_secondPlayerField->hide();
                    m_turn = Turn::Player1;
                    m_centralStack->setCurrentIndex(static_cast<int>(
                        Page::Game));
                    m_firstPlayerField->transferShipsTo(m_firstPlayerHiddenField);
                    m_secondPlayerField->transferShipsTo(m_secondPlayerHiddenField);
                    m_firstPlayerField->startGame();
                    m_secondPlayerField->startGame();
                    m_firstPlayerHiddenField->startGame();
                    m_secondPlayerHiddenField->startGame();
                    setWindowTitle("Морской бой -> Локальная игра -> Бой");

                    return;
                }

                QMessageBox::warning(this, " ", "Расставьте все корабли");
            }
            default: return;
            }
        }
    };
    connect(m_readyBtn, &QPushButton::clicked, readyBtnLambda);
}