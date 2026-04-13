#include "MainWindow.h"
#include "PlayerField.h"
#include "OpponentField.h"
#include "Server.h"
#include "Client.h"

#include <iostream>

#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QTcpSocket>

void MainWindow::shotsAreOverSlot()
{
    switch (m_gameVariant)
    {
        case GameVariant::Local:
        {
            switch (m_turn)
            {
                case Turn::Player1:
                {
                    m_firstPlayerHiddenField->activate();
                    m_turn = Turn::Player2;
                    m_whoseTurnLabel->setText("Ход Player2");
                    return;
                }
                case Turn::Player2:
                    m_secondPlayerHiddenField->activate();
                    m_turn = Turn::Player1;
                    m_whoseTurnLabel->setText("Ход Player1");
                    return;
                default: return;
            }
        }
        case GameVariant::Server:
        {
            m_server->send(0, 0, 3);
            m_whoseTurnLabel->setText("Ход противника");
            return;
        }
        case GameVariant::Client:
        {
            m_client->send(0, 0, 3);
            m_whoseTurnLabel->setText("Ход противника");
            return;
        }
        default: return;
    }
}

void MainWindow::allShipsAreDestroyedSlot()
{
    switch (m_gameVariant)
    {
        case GameVariant::Local:
        {
            switch (m_turn)
            {
                case Turn::Player1:
                {
                    QMessageBox::information(this, " ", "Player1 выиграл");
                    break;
                }
                case Turn::Player2:
                    QMessageBox::information(this, " ", "Player2 выиграл");
                default:;
            }
            break;
        }
        case GameVariant::Server:
        {
            QMessageBox::information(this, " ", "Вы выиграли");
            m_server->send(0, 0, 4);
            break;
        }
        case GameVariant::Client:
        {
            QMessageBox::information(this, " ", "Вы выиграли");
            m_client->send(0, 0, 4);
        }
        default:;
    }

    m_centralStack->setCurrentIndex(static_cast<int>(Page::Main));

    m_firstPlayerField->clear();
    m_secondPlayerField->clear();
    m_firstPlayerHiddenField->clear();
    m_secondPlayerHiddenField->clear();
}

void MainWindow::dataSlot(const int identifier, const int row, const int col)
{
    std::cerr << "incoming signal: " << identifier << ", " << row << ", " << col
    << std::endl;

    if (identifier == 0)
    {
        if (row == 0 && col == 0)
        {
            m_readyLabel->setText("Противник готов");

            if (m_gameVariant == GameVariant::Client && m_client->isReady())
            {
                m_client->send(0, 0, 2);
                startGame();
            }

            else if (m_gameVariant == GameVariant::Server && m_server->isReady())
            {
                m_server->send(0, 0, 2);
                startGame();
            }
        }

        else if (row == 0 && col == 1)
        {
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Prepare));
            m_firstPlayerField->show();
            m_secondPlayerField->hide();
        }

        else if (row == 0 && col == 2)
            startGame();

        else if (row == 0 && col == 3)
        {
            m_secondPlayerHiddenField->activate();
            m_whoseTurnLabel->setText("Ваш ход");
        }

        else if (row == 0 && col == 4)
        {
            QMessageBox::information(this, " ", "Вы проиграли");

            m_centralStack->setCurrentIndex(static_cast<int>(Page::Main));

            m_firstPlayerField->clear();
            m_secondPlayerField->clear();
            m_firstPlayerHiddenField->clear();
            m_secondPlayerHiddenField->clear();
        }
    }

    else if (identifier == 1)
        m_secondPlayerField->clickCellOnlineFunc(row, col);

    else if (identifier == 2)
        m_secondPlayerField->changeAddModeOnlineFunc(row);

    else if (identifier == 3)
        m_secondPlayerField->changeOrientationOnlineFunc(row);

    else if (identifier == 4)
    {
        m_firstPlayerHiddenField->activate();
        m_firstPlayerHiddenField->clickCellOnlineFunc(row, col);
        m_firstPlayerHiddenField->deactivate();
    }
}

void MainWindow::playerClickCellOnlineSlot(const int row, const int col)
{
    switch (m_gameVariant)
    {
    case GameVariant::Client:
    {
        m_client->send(1, row, col);
        return;
    }
    case GameVariant::Server:
    {
        m_server->send(1, row, col);
    }
    default:;
    }
}

