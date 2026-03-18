#include "Field.h"
#include "Cell.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>

void Field::enterCellSlot(const int row, const int col)
{
    if (m_owner == Owner::Player && m_stage == Stage::Prepare)
    {
        if (m_addMode == AddMode::NoMode || isTooManyShips() || isTooLong(row, col)
            || isShipNear(row, col)) return;

        if (m_addOrientation == AddOrientation::Vertical)
        {
            for (std::size_t index{ 0 }; index <= static_cast<int>(m_addMode); ++index)
                m_cellData[row + index][col]->setPreview();
        }

        else
        {
            for (std::size_t index{ 0 }; index <= static_cast<int>(m_addMode); ++index)
                m_cellData[row][col + index]->setPreview();
        }
    }

    else if (m_owner == Owner::Opponent && m_stage == Stage::Game)
    {
        if (m_shotMode == ShotMode::NoMode) return;


    }
}

void Field::leaveCellSlot(const int row, const int col)
{
    if (m_owner == Owner::Player && m_stage == Stage::Prepare)
    {
        if (m_addMode == AddMode::NoMode) return;

        for (std::size_t index{ 0 }; index < 4; ++index)
        {
            if (row + index < 10)
                m_cellData[row + index][col]->cancelPreview();
            if (col + index < 10)
                m_cellData[row][col + index]->cancelPreview();
        }
    }
}

void Field::clickCellSlot(const int row, const int col)
    {
        if (m_owner == Owner::Player && m_stage == Stage::Prepare)
        {
            if (m_cellData[row][col]->isShip())
            {
                int removedShipCellsCounter{ 0 };
                Cell* currentCell{ m_cellData[row][col]->getNextShipCellPtr() };
                m_cellData[row][col]->deleteNextCellShipPtr();

                while (currentCell != nullptr)
                {
                    currentCell->removeShip();
                    ++removedShipCellsCounter;
                    Cell* previousCell{ currentCell };
                    currentCell = currentCell->getNextShipCellPtr();
                    previousCell->deleteNextCellShipPtr();
                }

                --m_shipsLeft[removedShipCellsCounter - 1];

                return;
            }

            if (m_addMode == AddMode::NoMode || isTooManyShips() || isTooLong(row, col) || isShipNear(row, col)) return;

            if (m_addOrientation == AddOrientation::Vertical)
            {
                Cell* previousCell{ nullptr };
                ++m_shipsLeft[static_cast<int>(m_addMode)];

                for (std::size_t index{ 0 }; index <= static_cast<int>(m_addMode); ++index)
                {
                    Cell* currentCell{ m_cellData[row + index][col] };

                    if (index > 0 && previousCell) previousCell->link(currentCell);

                    if (index == static_cast<int>(m_addMode))
                        currentCell->link(m_cellData[row][col]);

                    currentCell->addShip();
                    previousCell = currentCell;
                }

                return;
            }

            if (m_addOrientation == AddOrientation::Horizontal)
            {
                Cell* previousCell{ nullptr };
                ++m_shipsLeft[static_cast<int>(m_addMode)];

                for (std::size_t index{ 0 }; index <= static_cast<int>(m_addMode); ++index)
                {
                    Cell* currentCell{ m_cellData[row][col + index] };

                    if (index > 0 && previousCell) previousCell->link(currentCell);

                    if (index == static_cast<int>(m_addMode))
                        currentCell->link(m_cellData[row][col]);

                    currentCell->addShip();
                    previousCell = currentCell;
                }
            }
        }
    }

Field::Field(const Owner owner, QWidget* parent)
    : QWidget(parent)
    , m_stage{ Stage::Prepare }
    , m_owner{ owner }
    , m_addMode{ AddMode::NoMode }
    , m_shotMode{ ShotMode::NoMode }
    , m_layout{ new QGridLayout{ this } }
    , m_addShipBtnLayout{ new QHBoxLayout{ this } }
    , m_addShip1Btn{ new QPushButton{ "Лодка", this } }
    , m_addShip2Btn{ new QPushButton{ "Шхуна", this } }
    , m_addShip3Btn{ new QPushButton{ "Фрегат", this } }
    , m_addShip4Btn{ new QPushButton{ "Линкор", this } }
    , m_changeOrientationBtn{ new QPushButton{ "Направление", this } }
{
    //Настройка виджета поля
    setFixedSize(400, 400);
    setLayout(m_layout);
    m_layout->setSpacing(0);

    //Добавление клеток поля
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
        {
            const int intRow{ static_cast<int>(row) };
            const int intCol{ static_cast<int>(col) };

            Cell* currentCell{ new Cell{ intRow, intCol, this } };
            m_cellData[row][col] = currentCell;
            m_layout->addWidget(currentCell, intRow, intCol, 1, 1);

            QObject::connect(currentCell, &Cell::enterCellSignal,
                this, &Field::enterCellSlot);
            QObject::connect(currentCell, &Cell::leaveCellSignal,
                this, &Field::leaveCellSlot);
            QObject::connect(currentCell, &Cell::clickSignal,
                this, &Field::clickCellSlot);
        }

    //Настройка кнопок размещения кораблей
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
    m_addShip1Btn->setShortcut(QKeySequence("1"));
    m_addShip2Btn->setShortcut(QKeySequence("2"));
    m_addShip3Btn->setShortcut(QKeySequence("3"));
    m_addShip4Btn->setShortcut(QKeySequence("4"));
    m_changeOrientationBtn->setShortcut(QKeySequence("R"));
    if (m_owner != Owner::Player)
        hideShipButtons();

    //Подключение кнопок размещения кораблей
    QObject::connect(m_addShip1Btn, &QPushButton::clicked,
        [this](){ m_addMode = AddMode::Ship1; });
    QObject::connect(m_addShip2Btn, &QPushButton::clicked,
        [this](){ m_addMode = AddMode::Ship2; });
    QObject::connect(m_addShip3Btn, &QPushButton::clicked,
        [this](){ m_addMode = AddMode::Ship3; });
    QObject::connect(m_addShip4Btn, &QPushButton::clicked,
        [this](){ m_addMode = AddMode::Ship4; });
    QObject::connect(m_changeOrientationBtn, &QPushButton::clicked,
        [this]() {
            m_addOrientation = static_cast<AddOrientation>((static_cast<int>(m_addOrientation) + 1)
            % static_cast<int>(AddOrientation::MaxAddOrientation));
        });
}

