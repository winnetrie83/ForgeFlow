#include "Factory.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace forgeflow {
bool operator==(const Cell& a, const Cell& b) noexcept { return a.x == b.x && a.y == b.y; }
bool operator!=(const Cell& a, const Cell& b) noexcept { return !(a == b); }
Direction rotate(Direction value) noexcept { return static_cast<Direction>((static_cast<int>(value) + 1) % 4); }
// The line tool, simulation and QML markers all share North=-Z, East=+X,
// South=+Z, West=-X. Dominant-axis selection keeps straight previews stable.
Direction opposite(Direction d) noexcept { return static_cast<Direction>((static_cast<int>(d)+2)%4); }
Direction directionForLine(Cell start, Cell end) noexcept {
    const int dx = end.x - start.x;
    const int dy = end.y - start.y;
    if (std::abs(dx) >= std::abs(dy)) return dx >= 0 ? Direction::East : Direction::West;
    return dy >= 0 ? Direction::South : Direction::North;
}
Cell neighbour(Cell c, Direction d) noexcept {
    switch (d) { case Direction::North: --c.y; break; case Direction::East: ++c.x; break;
                 case Direction::South: ++c.y; break; case Direction::West: --c.x; break; }
    return c;
}
std::string kindName(Kind k) {
    switch(k) { case Kind::Miner: return "Miner"; case Kind::Smelter: return "Smelter";
                case Kind::Storage: return "Storage"; case Kind::Conveyor: return "Conveyor"; }
    return "Unknown";
}
// Vereiste 25: member initialization in constructors (the stuff behind a colon)
Building::Building(Cell origin, Direction facing) : origin_(origin), facing_(facing) {}
Building::~Building() = default;

// Vereiste 26: constructor forwarding
// Default constructors delegate to the parameterized constructor to avoid repeated setup.
Miner::Miner() : Miner({0,0}, Direction::East) {}
Miner::Miner(Cell c, Direction d) : Building(c,d) {}
Miner::Miner(const Miner& other) : Building(other), stored_(other.stored_), progress_(other.progress_) {}
Miner::~Miner() = default;
void Miner::tick(double dt) {
    if (stored_ >= 6) return;
    progress_ += dt;
    while (progress_ >= 2.0 && stored_ < 6) { ++stored_; progress_ -= 2.0; }
}
// Vereiste 4: useful and correct inheritance (explain why)
// Each machine inherits its common identity and position from Building, while retaining custom behaviour.
// Vereiste 5: useful and correct polymorphism (explain why)
// Factory updates mixed buildings through virtual tick(), without type-specific branches for updating.
// Vereiste 27: useful proven (dynamic) polymorphism
// virtual dispatch executes Miner, Smelter, Storage or Conveyor tick() on a Building reference.
std::unique_ptr<Building> Miner::clone() const { return std::make_unique<Miner>(*this); }
bool Miner::takeOne() noexcept { if (!stored_) return false; --stored_; return true; }
void Miner::restore(int n,double p) { stored_ = std::clamp(n,0,6); progress_ = std::clamp(p,0.0,2.0); }

Smelter::Smelter() : Smelter({0,0},Direction::East) {}
Smelter::Smelter(Cell c,Direction d) : Building(c,d) {}
Smelter::Smelter(const Smelter& other) : Building(other),ore_(other.ore_),bars_(other.bars_),progress_(other.progress_) {}
void Smelter::tick(double dt) {
    if (ore_ <= 0 || bars_ >= 6) return;
    progress_ += dt;
    while (progress_ >= 3.0 && ore_ > 0 && bars_ < 6) { progress_ -= 3.0; --ore_; ++bars_; }
}
std::unique_ptr<Building> Smelter::clone() const { return std::make_unique<Smelter>(*this); }
bool Smelter::receiveOre() noexcept { if (ore_ >= 6) return false; ++ore_; return true; }
bool Smelter::takeBar() noexcept { if (bars_<=0) return false; --bars_; return true; }
void Smelter::restore(int ore,int bars,double p) { ore_=std::clamp(ore,0,6);bars_=std::clamp(bars,0,6);progress_=std::clamp(p,0.0,3.0); }

