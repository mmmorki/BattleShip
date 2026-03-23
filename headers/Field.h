#ifndef NEWFIELD_H
#define NEWFIELD_H

class Cell;

class QGridLayout;

#include <QWidget>

class Field : public QWidget
{
    Q_OBJECT

private slots:
    virtual void enterCellSlot(const int row, const int col) = 0;
    virtual void leaveCellSlot(const int row, const int col) = 0;
    virtual void clickCellSlot(const int row, const int col) = 0;

public:
    explicit Field(QWidget* parent = nullptr);

    ~Field() override = default;

    void transferShipsTo(const Field* other) const;

    virtual void clear() = 0;

protected:
    Cell* m_cellData[10][10]{};
    QGridLayout* m_layout{ nullptr };
};

#endif //NEWFIELD_H