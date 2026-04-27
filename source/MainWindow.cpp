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

/* Слот для принятия сигнала исчерпывания у игрока возможности стрелять. Для
 * локального режима игры сигнал отправляется с обоих полей боя, переключается
 * переменная перечисления хода, активируется поле боя другого игрока, меняется
 * надпись хода. Для режима игры по сети сигнал отправляется только от
 * m_secondPlayerHiddenField, отправляется сигнал (0, 0, 3), который активирует
 * m_secondPlayerHiddenField оппонента, и меняется надпись хода. */
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

/* Слот для принятия сигнала от уничтожения всех кораблей противника и победы
 * текущего игрока. Для локального режима игры сигнал отправляется с обоих полей
 * боя, выводится информацию о том, кто выиграл. Для режима игры по сети сигнал
 * отправляется только с m_secondPlayerHiddenFiled, затем отправляется сигнал
 * (0, 0, 4), который воспроизводит у противника звук поражения и выводит
 * информацию о поражении, воспроизводится сигнал победы и выводится информация
 * о победе. Затем запускается функция завершения боя. */
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

/* Слот для принятия сигналов от оппонента (от m_client/m_server) при игре по
 * сети. */
void MainWindow::dataSlot(const int identifier, const int row, const int col)
{
    // Отладочная информация о входящем от оппонента сигнале
    std::cerr << "incoming signal: (" << identifier << ", " << row << ", " << col
    << ')' << std::endl;

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
         * страницу расстановки кораблей, скрытие m_secondPlayerField и
         * отображение m_firstPlayerField. */
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
         * атаковать. Активируется m_secondPlayerHiddenField и сменяется надпись
         * хода. */
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

    /* (1, row, col) - Оппонент установил корабль на своём поле по координатам
     * [row, col]. Корабль устанавливается m_secondPlayerField. */
    else if (identifier == 1)
        m_secondPlayerField->clickCellOnlineFunc(row, col);

    /* (2, row, 0) - Оппонент изменил тип устанавливаемого корабля на [row].
     * Тип корабля устанавливается у m_secondPlayerField. */
    else if (identifier == 2)
        m_secondPlayerField->changeAddModeOnlineFunc(row);

    /* (3, row, 0) - Оппонент изменил ориентацию корабля на [row].
     * Ориентация установки корабля устанавливается у m_secondPlayerField.  */
    else if (identifier == 3)
        m_secondPlayerField->changeOrientationOnlineFunc(row);

    /* (4, row, col) - Оппонент нажал m_secondPlayerHiddenField по координатам
     * [row, col]. Происходит прострел m_firstPlayerHiddenField текущего игрока,
     * для чего оно сначала активируется, а затем деактивируется. */
    else if (identifier == 4)
    {
        m_firstPlayerHiddenField->activate();
        m_firstPlayerHiddenField->clickCellOnlineFunc(row, col);
        m_firstPlayerHiddenField->deactivate();
    }
}

/* Слот для отправки оппоненту сигнала типа (1, row, col) об установке корабля
 * игроком. Сигнал отправляется от m_firstPlayerField. */
void MainWindow::playerClickCellOnlineSlot(const int row, const int col) const
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

/* Слот для отправки оппоненту сигнала типа (2, row, 0) о смене типа
 * устанавливаемого корабля. Сигнал отправляется от m_firstPlayerField. */
void MainWindow::playerChangeShipVariantSlot(const int ID) const
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

/* Слот для отправки оппоненту сигнала типа (3, row, 0) о смене ориентации
 * установки корабля. Сигнал отправляется от m_firstPLayerField. */
void MainWindow::playerChangeOrientationSlot(const int ID) const
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

/* Слот для отправки оппоненту сигнала типа (4, row, col) о нажатии на
 * [row, col] клетку m_secondPlayerHiddenField. Сигнал отправляется от
 * m_secondPlayerHiddenField. */
void MainWindow::playerClickCellOnlineSlotOpponent(const int row, const int col) const
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

