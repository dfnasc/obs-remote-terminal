#ifndef REMOTECONTROLDIALOG_H
#define REMOTECONTROLDIALOG_H

#include <QDialog>

#include "commandserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RemoteControlDialog; }
QT_END_NAMESPACE

#include "obscommandhandler.h"

class RemoteControlDialog : public QDialog
{
    Q_OBJECT

public:
    RemoteControlDialog(QWidget *parent = nullptr, OBSCommandHandler *handler = nullptr);
    ~RemoteControlDialog();

private slots:
    void toggleState();
    void copyAuthKey();
    void authKeyRenewed(const QString& key);
    void clientConnected(const QString& remoteAddr, unsigned short remotePort);
    void clientDisconnected(const QString& remoteAddr, unsigned short remotePort);

private:
    Ui::RemoteControlDialog *ui;
    CommandServer *m_cmdServer;

};
#endif // REMOTECONTROLDIALOG_H
