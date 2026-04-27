#include "PlayerField.h"
#include "Cell.h"
#include "OpponentField.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QSoundEffect>

#include <array>
#include <vector>

void PlayerField::enterCellSlot(const int row, const int col)
{
    if (m_addMode == AddMode::None || isTooManyShips()
        || isTooLong(row, col) || isShipNear(row, col)) return;

    const int length{ static_cast<int>(m_addMode) };

    if (m_addOrientation == AddOrientation::Vertical)
        for (std::size_t index{ 0 }; index <= length; ++index)
            m_cellData[row + index][col]->setPreview();

    else
        for (std::size_t index{ 0 }; index <= length; ++index)
            m_cellData[row][col + index]->setPreview();
}

void PlayerField::leaveCellSlot(const int row, const int col)
{
    for (std::size_t index{ 0 }; index < 4; ++index)
    {
        if (row + index < 10)
            m_cellData[row + index][col]->cancelPreview();
        if (col + index < 10)
            m_cellData[row][col + index]->cancelPreview();
    }
}

void PlayerField::clickCellSlot(const int row, const int col)
{
    playerClickCellOnlineSignal(row, col);

    if (m_cellData[row][col]->isShip())
    {
        m_createAShip->play();
        const int removedShipCellsCounter{ removeShipByCoord(row, col) };
        --m_shipsLeft[removedShipCellsCounter - 1];
        updateShipBtnText();
        return;
    }

    if (m_addMode == AddMode::None || isTooManyShips()
        || isTooLong(row, col) || isShipNear(row, col)) return;

    const int length{ static_cast<int>(m_addMode) };
    std::vector<std::pair<int, int>> shipsToBind{};
    shipsToBind.reserve(length  + 1);
    m_createAShip->play();

    if (m_addOrientation == AddOrientation::Vertical)
    {
        for (std::size_t index{ 0 }; index <= length; ++index)
            shipsToBind.emplace_back(row + index, col);
        ++m_shipsLeft[static_cast<int>(m_addMode)];
    }

    else if (m_addOrientation == AddOrientation::Horizontal)
    {
        ++m_shipsLeft[static_cast<int>(m_addMode)];

        for (std::size_t index{ 0 }; index <= length; ++index)
            shipsToBind.emplace_back(row, col + index);
    }

    updateShipBtnText();
    createShipByCoord(shipsToBind);
}

