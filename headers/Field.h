#ifndef FIELD_H
#define FIELD_H

#include "Cell.h"

#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>

#include <array>

class Field final : public QWidget
{
    Q_OBJECT

public:
    enum class GameStage
    {
        Settings,
        Game,
    };

    enum class Owner
    {
        Player,
        Opponent,
    };

    enum class AddMode
    {
        NoMode,
        Ship1 = 1,
        Ship2 = 2,
        Ship3 = 3,
        Ship4 = 4,
    };

    enum class AddOrientation
    {
        Vertical,
        Horizontal,
        MaxAddOrientation,
    };

private slots:
    void enterCellSlot(int row, int col);

    void leaveCellSlot(int row, int col);

    void clickCellSlot(int row, int col);

public:
    explicit Field(Owner owner, QWidget* parent = nullptr);

    void showShipButtons() const;

    void hideShipButtons() const;

    [[nodiscard]] bool isTooManyShips() const;

private:
    Owner m_owner{};
    GameStage m_gameStage{ GameStage::Settings };
    AddMode m_addMode{ AddMode::NoMode };
    AddOrientation m_addOrientation{ AddOrientation::Vertical };
    Cell* m_cellData[10][10]{};
    std::array<int, 4> m_shipsLeft{ 0, 0, 0, 0 };
    QGridLayout* m_layout{ nullptr };
    QHBoxLayout* m_addShipBtnLayout{ nullptr };
    QPushButton* m_addShip1Btn{ nullptr };
    QPushButton* m_addShip2Btn{ nullptr };
    QPushButton* m_addShip3Btn{ nullptr };
    QPushButton* m_addShip4Btn{ nullptr };
    QPushButton* m_changeOrientationBtn{ nullptr };
};

#endif //FIELD_H