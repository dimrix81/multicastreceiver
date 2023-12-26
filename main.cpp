// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

// #include <QApplication>
#include <QCoreApplication>
#include "receiver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Receiver receiver(1, 1000000, 300);
    // receiver.show();
    // receiver.sendDatagram();
    return app.exec();
}

// #include "receiver.h"
// #include<QDebug>
// #include <QtCore/QCoreApplication>

// int main(int argc, char *argv[])
// {
//     QCoreApplication a(argc, argv);
//     qDebug()<<"123";
//     UDPtest test;
//     test.sendMessage("aha");
//     return a.exec();
// }
