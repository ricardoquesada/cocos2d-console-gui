import QtQuick 2.0

Rectangle {
    width: 90
    height: 20
    color: "#cbd9f3"
    radius:10
    z:0
    Text {
        width:parent.width;height:parent.height
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text:"Click me"
        font.pointSize: 8
    }
 }