//Конструктор окна приложения
MainWindow::MainWindow()
    : m_defeat{ new QSoundEffect{ this } }
    , m_victory{ new QSoundEffect{ this } }
    , m_menuClick{ new QSoundEffect{ this } }
    , m_themeMusicManager{ new ThemeMusicManager{ this } }
    , m_central{ new QWidget{ this } }
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
    , m_fromHostOrClientToMainBtn{ new QPushButton{ "Назад", this } }
    , m_chooseAddressPage{ new QWidget{ this } }
    , m_chooseAddressLine{ new QLineEdit{ "127.0.0.1", this } }
    , m_chooseAddressLabel{ new QLabel{ "Введите ip-адрес хоста:", this } }
    , m_chooseAddressBtn{ new QPushButton{ "Подключиться", this } }
    , m_chooseAddressLayout{ new QVBoxLayout{ this } }
    , m_connectingPage{ new QWidget{ this } }
    , m_connectingPageLayout{ new QGridLayout{ m_connectingPage } }
    , m_connectingLabel{ new QLabel{ "Ожидание подключения", m_connectingPage } }
    , m_preparePage{ new QWidget{ this } }
    , m_preparePageLayout{ new QGridLayout{ m_preparePage } }
    , m_firstPlayerField{ new PlayerField{ this } }
    , m_secondPlayerField{ new PlayerField{ this } }
    , m_readyBtn{ new QPushButton{ "Готов", this } }
    , m_readyLabel{ new QLabel{ "Противник не готов", this } }
    , m_gamePage{ new QWidget{ this } }
    , m_gamePageLayout{ new QGridLayout{ m_gamePage } }
    , m_firstPlayerHiddenField{ new OpponentField{ this } }
    , m_secondPlayerHiddenField{ new OpponentField{ this } }
    , m_whoseTurnLabel{ new QLabel{ this } }
{
    /* Подключение сигналов кнопок о смене типа устанавливаемого корабля,
     * о смене ориентации установки кораблей, об установке корабля на
     * m_firstPlayerField и о нажатиях на m_secondPlayerHiddenField к слотам
     * для режима игры по сети. */
    connect(m_firstPlayerField, &PlayerField::playerClickCellOnlineSignal,
        this, &MainWindow::playerClickCellOnlineSlot);
    connect(m_firstPlayerField, &PlayerField::playerChangeShipVariantSignal,
        this, &MainWindow::playerChangeShipVariantSlot);
    connect(m_firstPlayerField, &PlayerField::playerChangeOrientationSignal,
        this, &MainWindow::playerChangeOrientationSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::playerClickCellOnlineSignal,
        this, &MainWindow::playerClickCellOnlineSlotOpponent);

    /* Подключение сигналов об исчерпывании возможности стрелять или об
     * уничтожении всех кораблей от m_firstPlayerHiddenField и
     * m_secondPlayerHiddenField. */
    connect(m_firstPlayerHiddenField, &OpponentField::shotsAreOverSignal,
        this, &MainWindow::shotsAreOverSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::shotsAreOverSignal,
        this, &MainWindow::shotsAreOverSlot);
    connect(m_firstPlayerHiddenField, &OpponentField::allShipsAreDestroyedSignal,
        this, &MainWindow::allShipsAreDestroyedSlot);
    connect(m_secondPlayerHiddenField, &OpponentField::allShipsAreDestroyedSignal,
        this, &MainWindow::allShipsAreDestroyedSlot);

    //Настройка окна приложения.
    setFixedSize(1000, 500);
    setWindowTitle("Морской бой");
    setCentralWidget(m_central);
    m_central->setLayout(m_centralLayout);
    m_centralLayout->addWidget(m_centralStack);

    QPushButton btn{ this };

    //Функция настройки звуков приложения.
    setupSounds();

    //Функция настройки m_mainPage.
    setupMainPage();

    //Функция настройки m_hostOrClientPage.
    setupHostOrClientPage();

    //Функция настройки m_chooseAddressPage.
    setupChooseAddressPage();

    //Функция настройки m_connectingPage.
    setupConnectingPage();

    //Функция настройки m_preparePage.
    setupPreparePage();

    //Функция настройки m_gamePage.
    setupGamePage();

    //Подключения кнопки выбора локального режима игры и лямбда для неё.
    auto chooseLocalBtnLambda{
        [this] {
            m_menuClick->play();
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Prepare));
            setWindowTitle("Морской бой -> Локальная игра -> Расстановка");
            m_gameVariant = GameVariant::Local;
            m_turn = Turn::Player1;

            m_firstPlayerField->show();
            m_secondPlayerField->hide();
            //Скрытие кнопки готовности противника (нужна для игры по сети).
            m_readyLabel->hide();
        }
    };
    connect(m_chooseLocalBtn, &QPushButton::clicked, chooseLocalBtnLambda);

    //Подключение кнопки выбора режима игры по сети и лямбда для неё.
    auto chooseOnlineBtnLambda{
        [this] {
            m_menuClick->play();
            m_centralStack->setCurrentIndex(
                static_cast<int>(Page::ChooseHostOrClient));
            setWindowTitle("Морской бой -> Игра по сети");
        }
    };
    connect(m_chooseOnlineBtn, &QPushButton::clicked, chooseOnlineBtnLambda);

    /* Подключение кнопки возврата со страницы выбора способа игры по сети на
     * главную страницу и лямбда для неё. */
    auto fromHostOrOnlineToMainBtnLambda{
        [this] {
            m_menuClick->play();
            m_centralStack->setCurrentIndex(
                static_cast<int>(Page::Main));
            setWindowTitle("Морской бой");
            m_gameVariant = GameVariant::None;
        }
    };
    connect(m_fromHostOrClientToMainBtn, &QPushButton::clicked,
        fromHostOrOnlineToMainBtnLambda);

    //Подключение кнопки завершения расстановки кораблей и лямбда для неё.
    auto readyBtnLambda{
        [this] {
            m_menuClick->play();
            switch (m_gameVariant)
            {
            /* В локальном режиме игре показывается одно поле расстановки
             * корабля, и скрывается другое, сменяется переменная перечисления
             * хода. В случае, если расставлены не все корабли, выводится
             * предупреждение. */
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
            /* В режиме игры по сети устанавливается статус готовности игрока на
             * случай принятия сигнала (0, 0, 0) и ответа по (0, 0, 2),
             * отправляется сигнал (0, 0, 0) о готовности. В случае, если
             * расставлены не все корабли, выводится предупреждение. */
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
            default:;
            }
        }
    };
    connect(m_readyBtn, &QPushButton::clicked, readyBtnLambda);

    /* Подключение кнопки подтверждения ввода ip-адреса хоста и лямбда для неё.
     * В конструктор m_client передаётся ip-адрес хоста, полученный из
     * m_chooseAddressLine. Сигнал dataSignal от m_client подключается к
     * dataSlot. */
    auto chooseAddressBtnLambda{
        [this] {
            m_menuClick->play();
            m_client = new Client{ m_chooseAddressLine->text(), this };
            connect(m_client, &Client::dataSignal, this, &MainWindow::dataSlot);
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Connecting));

            /* Поля m_firstPlayerField и m_secondPlayerHiddenField
            * устанавливаются в режим sendToOnline для передачи сигналов нажатий
            * оппоненту. Поле m_firstPlayerHiddenField устанавливается в режим
            * getFromOnline для предотвращения генерации сигналов shotsAreOver и
            * allShipsAreDestroyed. */
            m_firstPlayerField->setSendToOnline();
            m_secondPlayerHiddenField->sendToOnline();
            m_firstPlayerHiddenField->getFromOnline();
        }
    };
    connect(m_chooseAddressBtn, &QPushButton::clicked, chooseAddressBtnLambda);

    /* Подключение кнопки выбора способа игры по сети как клиента и лямбда для
     * неё. */
    auto chooseClientBtnLambda{
        [this] {
            m_menuClick->play();
            m_centralStack->setCurrentIndex(static_cast<int>(Page::ChooseAddress));
            setWindowTitle("Морской бой -> Игра по сети -> Подключение к хосту");
            m_gameVariant = GameVariant::Client;
        }
    };
    connect(m_chooseClientBtn, &QPushButton::clicked, chooseClientBtnLambda);

    /* Подключение кнопки выбора способа игры по сети как хоста и лямбда для
     * неё. Создаётся m_server. Сигнал dataSignal от m_server подключается к
     * dataSlot. */
    auto chooseHostBtnLambda{
        [this] {
            m_menuClick->play();
            m_server = new Server{ this };
            connect(m_server, &Server::dataSignal, this, &MainWindow::dataSlot);
            m_centralStack->setCurrentIndex(static_cast<int>(Page::Connecting));
            setWindowTitle("Морской бой -> Игра по сети -> Ожидание подключения");
            m_gameVariant = GameVariant::Server;

            /* Поля m_firstPlayerField и m_secondPlayerHiddenField
            * устанавливаются в режим sendToOnline для передачи сигналов нажатий
            * оппоненту. Поле m_firstPlayerHiddenField устанавливается в режим
            * getFromOnline для предотвращения генерации сигналов shotsAreOver и
            * allShipsAreDestroyed. */
            m_firstPlayerField->setSendToOnline();
            m_secondPlayerHiddenField->sendToOnline();
            m_firstPlayerHiddenField->getFromOnline();
        }
    };
    connect(m_chooseHostBtn, &QPushButton::clicked, chooseHostBtnLambda);
}

