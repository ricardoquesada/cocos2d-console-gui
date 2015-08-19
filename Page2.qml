import QtQuick 2.0

Rectangle {
    id:page2Container
    width: 200
    height: 200
    x:80;y:80
    color:"yellow"
    Loader {
        id:page2
    }
    Text {
        text:"Second page loaded"
    }
    Button {
        id:buttonPage2
        anchors.bottom:page2Container.bottom;
        anchors.right: page2Container.right
        MouseArea {
            anchors.fill: parent
            onClicked: {
                page2.source="Page3.qml";
                buttonPage2.z=-1
            }
        }
    }
}

