#include "OpponentField.h"
#include "Cell.h"

void OpponentField::enterCellSlot(const int row, const int col)
{
    if (m_stage == Stage::Prepare || m_stage == Stage::None ||
        !m_canShot || m_cellData[row][col]->isChecked()) return;

    m_cellData[row][col]->setPreview();
}

void OpponentField::leaveCellSlot(const int row, const int col)
{
    if (m_stage == Stage::Prepare || !m_canShot) return;

    m_cellData[row][col]->cancelPreview();
}

void OpponentField::clickCellSlot(const int row, const int col)
{
    if (m_stage == Stage::Prepare || m_stage == Stage::None ||
        !m_canShot || m_cellData[row][col]->isChecked()) return;

    if (m_cellData[row][col]->isShip())
    {
        m_cellData[row][col]->setDamaged();
        emit damageShipFromHiddenSignal(row, col);
        --m_shipCellsLeft;

        int boolSum = false;
        Cell* startCell = m_cellData[row][col];
        boolSum += static_cast<int>(
            !startCell->isChecked());
        Cell* currentCell = startCell->getNextShipCellPtr();

        while (currentCell != startCell)
        {
            boolSum += static_cast<int>(
                !currentCell->isChecked());
            currentCell = currentCell->getNextShipCellPtr();
        }

        if (!boolSum)
        {
            startCell->setDestroyed();
            emit destroyShipFromHiddenSignal(row, col);
            missAroundShip(row, col);
            currentCell = startCell->getNextShipCellPtr();

            while (currentCell != startCell)
            {
                auto [row, col] = currentCell->getRowCol();
                currentCell->setDestroyed();
                emit destroyShipFromHiddenSignal(row, col);
                missAroundShip(row, col);
                currentCell = currentCell->getNextShipCellPtr();
            }
        }
    }

    else
    {
        m_cellData[row][col]->setMissed();
        emit missShipFromHiddenSignal(row, col);
        m_canShot = false;
    }
}

OpponentField::OpponentField(QWidget* parent)
    : Field(parent)
    , m_canShot{ false }
    , m_shipCellsLeft{ 20 }
{
}

void OpponentField::startGame()
{
    m_stage = Stage::Game;
}

bool OpponentField::allShipsDestroyed() const
{
    return m_shipCellsLeft == 0;
}

bool OpponentField::canShot() const
{
    return m_canShot;
}

void OpponentField::addShotOpportunity()
{
    m_canShot = true;
}

void OpponentField::missAroundShip(const int row, const int col)
{
    if (row - 1 >= 0)
    {
        if (!m_cellData[row - 1][col]->isChecked())
        {
            m_cellData[row - 1][col]->setMissed();
            emit missShipFromHiddenSignal(row - 1, col);
        }

        if (col - 1 >= 0 && !m_cellData[row - 1][col - 1]->isChecked())
        {
            m_cellData[row - 1][col - 1]->setMissed();
            emit missShipFromHiddenSignal(row - 1, col - 1);
        }


        if (col + 1 < 10 && !m_cellData[row - 1][col + 1]->isChecked())
        {
            m_cellData[row - 1][col + 1]->setMissed();
            emit missShipFromHiddenSignal(row - 1, col + 1);
        }
    }

    if (row + 1 < 10)
    {
        if (!m_cellData[row + 1][col]->isChecked())
        {
            m_cellData[row + 1][col]->setMissed();
            emit missShipFromHiddenSignal(row + 1, col);
        }

        if (col - 1 >= 0 && !m_cellData[row + 1][col - 1]->isChecked())
        {
            m_cellData[row + 1][col - 1]->setMissed();
            emit missShipFromHiddenSignal(row + 1, col - 1);
        }

        if (col + 1 < 10 && !m_cellData[row + 1][col + 1]->isChecked())
        {
            m_cellData[row + 1][col + 1]->setMissed();
            emit missShipFromHiddenSignal(row + 1, col + 1);
        }
    }

    if (col - 1 >= 0)
    {
        if (!m_cellData[row][col - 1]->isChecked())
        {
            m_cellData[row][col - 1]->setMissed();
            emit missShipFromHiddenSignal(row, col - 1);
        }

        if (row - 1 >= 0 && !m_cellData[row - 1][col - 1]->isChecked())
        {
            m_cellData[row - 1][col - 1]->setMissed();
            emit missShipFromHiddenSignal(row - 1, col - 1);
        }

        if (row + 1 < 10 && !m_cellData[row + 1][col - 1]->isChecked())
        {
            m_cellData[row + 1][col - 1]->setMissed();
            emit missShipFromHiddenSignal(row + 1, col - 1);
        }
    }

    if (col + 1 < 10)
    {
        if (!m_cellData[row][col + 1]->isChecked())
        {
            m_cellData[row][col + 1]->setMissed();
            emit missShipFromHiddenSignal(row, col + 1);
        }

        if (row - 1 >= 0 && !m_cellData[row - 1][col + 1]->isChecked())
        {
            m_cellData[row - 1][col + 1]->setMissed();
            emit missShipFromHiddenSignal(row - 1, col + 1);
        }

        if (row + 1 < 10 && !m_cellData[row + 1][col + 1]->isChecked())
        {
            m_cellData[row + 1][col + 1]->setMissed();
            emit missShipFromHiddenSignal(row + 1, col + 1);
        }
    }
}

void OpponentField::clear()
{
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
            m_cellData[row][col]->clear();

    addShotOpportunity();
    m_shipCellsLeft = 20;
    startPrepare();
}