#include "MainWindow.h"
#include "Field.h"

#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

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
    , m_firstPlayerField{ new Field{ Field::Owner::Player, this } }
    , m_firstPlayerHiddenField{ new Field{ Field::Owner::Opponent, this } }
    , m_firstPlayerGamePage{ new QWidget{ this } }
    , m_firstPlayerGamePageLayout{ new QGridLayout{ m_firstPlayerGamePage } }
    , m_secondPlayerField{ new Field{ Field::Owner::Player, this } }
    , m_secondPlayerHiddenField{ new Field{ Field::Owner::Opponent, this } }
    , m_secondPlayerGamePage{ new QWidget{ this } }
    , m_secondPlayerGamePageLayout{ new QGridLayout{ m_secondPlayerGamePage } }
    , m_changePlayerPage{ new QWidget{ this } }
    , m_changePlayerPageLayout{ new QHBoxLayout{ this } }
    , m_changePlayerBtn{ new QPushButton{ "Сменить игрока", this } }
    , m_fromHostOrOnlineToMainBtn{ new QPushButton{ "Назад", this } }
    , m_opponentIsReadyLabel{ new QLabel{ "Противник не готов", this } }
    , m_firstPlayerReadyBtn{ new QPushButton{ "Готов", this  } }
    , m_secondPlayerReadyBtn{ new QPushButton{ "Готов", this } }
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

    //Настройка m_firstPlayerGamePage
    m_centralStack->insertWidget(2, m_firstPlayerGamePage);
    m_firstPlayerGamePage->setLayout(m_firstPlayerGamePageLayout);
    m_firstPlayerGamePageLayout->addWidget(m_opponentIsReadyLabel, 0, 0, 1, 2);
    m_firstPlayerGamePageLayout->addWidget(m_firstPlayerField, 1, 0, 1, 1);
    m_firstPlayerGamePageLayout->addWidget(m_firstPlayerHiddenField, 1, 1, 1, 1);
    m_firstPlayerGamePageLayout->addWidget(m_firstPlayerReadyBtn, 2, 0, 1, 2);
    m_opponentIsReadyLabel->setAlignment(Qt::AlignCenter);

    //Настройка m_secondPlayerGamePage
    m_centralStack->insertWidget(3, m_secondPlayerGamePage);
    m_secondPlayerGamePage->setLayout(m_secondPlayerGamePageLayout);
    m_secondPlayerGamePageLayout->addWidget(m_secondPlayerField, 1, 0, 1, 1);
    m_secondPlayerGamePageLayout->addWidget(m_secondPlayerHiddenField, 1, 1, 1, 1);
    m_secondPlayerGamePageLayout->addWidget(m_secondPlayerReadyBtn, 2, 0, 1, 2);

    //Настройка m_changePlayerPage
    m_centralStack->insertWidget(4, m_changePlayerPage);
    m_changePlayerPage->setLayout(m_changePlayerPageLayout);
    m_changePlayerPageLayout->insertWidget(0, m_changePlayerBtn);

    auto chooseLocalBtnLambda{
        [this]() {
            m_centralStack->setCurrentIndex(static_cast<int>(Page::FirstPlayerGame));
            setWindowTitle("Морской бой -> Локальная игра -> Player1");
            m_gameVariant = GameVariant::Local;

            m_opponentIsReadyLabel->hide();
        }
    };
    QObject::connect(m_chooseLocalBtn, &QPushButton::clicked, chooseLocalBtnLambda);

    auto chooseOnlineBtnLambda{
        [this]() {
            m_centralStack->setCurrentIndex(
                static_cast<int>(Page::ChooseHostOrClient));
            setWindowTitle("Морской бой -> Игра по сети");
            m_gameVariant = GameVariant::Online;
        }
    };
    QObject::connect(m_chooseOnlineBtn, &QPushButton::clicked, chooseOnlineBtnLambda);

    auto fromHostOrOnlineToMainBtnLambda{
        [this]() {
            m_centralStack->setCurrentIndex(
                static_cast<int>(Page::Main));
            setWindowTitle("Морской бой");
            m_gameVariant = GameVariant::None;
        }
    };
    QObject::connect(m_fromHostOrOnlineToMainBtn, &QPushButton::clicked, fromHostOrOnlineToMainBtnLambda);

    auto readyBtnLambda{
        [this]() {
            if (m_currentPlayerPage == Page::FirstPlayerGame
                && m_firstPlayerField->isPrepare())
            {
                if (m_firstPlayerField->shipsCompletelyCreated())
                {
                    m_firstPlayerField->endPrepare();
                    m_firstPlayerHiddenField->endPrepare();
                    m_centralStack->setCurrentIndex(static_cast<int>(Page::ChangePlayer));
                    setWindowTitle("Морской бой -> Локальная игра -> Смена игрока");
                }

                else
                    QMessageBox::warning(this, "", "Расставьте все корабли");
            }

            else if (m_currentPlayerPage == Page::SecondPlayerGame
                && m_secondPlayerField->isPrepare())
            {
                if (m_secondPlayerField->shipsCompletelyCreated())
                {
                    m_secondPlayerField->endPrepare();
                    m_secondPlayerHiddenField->endPrepare();
                    m_centralStack->setCurrentIndex(static_cast<int>(Page::ChangePlayer));
                    setWindowTitle("Морской бой -> Локальная игра -> Смена игрока");
                }

                else
                    QMessageBox::warning(this, "", "Расставьте все корабли");
            }

            else if (m_currentPlayerPage == Page::FirstPlayerGame
                && m_firstPlayerField->isPrepare())
            {
                //if (m_firstPlayerHiddenField->shotsAreOver())

                //else
            }

            else if (m_currentPlayerPage == Page::SecondPlayerGame
                && m_secondPlayerField->isGame())
            {
                //if (m_secondPlayerHiddenField->shotsAreOver())

                //else
            }
        }
    };
    QObject::connect(m_firstPlayerReadyBtn, &QPushButton::clicked, readyBtnLambda);
    QObject::connect(m_secondPlayerReadyBtn, &QPushButton::clicked, readyBtnLambda);

    auto changePlayerBtnLambda{
        [this]() {
            if (m_currentPlayerPage == Page::FirstPlayerGame)
            {
                m_centralStack->setCurrentIndex(static_cast<int>(Page::SecondPlayerGame));
                m_firstPlayerField->switchOff();
                m_firstPlayerHiddenField->switchOff();
                m_secondPlayerField->switchOn();
                m_secondPlayerHiddenField->switchOn();
                m_currentPlayerPage = Page::SecondPlayerGame;
                setWindowTitle("Морской бой -> Локальная игра -> Player2");
            }

            else if (m_currentPlayerPage == Page::SecondPlayerGame)
            {
                m_centralStack->setCurrentIndex(static_cast<int>(Page::FirstPlayerGame));
                m_firstPlayerField->switchOn();
                m_firstPlayerHiddenField->switchOn();
                m_secondPlayerField->switchOff();
                m_secondPlayerHiddenField->switchOff();
                m_currentPlayerPage = Page::FirstPlayerGame;
                setWindowTitle("Морской бой -> Локальная игра -> Player1");
            }
        }
    };
    QObject::connect(m_changePlayerBtn, &QPushButton::clicked, changePlayerBtnLambda);
}

