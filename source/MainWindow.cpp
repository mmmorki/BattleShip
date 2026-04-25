#include "MainWindow.h"
#include "PlayerField.h"
#include "OpponentField.h"
#include "Server.h"
#include "Client.h"
#include "ThemeMusicManager.h"

#include <iostream>

#include <QGridLayout>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QTcpSocket>
#include <QSoundEffect>
#include <QLineEdit>

/* Слот для принятия сигнала исчерпывания у игрока возможности стрелять.
 * Для локального режима игры просто меняем переменную перечисления хода,
 * активируем другое поле и меняем надпись.
 * Для режима игры по сети отправляем сигнал (0, 0, 3), который активирует поле
 * противника у оппонента и меняем надпись. */
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
        {
            m_secondPlayerHiddenField->activate();
            m_turn = Turn::Player1;
            m_whoseTurnLabel->setText("Ход Player1");
        }
        default:return;
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
    }
    default:;
    }
}

/* Слот для принятия сигнала уничтожения всех кораблей противника и победы
 * текущего игрока.
 * Для локального режима игры выводим информацию о том, кто выиграл.
 * Для режима игры по сети отправляем сигнал (0, 0, 4), который воспроизводит у
 * противника звук поражения и выводит информацию о поражении, воспроизводим
 * сигнал победы и выводим информацию о победе.
 * Затем запускаем функцию завершения игры. */
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
        m_server->send(0, 0, 4);
        m_victory->play();
        QMessageBox::information(this, " ", "Вы выиграли");
        break;
    }
    case GameVariant::Client:
    {
        m_client->send(0, 0, 4);
        m_victory->play();
        QMessageBox::information(this, " ", "Вы выиграли");
    }
    default:;
    }

    endGame();
}

