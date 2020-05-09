#include "commandserver.h"

#include <QUuid>

CommandServer *CommandServer::m_instance = nullptr;

CommandServer::CommandServer()
 : QTcpServer(), m_clients() {
    this->setup("0.0.0.0", 3001);

    connect(this, SIGNAL(newConnection()), this, SLOT(handleConnection()));
}

CommandServer* CommandServer::instance() {
    if (m_instance == nullptr) {
        m_instance = new CommandServer();
    }

    return m_instance;
}

void CommandServer::setup(const QString& bindAddress, int port) {
    setBindAddress(bindAddress);
    setBindPort(port);
}

void CommandServer::setBindAddress(const QString& bindAddress) {
    m_addr = bindAddress;
}

void CommandServer::setBindPort(int port) {
    m_port = (unsigned short)port;
}

void CommandServer::start() {
    if (this->listen(QHostAddress(m_addr), m_port)) {
       emit up(m_addr, m_port);
    } else {
       emit error(errorString());
    }
}

void CommandServer::stop() {
    this->close();

    if (!this->isListening()) {
       emit down();
    }
}

void CommandServer::handleConnection() {

    QTcpSocket *socket = this->nextPendingConnection();

    if (socket != nullptr) {
        Client *client = new Client(socket);

        if (m_handler != nullptr) {
            connect(client, SIGNAL(commandReceived(Client*,QString,QStringList)), m_handler, SLOT(handleCommand(Client*,QString,QStringList)));
        }

        m_clients.append(client);
    }
}

void CommandServer::renewAuthKey() {
    m_authKey = QUuid::createUuid().toString(QUuid::WithoutBraces);

    emit authKeyRenewed(m_authKey);
}

void CommandServer::handleResponse(Client *cli, QString responseCode, QString responseBody) {
    cli->sendMessage(responseCode, responseBody);
}

bool CommandServer::authenticate(const QString& authKey) {
    return authKey == m_authKey;
}

QString CommandServer::authKey() const {
    return m_authKey;
}

void CommandServer::setOBSCommandHandler(OBSCommandHandler *handler) {

    m_handler = handler;
    connect(m_handler, &OBSCommandHandler::responseReady, this, &CommandServer::handleResponse);
}
