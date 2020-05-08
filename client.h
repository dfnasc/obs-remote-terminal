#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>

#include "command.h"

#define CMD_AUTH "auth"
#define CMD_QUIT "quit"

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QTcpSocket *socket);
    ~Client();

    bool isActive();
    QString localAddress() const;
    unsigned short localPort() const;

signals:
    void commandReceived(Client *cli, QString cmd, QStringList args);
    void disconnected(QString addr, unsigned short port);
public slots:
    void sendMessage(const QString& code, const QString& message);
    void sendError(const QString& errorMessage);

private slots:
    void auth(const QString& authKey);
    void dataAvailable();
private:
    bool checkArgs(Command *cmd, int expectedArgs);
    bool authorized();
    bool readLine(QString& line);
private:
    bool m_authorized;
    QTcpSocket *m_socket;
    QString m_localAddr;
    unsigned short m_localPort;
    QMutex m_mutex;
};

#endif // COMMANDHANDLER_H
