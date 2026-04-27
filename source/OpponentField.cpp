#include "OpponentField.h"
#include "Cell.h"

#include <iostream>

#include <QSoundEffect>

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
        m_dropTheBombHit->play();
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
            m_dropTheBombDestroy->play();
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
        m_dropTheBombMiss->play();
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
    , m_dropTheBombMiss{ new QSoundEffect{ this } }
    , m_dropTheBombHit{ new QSoundEffect{ this } }
    , m_dropTheBombDestroy{ new QSoundEffect{ this } }
{
    m_dropTheBombMiss->setSource(QUrl("qrc:/sounds/events/drop_the_bomb(miss).wav"));
    m_dropTheBombMiss->setVolume(0.3);
    m_dropTheBombHit->setSource(QUrl("qrc:/sounds/events/drop_the_bomb(hit).wav"));
    m_dropTheBombHit->setVolume(0.1);
    m_dropTheBombDestroy->setSource(QUrl("qrc:/sounds/events/drop_the_bomb(destroy).wav"));
    m_dropTheBombDestroy->setVolume(0.3);
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

    m_shipCellsLeft = 20;
    m_canShot = false;
    m_sendToOnline = false;
    m_getFromOnline = false;
}

void OpponentField::activate()
{
    m_canShot = true;
    cancelFieldDark();
}

void OpponentField::deactivate()
{
    m_canShot = false;
    makeFieldDark();
}

void OpponentField::makeFieldDark()
{
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
            m_cellData[row][col]->makeCellDark();
}

void OpponentField::cancelFieldDark()
{
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
            m_cellData[row][col]->cancelCellDark();
}

//Функции для онлайн-режима

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

void OpponentField::showShips() const
{
    for (std::size_t row{ 0 }; row < 10; ++row)
        for (std::size_t col{ 0 }; col < 10; ++col)
            if (m_cellData[row][col]->isShip())
                m_cellData[row][col]->showCell();
}