/* Функция настройки звуков поражения (m_defeat), победы (m_victory), нажатия
 * основных кнопок игры (m_click) и активации воспроизведения фоновой музыки
 * m_themeMusicManager. */
void MainWindow::setupSounds() const
{
    m_defeat->setSource(QUrl("qrc:/sounds/events/defeat.wav"));
    m_defeat->setVolume(0.5);
    m_victory->setSource(QUrl("qrc:/sounds/events/victory.wav"));
    m_victory->setVolume(0.4);
    m_menuClick->setSource(QUrl("qrc:/sounds/events/menu_click.wav"));
    m_menuClick->setVolume(0.5);
    m_themeMusicManager->playNext();
}

/* Функция настройки m_mainPage, на которой игрок выбирает режим игры
 * (локальный/по сети). */
void MainWindow::setupMainPage() const
{
    m_centralStack->insertWidget(0, m_mainPage);
    m_mainPage->setLayout(m_mainPageLayout);
    m_mainPageLayout->insertWidget(0, m_chooseLocalBtn);
    m_mainPageLayout->insertWidget(1, m_chooseOnlineBtn);
}

/* Настройка m_hostOrClientPage, на которой игрок выбирает способ игры по
 * сети (хост/клиент). */
void MainWindow::setupHostOrClientPage() const
{
    m_centralStack->insertWidget(1, m_hostOrClientPage);
    m_hostOrClientPage->setLayout(m_hostOrClientPageLayout);
    m_hostOrClientPageLayout->insertWidget(0, m_chooseHostBtn);
    m_hostOrClientPageLayout->insertWidget(1, m_chooseClientBtn);
    m_hostOrClientPageLayout->insertWidget(2, m_fromHostOrClientToMainBtn);
}

