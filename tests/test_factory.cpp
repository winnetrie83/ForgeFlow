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
void orientationAndConnections() {
    assert(directionForLine({4,4},{8,4}) == Direction::East);
    assert(directionForLine({8,4},{4,4}) == Direction::West);
    assert(directionForLine({4,4},{4,8}) == Direction::South);
    assert(directionForLine({4,8},{4,4}) == Direction::North);
    // Backside input connector is required; not merely adjacency.
    Factory wrong;
    wrong.place(Kind::Miner,{4,5},Direction::East);
    wrong.place(Kind::Conveyor,{5,5},Direction::East);
    wrong.place(Kind::Smelter,{6,5},Direction::North);
    for(int i=0;i<800;++i) wrong.simulate(0.05);
    const auto* bad=dynamic_cast<const Smelter*>(wrong.at({6,5}));
    assert(bad && bad->ore()==0 && bad->bars()==0);
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
// Regression: each compass direction must transport material all the way
// from a deposit through belts to a downstream machine's INPUT connector.
void directionalProduction() {
    const Cell deposit{4,5};
    for(Direction dir : {Direction::North,Direction::East,Direction::South,Direction::West}) {
        Factory factory;
        assert(factory.place(Kind::Miner,deposit,dir));
        const Cell one=neighbour(deposit,dir);
        const Cell two=neighbour(one,dir);
        const Cell three=neighbour(two,dir);
        assert(factory.place(Kind::Conveyor,one,dir));
        assert(factory.place(Kind::Conveyor,two,dir));
        assert(factory.place(Kind::Smelter,three,dir));
        bool sawOreOnBelt=false;
        bool sawOreInSmelter=false;
        for(int tick=0;tick<1000;++tick) {
            factory.simulate(0.05);
            for(Cell c : {one,two}) {
                const auto* belt=dynamic_cast<const Conveyor*>(factory.at(c));
                if(belt && belt->item()==1) sawOreOnBelt=true;
            }
            const auto* smelter=dynamic_cast<const Smelter*>(factory.at(three));
            if(smelter && (smelter->ore()>0 || smelter->bars()>0)) sawOreInSmelter=true;
        }
        assert(sawOreOnBelt && sawOreInSmelter);
    }
}
// Regression: 2-stage production reaches storage, not just the miner's buffer.
void fullProductionChain() {
    Factory f;
    const Cell start{4,5};
    const Direction east=Direction::East;
    for(int i=0;i<7;++i) {
        Kind kind=i==0?Kind::Miner:i==3?Kind::Smelter:i==6?Kind::Storage:Kind::Conveyor;
        assert(f.place(kind,{start.x+i,start.y},east));
    }
    bool sawOre=false,sawBar=false;
    for(int i=0;i<1400;++i) {
        f.simulate(0.05);
        for(int column : {5,6}) {
            const auto* c=dynamic_cast<const Conveyor*>(f.at({column,5}));
            if(c && c->item()==1) sawOre=true;
        }
        for(int column : {8,9}) {
            const auto* c=dynamic_cast<const Conveyor*>(f.at({column,5}));
            if(c && c->item()==2) sawBar=true;
        }
    }
    assert(sawOre && sawBar && f.producedBars()>0);
}
void blocked() {
    Factory f;f.place(Kind::Miner,{4,5},Direction::East);
    f.place(Kind::Conveyor,{5,5},Direction::West); // opposing belt: cannot extract
    for(int i=0;i<400;++i)f.simulate(0.05);
    auto* miner=dynamic_cast<const Miner*>(f.at({4,5}));
    assert(miner && miner->stored()==6);
}
int main(){placement();orientationAndConnections();cloning();production();directionalProduction();fullProductionChain();blocked();std::cout<<"All ForgeFlow core tests passed\n";}
