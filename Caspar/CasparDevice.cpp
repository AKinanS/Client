#include "CasparDevice.h"

#include <QtCore/QStringList>

CasparDevice::CasparDevice(QObject* parent) : AMCPDevice(parent)
{
}

CasparDevice::~CasparDevice()
{
}

void CasparDevice::connect(const QString& name, int port)
{
    AMCPDevice::connectDevice(name, port);
}

void CasparDevice::disconnect()
{
    AMCPDevice::disconnectDevice();
}

int CasparDevice::getPort() const
{
    return AMCPDevice::socket->peerPort();
}

QString CasparDevice::getName() const
{
    return AMCPDevice::socket->peerName();
}

void CasparDevice::refreshData()
{
    AMCPDevice::writeMessage("DATA LIST");
}

void CasparDevice::refreshVersion()
{
    AMCPDevice::writeMessage("VERSION");
}

void CasparDevice::refreshMedia()
{
    AMCPDevice::writeMessage("CLS");
}

void CasparDevice::refreshTemplate()
{
    AMCPDevice::writeMessage("TLS");
}

void CasparDevice::addTemplate(int channel, int flashlayer, const QString& name, int playOnLoad, const QString& data)
{
    AMCPDevice::writeMessage(QString("CG %1 ADD %2 %3 %4 %5")
                             .arg(channel)
                             .arg(flashlayer)
                             .arg(name)
                             .arg(playOnLoad)
                             .arg(data));
}

void CasparDevice::addTemplate(int channel, int layer, int flashlayer, const QString& name, int playOnLoad, const QString& data)
{
    AMCPDevice::writeMessage(QString("CG %1-%2 ADD %3 %4 %5 %6")
                             .arg(channel)
                             .arg(layer)
                             .arg(flashlayer)
                             .arg(name)
                             .arg(playOnLoad)
                             .arg(data));
}

void CasparDevice::playTemplate(int channel, int flashlayer)
{
    AMCPDevice::writeMessage(QString("CG %1 PLAY %2")
                             .arg(channel)
                             .arg(flashlayer));
}

void CasparDevice::playTemplate(int channel, int flashlayer, const QString& item)
{
    AMCPDevice::writeMessage(QString("CG %1 ADD %2 %3 1")
                             .arg(channel)
                             .arg(flashlayer)
                             .arg(item));
}

