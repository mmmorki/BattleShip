#include "Cell.h"

Cell::Cell(const int row, const int col, QWidget* parent)
    : QPushButton(parent)
    , m_row{ row }
    , m_col{ col }
    , m_isShip{ false }
    , m_isChecked{ false }
{
    m_currentStyle = new QString{ "background-color: white; border: 1px solid black" };
    setFixedSize(40, 40);
    setStyleSheet(*m_currentStyle);

    QObject::connect(this, &QPushButton::clicked,
        [this](){ emit clickCellSignal(m_row, m_col); });
}

std::pair<int, int> Cell::getRowCol() const
{
    return std::make_pair(m_row, m_col);
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

void Cell::hideCell()
{
    m_currentStyle->clear();
    m_currentStyle->append("background-color: white; border: 1px solid black");
    setStyleSheet(*m_currentStyle);
}

void Cell::removeShip()
{
    m_currentStyle->clear();
    m_currentStyle->append("background-color: white; border: 1px solid black");
    setStyleSheet(*m_currentStyle);
    m_isShip = false;
}

void Cell::setDestroyed()
{
    m_currentStyle->clear();
    m_currentStyle->append("background-color: green; border: 1px solid black");
    setStyleSheet(*m_currentStyle);
}

void Cell::setDamaged()
{
    m_currentStyle->clear();
    m_currentStyle->append("background-color: orange; border: 1px solid black");
    setStyleSheet(*m_currentStyle);
    m_isChecked = true;
}

void Cell::setMissed()
{
    m_currentStyle->clear();
    m_currentStyle->append("background-color: red; border: 1px solid black");
    setStyleSheet(*m_currentStyle);
    m_isChecked = true;
}

bool Cell::isShip() const
{
    return m_isShip;
}

bool Cell::isChecked() const
{
    return m_isChecked;
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

std::pair<int, int> Cell::getRowAndCol() const
{
    return std::make_pair(m_row, m_col);
}

void Cell::clear()
{
    removeShip();
    m_isChecked = false;
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