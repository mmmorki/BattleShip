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
    explicit Cell(int row, int col, QWidget* parent = nullptr);

    void setPreview();

    void cancelPreview();

    void addShip();

    void removeShip();

    [[nodiscard]] bool isShip() const;

    void link(Cell* nextShipCellPtr);

    [[nodiscard]] Cell* getNextShipCellPtr() const;

    void deleteNextCellShipPtr();

protected:
    void enterEvent(QEnterEvent* event) override;

    void leaveEvent(QEvent* event) override;

private:
    int m_row{};
    int m_col{};
    bool m_isShip{ false };
    QString* m_currentStyle{ nullptr };
    Cell* m_nextShipCellPtr{ nullptr };
};

#endif //CELL_H