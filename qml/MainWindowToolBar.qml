﻿import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SAK.Custom
import "common"

ToolBar {
    id: root
    width: toolColumnLayout.width
    property int pageIndex: 0
    property int fixedpage: 0

    background: Rectangle {
        radius: isWindowed ? 8 : 0
        color: Material.primary
        Rectangle {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: parent.color
            anchors.right: parent.right
            width: parent.radius
        }
    }

    signal invokeAddPage(var qmlFile)
    signal invokeRemovePage(var pageIndex)

    ButtonGroup {
        id: bg
    }

    ColumnLayout {
        id: toolColumnLayout
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        Item { Layout.minimumHeight: 24 }
        Repeater {
            id: btRepeater
            model: [
                ["qrc:/resources/icon/IconSerialPort.svg",      SAKToolsFactory.SerialportTool,      qsTr("SerialPort"),       "MainWindowSerialPortPage.qml"],
                ["qrc:/resources/icon/IconBlueTooth.svg",       SAKToolsFactory.SerialportTool,      qsTr("SerialPort"),       "MainWindowBlePage.qml"],
                ["qrc:/resources/icon/IconUdpClient.svg",       SAKToolsFactory.UdpClientTool,       qsTr("UDP Client"),       "MainWindowUdpClientPage.qml"],
                ["qrc:/resources/icon/IconUdpServer.svg",       SAKToolsFactory.UdpServerTool,       qsTr("UDP Server"),       "MainWindowUdpServerPage.qml"],
                ["qrc:/resources/icon/IconTcpClient.svg",       SAKToolsFactory.TcpClientTool,       qsTr("TCP Client"),       "MainWindowTcpClientPage.qml"],
                ["qrc:/resources/icon/IconTcpServer.svg",       SAKToolsFactory.TcpServerTool,       qsTr("TCP Server"),       "MainWindowTcpServerPage.qml"],
                ["qrc:/resources/icon/IconWebScoketClient.svg", SAKToolsFactory.WebSocketClientTool, qsTr("WebSocket Client"), "MainWindowWebSocketClientPage.qml"],
                ["qrc:/resources/icon/IconWebSocketServer.svg", SAKToolsFactory.WebSocketServerTool, qsTr("WebSocket Server"), "MainWindowWebSocketServerPage.qml"]
            ]
            SAKToolButton {
                id: bt
                checkable: true
                icon.source: modelData[0]
                onClicked: root.pageIndex = index
                checked: pageIndex === index
                onDoubleClicked: invokeRemovePage(index)
                tips: modelData[2]
                Component.onCompleted: {
                    bg.addButton(bt)
                    if (index === pageIndex) {
                        checked = true
                    }
                }
            }
        }
        ToolButton {
            icon.source: "qrc:/resources/icon/IconAdd.svg"
            onClicked: optionMenu.open()
            visible: false
            Menu {
                id: optionMenu
                Repeater {
                    model:  [
                        ["qrc:/resources/icon/IconSerialPort.svg",      SAKToolsFactory.SerialportTool,      qsTr("SerialPort"),       "MainWindowSerialPortPage.qml"],
                        ["qrc:/resources/icon/IconUdpClient.svg",       SAKToolsFactory.UdpClientTool,       qsTr("UDP Client"),       "MainWindowUdpClientPage.qml"],
                        ["qrc:/resources/icon/IconUdpServer.svg",       SAKToolsFactory.UdpServerTool,       qsTr("UDP Server"),       "MainWindowUdpServerPage.qml"],
                        ["qrc:/resources/icon/IconTcpClient.svg",       SAKToolsFactory.TcpClientTool,       qsTr("TCP Client"),       "MainWindowTcpClientPage.qml"],
                        ["qrc:/resources/icon/IconTcpServer.svg",       SAKToolsFactory.TcpServerTool,       qsTr("TCP Server"),       "MainWindowTcpServerPage.qml"],
                        ["qrc:/resources/icon/IconWebScoketClient.svg", SAKToolsFactory.WebSocketClientTool, qsTr("WebSocket Client"), "MainWindowWebSocketClientPage.qml"],
                        ["qrc:/resources/icon/IconWebSocketServer.svg", SAKToolsFactory.WebSocketServerTool, qsTr("WebSocket Server"), "MainWindowWebSocketServerPage.qml"]
                    ]
                    MenuItem {
                        text: modelData[2]
                        icon.source: modelData[0]
                        onTriggered: {
                            var temp = []
                            temp = btRepeater.model
                            temp.push(modelData)
                            btRepeater.model = temp
                            root.invokeAddPage(modelData[3])
                        }
                    }
                }
            }
        }
        Item { Layout.fillHeight: true }
        ToolButton {
            id: infoToolButton
            checkable: true
            icon.source: "qrc:/resources/icon/IconInfo.svg"
            onClicked: pageIndex = btRepeater.model.length
            Component.onCompleted:  bg.addButton(infoToolButton)
        }
        ToolButton {
            id: settingsToolButton
            checkable: true
            icon.source: "qrc:/resources/icon/IconSettings.svg"
            onClicked: pageIndex = btRepeater.model.length + 1
            Component.onCompleted: bg.addButton(settingsToolButton)
        }
        Item { Layout.minimumHeight: 24 }
    }

    Component.onCompleted: fixedpage = btRepeater.model.length

    function removePageIndex(index) {
        if (index >= fixedpage && index < btRepeater.model.length) {
            var newModel = btRepeater.model
            newModel.splice(index, 1)
            btRepeater.model = newModel
            pageIndex = pageIndex - 1
            console.info("Page index removed:", index)
        }
    }
}
