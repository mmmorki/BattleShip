#ifndef NEWOPPONENTFIELD_H
#define NEWOPPONENTFIELD_H

#include "Field.h"

class QSoundEffect;

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

    void missAroundShip(const int row, const int col);

    void clear() override;

    void activate();

    void makeFieldDark();

    void cancelFieldDark();

    //Функции для онлайн-режиме

    void clickCellOnlineFunc(const int row, const int col);

    void showShips() const;

private:
    bool m_canShot{};
    int m_shipCellsLeft{};
    QSoundEffect* m_dropTheBombMiss{ nullptr };
    QSoundEffect* m_dropTheBombHit{ nullptr };
    QSoundEffect* m_dropTheBombDestroy{ nullptr };
};

#endif //NEWOPPONENTFIELD_H