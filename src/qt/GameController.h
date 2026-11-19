#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <QTimer>
#include <QVariantList>
#include <QVariantMap>
#include <memory>
#include "../core/Factory.h"

namespace forgeflow {
class BuildingModel final : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { XRole=Qt::UserRole+1, YRole, RotationRole, KindRole, AssetRole, ItemRole, ProgressRole };
    explicit BuildingModel(QObject* parent=nullptr);
    int rowCount(const QModelIndex& parent={}) const override;
    QVariant data(const QModelIndex& index,int role=Qt::DisplayRole) const override;
    QHash<int,QByteArray> roleNames() const override;
    void refresh(const Factory* world);
private:
    const Factory* world_=nullptr;
};

class GameController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(BuildingModel* buildings READ buildings CONSTANT)
    Q_PROPERTY(QVariantList preview READ preview NOTIFY previewChanged)
    Q_PROPERTY(QVariantList deposits READ deposits CONSTANT)
    Q_PROPERTY(int selectedKind READ selectedKind WRITE setSelectedKind NOTIFY selectedKindChanged)
    Q_PROPERTY(int rotation READ rotation NOTIFY rotationChanged)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(int storedBars READ storedBars NOTIFY statsChanged)
    Q_PROPERTY(int ore READ ore NOTIFY statsChanged)
    Q_PROPERTY(int bars READ bars NOTIFY statsChanged)
    Q_PROPERTY(int buildingCount READ buildingCount NOTIFY statsChanged)
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap inspected READ inspected NOTIFY inspectedChanged)
public:
    explicit GameController(QObject* parent=nullptr);
    BuildingModel* buildings() noexcept { return &model_; }
    QVariantList preview() const { return preview_; }
    QVariantList deposits() const;
    int selectedKind() const noexcept { return selectedKind_; }
    void setSelectedKind(int kind);
    int rotation() const noexcept { return rotation_; }
    bool running() const noexcept { return running_; }
    void setRunning(bool state);
    int storedBars() const noexcept { return factory_.producedBars(); }
    int ore() const noexcept { return factory_.totalOre(); }
    int bars() const noexcept { return factory_.totalBars(); }
    int buildingCount() const noexcept { return static_cast<int>(factory_.buildings().size()); }
    QString message() const { return message_; }
    QVariantMap inspected() const { return inspected_; }
    Q_INVOKABLE void rotateSelected();
    Q_INVOKABLE void deactivateTool();
    Q_INVOKABLE void inspectCell(int x, int y);
    Q_INVOKABLE void clearInspection();
    Q_INVOKABLE void updateHover(int x,int y,bool lineMode);
    Q_INVOKABLE void cancelLine();
    Q_INVOKABLE void placePreview();
    Q_INVOKABLE void erase(int x,int y);
    Q_INVOKABLE void save();
    Q_INVOKABLE void load();
    Q_INVOKABLE void clearWorld();
    Q_INVOKABLE void demoFactory();
    Q_INVOKABLE QString helpText() const;
signals:
    void previewChanged();
    void selectedKindChanged();
    void rotationChanged();
    void runningChanged();
    void statsChanged();
    void messageChanged();
    void inspectedChanged();
private:
    // Vereiste 6: object composition, and Qt-facing separation of the C++ world model.
    Factory factory_;
    BuildingModel model_;
    QTimer timer_;
    QVariantList preview_;
    int selectedKind_ = -1;
    int rotation_ = 1;
    bool running_ = false;
    int anchorX_ = -1,anchorY_ = -1;
    int hoverX_ = -1,hoverY_ = -1;
    bool lineMode_ = false;
    QString message_;
    QVariantMap inspected_;
    int inspectedX_ = -1;
    int inspectedY_ = -1;
    void updateInspection();
    void refresh();
    void notify(const QString& text);
    void setPreviewFor(int x,int y,bool line);
    bool validCell(int x,int y) const noexcept;
    static QVariantMap cellVariant(int x,int y,int facing,bool allowed);
};
}
