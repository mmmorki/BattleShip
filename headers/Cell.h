#ifndef CELL_H
#define CELL_H

#include <QPushButton>

//Класс клетки поля.
class Cell final : public QPushButton
{
    Q_OBJECT

public: signals:
    void enterCellSignal(int row, int col);
    void leaveCellSignal(int row, int col);
    void clickCellSignal(int row, int col);

public:
    explicit Cell(int row, int col, QWidget* parent = nullptr);

    ~Cell() override = default;

    std::pair<int, int> getRowCol() const;

    void setPreview();

    void cancelPreview();

    void addShip();

    void hideCell();

    void showCell();

    void removeShip();

    void setDestroyed();

    void setDamaged();

    void setMissed();

    [[nodiscard]] bool isShip() const;

    void link(Cell* nextShipCellPtr);

    [[nodiscard]] Cell* getNextShipCellPtr() const;

    Cell* deleteNextCellShipPtr();

    [[nodiscard]] bool isChecked() const;

    [[nodiscard]] std::pair<int, int> getRowAndCol() const;

    void clear();

    void makeCellDark();

    void cancelCellDark();

protected:
    void enterEvent(QEnterEvent* event) override;

    void leaveEvent(QEvent* event) override;

private:
    int m_row{};
    int m_col{};
    bool m_isShip{};
    bool m_isChecked{};
    QString m_currentStyle{};
    Cell* m_nextShipCellPtr{ nullptr };
};

#endif //CELL_H