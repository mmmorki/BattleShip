#include "Cell.h"

Cell::Cell(const int row, const int col, QWidget* parent)
    : QPushButton(parent)
    , m_row{ row }
    , m_col{ col }
{
    m_currentStyle = new QString{ "background-color: white; border: 1px solid black" };
    setFixedSize(40, 40);
    setStyleSheet(*m_currentStyle);

    QObject::connect(this, &QPushButton::clicked,
        [this](){ emit clickSignal(m_row, m_col); });
}

void Cell::setPreview()
{
    setStyleSheet("background-color: grey; border: 1xp solid black");
}

void Cell::cancelPreview()
{
    setStyleSheet(*m_currentStyle);
}

void Cell::addShip()
{
    m_currentStyle->clear();
    m_currentStyle->append("background-color: black; border: 1px solid black");
    setStyleSheet(*m_currentStyle);
    m_isShip = true;
}

void Cell::removeShip()
{
    m_currentStyle->clear();
    m_currentStyle->append("background-color: white; border: 1px solid black");
    setStyleSheet(*m_currentStyle);
    m_isShip = false;
}

bool Cell::isShip() const
{
    return m_isShip;
}

void Cell::link(Cell* nextShipCellPtr)
{
    m_nextShipCellPtr = nextShipCellPtr;
}

Cell* Cell::getNextShipCellPtr() const
{
    return m_nextShipCellPtr;
}

void Cell::deleteNextCellShipPtr()
{
    m_nextShipCellPtr = nullptr;
}

void Cell::enterEvent(QEnterEvent* event)
{
    emit enterCellSignal(m_row, m_col);
    QPushButton::enterEvent(event);
}

void Cell::leaveEvent(QEvent* event)
{
    emit leaveCellSignal(m_row, m_col);
    QPushButton::leaveEvent(event);
}