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

    connect(ui->pbStart, &QPushButton::pressed, this, &RTConfigDialog::toggleState);
    connect(ui->pbNewAuthKey, &QPushButton::pressed, m_cmdServer, &CommandServer::renewAuthKey);
    connect(ui->pbCopy, &QPushButton::pressed, this, &RTConfigDialog::copyAuthKey);
    connect(ui->pbClearLog, &QPushButton::pressed, ui->txtLog, &QPlainTextEdit::clear);
    connect(m_cmdServer, &CommandServer::authKeyRenewed, this, &RTConfigDialog::authKeyRenewed);
    connect(m_cmdServer, &CommandServer::up, this, &RTConfigDialog::serverUp);
    connect(m_cmdServer, &CommandServer::down, this, &RTConfigDialog::serverDown);
    connect(m_cmdServer, &CommandServer::error, this, &RTConfigDialog::serverError);

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
        m_cmdServer->start();
    } else {
        m_cmdServer->stop();
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

void RTConfigDialog::serverUp(const QString& addr, unsigned short port) {
   ui->txtLog->appendHtml(QString("listening at <span style=\"font-size: 8pt;\">%1:%2</span>\n").arg(addr).arg(port));
   ui->pbStart->setText("Stop");
   ui->spinBoxPort->setEnabled(false);
   ui->lineEditAddr->setEnabled(false);
}

void RTConfigDialog::serverDown() {
   ui->txtLog->appendHtml("<span style=\"color: orange;\">server is down</span>");
   ui->pbStart->setText("Start");
   ui->spinBoxPort->setEnabled(true);
   ui->lineEditAddr->setEnabled(true);
}

void RTConfigDialog::serverError(const QString& error) {
   ui->txtLog->appendHtml(QString("<strong>ERROR: </strong><span style=\"color: red;\">%1</span>").arg(error));
}
