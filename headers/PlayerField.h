#ifndef NEWPLAYERFIELD_H
#define NEWPLAYERFIELD_H

class OpponentField;

class QHBoxLayout;
class QPushButton;

#include "Field.h"

class PlayerField final : public Field
{
    Q_OBJECT

public: signals:
    void playerCreateAShipSignal(const int row, const int col);
    void playerRemoveAShipSignal(const int row, const int col);

private slots:
    void enterCellSlot(const int row, const int col) override;

    void leaveCellSlot(const int row, const int col) override;

    void clickCellSlot(const int row, const int col) override;

public:
    enum class AddMode
    {
        Ship1,
        Ship2,
        Ship3,
        Ship4,
        None,
    };

    enum class AddOrientation
    {
        None,
        Vertical,
        Horizontal,
    };

    explicit PlayerField(QWidget* parent = nullptr);

    ~PlayerField() override = default;

    void showShipButtons() const;

    void hideShipButtons() const;

    [[nodiscard]] bool isTooManyShips() const;

    [[nodiscard]] bool isShipNear(const int row, const int col) const;

    [[nodiscard]] bool isTooLong(const int row, const int col) const;

    [[nodiscard]] bool allShipsCreated() const;

    void clear() override;

    void hideShips() const;

private:
    AddMode m_addMode{ AddMode::None };
    AddOrientation m_addOrientation{ AddOrientation::None };
    std::array<int, 4> m_shipsLeft{};
    QHBoxLayout* m_addShipBtnLayout{ nullptr };
    QPushButton* m_addShip1Btn{ nullptr };
    QPushButton* m_addShip2Btn{ nullptr };
    QPushButton* m_addShip3Btn{ nullptr };
    QPushButton* m_addShip4Btn{ nullptr };
    QPushButton* m_changeOrientationBtn{ nullptr };
};

#endif //NEWPLAYERFIELD_H