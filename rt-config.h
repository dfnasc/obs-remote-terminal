#ifndef RTCONFIGDIALOG_H
#define RTCONFIGDIALOG_H

#include <QDialog>

#include "commandserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RTConfigDialog; }
QT_END_NAMESPACE

#include "obscommandhandler.h"

class RTConfigDialog : public QDialog
{
    Q_OBJECT

public:
    RTConfigDialog(QWidget *parent = nullptr, OBSCommandHandler *handler = nullptr);
    ~RTConfigDialog();

private slots:
    void toggleState();
    void copyAuthKey();
    void authKeyRenewed(const QString& key);
    void clientConnected(const QString& remoteAddr, unsigned short remotePort);
    void clientDisconnected(const QString& remoteAddr, unsigned short remotePort);
    void serverUp(const QString& addr, unsigned short port);
    void serverDown();
    void serverError(const QString& error);

private:
    Ui::RTConfigDialog *ui;
    CommandServer *m_cmdServer;

};
#endif // RTCONFIGDIALOG_H