PlayerField::PlayerField(QWidget* parent)
    : Field(parent)
    , m_addOrientation{ AddOrientation::Vertical }
    , m_shipsLeft{ 0, 0, 0, 0 }
    , m_addShipBtnLayout{ new QHBoxLayout{ this } }
    , m_addShip1Btn{ new QPushButton{ "Лодка\n(0/4)", this } }
    , m_addShip2Btn{ new QPushButton{ "Шхуна\n(0/3)", this } }
    , m_addShip3Btn{ new QPushButton{ "Фрегат\n(0/2)", this } }
    , m_addShip4Btn{ new QPushButton{ "Линкор\n(0/1)", this } }
    , m_changeOrientationBtn{ new QPushButton{ "Направление\n(Вертикальное)", this } }
    , m_createAShip{ new QSoundEffect{ this } }
    , m_changeAddOrientation{ new QSoundEffect{ this } }
    , m_changeAShip{ new QSoundEffect{ this } }
{
    m_createAShip->setSource(QUrl("qrc:/sounds/events/create_a_ship.wav"));
    m_createAShip->setVolume(0.1);
    m_changeAddOrientation->setSource(QUrl("qrc:/sounds/events/change_add_orientation.wav"));
    m_changeAddOrientation->setVolume(0.3);
    m_changeAShip->setSource(QUrl("qrc:/sounds/events/change_a_ship.wav"));
    m_changeAShip->setVolume(0.3);

    m_layout->addLayout(m_addShipBtnLayout, 10, 0, 1, 10);
    m_addShipBtnLayout->addWidget(m_addShip1Btn);
    m_addShipBtnLayout->addWidget(m_addShip2Btn);
    m_addShipBtnLayout->addWidget(m_addShip3Btn);
    m_addShipBtnLayout->addWidget(m_addShip4Btn);
    m_addShipBtnLayout->addWidget(m_changeOrientationBtn);
    m_addShip1Btn->setFixedSize(80, 50);
    m_addShip2Btn->setFixedSize(80, 50);
    m_addShip3Btn->setFixedSize(80, 50);
    m_addShip4Btn->setFixedSize(80, 50);
    m_changeOrientationBtn->setFixedSize(80, 50);
    m_addShip1Btn->setShortcut(QKeySequence('1'));
    m_addShip2Btn->setShortcut(QKeySequence('2'));
    m_addShip3Btn->setShortcut(QKeySequence('3'));
    m_addShip4Btn->setShortcut(QKeySequence('4'));
    m_changeOrientationBtn->setShortcut(QKeySequence('R'));

    auto changeOrientationBtnLambda{
        [this] {
            m_changeAddOrientation->play();

            if (m_addOrientation == AddOrientation::Vertical)
            {
                m_addOrientation = AddOrientation::Horizontal;
                emit playerChangeOrientationSignal(2);
            }

            else
            {
                m_addOrientation = AddOrientation::Vertical;
                emit playerChangeOrientationSignal(1);
            }
        }
    };

    auto chooseShip1BtnLambda{
        [this] {
            m_changeAddOrientation->play();
            m_addMode = AddMode::Ship1;
            emit playerChangeShipVariantSignal(0);
        }
    };

    auto chooseShip2BtnLambda{
        [this] {
            m_changeAddOrientation->play();
            m_addMode = AddMode::Ship2;
            emit playerChangeShipVariantSignal(1);
        }
    };

    auto chooseShip3BtnLambda{
        [this] {
            m_changeAddOrientation->play();
            m_addMode = AddMode::Ship3;
            emit playerChangeShipVariantSignal(2);
        }
    };

    auto chooseShip4BtnLambda{
        [this] {
            m_changeAddOrientation->play();
            m_addMode = AddMode::Ship4;
            emit playerChangeShipVariantSignal(3);
        }
    };

    connect(m_addShip1Btn, &QPushButton::clicked, chooseShip1BtnLambda);
    connect(m_addShip2Btn, &QPushButton::clicked, chooseShip2BtnLambda);
    connect(m_addShip3Btn, &QPushButton::clicked, chooseShip3BtnLambda);
    connect(m_addShip4Btn, &QPushButton::clicked, chooseShip4BtnLambda);
    connect(m_changeOrientationBtn, &QPushButton::clicked,
        changeOrientationBtnLambda);
}

void PlayerField::showShipButtons() const
{
    m_addShip1Btn->show();
    m_addShip2Btn->show();
    m_addShip3Btn->show();
    m_addShip4Btn->show();
    m_changeOrientationBtn->show();
}

void PlayerField::hideShipButtons() const
{
    m_addShip1Btn->hide();
    m_addShip2Btn->hide();
    m_addShip3Btn->hide();
    m_addShip4Btn->hide();
    m_changeOrientationBtn->hide();
}

bool PlayerField::isTooManyShips() const
{
    const int maxShipsCount{ 4 - static_cast<int>(m_addMode) };
    return m_shipsLeft[static_cast<std::size_t>(m_addMode)]
        >= maxShipsCount;
}

bool PlayerField::isShipNear(const int row, const int col) const
{
    int boolSum{};

    const int length{ static_cast<int>(m_addMode) };

    switch (m_addOrientation)
    {
    case AddOrientation::Vertical:
    {
        if (col - 1 >= 0)
        {
            for (std::size_t index{ 0 }; index <= length; ++index)
                boolSum += static_cast<int>(
                    m_cellData[row + index][col - 1]->isShip());

            if (row - 1 >= 0)
            {
                boolSum += static_cast<int>(
                    m_cellData[row - 1][col - 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row - 1][col]->isShip());
            }

            if (row + length + 1 < 10)
            {
                boolSum += static_cast<int>(
                    m_cellData[row + length + 1][col - 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row + length + 1][col]->isShip());
            }
        }

        if (col + 1 < 10)
        {
            for (std::size_t index{ 0 }; index <= length; ++index)
                boolSum += static_cast<int>(
                    m_cellData[row + index][col + 1]->isShip());

            if (row - 1 >= 0)
            {
                boolSum += static_cast<int>(
                    m_cellData[row - 1][col + 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row - 1][col]->isShip());
            }

            if (row + length + 1 < 10)
            {
                boolSum += static_cast<int>(
                    m_cellData[row + length + 1][col + 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row + length + 1][col]->isShip());
            }
        }

        for (std::size_t index{ 0 }; index <= length; ++index)
            boolSum += static_cast<int>(
                m_cellData[row + index][col]->isShip());

        return static_cast<bool>(boolSum);
    }
    case AddOrientation::Horizontal:
    {
        if (row - 1 >= 0)
        {
            for (std::size_t index{ 0 }; index <= length; ++ index)
                boolSum += static_cast<int>(
                    m_cellData[row - 1][col + index]->isShip());

            if (col - 1 >= 0)
            {
                boolSum += static_cast<int>(
                    m_cellData[row - 1][col - 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row][col - 1]->isShip());
            }

            if (col + length + 1 < 10)
            {
                boolSum += static_cast<int>(
                    m_cellData[row - 1][col + length + 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row][col + length + 1]->isShip());
            }
        }

        if (row + 1 < 10)
        {
            for (std::size_t index{ 0 }; index <= length; ++index)
                boolSum += static_cast<int>(
                    m_cellData[row + 1][col + index]->isShip());

            if (col - 1 >= 0)
            {
                boolSum += static_cast<int>(
                    m_cellData[row + 1][col - 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row][col - 1]->isShip());
            }

            if (col + length + 1 < 10)
            {
                boolSum += static_cast<int>(
                    m_cellData[row + 1][col + length + 1]->isShip());
                boolSum += static_cast<int>(
                    m_cellData[row][col + length + 1]->isShip());
            }
        }

        for (std::size_t index{ 0 }; index <= length; ++index)
            boolSum += static_cast<int>(
                m_cellData[row][col + index]->isShip());

        return static_cast<bool>(boolSum);
    }
    default:
        return true;
    }
}

