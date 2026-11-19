import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.AssetUtils

ApplicationWindow {
    id: root
    width: 1280; height: 760; minimumWidth: 880; minimumHeight: 530
    visible: true
    title: "FORGEFLOW  /  Factory Sandbox  •  v0.5.1"
    color: "#081521"
    property real panX: 0
    property real panZ: 0
    property real yaw: -42
    property real tilt: -55
    property real zoom: 1700
    property int hoverX: -1
    property int hoverY: -1
    property bool shiftDown: false
    property real lastMouseX: 0
    property real lastMouseY: 0
    property bool didDrag: false
    property bool showHelp: false
    property bool eraseMode: false
    property bool showGrid: true
    property bool ignoreRightRelease: false
    property bool showConnections: false

    function cellAt(px,py) {
        let hit=scene.pick(px,py)
        if (!hit.objectHit || hit.objectHit !== ground) return null
        const x=Math.floor(hit.scenePosition.x/100 + 12)
        const y=Math.floor(hit.scenePosition.z/100 + 12)
        if(x<0 || y<0 || x>=24 || y>=24) return null
        return { x:x,y:y }
    }
    function setHover(mx,my) {
        const cell=cellAt(mx,my)
        if(cell) { hoverX=cell.x;hoverY=cell.y;game.updateHover(cell.x,cell.y,shiftDown) }
        else {hoverX=-1;hoverY=-1;game.updateHover(-1,-1,shiftDown)}
    }
    function changeTool(value){game.selectedKind=(game.selectedKind===value?-1:value);eraseMode=false}
    function toggleDemolition(){
        eraseMode=!eraseMode
        game.deactivateTool()
        game.clearInspection()
        root.setHover(root.lastMouseX,root.lastMouseY)
    }
    // C++ directions: North=-Z, East=+X, South=+Z, West=-X.
    // World yaw for a logical direction with North=-Z and East=+X.
    function directionYaw(facing){return 180-facing}
    // Kenney's conveyor-stripe GLB has its visible travel axis along local X,
    // whereas ForgeFlow's logical forward axis is local +Z. Apply this same
    // quarter-turn to the REAL instance and its GHOST; never rotate markers.
    function meshCorrection(kind){return kind===3?90:-90}
    function meshYaw(facing,kind){return directionYaw(facing)+meshCorrection(kind)}
    function directionName(facing){return ["NORTH (-Z)","EAST (+X)","SOUTH (+Z)","WEST (-X)"][Math.round(facing/90)%4]}

    Shortcut { sequence:"R"; onActivated:game.rotateSelected() }
    Shortcut { sequence:"Space"; onActivated:game.running=!game.running }
    Shortcut { sequence:"Escape"; onActivated: { shiftDown=false;game.deactivateTool();game.clearInspection();eraseMode=false;showHelp=false } }
    Shortcut { sequence:"F3"; onActivated:root.showConnections=!root.showConnections }
    Shortcut { sequence:"H"; onActivated:showHelp=!showHelp }
    Shortcut { sequence:"Delete"; onActivated: {if(hoverX>=0 && eraseMode)game.erase(hoverX,hoverY)} }
    Shortcut { sequence:"X"; onActivated: root.toggleDemolition() }
    Shortcut { sequence:"Ctrl+S"; onActivated: game.save() }
    Shortcut { sequence:"Ctrl+O"; onActivated: game.load() }
    Shortcut { sequence:"1"; onActivated:changeTool(0) }
    Shortcut { sequence:"2"; onActivated:changeTool(1) }
    Shortcut { sequence:"3"; onActivated:changeTool(2) }
    Shortcut { sequence:"4"; onActivated:changeTool(3) }

    Rectangle {
        anchors.fill:parent
        color:"#07111a"
    }
    RowLayout {
        anchors.fill:parent
        spacing:0
        Rectangle {
            id: sidebar
            Layout.preferredWidth:280;Layout.fillHeight:true
            color:"#0d2333"
            border.color:"#1a4c60"
            ColumnLayout {
                anchors.fill:parent
                anchors.margins:0
                spacing:0
                ScrollView {
                    id: sidebarScroll
                    Layout.fillWidth:true
                    Layout.fillHeight:true
                    clip:true
                    contentWidth:availableWidth
                    ScrollBar.vertical.policy:ScrollBar.AsNeeded
                    ColumnLayout {
                        width:Math.max(0,sidebarScroll.availableWidth-32)
                        x:16
                        spacing:11
                        Item { Layout.preferredHeight:5 }
                Image { source:"qrc:/assets/ui/button_square_header_large_rectangle.png";fillMode:Image.Stretch;Layout.fillWidth:true;Layout.preferredHeight:72
                    Text {anchors.centerIn:parent;text:"FORGEFLOW";font.pixelSize:25;font.bold:true;font.letterSpacing:2;color:"#e9fbff"}
                }
                Text {text:"FACTORY SANDBOX  /  ALPHA 0.5";color:"#4bbed7";font.pixelSize:11;font.letterSpacing:1.5;Layout.alignment:Qt.AlignHCenter}
                Rectangle {Layout.fillWidth:true;height:1;color:"#28536a"}
                Text {text:"BUILD STRUCTURES";font.pixelSize:12;font.bold:true;color:"#bfeaf3";font.letterSpacing:1.7}
                Text {text:"Click repeatedly to build · Right click cancels";color:"#7faabb";font.pixelSize:10;wrapMode:Text.WordWrap;Layout.fillWidth:true}
                Repeater {
                    model:[{label:"01   MINER",hint:"Place on an ore deposit",kind:0},{label:"02   SMELTER",hint:"Ore → iron bar (3s)",kind:1},{label:"03   STORAGE",hint:"Store produced iron bars",kind:2},{label:"04   CONVEYOR",hint:"One cell / Shift for a line",kind:3}]
                    delegate:ColumnLayout {
                        required property var modelData
                        Layout.fillWidth:true;spacing:3
                        Button {
                            Layout.fillWidth:true;Layout.preferredHeight:49
                            text:modelData.label
                            highlighted:game.selectedKind===modelData.kind&&!root.eraseMode
                            onClicked:root.changeTool(modelData.kind)
                            font.pixelSize:15;font.bold:true
                            contentItem:Text { text:parent.text;color:parent.highlighted?"#ffffff":"#bfdfea";font:parent.font;verticalAlignment:Text.AlignVCenter;leftPadding:20 }
                            background:Image {source:"qrc:/assets/ui/button_square_header_small_rectangle.png";fillMode:Image.Stretch;opacity:parent.highlighted?1:0.48 }
                        }
                        Text {text:modelData.hint;color:"#7faabb";font.pixelSize:11;Layout.leftMargin:7}
                    }
                }
                Rectangle {Layout.fillWidth:true;height:1;color:"#28536a";Layout.topMargin:8}
                Button {
                    id:demolitionButton
                    Layout.fillWidth:true
                    Layout.preferredHeight:46
                    text:root.eraseMode?"DEMOLITION  /  ACTIVE":"DEMOLITION TOOL  [X]"
                    highlighted:root.eraseMode
                    onClicked:root.toggleDemolition()
                    background:Rectangle {radius:7;color:root.eraseMode?"#a83e40":"#47313b";border.color:root.eraseMode?"#ffb5ad":"#b96a71";border.width:1}
                    contentItem:Text {text:parent.text;color:"#fff1ef";horizontalAlignment:Text.AlignHCenter;verticalAlignment:Text.AlignVCenter;font.bold:true;font.pixelSize:13}
                    ToolTip.visible:hovered;ToolTip.text:"Click a structure to demolish it. Right click or X to exit."
                }
                Text {text:"Demolish: left click building · right click cancels";font.pixelSize:10;color:"#c9a0a0";Layout.fillWidth:true;wrapMode:Text.WordWrap}
                Text {text:"PRODUCTION STATUS";font.bold:true;font.pixelSize:12;font.letterSpacing:1.5;color:"#bfeaf3"}
                Text {text:"Items should appear as AMBER ore or CYAN iron bars on belts when playing";color:"#83c8d3";font.pixelSize:10;wrapMode:Text.WordWrap;Layout.fillWidth:true}
                RowLayout {Layout.fillWidth:true
                    Text {text:"Machines";color:"#7faabb";Layout.fillWidth:true}
                    Text {text:String(game.buildingCount);color:"white";font.bold:true}
                }
                RowLayout {Layout.fillWidth:true
                    Text {text:"Ore in system";color:"#7faabb";Layout.fillWidth:true}
                    Text {text:String(game.ore);color:"white";font.bold:true}
                }
                RowLayout {Layout.fillWidth:true
                    Text {text:"Iron bars";color:"#7faabb";Layout.fillWidth:true}
                    Text {text:String(game.bars);color:"#73e8d4";font.bold:true}
                }
                RowLayout {Layout.fillWidth:true
                    Text {text:"In storage";color:"#7faabb";Layout.fillWidth:true}
                    Text {text:String(game.storedBars);color:"#73e8d4";font.bold:true}
                }
                Rectangle {Layout.fillWidth:true;height:1;color:"#28536a";Layout.topMargin:8}
                    Item { Layout.preferredHeight:6 }
                    }
                }
                Rectangle {
                    id:sidebarFooter
                    Layout.fillWidth:true
                    Layout.preferredHeight:184
                    color:"#102c3d"
                    border.color:"#315b6c"
                    ColumnLayout {
                        anchors.fill:parent
                        anchors.margins:10
                        spacing:5
                Text {text:"WORLD CONTROL";font.pixelSize:12;font.bold:true;font.letterSpacing:1.5;color:"#bfeaf3"}
                RowLayout { Layout.fillWidth:true;spacing:6
                    Button {Layout.fillWidth:true;text:game.running?"⏸ Pause":"▶ Play";onClicked:game.running=!game.running}
                    Button {Layout.fillWidth:true;text:"Demo";onClicked:game.demoFactory()}
                }
                RowLayout {Layout.fillWidth:true;spacing:6
                    Button {Layout.fillWidth:true;text:"Save";onClicked:game.save()}
                    Button {Layout.fillWidth:true;text:"Load";onClicked:game.load()}
                    Button {Layout.fillWidth:true;text:"Clear";onClicked:confirmClear.open()}
                }
                CheckBox {text:"Show grid";checked:root.showGrid;onToggled:root.showGrid=checked;palette.windowText:"#c8e8f0"}
                CheckBox {text:"Direction debug [F3]";checked:root.showConnections;onToggled:root.showConnections=checked;palette.windowText:"#c8e8f0"}
                Text {text:"C++17 / Qt Quick 3D";color:"#608697";font.pixelSize:11}                    }
                }
            }
        }
        Item {
            id: workspace
            Layout.fillWidth:true;Layout.fillHeight:true
            clip:true
            View3D {
                id:scene
                anchors.fill:parent
                environment: SceneEnvironment { backgroundMode:SceneEnvironment.Color;clearColor:"#0a1e2d";antialiasingMode:SceneEnvironment.MSAA;antialiasingQuality:SceneEnvironment.Medium }
                Node {
                    id:cameraPivot
                    position:Qt.vector3d(root.panX,0,root.panZ)
                    eulerRotation.y:root.yaw
                    Node {
                        eulerRotation.x:root.tilt
                        PerspectiveCamera { id:camera;position:Qt.vector3d(0,0,root.zoom);clipFar:7000;clipNear:10 }
                    }
                }
                camera:camera
                DirectionalLight {eulerRotation:Qt.vector3d(-55,-35,0);brightness:1.7;ambientColor:"#a2b4c8"}
                DirectionalLight {eulerRotation:Qt.vector3d(-25,105,0);brightness:0.5;color:"#59a6d3"}
                Model {
                    id:ground
                    source:"#Cube"
                    position:Qt.vector3d(0,-7,0)
                    scale:Qt.vector3d(24,0.12,24)
                    pickable:true
                    materials:PrincipledMaterial{baseColor:"#213c45";roughness:0.95}
                }
                Node {
                    visible:root.showGrid
                    Repeater3D {
                        model:25
                        delegate:Model {
                            source:"#Cube"
                            position:Qt.vector3d((index-12)*100,0,0)
                            scale:Qt.vector3d(0.007,0.007,24)
                            materials:DefaultMaterial {diffuseColor:index===12?"#346f89":"#335461"}
                        }
                    }
                    Repeater3D {
                        model:25
                        delegate:Model {
                            source:"#Cube"
                            position:Qt.vector3d(0,0,(index-12)*100)
                            scale:Qt.vector3d(24,0.007,0.007)
                            materials:DefaultMaterial {diffuseColor:index===12?"#346f89":"#335461"}
                        }
                    }
                }
                Repeater3D {
                    model:game.deposits
                    delegate:Node {
                        required property var modelData
                        position:Qt.vector3d((modelData.x-11.5)*100,2,(modelData.y-11.5)*100)
                        RuntimeLoader { source:"qrc:/assets/factory/cog-a.glb";scale:Qt.vector3d(73,73,73);eulerRotation.x:0 }
                        Model {source:"#Cylinder";position:Qt.vector3d(0,4,0);scale:Qt.vector3d(0.72,0.04,0.72);materials:DefaultMaterial {diffuseColor:"#d8b857";opacity:0.7}}
                    }
                }
                Repeater3D {
                    model:game.buildings
                    delegate:Node {
                        required property int gridX
                        required property int gridY
                        required property int facing
                        required property int buildingKind
                        required property string assetFile
                        required property int item
                        required property real progress
                        position:Qt.vector3d((gridX-11.5)*100,3,(gridY-11.5)*100)
                        // One coordinate convention for real models, ghost models and
                        // direction overlays. No nested/compensating yaw rotations.
                        RuntimeLoader {
                            source:"qrc:/assets/factory/"+assetFile+".glb"
                            eulerRotation.y:root.meshYaw(facing,buildingKind)
                            // The crane is a taller, longer decorative model. Scale
                            // it to one logical grid cell and centre its asymmetric base.
                            position:Qt.vector3d(buildingKind===0?30:0,0,0)
                            scale:Qt.vector3d(buildingKind===0?26:buildingKind===3?95:105,
                                              buildingKind===0?26:buildingKind===3?95:105,
                                              buildingKind===0?26:buildingKind===3?95:105)
                        }
                        // The Kenney meshes have their own orientation. The following
                        // indicator uses the SIMULATION direction rather than mesh orientation.
                        // Green = input, orange = output, aqua chevron = travel direction.
                        Node {
                            visible:root.showConnections
                            eulerRotation.y:root.directionYaw(facing)
                            // NOTE: facing is a degree value (0,90,180,270).
                            Model {
                                visible:buildingKind===3
                                source:"#Cube"
                                position:Qt.vector3d(0,57,-4)
                                scale:Qt.vector3d(0.075,0.025,0.47)
                                materials:DefaultMaterial{diffuseColor:"#17d9e8";lighting:DefaultMaterial.NoLighting}
                            }
                            Model {
                                visible:buildingKind===3
                                source:"#Cube"
                                position:Qt.vector3d(-12,57,19)
                                eulerRotation.y:45
                                scale:Qt.vector3d(0.075,0.025,0.27)
                                materials:DefaultMaterial{diffuseColor:"#17d9e8";lighting:DefaultMaterial.NoLighting}
                            }
                            Model {
                                visible:buildingKind===3
                                source:"#Cube"
                                position:Qt.vector3d(12,57,19)
                                eulerRotation.y:-45
                                scale:Qt.vector3d(0.075,0.025,0.27)
                                materials:DefaultMaterial{diffuseColor:"#17d9e8";lighting:DefaultMaterial.NoLighting}
                            }
                            Model {
                                visible:buildingKind!==0
                                source:"#Cube"
                                position:Qt.vector3d(0,54,-43)
                                scale:Qt.vector3d(0.29,0.095,0.12)
                                materials:DefaultMaterial{diffuseColor:"#2aea8e";lighting:DefaultMaterial.NoLighting}
                            }
                            Model {
                                visible:buildingKind!==2
                                source:"#Cube"
                                position:Qt.vector3d(0,54,43)
                                scale:Qt.vector3d(0.29,0.095,0.12)
                                materials:DefaultMaterial{diffuseColor:"#ff9a3b";lighting:DefaultMaterial.NoLighting}
                            }
                        }
                        Node {
                            // Conveyed item travels along the same logical axis
                            // as the arrows, independent of the Kenney mesh axes.
                            eulerRotation.y:root.directionYaw(facing)
                            Model {
                                visible:buildingKind===3&&item!==0
                                source:"#Cube"
                                // Center of belt top ~40 units above ground at scale 95.
                                // The box is intentionally large and unlit for visibility.
                                position:Qt.vector3d(0,52,(progress-0.5)*72)
                                scale:Qt.vector3d(0.31,0.25,0.31)
                                materials:DefaultMaterial {diffuseColor:item===1?"#ffbd35":"#55f4dd";lighting:DefaultMaterial.NoLighting}
                            }
                        }
                    }
                }
                Repeater3D {
                    model:game.preview
                    delegate:Node {
                        required property var modelData
                        position:Qt.vector3d((modelData.x-11.5)*100,1,(modelData.y-11.5)*100)
                        Model {
                            source:"#Cube"
                            position:Qt.vector3d(0,2,0)
                            scale:Qt.vector3d(0.96,0.045,0.96)
                            materials:PrincipledMaterial {baseColor:modelData.allowed?"#32ebaf":"#ff545c";opacity:0.48;alphaMode:PrincipledMaterial.Blend}
                        }
                        // Ghost connectors are ALWAYS visible, even when F3 debug
                        // visualization is switched off for placed buildings.
                        Node {
                            eulerRotation.y:root.directionYaw(modelData.rotation)
                            // Cyan arrow: forward direction (local +Z).
                            Model {
                                visible:game.selectedKind===3
                                source:"#Cube";position:Qt.vector3d(0,57,-4)
                                scale:Qt.vector3d(0.075,0.025,0.47)
                                materials:DefaultMaterial { diffuseColor:modelData.allowed?"#00f5e2":"#ff6060";lighting:DefaultMaterial.NoLighting }
                            }
                            Model {
                                visible:game.selectedKind===3
                                source:"#Cube";position:Qt.vector3d(-12,57,19);eulerRotation.y:45
                                scale:Qt.vector3d(0.075,0.025,0.27)
                                materials:DefaultMaterial { diffuseColor:modelData.allowed?"#00f5e2":"#ff6060";lighting:DefaultMaterial.NoLighting }
                            }
                            Model {
                                visible:game.selectedKind===3
                                source:"#Cube";position:Qt.vector3d(12,57,19);eulerRotation.y:-45
                                scale:Qt.vector3d(0.075,0.025,0.27)
                                materials:DefaultMaterial { diffuseColor:modelData.allowed?"#00f5e2":"#ff6060";lighting:DefaultMaterial.NoLighting }
                            }
                            // Input at -Z for all but Miner, output at +Z for all
                            // but Storage. Conveyor has both connectors.
                            Model {
                                visible:game.selectedKind!==0
                                source:"#Cube";position:Qt.vector3d(0,54,-43)
                                scale:Qt.vector3d(0.29,0.095,0.12)
                                materials:DefaultMaterial {diffuseColor:"#2aea8e";lighting:DefaultMaterial.NoLighting}
                            }
                            Model {
                                visible:game.selectedKind!==2
                                source:"#Cube";position:Qt.vector3d(0,54,43)
                                scale:Qt.vector3d(0.29,0.095,0.12)
                                materials:DefaultMaterial {diffuseColor:"#ff9a3b";lighting:DefaultMaterial.NoLighting}
                            }
                        }
                        RuntimeLoader {
                            source:"qrc:/assets/factory/"+(game.selectedKind===0?"crane":game.selectedKind===1?"machine-fortified":game.selectedKind===2?"box-large":"conveyor-stripe")+".glb"
                            scale:Qt.vector3d(game.selectedKind===0?26:game.selectedKind===3?95:105,
                                              game.selectedKind===0?26:game.selectedKind===3?95:105,
                                              game.selectedKind===0?26:game.selectedKind===3?95:105)
                            eulerRotation.y:root.meshYaw(modelData.rotation,game.selectedKind)
                            position:Qt.vector3d(game.selectedKind===0?30*Math.cos(modelData.rotation*Math.PI/180):0,0,
                                                 game.selectedKind===0?30*Math.sin(modelData.rotation*Math.PI/180):0)
                            opacity:0.4
                        }
                    }
                }
                // Red demolition target, rendered above the ground on occupied cells.
                Node {
                    visible:root.eraseMode && root.hoverX>=0 && game.hoverOccupied
                    position:Qt.vector3d((root.hoverX-11.5)*100,9,(root.hoverY-11.5)*100)
                    Model {
                        source:"#Cube"
                        scale:Qt.vector3d(0.97,0.10,0.97)
                        materials:PrincipledMaterial{baseColor:"#ff313c";opacity:0.55;alphaMode:PrincipledMaterial.Blend}
                    }
                }
            }
            MouseArea {
                id:mouse
                anchors.fill:parent
                hoverEnabled:true
                acceptedButtons:Qt.LeftButton|Qt.RightButton|Qt.MiddleButton
                focus:true
                onEntered:forceActiveFocus()
                onPositionChanged: function(m) {
                    if(pressedButtons & Qt.MiddleButton) {
                        const factor=root.zoom/1400
                        const dx=(m.x-root.lastMouseX)*factor*1.1
                        const dz=(m.y-root.lastMouseY)*factor*1.1
                        const theta=root.yaw*Math.PI/180
                        root.panX-=dx*Math.cos(theta)+dz*Math.sin(theta)
                        root.panZ+=dx*Math.sin(theta)-dz*Math.cos(theta)
                        root.didDrag=true
                    } else if(pressedButtons & Qt.RightButton) {
                        root.yaw+=(m.x-root.lastMouseX)*0.3
                        root.tilt=Math.max(-85,Math.min(-25,root.tilt-(m.y-root.lastMouseY)*0.2))
                        root.didDrag=true
                    }
                    root.lastMouseX=m.x;root.lastMouseY=m.y
                    root.setHover(m.x,m.y)
                }
                onPressed:function(m){root.lastMouseX=m.x;root.lastMouseY=m.y;root.didDrag=false;root.setHover(m.x,m.y)}
                onReleased:function(m){
                    if(!root.didDrag) {
                        if(m.button===Qt.LeftButton) {
                            if(root.eraseMode) {if(root.hoverX>=0)game.erase(root.hoverX,root.hoverY)}
                            else if(game.selectedKind>=0) game.placePreview()
                            else if(root.hoverX>=0)game.inspectCell(root.hoverX,root.hoverY)
                            else game.clearInspection()
                        } else if(m.button===Qt.RightButton) {
                            game.deactivateTool();root.eraseMode=false;game.clearInspection()
                        }
                    }
                }
                onWheel:function(w){root.zoom=Math.max(450,Math.min(3400,root.zoom-w.angleDelta.y*1.0));w.accepted=true}
                Keys.onPressed:function(e){if(e.key===Qt.Key_Shift && !e.isAutoRepeat){root.shiftDown=true;root.setHover(root.lastMouseX,root.lastMouseY);e.accepted=true}}
                Keys.onReleased:function(e){if(e.key===Qt.Key_Shift && !e.isAutoRepeat){root.shiftDown=false;game.cancelLine();e.accepted=true}}
            }
            Rectangle {
                anchors.left:parent.left;anchors.top:parent.top;anchors.margins:18
                width:Math.min(405,Math.max(200,parent.width-42));height:73;color:"#d90b2336";radius:8;border.color:"#39617a"
                Column {anchors.centerIn:parent;spacing:5
                    Text {text:"SECTOR 01 / INDUSTRIAL GRID";color:"#b7f0fa";font.pixelSize:16;font.bold:true;font.letterSpacing:1}
                    Text {text:"24 × 24 cells   •   4 iron deposits   •   free building";color:"#77afbf";font.pixelSize:12}
                }
            }
            Rectangle {
                anchors.right:parent.right;anchors.top:parent.top;anchors.margins:18
                visible:workspace.width>=760
                width:235;height:87;radius:8;color:"#d90b2336";border.color:"#39617a"
                Column {anchors.centerIn:parent;spacing:7
                    Text {text:"CAMERA  /  TACTICAL VIEW";color:"#b7f0fa";font.bold:true;font.pixelSize:12}
                    Text {text:"Middle drag: pan  •  Wheel: zoom";color:"#8badbc";font.pixelSize:11}
                    Text {text:"Right drag: orbit  •  R: rotate";color:"#8badbc";font.pixelSize:11}
                }
            }
            Rectangle {
                anchors.left:parent.left;anchors.right:parent.right;anchors.bottom:parent.bottom
                height:57;color:"#e80c2537";border.color:"#29475a"
                RowLayout {anchors.fill:parent;anchors.leftMargin:18;anchors.rightMargin:18
                    Text {text:"GRID  "+(root.hoverX<0?"--":(root.hoverX+", "+root.hoverY));color:"#70d9e2";font.family:"monospace";font.bold:true;Layout.preferredWidth:134}
                    Text {text:game.message;color:"#e4f6fb";elide:Text.ElideRight;Layout.fillWidth:true;font.pixelSize:12}
                    Text {text:root.eraseMode?"DEMOLITION MODE · click structure to delete":"GHOST: I/O visible  ·  F3: placed directions";color:root.eraseMode?"#ff9d91":"#8cbdcb";font.pixelSize:11}
                    Button {text:root.eraseMode?"STOP DEMOLITION":"DEMOLISH [X]";onClicked:root.toggleDemolition()}
                    Button {text:"? HELP";onClicked:root.showHelp=!root.showHelp}
                }
            }
            Rectangle {
                id:inspector
                visible:Object.keys(game.inspected).length>0
                anchors.right:parent.right
                anchors.bottom:parent.bottom
                anchors.rightMargin:18
                anchors.bottomMargin:72
                width:Math.min(300,Math.max(180,parent.width-30))
                height:184
                radius:10
                color:"#ef102b3e"
                border.color:"#3c9eb7"
                border.width:1
                ColumnLayout {
                    anchors.fill:parent;anchors.margins:16;spacing:10
                    RowLayout {
                        Layout.fillWidth:true
                        Text {text:"STRUCTURE INSPECTOR";color:"#7ee0ec";font.bold:true;font.pixelSize:12;Layout.fillWidth:true}
                        Button {text:"×";implicitWidth:30;implicitHeight:28;onClicked:game.clearInspection()}
                    }
                    Text {text:game.inspected.name||"";font.pixelSize:19;font.bold:true;color:"#e5faff"}
                    Text {text:"Grid: "+game.inspected.x+", "+game.inspected.y+"    Facing: "+root.directionName(game.inspected.facing);color:"#94bac9";font.pixelSize:12}
                    Rectangle {Layout.fillWidth:true;height:1;color:"#386276"}
                    Text {text:game.inspected.content||"";color:"#7bedd0";font.pixelSize:15;font.bold:true}
                    Text {text:game.inspected.detail||"";color:"#acced8";font.pixelSize:11;wrapMode:Text.WordWrap;Layout.fillWidth:true}
                    Item {Layout.fillHeight:true}
                }
            }
            Rectangle {
                visible:root.showHelp
                anchors.centerIn:parent;width:Math.min(parent.width-80,620);height:355
                radius:12;color:"#f20b2536";border.color:"#43a9c1";border.width:2
                Column {anchors.fill:parent;anchors.margins:24;spacing:14
                    Text {text:"BUILDING CONTROLS";color:"#dffaff";font.pixelSize:22;font.bold:true}
                    Text {width:parent.width;wrapMode:Text.WordWrap;color:"#c1e3ed";font.pixelSize:14;lineHeight:1.55
                        text:"LEFT CLICK  – Place repeatedly / inspect when tool inactive\nRIGHT CLICK / ESC  – Leave build, inspection or demolition\nSHIFT + MOVE  – Preview a straight conveyor line (cyan is output direction)\nSHIFT + LEFT CLICK  – Place full line, tool stays selected\nR  – Rotate 90 degrees when building\nF3  – Show/hide I/O and direction markers on placed structures\nDEMOLITION [X]  – Left click a structure to delete it\nMIDDLE DRAG  – Pan across the map\nRIGHT DRAG  – Orbit camera\nWHEEL  – Zoom\nSPACE  – Start / stop simulation\nCTRL+S / CTRL+O  – Save / load\n1–4  – Select building tool"
                    }
                    Button {text:"CLOSE  [H]";onClicked:root.showHelp=false}
                }
            }
        }
    }
    Dialog {
        id:confirmClear;title:"Clear the factory?";modal:true;anchors.centerIn:parent;standardButtons:Dialog.Yes|Dialog.Cancel
        onAccepted:game.clearWorld()
        Label {text:"This removes all buildings. Saved factories remain on disk."}
    }
}