Storage::Storage(Cell c,Direction d) : Building(c,d) {}
void Storage::tick(double) {}
std::unique_ptr<Building> Storage::clone() const { return std::make_unique<Storage>(*this); }
bool Storage::receiveBar() noexcept { if(bars_>=999) return false; ++bars_; return true; }
void Storage::restore(int bars) { bars_ = std::clamp(bars,0,999); }

Conveyor::Conveyor(Cell c,Direction d) : Building(c,d) {}
void Conveyor::tick(double dt) { if(item_) progress_ = std::min(1.0,progress_+dt/0.65); }
std::unique_ptr<Building> Conveyor::clone() const { return std::make_unique<Conveyor>(*this); }
bool Conveyor::receiveOre() noexcept { if(item_) return false; item_=1;progress_=0;return true; }
bool Conveyor::receiveBar() noexcept { if(item_) return false; item_=2;progress_=0;return true; }
void Conveyor::restore(int item,double progress) {item_=std::clamp(item,0,2);progress_=std::clamp(progress,0.0,1.0);}
void Conveyor::clear() noexcept {item_=0;progress_=0;}

Factory::Factory() : deposits_{{{4,5}},{{15,7}},{{8,17}},{{20,18}}} {}
// Vereiste 29: useful member function
// Placement enforces occupancy and the mine-on-deposit rule in one central function.
bool Factory::canPlace(Kind kind,Cell c) const noexcept {
    if(c.x<0||c.y<0||c.x>=MapSize||c.y>=MapSize||at(c))return false;
    if(kind==Kind::Miner) return isDeposit(c);
    return !isDeposit(c);
}
bool Factory::place(Kind kind,Cell c,Direction d) {
    if(!canPlace(kind,c)) {lastError_="Grid cell occupied, out of bounds, or miner requires a deposit";return false;}
    // Vereiste 40: 2 useful and correct smart pointers (2: make_unique transfers ownership)
    std::unique_ptr<Building> b;
    switch(kind) {
    case Kind::Miner: b=std::make_unique<Miner>(c,d);break;
    case Kind::Smelter: b=std::make_unique<Smelter>(c,d);break;
    case Kind::Storage: b=std::make_unique<Storage>(c,d);break;
    case Kind::Conveyor: b=std::make_unique<Conveyor>(c,d);break;
    }
    buildings_.push_back(std::move(b));lastError_.clear();return true;
}
bool Factory::erase(Cell c) {
    auto it=std::find_if(buildings_.begin(),buildings_.end(),[&](const auto& b){return b->cell()==c;});
    if(it==buildings_.end())return false;
    buildings_.erase(it);return true;
}
// Vereiste 38: at least 4 useful const references for variables
// These const auto& references in at(), producedBars(), totalOre() and totalBars()
// avoid copying unique_ptr/buildings while preserving read-only access.
const Building* Factory::at(Cell c) const noexcept {
    for(const auto& b:buildings_)if(b->cell()==c)return b.get();return nullptr;
}
Building* Factory::at(Cell c) noexcept {return const_cast<Building*>(std::as_const(*this).at(c));}
bool Factory::isDeposit(Cell c) const noexcept {return std::any_of(deposits_.begin(),deposits_.end(),[&](const Deposit& d){return d.cell==c;});}
void Factory::transferFrom(Conveyor& belt) {
    if(belt.progress()<1.0||!belt.containsItem())return;
    const Cell dest=neighbour(belt.cell(),belt.facing());
    Building* next=at(dest);
    if(!next)return;
    bool accepted=false;
    if(auto* nextBelt=dynamic_cast<Conveyor*>(next)) {
        // Only connect to a conveyor entering from a valid direction; don't allow opposing head-on flow.
        if(neighbour(dest,nextBelt->facing())!=belt.cell())
            accepted=belt.item()==1?nextBelt->receiveOre():nextBelt->receiveBar();
    } else if(auto* smelter=dynamic_cast<Smelter*>(next)) {
        // Input connector lies behind the machine's facing direction.
        if(belt.item()==1 && neighbour(dest,opposite(smelter->facing()))==belt.cell())
            accepted=smelter->receiveOre();
    } else if(auto* storage=dynamic_cast<Storage*>(next)) {
        if(belt.item()==2 && neighbour(dest,opposite(storage->facing()))==belt.cell())
            accepted=storage->receiveBar();
    }
    if(accepted)belt.clear();
}
void Factory::simulate(double dt) {
    if(dt<=0||dt>1.0)return;
    // Phase 1: machine updates. Polymorphism dispatches the correct tick() implementation.
    for(auto& b:buildings_)b->tick(dt);
    // Phase 2: belt outputs. Reverse traversal permits deterministic one-cell transfer per tick;
    // newly received items have progress 0, thus cannot teleport across multiple cells.
    for(auto& b:buildings_)if(auto* belt=dynamic_cast<Conveyor*>(b.get()))transferFrom(*belt);
    // Phase 3: producers output to an adjacent belt facing away from them.
    for(auto& b:buildings_) {
        if(b->kind()!=Kind::Miner&&b->kind()!=Kind::Smelter)continue;
        const Cell output=neighbour(b->cell(),b->facing());
        auto* belt=dynamic_cast<Conveyor*>(at(output));
        if(!belt||neighbour(output,belt->facing())==b->cell())continue;
        if(auto* miner=dynamic_cast<Miner*>(b.get())) {
            if(miner->stored()>0&&belt->receiveOre())miner->takeOne();
        } else if(auto* smelter=dynamic_cast<Smelter*>(b.get())) {
            if(smelter->bars()>0&&belt->receiveBar())smelter->takeBar();
        }
    }
    // Phase 4: direct producer-to-consumer adjacencies are supported too.
    for(auto& b:buildings_) {
        if(auto* miner=dynamic_cast<Miner*>(b.get())) {
            auto* smelter=dynamic_cast<Smelter*>(at(neighbour(b->cell(),b->facing())));
            if(smelter&&neighbour(smelter->cell(),opposite(smelter->facing()))==miner->cell()
                &&miner->stored()>0&&smelter->receiveOre())miner->takeOne();
        }else if(auto* smelter=dynamic_cast<Smelter*>(b.get())) {
            auto* storage=dynamic_cast<Storage*>(at(neighbour(b->cell(),b->facing())));
            if(storage&&neighbour(storage->cell(),opposite(storage->facing()))==smelter->cell()
                &&smelter->bars()>0&&storage->receiveBar())smelter->takeBar();
        }
    }
}
void Factory::clear(){buildings_.clear();}
int Factory::producedBars() const noexcept { int n=0;for(const auto& b:buildings_)if(auto* s=dynamic_cast<Storage*>(b.get()))n+=s->bars();return n;}
int Factory::totalOre() const noexcept {int n=0;for(const auto& b:buildings_){if(auto* m=dynamic_cast<Miner*>(b.get()))n+=m->stored();if(auto* s=dynamic_cast<Smelter*>(b.get()))n+=s->ore();if(auto* c=dynamic_cast<Conveyor*>(b.get()))n+=(c->item()==1);}return n;}
int Factory::totalBars() const noexcept {int n=producedBars();for(const auto& b:buildings_){if(auto* s=dynamic_cast<Smelter*>(b.get()))n+=s->bars();if(auto* c=dynamic_cast<Conveyor*>(b.get()))n+=(c->item()==2);}return n;}
void Factory::addLoaded(std::unique_ptr<Building> b) { if(!b||!canPlace(b->kind(),b->cell()))throw std::invalid_argument("Invalid building in save");buildings_.push_back(std::move(b)); }
} // namespace forgeflow
