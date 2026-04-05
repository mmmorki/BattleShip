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

void Field::bindShips(std::vector<std::pair<int, int>>& vector) const
{
    std::vector<Cell*> cells{};
    std::size_t vectorSize{ vector.size() };

    for (std::size_t index{ 0 }; index < vectorSize; ++index)
        cells.push_back(m_cellData[vector[index].first][vector[index].second]);

    for (std::size_t index{ 0 }; index < vectorSize; ++index)
    {
        if (index == vectorSize - 1)
            cells[index]->link(cells[0]);

        else
            cells[index]->link(cells[index + 1]);
    }
}