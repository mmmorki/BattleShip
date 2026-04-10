#include "OpponentField.h"
#include "Cell.h"

#include <iostream>

void OpponentField::enterCellSlot(const int row, const int col)
{
    if (!m_canShot || m_cellData[row][col]->isChecked()) return;

    m_cellData[row][col]->setPreview();
}

void OpponentField::leaveCellSlot(const int row, const int col)
{
    if (!m_canShot) return;

    m_cellData[row][col]->cancelPreview();
}

void OpponentField::clickCellSlot(const int row, const int col)
{
    if (!m_canShot || m_cellData[row][col]->isChecked()) return;

    if (m_sendToOnline)
        emit playerClickCellOnlineSignal(row, col);

    if (m_cellData[row][col]->isShip())
    {
        m_cellData[row][col]->setDamaged();
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
            missAroundShip(row, col);
            currentCell = startCell->getNextShipCellPtr();

            while (currentCell != startCell)
            {
                auto [row, col] = currentCell->getRowCol();
                currentCell->setDestroyed();
                missAroundShip(row, col);
                currentCell = currentCell->getNextShipCellPtr();
            }
        }
    }

    else
    {
        m_cellData[row][col]->setMissed();
        if (!m_getFromOnline)
            emit shotsAreOverSignal();
        m_canShot = false;
    }

    if (m_shipCellsLeft == 0 && !m_getFromOnline)
        emit allShipsAreDestroyedSignal();
}

OpponentField::OpponentField(QWidget* parent)
    : Field{ parent }
    , m_canShot{ false }
    , m_shipCellsLeft{ 20 }
    , m_sendToOnline{ false }
    , m_getFromOnline{ false }
{
}

bool OpponentField::allShipsDestroyed() const
{
    return m_shipCellsLeft == 0;
}

bool OpponentField::canShot() const
{
    return m_canShot;
}

void OpponentField::missAroundShip(const int row, const int col)
{
    if (row - 1 >= 0)
    {
        if (!m_cellData[row - 1][col]->isChecked())
        {
            m_cellData[row - 1][col]->setMissed();
        }

        if (col - 1 >= 0 && !m_cellData[row - 1][col - 1]->isChecked())
        {
            m_cellData[row - 1][col - 1]->setMissed();
        }


        if (col + 1 < 10 && !m_cellData[row - 1][col + 1]->isChecked())
        {
            m_cellData[row - 1][col + 1]->setMissed();
        }
    }

    if (row + 1 < 10)
    {
        if (!m_cellData[row + 1][col]->isChecked())
        {
            m_cellData[row + 1][col]->setMissed();
        }

        if (col - 1 >= 0 && !m_cellData[row + 1][col - 1]->isChecked())
        {
            m_cellData[row + 1][col - 1]->setMissed();
        }

        if (col + 1 < 10 && !m_cellData[row + 1][col + 1]->isChecked())
        {
            m_cellData[row + 1][col + 1]->setMissed();
        }
    }

    if (col - 1 >= 0)
    {
        if (!m_cellData[row][col - 1]->isChecked())
        {
            m_cellData[row][col - 1]->setMissed();
        }

        if (row - 1 >= 0 && !m_cellData[row - 1][col - 1]->isChecked())
        {
            m_cellData[row - 1][col - 1]->setMissed();
        }

        if (row + 1 < 10 && !m_cellData[row + 1][col - 1]->isChecked())
        {
            m_cellData[row + 1][col - 1]->setMissed();
        }
    }

    if (col + 1 < 10)
    {
        if (!m_cellData[row][col + 1]->isChecked())
        {
            m_cellData[row][col + 1]->setMissed();
        }

        if (row - 1 >= 0 && !m_cellData[row - 1][col + 1]->isChecked())
        {
            m_cellData[row - 1][col + 1]->setMissed();
        }

        if (row + 1 < 10 && !m_cellData[row + 1][col + 1]->isChecked())
        {
            m_cellData[row + 1][col + 1]->setMissed();
        }
    }
}

void OpponentField::clear()
{
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
            m_cellData[row][col]->clear();

    activate();
    m_shipCellsLeft = 20;
}

void OpponentField::activate()
{
    m_canShot = true;
}

void OpponentField::deactivate()
{
    m_canShot = false;
}

void OpponentField::createShip(const int row, const int col) const
{
    m_cellData[row][col]->addShip();
    m_cellData[row][col]->hideCell();
}

void OpponentField::removeShip(const int row, const int col) const
{
    m_cellData[row][col]->removeShip();
}

///Функции для онлайн-режима

void OpponentField::sendToOnline()
{
    m_sendToOnline = true;
}

void OpponentField::clickCellOnlineFunc(const int row, const int col)
{
    clickCellSlot(row, col);
}

void OpponentField::getFromOnline()
{
    m_getFromOnline = true;
}