// Слот для принятия сигналов от оппонента при игре по сети
void MainWindow::dataSlot(const int identifier, const int row, const int col)
{
    // Отладочная информация о входящем от оппонента сигнале
    std::cerr << "incoming signal: " << identifier << ", " << row << ", " << col
    << std::endl;

    if (identifier == 0)
    {
        /* (0, 0, 0) - Оппонент готов. Если текущий игрок тоже готов, то
         * оппоненту отправляется сигнал (0, 0, 2) и запускается функция
         * начала боя. Меняется надпись о готовности оппонента. */
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

        /* (0, 0, 1) - Оппонент установил соединение с игроком. Переключение на
         * страницу расстановки кораблей, скрытие поля расстановки оппонента и
         * отображение поля расстановки текущего игрока. */
        else if (row == 0 && col == 1)
        {
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Prepare));
            m_firstPlayerField->show();
            m_secondPlayerField->hide();
        }

        /* (0, 0, 2) - Оппонент подтверждает свою готовность после того, как
         * текущий игрок отправил сигнал о своей готовности (0, 0, 0), после
         * чего запускается функция начал боя. */
        else if (row == 0 && col == 2)
            startGame();

        /* (0, 0, 3) - Оппонент завершил свой ход исчерпыванием возможности
         * атаковать. Происходит активация поля противника у текущего игрока и
         * сменяется надпись хода. */
        else if (row == 0 && col == 3)
        {
            m_secondPlayerHiddenField->activate();
            m_whoseTurnLabel->setText("Ваш ход");
        }

        /* (0, 0, 4) - Оппонент завершил свой ход полным уничтожением кораблей
         * текущего игрока. У текущего игрока проигрывается звук поражения,
         * выводится информация о проигрыше и запускается функция завершения
         * боя. */
        else if (row == 0 && col == 4)
        {
            m_defeat->play();
            QMessageBox::information(this, " ", "Вы проиграли");
            endGame();
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
    , m_defeat{ new QSoundEffect{ this } }
    , m_victory{ new QSoundEffect{ this } }
    , m_menuClick{ new QSoundEffect{ this } }
    , m_chooseAddressPage{ new QWidget{ this } }
    , m_chooseAddressLine{ new QLineEdit{ "127.0.0.1", this } }
    , m_chooseAddressLabel{ new QLabel{ "Введите ip-адрес хоста:", this } }
    , m_chooseAddressBtn{ new QPushButton{ "Подключиться", this } }
    , m_chooseAddressLayout{ new QVBoxLayout{ this } }
    , m_themeMusicManager{ new ThemeMusicManager{ this } }
{
    //Подключение сигналов кнопок смены кораблей и смены ориентации расстановки
    //кораблей
    connect(m_firstPlayerField, &PlayerField::playerClickCellOnlineSignal,
        this, &MainWindow::playerClickCellOnlineSlot);
    connect(m_firstPlayerField, &PlayerField::playerChangeShipVariantSignal,
        this, &MainWindow::playerChangeShipVariantSlot);
    connect(m_firstPlayerField, &PlayerField::playerChangeOrientationSignal,
        this, &MainWindow::playerChangeOrientationSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::playerClickCellOnlineSignal,
        this, &MainWindow::playerClickCellOnlineSlotOpponent);

    //Подключение сигналов сценариев завершения хода
    connect(m_firstPlayerHiddenField, &OpponentField::shotsAreOverSignal,
        this, &MainWindow::shotsAreOverSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::shotsAreOverSignal,
        this, &MainWindow::shotsAreOverSlot);
    connect(m_firstPlayerHiddenField, &OpponentField::allShipsAreDestroyedSignal,
        this, &MainWindow::allShipsAreDestroyedSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::allShipsAreDestroyedSignal,
        this, &MainWindow::allShipsAreDestroyedSlot);

    //Настройка звуков
    m_defeat->setSource(QUrl("qrc:/sounds/events/defeat.wav"));
    m_defeat->setVolume(0.5);
    m_victory->setSource(QUrl("qrc:/sounds/events/victory.wav"));
    m_victory->setVolume(0.5);
    m_menuClick->setSource(QUrl("qrc:/sounds/events/menu_click.wav"));
    m_menuClick->setVolume(0.5);
    m_themeMusicManager->playNext();

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

    //Настройка m_gamePage
    m_centralStack->insertWidget(3, m_gamePage);
    m_gamePage->setLayout(m_gamePageLayout);
    m_gamePageLayout->addWidget(m_firstPlayerHiddenField, 1, 0, 1, 1);
    m_gamePageLayout->addWidget(m_secondPlayerHiddenField, 1, 1, 1, 1);
    m_gamePageLayout->addWidget(m_whoseTurnLabel, 0, 0, 1, 2);
    m_whoseTurnLabel->setAlignment(Qt::AlignCenter);

    //Настройка m_connectingPage
    m_centralStack->insertWidget(4, m_connectingPage);
    m_connectingPage->setLayout(m_connectingPageLayout);
    m_connectingPageLayout->addWidget(m_connectingLabel, 0, 0, 1, 1);
    m_connectingLabel->setAlignment(Qt::AlignCenter);

    //Настройка m_chooseAddressPage
    m_centralStack->insertWidget(5, m_chooseAddressPage);
    m_chooseAddressPage->setLayout(m_chooseAddressLayout);
    m_chooseAddressLayout->insertWidget(0, m_chooseAddressLabel);
    m_chooseAddressLayout->insertWidget(1, m_chooseAddressLine);
    m_chooseAddressLayout->insertWidget(2, m_chooseAddressBtn);

    //Подключения кнопки выбора локального режима игры и лямбда для неё
    auto chooseLocalBtnLambda{
        [this] {
            m_menuClick->play();
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
            m_menuClick->play();
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
            m_menuClick->play();
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
            m_menuClick->play();
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

    //Подключение кнопки подтверждения ввода ip-адреса и лямбда для неё
    auto chooseAddressBtnLambda{
        [this] {
            m_menuClick->play();
            m_client = new Client{ m_chooseAddressLine->text(), this };
            connect(m_client, &Client::dataSignal, this, &MainWindow::dataSlot);
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Connecting));

            m_firstPlayerField->setSendToOnline();
            m_secondPlayerHiddenField->sendToOnline();
            m_firstPlayerHiddenField->getFromOnline();

            //dataSlot(0, 0, 1);
        }
    };
    connect(m_chooseAddressBtn, &QPushButton::clicked, chooseAddressBtnLambda);

    //Подключение кнопки выбора способа подключения как клиента и лямбда для неё
    auto chooseClientBtnLambda{
        [this] {
            m_menuClick->play();
            m_centralStack->setCurrentIndex(static_cast<int>(Page::ChooseAddress));
            setWindowTitle("Морской бой -> Игра по сети -> Подключение к хосту");
            m_gameVariant = GameVariant::Client;
        }
    };
    connect(m_chooseClientBtn, &QPushButton::clicked, chooseClientBtnLambda);

    //Подключение кнопки выбора способа подключения как хоста и лямбда для неё
    auto chooseHostBtnLambda{
        [this] {
            m_menuClick->play();
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

//Функция старта игры после завершения расстановки кораблей
void MainWindow::startGame()
{
    //Переключение на виджет непосредственно боя
    m_centralStack->setCurrentIndex(static_cast<int>(Page::Game));

    //Перенос кораблей с полей расстановки на поля боя
    m_firstPlayerField->transferShipsTo(m_firstPlayerHiddenField);
    m_secondPlayerField->transferShipsTo(m_secondPlayerHiddenField);

    //Если режим игры локальный, то меняем название окна, активируем второе поле
    //боя (ход первого игрока), и обозначаем ход Player1
    if (m_gameVariant == GameVariant::Local)
    {
        setWindowTitle("Морской бой -> Локальная игра -> Бой");
        m_secondPlayerHiddenField->activate();
        m_whoseTurnLabel->setText("Ход Player1");
    }

    //Если режим игры по сети и способ подключения - хост, то меняем название
    //окна, активируем второе поле (ход игрока-хоста) и показываем свои корабли
    //на своём поле
    else if (m_gameVariant == GameVariant::Server)
    {
        setWindowTitle("Морской бой -> Игра по сети (сервер) -> Бой");
        m_secondPlayerHiddenField->activate();
        m_whoseTurnLabel->setText("Ваш ход");
        m_firstPlayerHiddenField->showShips();
    }

    //Если режим игры по сети и способ подключения - клиент, то меняем название
    //окна и показываем свои корабли на своём поле. Второе поле не активируется,
    //так как первым ходит игрок-хост
    else
    {
        setWindowTitle("Морской бой -> Игра по сети (клиент) -> Бой");
        m_whoseTurnLabel->setText("Ход противника");
        m_firstPlayerHiddenField->showShips();
    }
}

//Функция завершения игры после боя
void MainWindow::endGame()
{
    //Удаляем слоты подключения в режиме игры по сети
    delete m_server;
    delete m_client;

    //Сбрасываем режим игры
    m_gameVariant = GameVariant::None;

    m_readyLabel->setText("Противник не готов");

    //Переключаемся обратно на главный виджет
    m_centralStack->setCurrentIndex(static_cast<int>(Page::Main));

    //Очищаем игровые поля
    m_firstPlayerField->clear();
    m_secondPlayerField->clear();
    m_firstPlayerHiddenField->clear();
    m_secondPlayerHiddenField->clear();
}