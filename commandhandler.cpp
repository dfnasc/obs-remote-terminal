#include "commandhandler.h"

#include <obs-frontend-api.h>
#include <QDebug>
#include <QMutexLocker>

#include "commandserver.h"

CommandHandler::CommandHandler(QTcpSocket *socket)
 : QThread(), m_mutex() {
    m_socket = socket;
    m_authorized = false;
    m_localAddr = socket->localAddress().toString();
    m_localPort = socket->localPort();
}

CommandHandler::~CommandHandler() {
    QMutexLocker locker(&m_mutex);

    if (m_socket->isOpen())
        m_socket->close();

    delete m_socket;
}

bool CommandHandler::isActive() {
    QMutexLocker locker(&m_mutex);

    return m_socket->isOpen();
}

QString CommandHandler::localAddress() const {
    return m_localAddr;
}

unsigned short CommandHandler::localPort() const {
    return m_localPort;
}

void CommandHandler::run() {

    QString line;
    Command *cmd = nullptr;

    while (isActive()) {

        if (!readLine(line)) {
            msleep(10);
            continue;
        }

        cmd = Command::parse(line);

        if (cmd != nullptr) {
            exec(cmd);
            delete cmd;
        }
    }

    emit clientDisconnected(this);
}

bool CommandHandler::readLine(QString& line) {
    QMutexLocker locker(&m_mutex);

    if (m_socket->canReadLine()) {
        line = m_socket->readLine();
        return true;
    }

    return false;
}

void CommandHandler::stop() {

    QMutexLocker locker(&m_mutex);

    m_socket->close();
}

void CommandHandler::exec(Command *cmd) {

    if (cmd->name() == CMD_AUTH && checkArgs(cmd, 1)) {
        this->auth(cmd->args().at(1));
    } else if (cmd->name() == CMD_MUTE_DESKTOP && checkArgs(cmd, 1)) {
        this->muteDesktop(cmd->args().at(1) == "on");
    } else if (cmd->name() == CMD_MUTE_MICROPHONE && checkArgs(cmd, 1)) {
        this->muteMicrophone(cmd->args().at(1) == "on");
    } else if (cmd->name() == CMD_START_TRANSMISSION) {
        this->startTransmission();
    } else if (cmd->name() == CMD_STOP_TRANSMISSION) {
        this->stopTransmission();
    } else if (cmd->name() == CMD_REC && checkArgs(cmd, 1)) {
        this->record(cmd->args().at(1) == "on");
    } else if (cmd->name() == CMD_LIST_SCENES) {
        this->listScenes();
    } else if (cmd->name() == CMD_SWITCH_SCENE && checkArgs(cmd, 1)) {
        this->switchScene(cmd->args().at(1));
    } else if (cmd->name() == CMD_SLEEP && checkArgs(cmd, 1)) {

        bool ok = false;
        unsigned long delay = 0;

        delay = cmd->args().at(1).toLong(&ok);

        if (ok) {
            this->msleep(delay);
        } else {
            sendError("invalid sleep argument. an signed integer is expected.");
        }
    }
}

void CommandHandler::sendMessage(const QString& code, const QString& message) {

    QString response = code + " " + message + "\n";

    m_socket->write(response.toUtf8().data(), response.length());
    m_socket->flush();
}

bool CommandHandler::authorized() {
    return m_authorized;
}

void CommandHandler::sendError(const QString& errorMessage) {
    sendMessage("ERROR", errorMessage);
}

bool CommandHandler::checkArgs(Command *cmd, int expectedArgs) {

    if (!cmd->validateArgumentNumber(expectedArgs)) {
        sendError("unexpected argument number for \""+ cmd->name() +"\"");
        return false;
    }

    return true;
}

void CommandHandler::auth(const QString& authKey) {

    m_authorized = CommandServer::instance()->authenticate(authKey);

    if (m_authorized) {
        sendMessage("OK", "authenticated");
    } else {
        sendError("invalid authentication key");
    }
}

void CommandHandler::muteDesktop(bool muted) {
    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    if (muted) {
        sendMessage("OK", "muted");
    } else {
        sendMessage("OK", "unmuted");
    }
}

void CommandHandler::muteMicrophone(bool muted) {
    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    if (muted) {
        sendMessage("OK", "muted");
    } else {
        sendMessage("OK", "unmuted");
    }
}

void CommandHandler::startTransmission() {

    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    sendMessage("OK", "transmission started");
}

void CommandHandler::stopTransmission() {

    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    sendMessage("OK", "transmission stopped");
}

void CommandHandler::record(bool on) {

    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    if (on) {
        sendMessage("OK", "recording");
    } else {
        sendMessage("OK", "record finished");
    }
}

void CommandHandler::setupTransmission(const QString& type, const QString& key) {
    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    sendError("not implemented");
}

void CommandHandler::listScenes() {

    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    emit cmdListScenes(this);

}

void CommandHandler::switchScene(const QString& scene) {

    if (!m_authorized) {
        sendError("unauthorized");
        return;
    }

    sendError("not implemented");

}
