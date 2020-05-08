#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QMutex>

#include "command.h"

#define CMD_AUTH "auth"
#define CMD_MUTE_DESKTOP "desktop_mute"
#define CMD_MUTE_MICROPHONE "mic_mute"
#define CMD_START_TRANSMISSION "start_transmission"
#define CMD_STOP_TRANSMISSION "stop_transmission"
#define CMD_REC "rec"
#define CMD_SETUP_TRANSMISSION "setup_transmission"
#define CMD_LIST_SCENES "scenes"
#define CMD_SWITCH_SCENE "switch_scene"
#define CMD_SLEEP "sleep"

class CommandHandler : public QThread
{
    Q_OBJECT

public:
    CommandHandler(QTcpSocket *socket);
    ~CommandHandler();

    bool isActive();
    QString localAddress() const;
    unsigned short localPort() const;

signals:
    void clientDisconnected(CommandHandler *handler);

    void cmdListScenes(CommandHandler *handler);

public slots:
    void run() override;
    void stop();
    void exec(Command *cmd);
    void sendMessage(const QString& code, const QString& message);
    void sendError(const QString& errorMessage);

private slots:
    void auth(const QString& authKey);
    void muteDesktop(bool muted);
    void muteMicrophone(bool muted);
    void startTransmission();
    void stopTransmission();
    void record(bool value);
    void setupTransmission(const QString& type, const QString& key);
    void listScenes();
    void switchScene(const QString& scene);

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
