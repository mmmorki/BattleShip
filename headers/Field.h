#ifndef FIELD_H
#define FIELD_H

#include <array>

#include <QWidget>

class Cell;

class QGridLayout;
class QHBoxLayout;
class QPushButton;

class Field final : public QWidget
{
    Q_OBJECT

public:
    enum class Stage
    {
        Prepare,
        Game,
    };
    enum class Owner
    {
        Player,
        Opponent,
    };

    enum class AddMode
    {
        Ship1,
        Ship2,
        Ship3,
        Ship4,
        NoMode,
    };

    enum class ShotMode
    {
        Shot,
        NoMode,
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

    ~Field() override = default;

    void showShipButtons() const;

    void hideShipButtons() const;

    [[nodiscard]] bool shipsCompletelyCreated() const;

    void endPrepare();

    [[nodiscard]] bool isPrepare() const;

    [[nodiscard]] bool isGame() const;

    void switchOff();

    void switchOn();

protected:
    [[nodiscard]] bool isTooManyShips() const;

    [[nodiscard]] bool isShipNear(const int row, const int col) const;

    [[nodiscard]] bool isTooLong(const int row, const int col) const;

private:
    Stage m_stage{};
    Owner m_owner{};
    AddMode m_addMode{};
    ShotMode m_shotMode{};
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