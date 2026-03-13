#ifndef CELL_H
#define CELL_H

#include <QPushButton>

class Cell final : public QPushButton
{
    Q_OBJECT

public: signals:
    void enterCellSignal(int row, int col);
    void leaveCellSignal(int row, int col);
    void clickSignal(int row, int col);

public:
    explicit Cell(const int row, const int col, QWidget* parent = nullptr)
        : m_row{ row }
        , m_col{ col }
    {
        m_currentStyle = new QString{ "background-color: white; border: 1px solid black" };
        setFixedSize(40, 40);
        setStyleSheet(*m_currentStyle);

        QObject::connect(this, &QPushButton::clicked,
            [this](){ emit clickSignal(m_row, m_col); });
    }

    void setPreview()
    {
        setStyleSheet("background-color: grey; border: 1xp solid black");
    }

    void cancelPreview()
    {
        setStyleSheet(*m_currentStyle);
    }

    void addShip()
    {
        m_currentStyle->clear();
        m_currentStyle->append("background-color: black; border: 1px solid black");
        setStyleSheet(*m_currentStyle);
        m_isShip = true;
    }

    void removeShip()
    {
        m_currentStyle->clear();
        m_currentStyle->append("background-color: white; border: 1px solid black");
        setStyleSheet(*m_currentStyle);
        m_isShip = false;
    }

    [[nodiscard]] bool isShip() const
    {
        return m_isShip;
    }

    void link(Cell* nextShipCellPtr)
    {
        m_nextShipCellPtr = nextShipCellPtr;
    }

    [[nodiscard]] Cell* getNextShipCellPtr() const
    {
        return m_nextShipCellPtr;
    }

    void deleteNextCellShipPtr()
    {
        m_nextShipCellPtr = nullptr;
    }

protected:
    void enterEvent(QEnterEvent* event) override
    {
        emit enterCellSignal(m_row, m_col);
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override
    {
        emit leaveCellSignal(m_row, m_col);
        QPushButton::leaveEvent(event);
    }

private:
    int m_row{};
    int m_col{};
    bool m_isShip{ false };
    QString* m_currentStyle{ nullptr };
    Cell* m_nextShipCellPtr{ nullptr };
};

#endif //CELL_H