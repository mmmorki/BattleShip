#include "Cell.h"

//Конструктор клетки поля.
Cell::Cell(const int row, const int col, QWidget* parent)
    : QPushButton(parent)
    , m_row{ row }
    , m_col{ col }
{
    m_currentStyle = "background-color: white; border: 1px solid black";
    setFixedSize(40, 40);
    setStyleSheet(m_currentStyle);

    connect(this, &QPushButton::clicked,
        [this](){ emit clickCellSignal(m_row, m_col); });
}

std::pair<int, int> Cell::getRowCol() const
{
    return std::make_pair(m_row, m_col);
}

/* Функция меняет цвет клетки на серую для предпросмотра игроком
 * силуэта устанавливаемого корабля или клетки атаки противника. Функция
 * вызывается из слота наведения на клетку в Field. */
void Cell::setPreview()
{
    setStyleSheet("background-color: grey; border: 1px solid black");
}

/* Функция меняет цвет обратно на текущий установленный при уходе курсора
 * игрока с клетки. Функция вызывается из слота покидания клетки в Field. */
void Cell::cancelPreview()
{
    setStyleSheet(m_currentStyle);
}

void Cell::addShip()
{
    m_currentStyle = "background-color: black; border: 1px solid black";
    setStyleSheet(m_currentStyle);
    m_isShip = true;
}

void Cell::hideCell()
{
    m_currentStyle = "background-color: white; border: 1px solid black";
    setStyleSheet(m_currentStyle);
}

void Cell::removeShip()
{
    m_currentStyle = "background-color: white; border: 1px solid black";
    setStyleSheet(m_currentStyle);
    m_isShip = false;
}

void Cell::setDestroyed()
{
    m_currentStyle = "background-color: green; border: 1px solid black";
    setStyleSheet(m_currentStyle);
}

void Cell::setDamaged()
{
    m_currentStyle = "background-color: orange; border: 1px solid black";
    setStyleSheet(m_currentStyle);
    m_isChecked = true;
}

void Cell::setMissed()
{
    m_currentStyle = "background-color: red; border: 1px solid black";
    setStyleSheet(m_currentStyle);
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

Cell* Cell::deleteNextCellShipPtr()
{
    Cell* nextShipCellPtr{ m_nextShipCellPtr };
    m_nextShipCellPtr = nullptr;

    return nextShipCellPtr;
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

void Cell::showCell()
{
    m_currentStyle = "background-color: black; border: 1px solid black";
    setStyleSheet(m_currentStyle);
}

void Cell::makeCellDark()
{
    if (m_currentStyle == "background-color: white; border: 1px solid black")
        setStyleSheet("background-color: rgb(128, 128, 128); border: 1px solid black");

    else if (m_currentStyle == "background-color: green; border: 1px solid black")
        setStyleSheet("background-color: rgb(0, 64, 0); border: 1px solid black");

    else if (m_currentStyle == "background-color: orange; border: 1px solid black")
        setStyleSheet("background-color: rgb(128, 83, 0); border: 1px solid black");

    else if (m_currentStyle == "background-color: red; border: 1px solid black")
        setStyleSheet("background-color: rgb(128, 0, 0); border: 1px solid black");
}

void Cell::cancelCellDark()
{
    setStyleSheet(m_currentStyle);
}