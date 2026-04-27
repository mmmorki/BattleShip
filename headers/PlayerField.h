#ifndef NEWPLAYERFIELD_H
#define NEWPLAYERFIELD_H

class OpponentField;

class QHBoxLayout;
class QPushButton;
class QSoundEffect;

#include "Field.h"

class PlayerField final : public Field
{
    Q_OBJECT

public: signals:
    void playerClickCellOnlineSignal(const int row, const int col);
    void playerChangeShipVariantSignal(const int ID);
    void playerChangeOrientationSignal(const int ID);

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

    void updateShipBtnText() const;

    //Функции для онлайн режима

    void clickCellOnlineFunc(const int row, const int col);

    void changeOrientationOnlineFunc(const int ID);

    void changeAddModeOnlineFunc(const int ID);

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
    QSoundEffect* m_createAShip{ nullptr };
    QSoundEffect* m_changeAddOrientation{ nullptr };
    QSoundEffect* m_changeAShip{ nullptr };
};

#endif //NEWPLAYERFIELD_H