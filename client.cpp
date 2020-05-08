#include "client.h"

#include <QDebug>
#include <QMutexLocker>

#include "commandserver.h"

Client::Client(QTcpSocket *socket)
 : QObject(), m_mutex() {
    m_socket = socket;
    m_authorized = false;
    m_localAddr = socket->localAddress().toString();
    m_localPort = socket->localPort();

    connect(m_socket, SIGNAL(readyRead()), this, SLOT(dataAvailable()));
}

Client::~Client() {
    QMutexLocker locker(&m_mutex);

    if (m_socket->isOpen())
        m_socket->close();

    delete m_socket;
}

bool Client::isActive() {
    QMutexLocker locker(&m_mutex);

    return m_socket->isOpen();
}

QString Client::localAddress() const {
    return m_localAddr;
}

unsigned short Client::localPort() const {
    return m_localPort;
}

void Client::dataAvailable() {

    QString line;
    Command *cmd = nullptr;

    if (!readLine(line)) {
        return;
    }

    cmd = Command::parse(line);

    if (cmd != nullptr) {

        if (cmd->name() == CMD_AUTH) {
            if (cmd->args().length() == 2) {
                this->auth(cmd->args().at(1));
            } else {
                sendError("the auth key must be informed.");
            }

            delete cmd;
        } else if (cmd->name() == CMD_QUIT) {
            m_socket->disconnectFromHost();
            emit disconnected(localAddress(), localPort());
            delete cmd;
        } else if (authorized()) {
            emit commandReceived(this, cmd->name(), cmd->args());
        } else {
            sendError("unauthorized. use \"auth <key>\" command to authenticate.");
        }

    } // cmd != nullptr
}

bool Client::readLine(QString& line) {
    QMutexLocker locker(&m_mutex);

    if (m_socket->canReadLine()) {
        line = m_socket->readLine();
        return true;
    }

    return false;
}

void Client::sendMessage(const QString& code, const QString& message) {

    QString response = code + " " + message + "\n";

    m_socket->write(response.toUtf8().data(), response.length());
    m_socket->flush();
}

bool Client::authorized() {
    return m_authorized;
}

void Client::sendError(const QString& errorMessage) {
    sendMessage("ERROR", errorMessage);
}

bool Client::checkArgs(Command *cmd, int expectedArgs) {

    if (!cmd->validateArgumentNumber(expectedArgs)) {
        sendError("unexpected argument number for \""+ cmd->name() +"\"");
        return false;
    }

    return true;
}

void Client::auth(const QString& authKey) {

    m_authorized = CommandServer::instance()->authenticate(authKey);

    if (m_authorized) {
        sendMessage("OK", "authenticated");
    } else {
        sendError("invalid authentication key");
    }
}
