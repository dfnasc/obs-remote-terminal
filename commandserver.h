#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QMap>

#include "client.h"
#include "obscommandhandler.h"

class CommandServer : public QTcpServer
{
    Q_OBJECT
private:
    CommandServer();

public:
    static CommandServer* instance();

    bool authenticate(const QString& authKey);
    QString authKey() const;

    void setOBSCommandHandler(OBSCommandHandler *handler);
signals:
    void authKeyRenewed(const QString& authKey);
    void clientConnected(const QString remoteAddr, unsigned short remotePort);
    void clientDisconnected(const QString remoteAddr, unsigned short remotePort);
public slots:
    void start();
    void stop();
    void setup(const QString& address, unsigned short port);
    void setBindAddress(const QString& address);
    void setBindPort(unsigned short port);
    void renewAuthKey();

    void handleResponse(Client *cli, QString responseCode, QString responeBody);

private slots:
    void handleConnection();

private:
    static CommandServer *m_instance;
    QString m_addr;
    unsigned short m_port;
    QString m_authKey;
    QList<Client*> m_clients;
    OBSCommandHandler *m_handler;
};

#endif // COMMANDSERVER_H