/* Настройка m_chooseAddressPage, на которой игрок-клиент вводит ip-адрес для
 * подключения к хосту. */
void MainWindow::setupChooseAddressPage() const
{
    m_centralStack->insertWidget(2, m_chooseAddressPage);
    m_chooseAddressPage->setLayout(m_chooseAddressLayout);
    m_chooseAddressLayout->insertWidget(0, m_chooseAddressLabel);
    m_chooseAddressLayout->insertWidget(1, m_chooseAddressLine);
    m_chooseAddressLayout->insertWidget(2, m_chooseAddressBtn);
}

//Настройка m_connectingPage, на которой игрок ожидает подключения оппонента.
void MainWindow::setupConnectingPage() const
{
    m_centralStack->insertWidget(3, m_connectingPage);
    m_connectingPage->setLayout(m_connectingPageLayout);
    m_connectingPageLayout->addWidget(m_connectingLabel, 0, 0, 1, 1);
    m_connectingLabel->setAlignment(Qt::AlignCenter);
}

//Настройка m_preparePage, на которой игрок/игроки расставляют корабли.
void MainWindow::setupPreparePage() const
{
    m_centralStack->insertWidget(4, m_preparePage);
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
}

//Настройка m_gamePage, на которой игрок/игроки атакуют поля друг друга.
void MainWindow::setupGamePage() const
{
    m_centralStack->insertWidget(5, m_gamePage);
    m_gamePage->setLayout(m_gamePageLayout);
    m_gamePageLayout->addWidget(m_firstPlayerHiddenField, 1, 0, 1, 1);
    m_gamePageLayout->addWidget(m_secondPlayerHiddenField, 1, 1, 1, 1);
    m_gamePageLayout->addWidget(m_whoseTurnLabel, 0, 0, 1, 2);
    m_whoseTurnLabel->setAlignment(Qt::AlignCenter);
}

/* Функция начала боя после завершения расстановки кораблей. Корабли
 * m_first(second)PlayerField переносятся на m_first(second)PlayerHiddenField.
 * Если выбран режим игры по сети, то корабли m_firstPlayerHiddenField видны
 * игроку. */
void MainWindow::startGame()
{
    m_centralStack->setCurrentIndex(static_cast<int>(Page::Game));

    m_firstPlayerField->transferShipsTo(m_firstPlayerHiddenField);
    m_secondPlayerField->transferShipsTo(m_secondPlayerHiddenField);

    if (m_gameVariant == GameVariant::Local)
    {
        setWindowTitle("Морской бой -> Локальная игра -> Бой");
        m_firstPlayerHiddenField->deactivate();
        m_secondPlayerHiddenField->activate();
        m_whoseTurnLabel->setText("Ход Player1");
    }

    else if (m_gameVariant == GameVariant::Server)
    {
        setWindowTitle("Морской бой -> Игра по сети (сервер) -> Бой");
        m_secondPlayerHiddenField->activate();
        m_whoseTurnLabel->setText("Ваш ход");
        m_firstPlayerHiddenField->showShips();
    }

    else
    {
        setWindowTitle("Морской бой -> Игра по сети (клиент) -> Бой");
        m_whoseTurnLabel->setText("Ход противника");
        m_firstPlayerHiddenField->showShips();
    }
}

//Функция завершения игры после боя.
void MainWindow::endGame()
{
    //Удаляем слоты подключения в режиме игры по сети.
    delete m_server;
    delete m_client;
    m_server = nullptr;
    m_client = nullptr;

    m_gameVariant = GameVariant::None;

    m_readyLabel->setText("Противник не готов");

    m_centralStack->setCurrentIndex(static_cast<int>(Page::Main));

    //Очищаем игровые поля.
    m_firstPlayerField->clear();
    m_secondPlayerField->clear();
    m_firstPlayerHiddenField->clear();
    m_secondPlayerHiddenField->clear();
}