#include "rt-config.h"
#include "./ui_rt-config.h"

#include <QDebug>
#include <QClipboard>

RTConfigDialog::RTConfigDialog(QWidget *parent, OBSCommandHandler *handler)
    : QDialog(parent)
    , ui(new Ui::RTConfigDialog)
{
    ui->setupUi(this);

    ui->lineEditAuthKey->setReadOnly(true);
    this->setWindowTitle("Remote Terminal Configuration");

    m_cmdServer = CommandServer::instance();
    m_cmdServer->setOBSCommandHandler(handler);
    m_cmdServer->setup(ui->lineEditAddr->text(), (unsigned short)ui->spinBoxPort->value());

    connect(ui->pbStart, SIGNAL(pressed()), this, SLOT(toggleState()));
    connect(ui->pbNewAuthKey, SIGNAL(pressed()), m_cmdServer, SLOT(renewAuthKey()));
    connect(ui->pbCopy, SIGNAL(pressed()), this, SLOT(copyAuthKey()));
    connect(ui->pbClearLog, SIGNAL(pressed()), ui->txtLog, SLOT(clear()));
    connect(m_cmdServer, SIGNAL(authKeyRenewed(const QString&)), this, SLOT(authKeyRenewed(const QString&)));
    connect(m_cmdServer, SIGNAL(clientConnected(const QString&, unsigned short)), this, SLOT(clientConnected(const QString&, unsigned short)));
    connect(m_cmdServer, SIGNAL(clientDisconnected(const QString, unsigned short)), this, SLOT(clientDisconnected(const QString, unsigned short)));

    m_cmdServer->renewAuthKey();
    m_cmdServer->start();
}

RTConfigDialog::~RTConfigDialog()
{
    delete ui;
}

void RTConfigDialog::toggleState() {

    bool isRunning = m_cmdServer->isListening();

    ui->lineEditAddr->setEnabled(isRunning);
    ui->spinBoxPort->setEnabled(isRunning);

    if (!isRunning) {
        ui->txtLog->appendHtml("listening on <span style=\"color: blue; font-size: 8pt;\">" + ui->lineEditAddr->text() +
                               QString::number(ui->spinBoxPort->value()) + "</span>");
        m_cmdServer->start();
        ui->pbStart->setText("Stop");
    } else {
        ui->txtLog->appendPlainText("stopping server...");
        m_cmdServer->stop();
        ui->txtLog->appendPlainText("server stopped");
        ui->pbStart->setText("Start");
    }
}

void RTConfigDialog::copyAuthKey() {
    QClipboard *cb = QApplication::clipboard();

    ui->lineEditAuthKey->setSelection(0, ui->lineEditAuthKey->text().length());

    cb->setText(m_cmdServer->authKey());
}

void RTConfigDialog::authKeyRenewed(const QString& key) {
    
   qDebug() << "\n\n";
   qDebug() << "[remote-terminal] ************************************************************";
   qDebug() << "[remote-terminal] Auth Key: " << key;
   qDebug() << "[remote-terminal] ************************************************************";
   qDebug() << "\n\n";

    ui->lineEditAuthKey->setText(key);
    ui->txtLog->appendHtml("authentication key renewed: <span style=\"font-size: 9pt; font-weight: bold;\">" + key + "</span>");
}

void RTConfigDialog::clientConnected(const QString &remoteAddr, unsigned short remotePort) {
    ui->txtLog->appendHtml("connected: <span style=\"font-size: 8pt; color: green;\">" + remoteAddr + ":" + QString::number(remotePort) + "</span>" );
}

void RTConfigDialog::clientDisconnected(const QString &remoteAddr, unsigned short remotePort) {
    ui->txtLog->appendHtml("disconnected: <span style=\"font-size: 8pt; color: green;\">" + remoteAddr + ":" + QString::number(remotePort) + "</span>" );
}
