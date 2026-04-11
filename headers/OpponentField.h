#ifndef NEWOPPONENTFIELD_H
#define NEWOPPONENTFIELD_H

#include "Field.h"

class OpponentField final : public Field
{
    Q_OBJECT

public: signals:
    void playerClickCellOnlineSignal(const int row, const int col);
    void shotsAreOverSignal();
    void allShipsAreDestroyedSignal();

private slots:
    void enterCellSlot(const int row, const int col) override;
    void leaveCellSlot(const int row, const int col) override;
    void clickCellSlot(const int row, const int col) override;

public:
    explicit OpponentField(QWidget* parent = nullptr);

    [[nodiscard]] bool allShipsDestroyed() const;

    [[nodiscard]] bool canShot() const;

    void missAroundShip(const int row, const int col);

    void clear() override;

    void activate();

    void deactivate();

    //Функции для онлайн-режиме

    void sendToOnline();

    void getFromOnline();

    void clickCellOnlineFunc(const int row, const int col);

private:
    bool m_canShot{};
    int m_shipCellsLeft{};
    bool m_sendToOnline{};
    bool m_getFromOnline{};
};

#endif //NEWOPPONENTFIELD_H