void CasparDevice::playTemplate(int channel, int layer, int flashlayer)
{
    AMCPDevice::writeMessage(QString("CG %1-%2 PLAY %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(flashlayer));
}

void CasparDevice::playTemplate(int channel, int layer, int flashlayer, const QString& item)
{
    AMCPDevice::writeMessage(QString("CG %1-%2 ADD %3 %4 1")
                             .arg(channel)
                             .arg(layer)
                             .arg(flashlayer)
                             .arg(item));
}

void CasparDevice::stopTemplate(int channel, int flashlayer)
{
    AMCPDevice::writeMessage(QString("CG %1 STOP %2")
                             .arg(channel)
                             .arg(flashlayer));
}

void CasparDevice::stopTemplate(int channel, int layer, int flashlayer)
{
    AMCPDevice::writeMessage(QString("CG %1-%2 STOP %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(flashlayer));
}

void CasparDevice::clearVideo(int channel)
{
    AMCPDevice::writeMessage(QString("CLEAR %1")
                             .arg(channel));
}

void CasparDevice::clearVideo(int channel, int layer)
{
    AMCPDevice::writeMessage(QString("CLEAR %1-%2")
                             .arg(channel)
                             .arg(layer));
}

void CasparDevice::playVideo(int channel, const QString& item)
{
    AMCPDevice::writeMessage(QString("PLAY %1 %2")
                             .arg(channel)
                             .arg(item));
}

void CasparDevice::playVideo(int channel, int layer, const QString& item)
{
    AMCPDevice::writeMessage(QString("PLAY %1-%2 %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(item));
}

void CasparDevice::stopVideo(int channel)
{
    AMCPDevice::writeMessage(QString("STOP %1")
                             .arg(channel));
}

void CasparDevice::stopVideo(int channel, int layer)
{
    AMCPDevice::writeMessage(QString("STOP %1-%2")
                             .arg(channel)
                             .arg(layer));
}

void CasparDevice::setVolume(int channel, float volume)
{
    AMCPDevice::writeMessage(QString("MIXER %1 VOLUME %2")
                             .arg(channel)
                             .arg(volume));
}

void CasparDevice::setVolume(int channel, float volume, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 VOLUME %2 %3 %4")
                             .arg(channel)
                             .arg(volume)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setVolume(int channel, int layer, float volume)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 VOLUME %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(volume));
}

void CasparDevice::setVolume(int channel, int layer, float volume, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 VOLUME %3 %4 %5")
                             .arg(channel)
                             .arg(layer)
                             .arg(volume)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setOpacity(int channel, float opacity)
{
    AMCPDevice::writeMessage(QString("MIXER %1 OPACITY %2")
                             .arg(channel)
                             .arg(opacity));
}

void CasparDevice::setOpacity(int channel, float opacity, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 OPACITY %2 %3 %4")
                             .arg(channel)
                             .arg(opacity)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setOpacity(int channel, int layer, float opacity)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 OPACITY %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(opacity));
}

void CasparDevice::setOpacity(int channel, int layer, float opacity, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 OPACITY %3 %4 %5")
                             .arg(channel)
                             .arg(layer)
                             .arg(opacity)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setBrightness(int channel, float brightness)
{
    AMCPDevice::writeMessage(QString("MIXER %1 BRIGHTNESS %2")
                             .arg(channel)
                             .arg(brightness));
}

void CasparDevice::setBrightness(int channel, float brightness, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 BRIGHTNESS %2 %3 %4")
                             .arg(channel)
                             .arg(brightness)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setBrightness(int channel, int layer, float brightness)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 BRIGHTNESS %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(brightness));
}

void CasparDevice::setBrightness(int channel, int layer, float brightness, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 BRIGHTNESS %3 %4 %5")
                             .arg(channel)
                             .arg(layer)
                             .arg(brightness)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setContrast(int channel, float contrast)
{
    AMCPDevice::writeMessage(QString("MIXER %1 CONTRAST %2")
                             .arg(channel)
                             .arg(contrast));
}

void CasparDevice::setContrast(int channel, float contrast, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 CONTRAST %2 %3 %4")
                             .arg(channel)
                             .arg(contrast)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setContrast(int channel, int layer, float contrast)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 CONTRAST %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(contrast));
}

void CasparDevice::setContrast(int channel, int layer, float contrast, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 CONTRAST %3 %4 %5")
                             .arg(channel)
                             .arg(layer)
                             .arg(contrast)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setSaturation(int channel, float saturation)
{
    AMCPDevice::writeMessage(QString("MIXER %1 SATURATION %2")
                             .arg(channel)
                             .arg(saturation));
}

void CasparDevice::setSaturation(int channel, float saturation, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 SATURATION %2 %3 %4")
                             .arg(channel)
                             .arg(saturation)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setSaturation(int channel, int layer, float saturation)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 SATURATION %3")
                             .arg(channel)
                             .arg(layer)
                             .arg(saturation));
}

void CasparDevice::setSaturation(int channel, int layer, float saturation, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 SATURATION %3 %4 %5")
                             .arg(channel)
                             .arg(layer)
                             .arg(saturation)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setLevels(int channel, float minIn, float maxIn, float gamma, float minOut, float maxOut)
{
    AMCPDevice::writeMessage(QString("MIXER %1 LEVELS %2 %3 %4 %5 %6")
                             .arg(channel)
                             .arg(minIn)
                             .arg(maxIn)
                             .arg(gamma)
                             .arg(minOut)
                             .arg(maxOut));
}

void CasparDevice::setLevels(int channel, float minIn, float maxIn, float gamma, float minOut, float maxOut, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 LEVELS %2 %3 %4 %5 %6 %7 %8")
                             .arg(channel)
                             .arg(minIn)
                             .arg(maxIn)
                             .arg(gamma)
                             .arg(minOut)
                             .arg(maxOut)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setLevels(int channel, int layer, float minIn, float maxIn, float gamma, float minOut, float maxOut)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 LEVELS %3 %4 %5 %6 %7")
                             .arg(channel)
                             .arg(layer)
                             .arg(minIn)
                             .arg(maxIn)
                             .arg(gamma)
                             .arg(minOut)
                             .arg(maxOut));
}

void CasparDevice::setLevels(int channel, int layer, float minIn, float maxIn, float gamma, float minOut, float maxOut, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 LEVELS %3 %4 %5 %6 %7 %8 %9")
                             .arg(channel)
                             .arg(layer)
                             .arg(minIn)
                             .arg(maxIn)
                             .arg(gamma)
                             .arg(minOut)
                             .arg(maxOut)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setGeometry(int channel, float positionX, float positionY, float scaleX, float scaleY)
{
    AMCPDevice::writeMessage(QString("MIXER %1 FILL %2 %3 %4 %5")
                             .arg(channel)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY));
}

void CasparDevice::setGeometry(int channel, float positionX, float positionY, float scaleX, float scaleY, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 FILL %2 %3 %4 %5 %6 %7")
                             .arg(channel)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setGeometry(int channel, int layer, float positionX, float positionY, float scaleX, float scaleY)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 FILL %3 %4 %5 %6")
                             .arg(channel)
                             .arg(layer)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY));
}

void CasparDevice::setGeometry(int channel, int layer, float positionX, float positionY, float scaleX, float scaleY, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 FILL %3 %4 %5 %6 %7 %8")
                             .arg(channel)
                             .arg(layer)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setClipping(int channel, float positionX, float positionY, float scaleX, float scaleY)
{
    AMCPDevice::writeMessage(QString("MIXER %1 CLIP %2 %3 %4 %5")
                             .arg(channel)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY));
}

void CasparDevice::setClipping(int channel, float positionX, float positionY, float scaleX, float scaleY, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1 CLIP %2 %3 %4 %5 %6 %7")
                             .arg(channel)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::setClipping(int channel, int layer, float positionX, float positionY, float scaleX, float scaleY)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 CLIP %3 %4 %5 %6")
                             .arg(channel)
                             .arg(layer)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY));
}

void CasparDevice::setClipping(int channel, int layer, float positionX, float positionY, float scaleX, float scaleY, int duration, QString easing)
{
    AMCPDevice::writeMessage(QString("MIXER %1-%2 CLIP %3 %4 %5 %6 %7 %8")
                             .arg(channel)
                             .arg(layer)
                             .arg(positionX)
                             .arg(positionY)
                             .arg(scaleX)
                             .arg(scaleY)
                             .arg(duration)
                             .arg(easing));
}

void CasparDevice::sendNotification()
{
    switch (AMCPDevice::command)
    {
        case AMCPDevice::CLS:
        {
            QList<CasparMedia> items;
            foreach (QString response, AMCPDevice::response)
            {
                QString name = response.split("\" ").at(0);
                name.remove(QRegExp("^\"")).remove(QRegExp("\"$"));

                QString type = response.split("\" ").at(1).trimmed().split(" ").at(0);

                items.push_back(CasparMedia(name, type));
            }

            emit mediaChanged(items, *this);
        }
        break;
        case AMCPDevice::TLS:
        {
            QList<CasparTemplate> items;
            foreach (QString response, AMCPDevice::response)
            {
                QString name = response.split("\" ").at(0);
                name.remove(QRegExp("^\"")).remove(QRegExp("\"$"));

                items.push_back(CasparTemplate(name));
            }

            emit templateChanged(items, *this);
        }
        break;
        case AMCPDevice::CINF:
            emit mediaInfoChanged(AMCPDevice::response, *this);
            break;
        case AMCPDevice::INFO:
            emit infoChanged(AMCPDevice::response, *this);
            break;
        case AMCPDevice::INFOSYSTEM:
            emit infoSystemChanged(AMCPDevice::response, *this);
            break;
        case AMCPDevice::DATA:
        {
            QList<CasparData> items;
            foreach (QString response, AMCPDevice::response)
            {
                QString name = response.split("\" ").at(0);
                name.remove(QRegExp("^\"")).remove(QRegExp("\"$"));

                items.push_back(CasparData(name));
            }

            emit dataChanged(items, *this);
        }
        break;
        case AMCPDevice::VERSION:
            emit versionChanged(CasparVersion(AMCPDevice::response.at(0)), *this);
            break;
        case AMCPDevice::CONNECTIONSTATE:
            emit connectionStateChanged(*this);
            break;
        default:
            break;
    }

    AMCPDevice::resetDevice();
}
