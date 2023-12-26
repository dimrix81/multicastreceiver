// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef RECEIVER_H
#define RECEIVER_H

#include <QDialog>
#include <QHostAddress>
#include <QUdpSocket>
#include <QVector>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

struct client_udp_s
{
    quint16 senderPort;
    QHostAddress sender;
    double *datagram_from_server;
    uint32_t link;
    QByteArray last_packet;

    client_udp_s(): senderPort(0), datagram_from_server(nullptr), link(0) {}
};

typedef QVector<client_udp_s> client_udp_t;
typedef QVector<client_udp_s>::iterator client_udp_p;

class Receiver : public QDialog
{
    Q_OBJECT

public:
    explicit Receiver(const uint32_t version_protocol, const uint32_t size_array, const uint32_t max_size_packet, QWidget *parent = nullptr);
    void sendDatagram(const QHostAddress &sender, quint16 senderPort, const QByteArray &answer);

private slots:
    void processPendingDatagrams();

    // void sendMessage(QString message);

private:
    QLabel *statusLabel = nullptr;
    QUdpSocket udpSocket4;
    QHostAddress groupAddress4;
    const uint32_t version_protocol;
    const uint32_t size_array;
    const uint32_t max_size_packet;
    client_udp_t clients_udp;
};

#endif

