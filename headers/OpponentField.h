#ifndef NEWOPPONENTFIELD_H
#define NEWOPPONENTFIELD_H

#include "Field.h"

class OpponentField final : public Field
{
    Q_OBJECT

private slots:
    void enterCellSlot(const int row, const int col) override;

    void leaveCellSlot(const int row, const int col) override;

    void clickCellSlot(const int row, const int col) override;

public:
    explicit OpponentField(QWidget* parent = nullptr);

    [[nodiscard]] bool allShipsDestroyed() const;

    [[nodiscard]] bool canShot() const;

    void addShotOpportunity();

    void missAroundShip(const int row, const int col);

    void clear() override;

    void activate();

public: signals:
    void missShipFromHiddenSignal(const int row, const int col);
    void damageShipFromHiddenSignal(const int row, const int col);
    void destroyShipFromHiddenSignal(const int row, const int col);
    void shotsAreOverSignal();
    void allShipsAreDestroyedSignal();

private:
    bool m_canShot{};
    int m_shipCellsLeft{};
};

#endif //NEWOPPONENTFIELD_H