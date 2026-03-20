#include "Field.h"
#include "Cell.h"

#include <QGridLayout>

Field::Field(QWidget* parent)
    : QWidget(parent)
    , m_layout{ new QGridLayout{ this } }
{
    setFixedSize(400, 400);
    setLayout(m_layout);
    m_layout->setSpacing(0);

    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
        {
            const int intRow{ static_cast<int>(row) };
            const int intCol{ static_cast<int>(col) };

            Cell* currentCell{ new Cell{ intRow, intCol, this } };
            m_cellData[row][col] = currentCell;
            m_layout->addWidget(currentCell, intRow, intCol, 1, 1);

            connect(currentCell, &Cell::enterCellSignal,
                this, &Field::enterCellSlot);
            connect(currentCell, &Cell::leaveCellSignal,
                this, &Field::leaveCellSlot);
            connect(currentCell, &Cell::clickCellSignal,
                this, &Field::clickCellSlot);
        }
}

bool Field::isPrepare() const
{
    return m_stage == Stage::Prepare;
}

void Field::startPrepare()
{
    m_stage = Stage::Prepare;
}

bool Field::isGame() const
{
    return m_stage == Stage::Game;
}

void Field::transferShipsTo(const Field* other) const
{
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
        {
            Cell* currentCell{ m_cellData[row][col] };
            Cell* otherCell{ other->m_cellData[row][col] };

            if (m_cellData[row][col]->isShip())
            {
                otherCell->addShip();
                otherCell->hideCell();

                if (currentCell->getNextShipCellPtr())
                {
                    Cell* linkedCell{ currentCell->getNextShipCellPtr() };
                    auto [linkedRow, linkedCol] = linkedCell->getRowCol();
                    otherCell->link(other->m_cellData[linkedRow][linkedCol]);
                }
            }
        }
}