#include "VideolayerChangedEvent.h"

#include "Global.h"

VideolayerChangedEvent::VideolayerChangedEvent(int videolayer)
    : QEvent(static_cast<QEvent::Type>(Event::EventType::VideolayerChanged)), videolayer(videolayer)
{
}

int VideolayerChangedEvent::getVideolayer() const
{
    return this->videolayer;
}