void MainWindow::playerChangeShipVariantSlot(const int ID)
{
    switch (m_gameVariant)
    {
    case GameVariant::Client:
    {
        m_client->send(2, ID, 0);
        return;
    }
    case GameVariant::Server:
    {
        m_server->send(2, ID, 0);
    }
    default:;
    }
}

void MainWindow::playerChangeOrientationSlot(const int ID)
{
    switch (m_gameVariant)
    {
    case GameVariant::Client:
    {
        m_client->send(3, ID, 0);
        return;
    }
    case GameVariant::Server:
    {
        m_server->send(3, ID, 0);
    }
    default:;
    }
}

void MainWindow::playerClickCellOnlineSlotOpponent(const int row, const int col)
{
    switch (m_gameVariant)
    {
        case GameVariant::Client:
        {
            m_client->send(4, row, col);
            return;
        }
        case GameVariant::Server:
        {
            m_server->send(4, row, col);
        }
        default:;
    }
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
    , m_readyLabel{ new QLabel{ "Противник не готов", this } }
    , m_connectingPage{ new QWidget{ this } }
    , m_connectingPageLayout{ new QGridLayout{ m_connectingPage } }
    , m_connectingLabel{ new QLabel{ "Ожидание подключения", m_connectingPage } }
    , m_whoseTurnLabel{ new QLabel{ this } }
{
    connect(m_firstPlayerField, &PlayerField::playerClickCellOnlineSignal,
        this, &MainWindow::playerClickCellOnlineSlot);
    connect(m_firstPlayerField, &PlayerField::playerChangeShipVariantSignal,
        this, &MainWindow::playerChangeShipVariantSlot);
    connect(m_firstPlayerField, &PlayerField::playerChangeOrientationSignal,
        this, &MainWindow::playerChangeOrientationSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::playerClickCellOnlineSignal,
        this, &MainWindow::playerClickCellOnlineSlotOpponent);

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
    m_preparePageLayout->addWidget(m_firstPlayerField, 1, 0, 1, 1);
    m_preparePageLayout->addWidget(m_secondPlayerField, 1, 1, 1, 1);
    m_preparePageLayout->addWidget(m_readyBtn, 2, 0, 1, 2);
    m_preparePageLayout->addWidget(m_readyLabel, 0, 0, 1, 2);
    m_readyLabel->setAlignment(Qt::AlignCenter);
    QSizePolicy firstFieldPolicy{ m_firstPlayerField->sizePolicy() };
    firstFieldPolicy.setRetainSizeWhenHidden(true);
    m_firstPlayerField->setSizePolicy(firstFieldPolicy);
    QSizePolicy secondFieldPolicy{ m_secondPlayerField->sizePolicy() };
    secondFieldPolicy.setRetainSizeWhenHidden(true);
    m_secondPlayerField->setSizePolicy(secondFieldPolicy);

    //Настройка m_connectingPage
    m_centralStack->insertWidget(4, m_connectingPage);
    m_connectingPage->setLayout(m_connectingPageLayout);
    m_connectingPageLayout->addWidget(m_connectingLabel, 0, 0, 1, 1);
    m_connectingLabel->setAlignment(Qt::AlignCenter);

    //Настройка m_gamePage
    m_centralStack->insertWidget(3, m_gamePage);
    m_gamePage->setLayout(m_gamePageLayout);
    m_gamePageLayout->addWidget(m_firstPlayerHiddenField, 0, 1, 1, 1);
    m_gamePageLayout->addWidget(m_secondPlayerHiddenField, 0, 2, 1, 1);
    m_gamePageLayout->addWidget(m_whoseTurnLabel, 0, 0, 1, 2);
    m_whoseTurnLabel->setAlignment(Qt::AlignCenter);

    //Подключения кнопки выбора локального режима игры и лямбда для неё
    auto chooseLocalBtnLambda{
        [this] {
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Prepare));
            setWindowTitle("Морской бой -> Локальная игра -> Расстановка");
            m_gameVariant = GameVariant::Local;

            m_turn = Turn::Player1;
            m_firstPlayerField->show();
            m_secondPlayerField->hide();
            m_readyLabel->hide();
        }
    };
    connect(m_chooseLocalBtn, &QPushButton::clicked, chooseLocalBtnLambda);

    //Подключение кнопки выбора режима игры по сети и лямбда для неё
    auto chooseOnlineBtnLambda{
        [this] {
            m_centralStack->setCurrentIndex(
                static_cast<int>(Page::ChooseHostOrClient));
            setWindowTitle("Морской бой -> Игра по сети");
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
            switch (m_gameVariant)
            {
            case GameVariant::Local:
            {
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
                        startGame();

                        return;
                    }

                    QMessageBox::warning(this, " ", "Расставьте все корабли");
                }
                default: return;
                }
            }
            case GameVariant::Server:
            {
                if (m_firstPlayerField->allShipsCreated())
                {
                    m_server->getReady();
                    m_server->send(0, 0, 0);
                    return;
                }

                QMessageBox::warning(this, " ", "Расставьте все корабли");
                return;
            }
            case GameVariant::Client:
            {
                if (m_firstPlayerField->allShipsCreated())
                {
                    m_client->getReady();
                    m_client->send(0, 0, 0);
                    return;
                }

                QMessageBox::warning(this, " ", "Расставьте все корабли");
            }
            default: return;
            }
        }
    };
    connect(m_readyBtn, &QPushButton::clicked, readyBtnLambda);

    connect(m_firstPlayerHiddenField, &OpponentField::shotsAreOverSignal,
        this, &MainWindow::shotsAreOverSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::shotsAreOverSignal,
        this, &MainWindow::shotsAreOverSlot);
    connect(m_firstPlayerHiddenField, &OpponentField::allShipsAreDestroyedSignal,
        this, &MainWindow::allShipsAreDestroyedSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::allShipsAreDestroyedSignal,
        this, &MainWindow::allShipsAreDestroyedSlot);

    auto chooseClientBtnLambda{
        [this] {
            m_client = new Client{ this };
            connect(m_client, &Client::dataSignal, this, &MainWindow::dataSlot);

            m_centralStack->setCurrentIndex(static_cast<int>(Page::Connecting));
            setWindowTitle("Морской бой -> Игра по сети -> Ожидание подключения");
            m_gameVariant = GameVariant::Client;
            m_firstPlayerField->setSendToOnline();
            m_secondPlayerHiddenField->sendToOnline();
            m_firstPlayerHiddenField->getFromOnline();

            dataSlot(0, 0, 1);
        }
    };
    connect(m_chooseClientBtn, &QPushButton::clicked, chooseClientBtnLambda);

    auto chooseHostBtnLambda{
        [this] {
            m_server = new Server{ this };
            connect(m_server, &Server::dataSignal, this, &MainWindow::dataSlot);

            m_centralStack->setCurrentIndex(static_cast<int>(Page::Connecting));
            setWindowTitle("Морской бой -> Игра по сети -> Ожидание подключения");
            m_gameVariant = GameVariant::Server;
            m_firstPlayerField->setSendToOnline();
            m_secondPlayerHiddenField->sendToOnline();
            m_firstPlayerHiddenField->getFromOnline();
        }
    };
    connect(m_chooseHostBtn, &QPushButton::clicked, chooseHostBtnLambda);
}

void MainWindow::startGame()
{
    m_centralStack->setCurrentIndex(static_cast<int>(Page::Game));

    m_firstPlayerField->transferShipsTo(m_firstPlayerHiddenField);
    m_secondPlayerField->transferShipsTo(m_secondPlayerHiddenField);

    if (m_gameVariant == GameVariant::Local)
    {
        setWindowTitle("Морской бой -> Локальная игра -> Бой");
        m_secondPlayerHiddenField->activate();
        m_whoseTurnLabel->setText("Ход Player1");
    }

    else if (m_gameVariant == GameVariant::Server)
    {
        setWindowTitle("Морской бой -> Игра по сети (сервер) -> Бой");
        m_secondPlayerHiddenField->activate();
        m_whoseTurnLabel->setText("Ваш ход");
    }

    else
    {
        setWindowTitle("Морской бой -> Игра по сети (клиент) -> Бой");
        m_whoseTurnLabel->setText("Ход противника");
    }
}