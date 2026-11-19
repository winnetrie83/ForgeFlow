#include "GameController.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUrl>
#include <algorithm>
#include <stdexcept>

namespace forgeflow {
BuildingModel::BuildingModel(QObject* p):QAbstractListModel(p) {}
int BuildingModel::rowCount(const QModelIndex& p) const {return p.isValid()||!world_?0:static_cast<int>(world_->buildings().size());}
QHash<int,QByteArray> BuildingModel::roleNames() const {return {{XRole,"gridX"},{YRole,"gridY"},{RotationRole,"facing"},{KindRole,"buildingKind"},{AssetRole,"assetFile"},{ItemRole,"item"},{ProgressRole,"progress"}};}
QVariant BuildingModel::data(const QModelIndex& index,int role) const {
    if(!world_||!index.isValid()||index.row()<0||index.row()>=rowCount())return {};
    const Building& b=*world_->buildings()[static_cast<size_t>(index.row())];
    switch(role) {
    case XRole:return b.cell().x; case YRole:return b.cell().y;
    case RotationRole:return static_cast<int>(b.facing())*90;
    case KindRole:return static_cast<int>(b.kind());
    case AssetRole:
        switch(b.kind()) {case Kind::Miner:return "scanner-low"; case Kind::Smelter:return "machine-fortified";
                          case Kind::Storage:return "box-large";case Kind::Conveyor:return "conveyor-stripe";}
        break;
    case ItemRole: if(const auto* c=dynamic_cast<const Conveyor*>(&b))return c->item();return 0;
    case ProgressRole:if(const auto* c=dynamic_cast<const Conveyor*>(&b))return c->progress();return 0.0;
    }
    return {};
}
void BuildingModel::refresh(const Factory* world){beginResetModel();world_=world;endResetModel();}

// Vereiste 51: useful Qt class (self made)
// The controller owns UI-facing Q_PROPERTIES, a QAbstractListModel and a QTimer.
GameController::GameController(QObject* parent):QObject(parent),model_(this) {
    model_.refresh(&factory_);
    timer_.setInterval(50);
    // Vereiste 52: useful usage of signals/slots
    // The timeout signal advances the simulation; stats and model change signals refresh the QML view.
    connect(&timer_,&QTimer::timeout,this,[this] {
        factory_.simulate(0.05);refresh();
    });
    notify("Ready. Choose a building and click on the grid. Press H for help.");
}
QVariantList GameController::deposits() const {QVariantList out;for(const Deposit& d:factory_.deposits())out<<QVariantMap{{"x",d.cell.x},{"y",d.cell.y}};return out;}
void GameController::setSelectedKind(int n) {if(n<-1||n>3||selectedKind_==n)return;selectedKind_=n;cancelLine();emit selectedKindChanged();if(n>=0)clearInspection();setPreviewFor(hoverX_,hoverY_,false);}
void GameController::setRunning(bool state){if(running_==state)return;running_=state;if(state)timer_.start();else timer_.stop();emit runningChanged();notify(state?"Simulation running":"Simulation paused");}
void GameController::deactivateTool(){setSelectedKind(-1);}
void GameController::clearInspection(){inspectedX_=inspectedY_=-1;updateInspection();}
void GameController::inspectCell(int x,int y){deactivateTool();inspectedX_=x;inspectedY_=y;updateInspection();}
void GameController::updateInspection(){
    QVariantMap next;
    if(const Building* b=factory_.at({inspectedX_,inspectedY_})) {
        next={{"name",QString::fromStdString(kindName(b->kind()))},{"x",inspectedX_},{"y",inspectedY_},
              {"facing",static_cast<int>(b->facing())*90},{"kind",static_cast<int>(b->kind())}};
        if(const auto* m=dynamic_cast<const Miner*>(b)) {next["content"]=QString("Iron ore: %1").arg(m->stored());next["detail"]=QString("Mining progress: %1%").arg(static_cast<int>(m->progress()*100));}
        if(const auto* m=dynamic_cast<const Smelter*>(b)) {next["content"]=QString("Input: %1 iron ore").arg(m->ore());next["detail"]=QString("Output: %1 iron bars").arg(m->bars());}
        if(const auto* m=dynamic_cast<const Storage*>(b)) {next["content"]=QString("Iron bars: %1").arg(m->bars());next["detail"]="Stored inventory (current model: iron bars)";}
        if(const auto* m=dynamic_cast<const Conveyor*>(b)) {const char* item=m->item()==1?"Iron ore":m->item()==2?"Iron bar":"Empty";next["content"]=QString("Contents: %1").arg(QString::fromLatin1(item));next["detail"]=QString("Travel: %1%").arg(static_cast<int>(m->progress()*100));}
    }
    if(next!=inspected_){inspected_=next;emit inspectedChanged();}
}
void GameController::rotateSelected(){if(selectedKind_<0)return;rotation_=(rotation_+1)%4;emit rotationChanged();setPreviewFor(hoverX_,hoverY_,lineMode_);}
bool GameController::validCell(int x,int y) const noexcept{return x>=0&&y>=0&&x<MapSize&&y<MapSize;}
QVariantMap GameController::cellVariant(int x,int y,int facing,bool allowed){return {{"x",x},{"y",y},{"rotation",facing*90},{"allowed",allowed}};}
void GameController::setPreviewFor(int x,int y,bool line) {
    preview_.clear();
    if(selectedKind_<0||!validCell(x,y)) {emit previewChanged();return;}
    if(line&&selectedKind_==3) {
        if(anchorX_<0) {anchorX_=x;anchorY_=y;}
        const int deltaX=x-anchorX_,deltaY=y-anchorY_;
        const bool horizontal=std::abs(deltaX)>=std::abs(deltaY);
        const int count=horizontal?std::abs(deltaX):std::abs(deltaY);
        const int step=horizontal?(deltaX>=0?1:-1):(deltaY>=0?1:-1);
        const int dir=horizontal?(step>0?1:3):(step>0?2:0);
        for(int i=0;i<=count;++i){int px=anchorX_+(horizontal?i*step:0),py=anchorY_+(horizontal?0:i*step);
            if(validCell(px,py))preview_<<cellVariant(px,py,dir,factory_.canPlace(Kind::Conveyor,{px,py}));
        }
    }else preview_<<cellVariant(x,y,rotation_,factory_.canPlace(static_cast<Kind>(selectedKind_),{x,y}));
    emit previewChanged();
}
void GameController::updateHover(int x,int y,bool line) {
    hoverX_=x;hoverY_=y;
    const bool effectiveLine=line&&selectedKind_==3;
    if(!effectiveLine)anchorX_=anchorY_=-1;
    lineMode_=effectiveLine;setPreviewFor(x,y,effectiveLine);
}
void GameController::cancelLine(){anchorX_=anchorY_=-1;lineMode_=false;setPreviewFor(hoverX_,hoverY_,false);}
void GameController::placePreview() {
    if(selectedKind_<0||preview_.isEmpty())return;
    // Multi-placement is atomic: reject the entire line if any cell is invalid.
    for(const QVariant& v:preview_)if(!v.toMap().value("allowed").toBool()) {notify("Cannot place: at least one cell is blocked");return;}
    int placed=0;
    for(const QVariant& v:preview_) {const auto p=v.toMap();
        if(factory_.place(static_cast<Kind>(selectedKind_),{p.value("x").toInt(),p.value("y").toInt()},
                          static_cast<Direction>((p.value("rotation").toInt()/90)%4)))++placed;
    }
    notify(QString("Placed %1 %2").arg(placed).arg(QString::fromStdString(kindName(static_cast<Kind>(selectedKind_)))));
    anchorX_=anchorY_=-1;refresh();deactivateTool();
}
void GameController::erase(int x,int y){if(factory_.erase({x,y})){notify("Building removed");refresh();setPreviewFor(x,y,false);}}
void GameController::refresh(){model_.refresh(&factory_);emit statsChanged();updateInspection();}
void GameController::notify(const QString& s){message_=s;emit messageChanged();}
static QString savePath(){const QString dir=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);QDir().mkpath(dir);return dir+"/factory.json";}
// Vereiste 47: useful usage of (modern) file-I/O
// QSaveFile writes to a temporary file and commits atomically, avoiding truncated saves.
void GameController::save() {
    QJsonArray records;
    for(const auto& ptr:factory_.buildings()) {
        const Building& b=*ptr;
        QJsonObject obj{{"kind",static_cast<int>(b.kind())},{"x",b.cell().x},{"y",b.cell().y},{"facing",static_cast<int>(b.facing())}};
        if(const auto* m=dynamic_cast<const Miner*>(&b)){obj["stored"]=m->stored();obj["progress"]=m->progress();}
        if(const auto* s=dynamic_cast<const Smelter*>(&b)){obj["ore"]=s->ore();obj["bars"]=s->bars();obj["progress"]=s->progress();}
        if(const auto* s=dynamic_cast<const Storage*>(&b))obj["bars"]=s->bars();
        if(const auto* c=dynamic_cast<const Conveyor*>(&b)){obj["item"]=c->item();obj["progress"]=c->progress();}
        records.append(obj);
    }
    QSaveFile f(savePath());
    // Vereiste 48: useful exception handling
    // File-write failures are exposed to the player instead of terminating the app.
    if(!f.open(QIODevice::WriteOnly)) {notify("Save failed: "+f.errorString());return;}
    if(f.write(QJsonDocument(QJsonObject{{"version",1},{"buildings",records}}).toJson())<0||!f.commit()){
        notify("Save failed: "+f.errorString());return;
    }
    notify("Factory saved to "+savePath());
}
void GameController::load() {
    QFile f(savePath());if(!f.open(QIODevice::ReadOnly)){notify("No save file yet");return;}
    QJsonParseError error;const QJsonDocument document=QJsonDocument::fromJson(f.readAll(),&error);
    if(error.error!=QJsonParseError::NoError||!document.isObject()) {notify("Invalid save JSON");return;}
    const QJsonObject root=document.object();if(root.value("version").toInt()!=1||!root.value("buildings").isArray()) {notify("Unsupported save version");return;}
    Factory candidate;
    // Load into a separate world first: invalid saves cannot damage the current factory.
    try {
        for(const QJsonValue& value:root.value("buildings").toArray()) {
            if(!value.isObject())throw std::runtime_error("Non-object building");
            const QJsonObject o=value.toObject();
            for(const char* key:{"kind","x","y","facing"})if(!o.value(key).isDouble())throw std::runtime_error("Missing building field");
            const int id=o.value("kind").toInt(-1),d=o.value("facing").toInt(-1);
            if(id<0||id>3||d<0||d>3)throw std::runtime_error("Invalid building type/rotation");
            const Cell c{o.value("x").toInt(-1),o.value("y").toInt(-1)};
            if(!candidate.place(static_cast<Kind>(id),c,static_cast<Direction>(d)))throw std::runtime_error("Overlapping or invalid building");
            Building* b=candidate.at(c);
            if(auto* m=dynamic_cast<Miner*>(b))m->restore(o.value("stored").toInt(),o.value("progress").toDouble());
            if(auto* s=dynamic_cast<Smelter*>(b))s->restore(o.value("ore").toInt(),o.value("bars").toInt(),o.value("progress").toDouble());
            if(auto* s=dynamic_cast<Storage*>(b))s->restore(o.value("bars").toInt());
            if(auto* belt=dynamic_cast<Conveyor*>(b))belt->restore(o.value("item").toInt(),o.value("progress").toDouble());
        }
    }catch(const std::exception& ex){notify("Load rejected: "+QString::fromUtf8(ex.what()));return;}
    setRunning(false);clearInspection();deactivateTool();
    factory_=std::move(candidate);refresh();cancelLine();notify("Factory loaded");
}
void GameController::clearWorld(){setRunning(false);clearInspection();deactivateTool();factory_.clear();cancelLine();refresh();notify("Empty factory created");}
void GameController::demoFactory() {
    clearWorld();
    // Deposit at (4,5), producer faces east, then two belts and smelter, two belts and storage.
    factory_.place(Kind::Miner,{4,5},Direction::East);
    factory_.place(Kind::Conveyor,{5,5},Direction::East);
    factory_.place(Kind::Conveyor,{6,5},Direction::East);
    factory_.place(Kind::Smelter,{7,5},Direction::East);
    factory_.place(Kind::Conveyor,{8,5},Direction::East);
    factory_.place(Kind::Conveyor,{9,5},Direction::East);
    factory_.place(Kind::Storage,{10,5},Direction::East);
    refresh();notify("Example: Miner → belts → Smelter → belts → Storage. Press Play.");
}
QString GameController::helpText() const {return "Select tool and left click: place | Left click idle: inspect | Right click / Escape: cancel tool | R: rotate | Shift + move: conveyor line preview | Middle drag: pan | Right drag: orbit | Wheel: zoom | Delete: remove in erase mode | H: help";}
} // namespace forgeflow