bool PlayerField::isTooLong(const int row, const int col) const
{
    const int length{ static_cast<int>(m_addMode) };

    switch (m_addOrientation)
    {
        case AddOrientation::Vertical:
            return row + length >= 10;
        case AddOrientation::Horizontal:
            return col + length >= 10;
        default: return true;
    }
}

bool PlayerField::allShipsCreated() const
{
    return m_shipsLeft[static_cast<int>(AddMode::Ship1)] == 4
        && m_shipsLeft[static_cast<int>(AddMode::Ship2)] == 3
        && m_shipsLeft[static_cast<int>(AddMode::Ship3)] == 2
        && m_shipsLeft[static_cast<int>(AddMode::Ship4)] == 1;
}

void PlayerField::clear()
{
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
            m_cellData[row][col]->clear();

    m_addOrientation = AddOrientation::Vertical;
    m_addMode = AddMode::None;
    m_shipsLeft = { 0, 0, 0, 0 };

    updateShipBtnText();
    showShipButtons();
}

void PlayerField::updateShipBtnText() const
{
    m_addShip1Btn->setText(QString("Лодка\n(%1/4)").arg(m_shipsLeft[0]));
    m_addShip2Btn->setText(QString("Шхуна\n(%1/3)").arg(m_shipsLeft[1]));
    m_addShip3Btn->setText(QString("Фрегат\n(%1/2)").arg(m_shipsLeft[2]));
    m_addShip4Btn->setText(QString("Линкор\n(%1/1)").arg(m_shipsLeft[3]));
}

void PlayerField::clickCellOnlineFunc(const int row, const int col)
{
    if (m_cellData[row][col]->isShip())
    {
        const int removedShipCellsCounter{ removeShipByCoord(row, col) };
        --m_shipsLeft[removedShipCellsCounter - 1];
        return;
    }

    if (m_addMode == AddMode::None || isTooManyShips()
        || isTooLong(row, col) || isShipNear(row, col)) return;

    const int length{ static_cast<int>(m_addMode) };
    std::vector<std::pair<int, int>> shipsToBind{};
    shipsToBind.reserve(length  + 1);

    if (m_addOrientation == AddOrientation::Vertical)
    {
        for (std::size_t index{ 0 }; index <= length; ++index)
            shipsToBind.emplace_back(row + index, col);
        ++m_shipsLeft[static_cast<int>(m_addMode)];
    }

    else if (m_addOrientation == AddOrientation::Horizontal)
    {
        ++m_shipsLeft[static_cast<int>(m_addMode)];

        for (std::size_t index{ 0 }; index <= length; ++index)
            shipsToBind.emplace_back(row, col + index);
    }

    createShipByCoord(shipsToBind);
}

void PlayerField::changeOrientationOnlineFunc(const int ID)
{
    m_addOrientation = static_cast<AddOrientation>(ID);
}

void PlayerField::changeAddModeOnlineFunc(const int ID)
{
    m_addMode = static_cast<AddMode>(ID);
}