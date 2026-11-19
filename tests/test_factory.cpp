#include "Factory.h"
#include <cassert>
#include <iostream>
using namespace forgeflow;
// Vereiste 53: test-driven development (= written test plan or unit tests)
// Buildable tests check placement, rotation, memory ownership and actual production flow.
void placement() {
    Factory f;
    assert(!f.place(Kind::Miner,{1,1},Direction::East));
    assert(f.place(Kind::Miner,{4,5},Direction::East));
    assert(!f.place(Kind::Miner,{4,5},Direction::West));
    assert(!f.place(Kind::Conveyor,{4,5},Direction::East));
    assert(!f.place(Kind::Storage,{-1,2},Direction::North));
    assert(f.erase({4,5}));
    assert(!f.erase({4,5}));
    assert(rotate(Direction::West)==Direction::North);
    assert(neighbour({8,8},Direction::South)==(Cell{8,9}));
}
void cloning() {
    Miner miner({4,5},Direction::East);miner.tick(5);
    std::unique_ptr<Building> copy=miner.clone();
    assert(copy->kind()==Kind::Miner);
    assert(dynamic_cast<Miner*>(copy.get())->stored()==2);
    assert(copy.get()!=&miner);
}
void production() {
    Factory f;
    assert(f.place(Kind::Miner,{4,5},Direction::East));
    assert(f.place(Kind::Conveyor,{5,5},Direction::East));
    assert(f.place(Kind::Conveyor,{6,5},Direction::East));
    assert(f.place(Kind::Smelter,{7,5},Direction::East));
    assert(f.place(Kind::Conveyor,{8,5},Direction::East));
    assert(f.place(Kind::Conveyor,{9,5},Direction::East));
    assert(f.place(Kind::Storage,{10,5},Direction::East));
    for(int i=0;i<2000;++i)f.simulate(0.05);
    assert(f.producedBars()>0);
    assert(f.totalBars()>=f.producedBars());
    // v0.2: the inspection panel reports this precise per-building inventory.
    const auto* storage=dynamic_cast<const Storage*>(f.at({10,5}));
    assert(storage != nullptr && storage->bars()>0);
}
void blocked() {
    Factory f;f.place(Kind::Miner,{4,5},Direction::East);
    f.place(Kind::Conveyor,{5,5},Direction::West); // opposing belt: cannot extract
    for(int i=0;i<400;++i)f.simulate(0.05);
    auto* miner=dynamic_cast<const Miner*>(f.at({4,5}));
    assert(miner && miner->stored()==6);
}
int main(){placement();cloning();production();blocked();std::cout<<"All ForgeFlow core tests passed\n";}
