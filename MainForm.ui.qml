import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    id: item1
    width: 640
    height: 480

    ColumnLayout {
        id: columnLayout1
        anchors.rightMargin: 10
        anchors.leftMargin: 10
        anchors.bottomMargin: 10
        anchors.topMargin: 10
        anchors.fill: parent

        Text {
            id: textChooseTemplate
            y: 10
            text: "Choose a template:"
            anchors.right: parent.right
            anchors.left: parent.left
            font.pixelSize: 12
        }

        Rectangle {
            id: rectangleLeft
            color: "#ffffff"
            anchors.fill: parent
            border.width: 1
            Layout.fillWidth: true

            ListView {
                id: listView1
                x: 0
                y: 0
                width: 110
                height: 160
                model: ListModel {
                    ListElement {
                        name: "Grey"
                        colorCode: "grey"
                    }

                    ListElement {
                        name: "Red"
                        colorCode: "red"
                    }

                    ListElement {
                        name: "Blue"
                        colorCode: "blue"
                    }

                    ListElement {
                        name: "Green"
                        colorCode: "green"
                    }
                }
                delegate: Item {
                    x: 5
                    width: 80
                    height: 40
                    Row {
                        id: row1
                        spacing: 10
                        Rectangle {
                            width: 40
                            height: 40
                            color: colorCode
                        }

                        Text {
                            text: name
                            font.bold: true
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }


        RowLayout {
            id: rowLayoutButtons
            anchors.right: parent.right

            Button {
                id: buttonCancel
                text: "Cancel"
            }

            Button {
                id: buttonChoose
                text: "Choose..."
                isDefault: true
            }
        }

    }

    //    property alias button3: button3
    //    property alias button2: button2
    //    property alias button1: button1
}
