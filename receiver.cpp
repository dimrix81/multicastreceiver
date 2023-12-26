// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

// #include <QtWidgets>
#include <QtNetwork>

#include "receiver.h"

static const quint16 wCRCTable[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 };

static const quint8 c_version_p = 0x00;
static const quint8 c_data_size = 0x01;
static const quint8 c_data_send = 0x02;
// static const quint8 c_data_ok = 0x02;
static const quint8 c_data_resend = 0x03;
static const quint8 c_end_data = 0x0F;
static const quint8 c_error = 0xFF;

Receiver::Receiver(const uint32_t version_protocol_, const uint32_t size_array_, const uint32_t max_size_packet_, QObject *parent)
    : QObject(parent),
      groupAddress4(QStringLiteral("239.255.43.21")),
      // groupAddress6(QStringLiteral("ff12::2115"))
    version_protocol(version_protocol_),
    size_array(size_array_),
    max_size_packet(max_size_packet_)
{
    // statusLabel = new QLabel(("Version protocol: " + QString::number(version_protocol)) + " size: " + QString::number(size_array));
    // auto quitButton = new QPushButton(tr("&Quit"));

    // auto buttonLayout = new QHBoxLayout;
    // buttonLayout->addStretch(1);
    // buttonLayout->addWidget(quitButton);
    // buttonLayout->addStretch(1);

    // auto mainLayout = new QVBoxLayout;
    // mainLayout->addWidget(statusLabel);
    // mainLayout->addLayout(buttonLayout);
    // setLayout(mainLayout);

    // setWindowTitle(tr("Multicast Receiver"));

    udpSocket4.bind(QHostAddress::AnyIPv4, 45454, QUdpSocket::ShareAddress);
    udpSocket4.joinMulticastGroup(groupAddress4);

    connect(&udpSocket4, &QUdpSocket::readyRead,
            this, &Receiver::processPendingDatagrams);
    // connect(quitButton, &QPushButton::clicked,
    //         qApp, &QCoreApplication::quit);
}

quint16 Crc16(unsigned char *array, size_t len)  // CRC-CCITT
{
    quint16 crc = 0x0000;
    while (len--)
        crc = (crc << 8) ^ wCRCTable[(crc >> 8) ^ *array++];
    return crc;
}

bool checkmeesage(QByteArray *datagram)
{
    qint64 size_packet = datagram->size();
    qDebug()<<"receive:" << size_packet;
    quint16 crc_calc = Crc16((uchar * )(datagram->data()), size_packet - 2);
    quint16 *crc_control = (quint16 * )(datagram->data() + (size_packet - 2));
    qDebug() << "crc_calc: " << crc_calc << "crc_control: " << *crc_control;
    return (crc_calc == *crc_control);
}

void Receiver::processPendingDatagrams()
{
    if(udpSocket4.hasPendingDatagrams())
    {
        QByteArray answer;
        QByteArray datagram;
        quint16 senderPort;
        QHostAddress sender;
        datagram.resize(udpSocket4.pendingDatagramSize());
        udpSocket4.readDatagram(datagram.data(),datagram.size(), &sender, &senderPort);
        const auto lambda_find_client = [&sender, &senderPort]( const client_udp_s& cl){//qDebug()<<"find"<<cl.senderPort<<" ? "<<senderPort;
                    return ((cl.sender == sender) && (cl.senderPort == senderPort)); };
        client_udp_p client_udp = std::find_if( std::begin(clients_udp), std::end(clients_udp), lambda_find_client );// = find_if(clients_udp.begin(), clients_udp.end(), [] (const client_udp_s& cl) { return cl.sender == 0; } );
        qDebug()<<"Receiver HEX: "<<datagram.toHex()<<" size: " << datagram.size();
        if (datagram.size())
        {
            if (checkmeesage(&datagram))
            {
                switch (*(quint8 * )(datagram.data())) {
                case c_version_p:
                    qDebug() << "version: " << *(quint32 * )(datagram.data() + 1);
                    answer = (version_protocol == *(quint32 * )(datagram.data() + 1))
                            ? QByteArray::fromRawData((const char *)&c_version_p, sizeof(c_version_p))
                            : QByteArray::fromRawData((const char *)&c_error, sizeof(c_error));
                    break;
                case c_data_size:
                {
                    qDebug() << "x: " << *(double * )(datagram.data() + 1);
                    answer = QByteArray::fromRawData((const char *)&c_data_size, sizeof(c_data_size));
                    answer += QByteArray::fromRawData((const char *)&size_array, sizeof(size_array));
                    // link = 0;
                    client_udp_s new_client;
                    new_client.senderPort = senderPort;
                    new_client.sender = sender;
                    new_client.datagram_from_server = (double *)malloc(size_array * sizeof(double));

                    double upper_bound = *(double * )(datagram.data() + 1);
                    double lower_bound = -upper_bound;
                    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
                    std::default_random_engine re;
                    for (uint32_t var = 0; var < size_array; ++var) {
                        new_client.datagram_from_server[var] = unif(re);
                    }
                    new_client.link = 0;
                    new_client.last_packet = answer;
                    clients_udp.push_back(new_client);
                }
                    break;
                case c_data_resend:
                    if (client_udp != clients_udp.end())
                    {
                        answer = client_udp->last_packet;
                    }
                    else
                    {
                        qDebug() << "resend client not find!";
                    }
                    break;
                case c_data_send:
                    if (client_udp != clients_udp.end())
                    {
                        if (client_udp->link < size_array)
                        {
                            answer = QByteArray::fromRawData((const char *)&c_data_send, sizeof(c_data_send));
                            // double x=99.9;
                            for (uint32_t var = client_udp->link; var < std::min(client_udp->link + max_size_packet, size_array); ++var) {
                                answer += QByteArray::fromRawData((const char *)&(client_udp->datagram_from_server[var]), sizeof(double));
                            }
                            client_udp->link = std::min(client_udp->link + max_size_packet, size_array);
                        }
                        client_udp->last_packet = answer;
                        qDebug() << "link_send " << client_udp->link;
                    }
                    else
                    {
                        qDebug() << "client not find!";
                    }
                    break;
                default:
                    qDebug() << "Unknow";
                    break;
                }
            }
            else
            {
                qDebug()<<"Error crc";
            }
        }
        else
        {
            answer = QByteArray::fromRawData((const char *)&c_error, sizeof(c_error));
        }
        if (answer.size() > 0)
        {
            quint16 crc = Crc16((uchar * )(answer.data()), answer.length());
            qDebug()<<"crc: "<<crc;
            answer += QByteArray::fromRawData((const char *)&crc, sizeof(crc));

            // qDebug()<<"HEX: "<<answer.toHex()<<" size: " << answer.size();
            sendDatagram(sender, senderPort, answer);
        }
        // }
    }
}

void Receiver::sendDatagram(const QHostAddress &sender, quint16 senderPort, const QByteArray &answer)
{
    // statusLabel->setText(tr("Now sending datagram %1").arg(messageNo));
    if (senderPort)
    {
        qDebug()<<"sendMessage: "<< senderPort;
        // QByteArray datagram = "Multicast message " + QByteArray::number(messageNo);
        udpSocket4.writeDatagram(answer, sender, senderPort);
        // qDebug()<<"sendDatagram: "<<answer.toHex()<<" size: " << answer.size();
        // ++messageNo;
    }
}