void Field::showShipButtons() const
{
    if (m_owner == Owner::Player)
    {
        m_addShip1Btn->show();
        m_addShip2Btn->show();
        m_addShip3Btn->show();
        m_addShip4Btn->show();
        m_changeOrientationBtn->show();
    }
}

void Field::hideShipButtons() const
{
    m_addShip1Btn->hide();
    m_addShip2Btn->hide();
    m_addShip3Btn->hide();
    m_addShip4Btn->hide();
    m_changeOrientationBtn->hide();
}

bool Field::isTooManyShips() const
{
    return m_shipsLeft[static_cast<std::size_t>(m_addMode)] >= 4 - static_cast<int>(m_addMode);
}

bool Field::isShipNear(const int row, const int col) const
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
                boolSum += static_cast<int>(m_cellData[row + index][col - 1]->isShip());

            if (row - 1 >= 0)
            {
                boolSum += static_cast<int>(m_cellData[row - 1][col - 1]->isShip());
                boolSum += static_cast<int>(m_cellData[row - 1][col]->isShip());
            }

            if (row + length + 1 < 10)
            {
                boolSum += static_cast<int>(m_cellData[row + length + 1][col - 1]->isShip());
                boolSum += static_cast<int>(m_cellData[row + length + 1][col]->isShip());
            }
        }

        if (col + 1 < 10)
        {
            for (std::size_t index{ 0 }; index <= length; ++index)
                boolSum += static_cast<int>(m_cellData[row + index][col + 1]->isShip());

            if (row - 1 >= 0)
                boolSum += static_cast<int>(m_cellData[row - 1][col + 1]->isShip());

            if (row + length + 1 < 10)
                boolSum += static_cast<int>(m_cellData[row + length + 1][col + 1]->isShip());
        }

        for (std::size_t index{ 0 }; index <= length; ++index)
            boolSum += static_cast<int>(m_cellData[row + index][col]->isShip());

        return static_cast<bool>(boolSum);
    }
    case AddOrientation::Horizontal:
    {
        if (row - 1 >= 0)
        {
            for (std::size_t index{ 0 }; index <= length; ++ index)
                boolSum += static_cast<int>(m_cellData[row - 1][col + index]->isShip());

            if (col - 1 >= 0)
            {
                boolSum += static_cast<int>(m_cellData[row - 1][col - 1]->isShip());
                boolSum += static_cast<int>(m_cellData[row][col - 1]->isShip());
            }

            if (col + length + 1 < 10)
            {
                boolSum += static_cast<int>(m_cellData[row - 1][col + length + 1]->isShip());
                boolSum += static_cast<int>(m_cellData[row][col + length + 1]->isShip());
            }
        }

        if (row + 1 < 10)
        {
            for (std::size_t index{ 0 }; index <= length; ++index)
                boolSum += static_cast<int>(m_cellData[row + 1][col + index]->isShip());

            if (col - 1 >= 0)
                boolSum += static_cast<int>(m_cellData[row + 1][col - 1]->isShip());

            if (col + length + 1 < 10)
                boolSum += static_cast<int>(m_cellData[row + 1][col + length + 1]->isShip());
        }

        for (std::size_t index{ 0 }; index <= length; ++index)
            boolSum += static_cast<int>(m_cellData[row][col + index]->isShip());

        return static_cast<bool>(boolSum);
    }
    default:
        return true;
    }
}

bool Field::isTooLong(const int row, const int col) const
{
    switch (m_addOrientation)
    {
    case AddOrientation::Vertical:
        return row + static_cast<int>(m_addMode) >= 10;
    case AddOrientation::Horizontal:
        return col + static_cast<int>(m_addMode) >= 10;
    default:
        return true;
    }
}

bool Field::shipsCompletelyCreated() const
{
    return m_shipsLeft[static_cast<int>(AddMode::Ship1)] == 4
        && m_shipsLeft[static_cast<int>(AddMode::Ship2)] == 3
        && m_shipsLeft[static_cast<int>(AddMode::Ship3)] == 2
        && m_shipsLeft[static_cast<int>(AddMode::Ship4)] == 1;
}

void Field::endPrepare()
{
    m_stage = Stage::Game;
    hideShipButtons();
}

bool Field::isPrepare() const
{
    return m_stage == Stage::Prepare;
}

bool Field::isGame() const
{
    return m_stage == Stage::Game;
}

void Field::switchOff()
{
    m_addMode = AddMode::NoMode;
    m_shotMode = ShotMode::NoMode;
}

void Field::switchOn()
{
    m_shotMode = ShotMode::Shot;
}