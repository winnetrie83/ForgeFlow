#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace forgeflow {
// Vereiste 39: at least 4 useful const references for functions
// operator== and operator!= accept const Cell&, setError accepts const std::string&;
// Qt model data() and rowCount() accept const QModelIndex& (see GameController).
// Vereiste 36: everything in one or more self-made useful namespace(s)
// The simulation has its own namespace, separate from Qt and application UI code.

constexpr int MapSize = 24;
enum class Kind : std::uint8_t { Miner, Smelter, Storage, Conveyor };
enum class Direction : std::uint8_t { North, East, South, West };

struct Cell { int x = 0; int y = 0; };
bool operator==(const Cell& left, const Cell& right) noexcept;
bool operator!=(const Cell& left, const Cell& right) noexcept;
Cell neighbour(Cell origin, Direction direction) noexcept;
Direction rotate(Direction direction) noexcept;
Direction directionForLine(Cell start, Cell end) noexcept;
std::string kindName(Kind kind);

// Vereiste 2: useful and correct abstraction (explain why)
// Building defines common placement and update behaviour without exposing machine internals.
// Vereiste 7: useful and correct base class
// Every constructible factory entity shares this useful common base.
// Vereiste 8: useful and correct abstract base class
// Building cannot be instantiated because tick() is pure virtual.
class Building {
public:
    // Vereiste 22: at least 2 parameterized constructors (1/2: Building)
    Building(Cell origin, Direction facing);
    // Vereiste 24: at least 2 destructors (1/2: Building)
    virtual ~Building();
    Building(const Building&) = default;
    Building& operator=(const Building&) = default;

    // Vereiste 9: useful and correct virtual function
    virtual void tick(double seconds) = 0;
    virtual Kind kind() const noexcept = 0;
    virtual std::unique_ptr<Building> clone() const = 0;
    virtual int width() const noexcept { return 1; }
    virtual int height() const noexcept { return 1; }
    Cell cell() const noexcept { return origin_; }
    Direction facing() const noexcept { return facing_; }
    void setCell(Cell position) noexcept { origin_ = position; }
    void setFacing(Direction facing) noexcept { facing_ = facing; }

protected:
    // Vereiste 3: useful and correct encapsulation (explain why)
    // Coordinates and rotation are accessed through a controlled interface, not public writable fields.
    Cell origin_;
    Direction facing_;
};

class Miner final : public Building {
public:
    // Vereiste 21: at least 2 default constructors (1/2: Miner)
    Miner();
    // Vereiste 22: at least 2 parameterized constructors (2/2: Miner)
    Miner(Cell cell, Direction facing);
    // Vereiste 23: at least 2 copy constructors (1/2: Miner)
    Miner(const Miner& other);
    // Vereiste 24: at least 2 destructors (2/2: Miner)
    ~Miner() override;
    void tick(double seconds) override;
    Kind kind() const noexcept override { return Kind::Miner; }
    std::unique_ptr<Building> clone() const override;
    int stored() const noexcept { return stored_; }
    bool takeOne() noexcept;
    void restore(int count, double progress);
    double progress() const noexcept { return progress_; }
private:
    int stored_ = 0;
    double progress_ = 0.0;
};

class Smelter final : public Building {
public:
    // Vereiste 21: at least 2 default constructors (2/2: Smelter)
    Smelter();
    Smelter(Cell cell, Direction facing);
    // Vereiste 23: at least 2 copy constructors (2/2: Smelter)
    Smelter(const Smelter& other);
    void tick(double seconds) override;
    Kind kind() const noexcept override { return Kind::Smelter; }
    std::unique_ptr<Building> clone() const override;
    bool receiveOre() noexcept;
    bool takeBar() noexcept;
    int ore() const noexcept { return ore_; }
    int bars() const noexcept { return bars_; }
    double progress() const noexcept { return progress_; }
    void restore(int ore, int bars, double progress);
private:
    int ore_ = 0, bars_ = 0;
    double progress_ = 0.0;
};

class Storage final : public Building {
public:
    Storage(Cell cell = {}, Direction direction = Direction::North);
    void tick(double seconds) override;
    Kind kind() const noexcept override { return Kind::Storage; }
    std::unique_ptr<Building> clone() const override;
    bool receiveBar() noexcept;
    int bars() const noexcept { return bars_; }
    void restore(int bars);
private:
    int bars_ = 0;
};

class Conveyor final : public Building {
public:
    Conveyor(Cell cell = {}, Direction direction = Direction::East);
    void tick(double seconds) override;
    Kind kind() const noexcept override { return Kind::Conveyor; }
    std::unique_ptr<Building> clone() const override;
    bool receiveOre() noexcept;
    bool receiveBar() noexcept;
    bool containsItem() const noexcept { return item_ != 0; }
    int item() const noexcept { return item_; } // 0 empty, 1 ore, 2 iron bar
    double progress() const noexcept { return progress_; }
    void restore(int item, double progress);
    void clear() noexcept;
private:
    int item_ = 0;
    double progress_ = 0;
};

struct Deposit { Cell cell; };

// Vereiste 6: useful and correct object composition (explain why)
// Factory owns the buildings and deposits that constitute its persistent world.
class Factory {
public:
    Factory();
    // Vereiste 40: 2 useful and correct smart pointers (1: unique_ptr per building)
    // Ownership is explicit; deleting a building does not leak memory.
    const std::vector<std::unique_ptr<Building>>& buildings() const noexcept { return buildings_; }
    const std::vector<Deposit>& deposits() const noexcept { return deposits_; }
    bool place(Kind kind, Cell cell, Direction direction);
    bool erase(Cell cell);
    bool canPlace(Kind kind, Cell cell) const noexcept;
    const Building* at(Cell cell) const noexcept;
    Building* at(Cell cell) noexcept;
    bool isDeposit(Cell cell) const noexcept;
    void simulate(double dt);
    void clear();
    int producedBars() const noexcept;
    int totalOre() const noexcept;
    int totalBars() const noexcept;
    std::string lastError() const { return lastError_; }
    void setError(const std::string& error) { lastError_ = error; }
    void addLoaded(std::unique_ptr<Building> building);
private:
    // Vereiste 45: useful container class
    std::vector<std::unique_ptr<Building>> buildings_;
    std::vector<Deposit> deposits_;
    std::string lastError_;
    void transferFrom(Conveyor& belt);
};
} // namespace forgeflow
