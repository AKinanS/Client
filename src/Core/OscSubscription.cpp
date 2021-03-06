#include "OscSubscription.h"
#include "DeviceManager.h"

#include <QtCore/QSharedPointer>

OscSubscription::OscSubscription(const QString& predicate, QObject *parent)
    : QObject(parent),
      predicate(predicate)
{  
    QObject::connect(DeviceManager::getInstance().getOscListener().data(), SIGNAL(messageReceived(const QString&, const QList<QVariant>&)),
                     this, SLOT(messageReceived(const QString&, const QList<QVariant>&)));
}

void OscSubscription::messageReceived(const QString& pattern, const QList<QVariant>& arguments)
{
    if (predicate.exactMatch(pattern))
        emit subscriptionReceived(predicate.pattern(), arguments);
}
