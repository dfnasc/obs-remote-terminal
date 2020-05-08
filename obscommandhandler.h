#ifndef OBSCOMMANDHANDLER_H
#define OBSCOMMANDHANDLER_H

#include <QObject>
#include "client.h"

class OBSCommandHandler : public QObject
{
    Q_OBJECT
public:
    explicit OBSCommandHandler(QObject *parent = nullptr);

    void listScenes(Client *cli);
    void setScene(Client *cli, size_t index);
    void transition(Client *cli);

    void muteDesktop(Client *cli);
    void unmuteDesktop(Client *cli);

    void muteMicrophone(Client *cli);
    void unmuteMicrophone(Client *cli);

    void startStreaming(Client *cli);
    void stopStreaming(Client *cli);
    void streamingStatus(Client *cli);

    void startRecord(Client *cli);
    void stopRecord(Client *cli);
    void recordingStatus(Client *cli);

    void sendUsage(Client *cli);
signals:
    void responseReady(Client *cli, QString responseCode, QString responseBody);
public slots:
    void handleCommand(Client *cli, QString cmd, QStringList args);
};

#endif // OBSCOMMANDHANDLER_H
