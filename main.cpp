// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QCoreApplication>
#include "receiver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Receiver receiver(1, 1000000, 300);
    return app.exec();
}
