import QtQuick 2.0

Rectangle {
    id:page1Container
    width: 200
    height: 300
    x:40;y:40
    color:"pink"
    Loader{
        id:page1
    } //Declaration of a new Loader element
    Text{
        text:"First page loaded"
    }
    Button{
        id:buttonPage1
        anchors.bottom:page1Container.bottom;
        anchors.right: page1Container.right
        MouseArea {
            anchors.fill: parent
            onClicked: {
                page1.source="Page2.qml";
                buttonPage1.z=-1 //Hide button
            }
        }
    }
}
