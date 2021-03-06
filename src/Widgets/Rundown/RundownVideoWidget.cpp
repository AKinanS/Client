#include "RundownVideoWidget.h"

#include "Global.h"

#include "DatabaseManager.h"
#include "DeviceManager.h"
#include "GpiManager.h"
#include "Events/ConnectionStateChangedEvent.h"
#include "Events/Inspector/TargetChangedEvent.h"
#include "Events/Inspector/LabelChangedEvent.h"
#include "Events/Inspector/DeviceChangedEvent.h"

#include <math.h>

#include <QtCore/QObject>
#include <QtCore/QFileInfo>

#include <QtGui/QPixmap>

RundownVideoWidget::RundownVideoWidget(const LibraryModel& model, QWidget* parent, const QString& color, bool active,
                                       bool loaded, bool paused, bool playing, bool inGroup, bool compactView)
    : QWidget(parent),
      active(active), loaded(loaded), paused(paused), playing(playing), inGroup(inGroup), compactView(compactView), color(color), model(model),
      fileModel(NULL), timeSubscription(NULL), frameSubscription(NULL), fpsSubscription(NULL), pathSubscription(NULL), pausedSubscription(NULL),
      loopSubscription(NULL), animation(NULL), reverseOscTime(false)
{
    setupUi(this);

    this->animation = new ActiveAnimation(this->labelActiveColor);

    setThumbnail();
    setColor(this->color);
    setActive(this->active);
    setCompactView(this->compactView);

    this->command.setVideoName(this->model.getName());

    this->labelLoopOverlay->setVisible(false);

    this->labelGroupColor->setVisible(this->inGroup);
    this->labelGroupColor->setStyleSheet(QString("background-color: %1;").arg(Color::DEFAULT_GROUP_COLOR));
    this->labelColor->setStyleSheet(QString("background-color: %1;").arg(Color::DEFAULT_MOVIE_COLOR));

    this->labelLabel->setText(this->model.getLabel());
    this->labelChannel->setText(QString("Channel: %1").arg(this->command.getChannel()));
    this->labelVideolayer->setText(QString("Video layer: %1").arg(this->command.getVideolayer()));
    this->labelDelay->setText(QString("Delay: %1").arg(this->command.getDelay()));
    this->labelDevice->setText(QString("Server: %1").arg(this->model.getDeviceName()));

    this->executeTimer.setSingleShot(true);
    QObject::connect(&this->executeTimer, SIGNAL(timeout()), SLOT(executePlay()));

    QObject::connect(&this->command, SIGNAL(channelChanged(int)), this, SLOT(channelChanged(int)));
    QObject::connect(&this->command, SIGNAL(videolayerChanged(int)), this, SLOT(videolayerChanged(int)));
    QObject::connect(&this->command, SIGNAL(delayChanged(int)), this, SLOT(delayChanged(int)));
    QObject::connect(&this->command, SIGNAL(allowGpiChanged(bool)), this, SLOT(allowGpiChanged(bool)));
    QObject::connect(&this->command, SIGNAL(loopChanged(bool)), this, SLOT(loopChanged(bool)));

    QObject::connect(&DeviceManager::getInstance(), SIGNAL(deviceAdded(CasparDevice&)), this, SLOT(deviceAdded(CasparDevice&)));
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device != NULL)
        QObject::connect(device.data(), SIGNAL(connectionStateChanged(CasparDevice&)), this, SLOT(deviceConnectionStateChanged(CasparDevice&)));

    QObject::connect(GpiManager::getInstance().getGpiDevice().data(), SIGNAL(connectionStateChanged(bool, GpiDevice*)), this, SLOT(gpiConnectionStateChanged(bool, GpiDevice*)));

    this->reverseOscTime = (DatabaseManager::getInstance().getConfigurationByName("ReverseOscTime").getValue() == "true") ? true : false;

    checkEmptyDevice();
    checkGpiConnection();
    checkDeviceConnection();

    configureOscSubscriptions();

    qApp->installEventFilter(this);
}

bool RundownVideoWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == static_cast<QEvent::Type>(Event::EventType::ChannelChanged) || event->type() == static_cast<QEvent::Type>(Event::EventType::VideolayerChanged))
    {
        // This event is not for us.
        if (!this->active)
            return false;

        configureOscSubscriptions();

        return true;
    }
    if (event->type() == static_cast<QEvent::Type>(Event::EventType::TargetChanged))
    {
        // This event is not for us.
        if (!this->active)
            return false;

        TargetChangedEvent* targetChangedEvent = dynamic_cast<TargetChangedEvent*>(event);
        this->model.setName(targetChangedEvent->getTarget());
        this->command.setVideoName(targetChangedEvent->getTarget());

        return true;
    }
    else if (event->type() == static_cast<QEvent::Type>(Event::EventType::LabelChanged))
    {
        // This event is not for us.
        if (!this->active)
            return false;

        LabelChangedEvent* labelChanged = dynamic_cast<LabelChangedEvent*>(event);
        this->model.setLabel(labelChanged->getLabel());

        this->labelLabel->setText(this->model.getLabel());

        return true;
    }
    else if (event->type() == static_cast<QEvent::Type>(Event::EventType::DeviceChanged))
    {
        // This event is not for us.
        if (!this->active)
            return false;

        // Should we update the device name?
        DeviceChangedEvent* deviceChangedEvent = dynamic_cast<DeviceChangedEvent*>(event);
        if (!deviceChangedEvent->getDeviceName().isEmpty() && deviceChangedEvent->getDeviceName() != this->model.getDeviceName())
        {
            // Disconnect connectionStateChanged() from the old device.
            const QSharedPointer<CasparDevice> oldDevice = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
            if (oldDevice != NULL)
                QObject::disconnect(oldDevice.data(), SIGNAL(connectionStateChanged(CasparDevice&)), this, SLOT(deviceConnectionStateChanged(CasparDevice&)));

            // Update the model with the new device.
            this->model.setDeviceName(deviceChangedEvent->getDeviceName());
            this->labelDevice->setText(QString("Server: %1").arg(this->model.getDeviceName()));

            // Connect connectionStateChanged() to the new device.
            const QSharedPointer<CasparDevice> newDevice = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
            if (newDevice != NULL)
                QObject::connect(newDevice.data(), SIGNAL(connectionStateChanged(CasparDevice&)), this, SLOT(deviceConnectionStateChanged(CasparDevice&)));
        }

        checkEmptyDevice();
        checkDeviceConnection();
        configureOscSubscriptions();
    }

    return QObject::eventFilter(target, event);
}

AbstractRundownWidget* RundownVideoWidget::clone()
{
    RundownVideoWidget* widget = new RundownVideoWidget(this->model, this->parentWidget(), this->color, this->active,
                                                        this->loaded, this->paused, this->playing, this->inGroup, this->compactView);

    VideoCommand* command = dynamic_cast<VideoCommand*>(widget->getCommand());
    command->setChannel(this->command.getChannel());
    command->setVideolayer(this->command.getVideolayer());
    command->setDelay(this->command.getDelay());
    command->setAllowGpi(this->command.getAllowGpi());
    command->setVideoName(this->command.getVideoName());
    command->setTransition(this->command.getTransition());
    command->setDuration(this->command.getDuration());
    command->setTween(this->command.getTween());
    command->setDirection(this->command.getDirection());
    command->setLoop(this->command.getLoop());
    command->setSeek(this->command.getSeek());
    command->setLength(this->command.getLength());
    command->setUseAuto(this->command.getUseAuto());
    command->setFreezeOnLoad(this->command.getFreezeOnLoad());
    command->setTriggerOnNext(this->command.getTriggerOnNext());

    return widget;
}

void RundownVideoWidget::setCompactView(bool compactView)
{
    if (compactView)
    {
        this->labelIcon->setFixedSize(Rundown::COMPACT_ICON_WIDTH, Rundown::COMPACT_ICON_HEIGHT);
        this->labelLoopOverlay->setFixedSize(Rundown::COMPACT_ICON_WIDTH, Rundown::COMPACT_ICON_HEIGHT);
        this->labelGpiConnected->setFixedSize(Rundown::COMPACT_ICON_WIDTH, Rundown::COMPACT_ICON_HEIGHT);
        this->labelDisconnected->setFixedSize(Rundown::COMPACT_ICON_WIDTH, Rundown::COMPACT_ICON_HEIGHT);
        this->labelThumbnail->setFixedSize(Rundown::COMPACT_THUMBNAIL_WIDTH, Rundown::COMPACT_THUMBNAIL_HEIGHT);
    }
    else
    {
        this->labelIcon->setFixedSize(Rundown::DEFAULT_ICON_WIDTH, Rundown::DEFAULT_ICON_HEIGHT);
        this->labelLoopOverlay->setFixedSize(Rundown::DEFAULT_ICON_WIDTH, Rundown::DEFAULT_ICON_HEIGHT);
        this->labelGpiConnected->setFixedSize(Rundown::DEFAULT_ICON_WIDTH, Rundown::DEFAULT_ICON_HEIGHT);
        this->labelDisconnected->setFixedSize(Rundown::DEFAULT_ICON_WIDTH, Rundown::DEFAULT_ICON_HEIGHT);
        this->labelThumbnail->setFixedSize(Rundown::DEFAULT_THUMBNAIL_WIDTH, Rundown::DEFAULT_THUMBNAIL_HEIGHT);
    }

    this->widgetOscTime->setCompactView(compactView);

    this->compactView = compactView;
}

void RundownVideoWidget::readProperties(boost::property_tree::wptree& pt)
{
    if (pt.count(L"color") > 0) setColor(QString::fromStdWString(pt.get<std::wstring>(L"color")));
}

void RundownVideoWidget::writeProperties(QXmlStreamWriter* writer)
{
    writer->writeTextElement("color", this->color);
}

bool RundownVideoWidget::isGroup() const
{
    return false;
}

AbstractCommand* RundownVideoWidget::getCommand()
{
    return &this->command;
}

LibraryModel* RundownVideoWidget::getLibraryModel()
{
    return &this->model;
}

void RundownVideoWidget::setThumbnail()
{
    if (this->model.getType() == "AUDIO")
    {
        this->labelThumbnail->setVisible(false);
        return;
    }

    QString data = DatabaseManager::getInstance().getThumbnailById(this->model.getThumbnailId()).getData();
    if (data.isEmpty())
        data = DatabaseManager::getInstance().getThumbnailByNameAndDeviceName(this->model.getName(), this->model.getDeviceName()).getData();

    QImage image;
    image.loadFromData(QByteArray::fromBase64(data.toAscii()), "PNG");
    this->labelThumbnail->setPixmap(QPixmap::fromImage(image));

    bool displayThumbnailTooltip = (DatabaseManager::getInstance().getConfigurationByName("ShowThumbnailTooltip").getValue() == "true") ? true : false;
    if (displayThumbnailTooltip)
        this->labelThumbnail->setToolTip(QString("<img src=\"data:image/png;base64,%1 \"/>").arg(data));
}

void RundownVideoWidget::setActive(bool active)
{
    this->active = active;

    this->animation->stop();

    if (this->active)
        this->labelActiveColor->setStyleSheet(QString("background-color: %1;").arg(Color::DEFAULT_ACTIVE_COLOR));
    else
        this->labelActiveColor->setStyleSheet("");
}

void RundownVideoWidget::setInGroup(bool inGroup)
{
    this->inGroup = inGroup;
    this->labelGroupColor->setVisible(this->inGroup);
}

void RundownVideoWidget::setColor(const QString& color)
{
    this->color = color;
    this->setStyleSheet(QString("#frameItem, #frameStatus { background-color: rgba(%1); }").arg(color));
}

void RundownVideoWidget::checkEmptyDevice()
{
    if (this->labelDevice->text() == "Device: ")
        this->labelDevice->setStyleSheet("color: black;");
    else
        this->labelDevice->setStyleSheet("");
}

bool RundownVideoWidget::executeCommand(enum Playout::PlayoutType::Type type)
{
    if (type == Playout::PlayoutType::Stop)
        QTimer::singleShot(0, this, SLOT(executeStop()));
    else if ((type == Playout::PlayoutType::Play && !this->command.getTriggerOnNext()) || type == Playout::PlayoutType::Update)
    {
        if (!this->model.getDeviceName().isEmpty()) // The user need to select a device.
        {
            const QStringList& channelFormats = DatabaseManager::getInstance().getDeviceByName(this->model.getDeviceName()).getChannelFormats().split(",");
            double framesPerSecond = DatabaseManager::getInstance().getFormat(channelFormats[this->command.getChannel() - 1]).getFramesPerSecond().toDouble();

            this->executeTimer.setInterval(floor(this->command.getDelay() * (1000 / framesPerSecond)));
            this->executeTimer.start();
        }
    }
    else if (type == Playout::PlayoutType::Pause)
        QTimer::singleShot(0, this, SLOT(executePause()));
    else if (type == Playout::PlayoutType::Load)
        QTimer::singleShot(0, this, SLOT(executeLoad()));
    else if (type == Playout::PlayoutType::Next && this->command.getTriggerOnNext())
        QTimer::singleShot(0, this, SLOT(executePlay()));
    else if (type == Playout::PlayoutType::Clear)
        QTimer::singleShot(0, this, SLOT(executeClearVideolayer()));
    else if (type == Playout::PlayoutType::ClearVideolayer)
        QTimer::singleShot(0, this, SLOT(executeClearVideolayer()));
    else if (type == Playout::PlayoutType::ClearChannel)
        QTimer::singleShot(0, this, SLOT(executeClearChannel()));

    if (this->active)
        this->animation->start(1);

    return true;
}

void RundownVideoWidget::executeStop()
{
    this->executeTimer.stop();

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
        device->stopVideo(this->command.getChannel(), this->command.getVideolayer());

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getDeviceByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
            deviceShadow->stopVideo(this->command.getChannel(), this->command.getVideolayer());
    }

    this->paused = false;
    this->loaded = false;
    this->playing = false;
}

void RundownVideoWidget::executePlay()
{
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        if (this->loaded)
        {
            device->playVideo(this->command.getChannel(), this->command.getVideolayer());
        }
        else
        {
            device->playVideo(this->command.getChannel(), this->command.getVideolayer(), this->command.getVideoName(),
                              this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                              this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                              this->command.getLoop(), this->command.getUseAuto());
        }
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice>  deviceShadow = DeviceManager::getInstance().getDeviceByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            if (this->loaded)
            {
                deviceShadow->playVideo(this->command.getChannel(), this->command.getVideolayer());
            }
            else
            {
                deviceShadow->playVideo(this->command.getChannel(), this->command.getVideolayer(), this->command.getVideoName(),
                                        this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                                        this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                                        this->command.getLoop(), this->command.getUseAuto());
            }
        }
    }

    this->paused = false;
    this->loaded = false;
    this->playing = true;
}

void RundownVideoWidget::executePause()
{
    if (!this->playing)
        return;

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        if (this->paused)
            device->playVideo(this->command.getChannel(), this->command.getVideolayer());
        else
            device->pauseVideo(this->command.getChannel(), this->command.getVideolayer());
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getDeviceByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            if (this->paused)
                deviceShadow->playVideo(this->command.getChannel(), this->command.getVideolayer());
            else
                deviceShadow->pauseVideo(this->command.getChannel(), this->command.getVideolayer());
        }
    }

    this->paused = !this->paused;
}

void RundownVideoWidget::executeLoad()
{
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        device->loadVideo(this->command.getChannel(), this->command.getVideolayer(), this->command.getVideoName(),
                          this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                          this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                          this->command.getLoop(), this->command.getFreezeOnLoad(), this->command.getUseAuto());
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice>  deviceShadow = DeviceManager::getInstance().getDeviceByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            deviceShadow->loadVideo(this->command.getChannel(), this->command.getVideolayer(), this->command.getVideoName(),
                                    this->command.getTransition(), this->command.getDuration(), this->command.getTween(),
                                    this->command.getDirection(), this->command.getSeek(), this->command.getLength(),
                                    this->command.getLoop(), this->command.getFreezeOnLoad(), this->command.getUseAuto());
        }
    }

    this->loaded = true;
    this->paused = false;
    this->playing = false;
}

void RundownVideoWidget::executeClearVideolayer()
{
    this->executeTimer.stop();

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
        device->clearVideolayer(this->command.getChannel(), this->command.getVideolayer());

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getDeviceByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
            deviceShadow->clearVideolayer(this->command.getChannel(), this->command.getVideolayer());
    }

    this->paused = false;
    this->loaded = false;
    this->playing = false;

    this->widgetOscTime->reset();
}

void RundownVideoWidget::executeClearChannel()
{
    this->executeTimer.stop();

    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device != NULL && device->isConnected())
    {
        device->clearChannel(this->command.getChannel());
        device->clearMixerChannel(this->command.getChannel());
    }

    foreach (const DeviceModel& model, DeviceManager::getInstance().getDeviceModels())
    {
        if (model.getShadow() == "No")
            continue;

        const QSharedPointer<CasparDevice> deviceShadow = DeviceManager::getInstance().getDeviceByName(model.getName());
        if (deviceShadow != NULL && deviceShadow->isConnected())
        {
            deviceShadow->clearChannel(this->command.getChannel());
            deviceShadow->clearMixerChannel(this->command.getChannel());
        }
    }

    this->paused = false;
    this->loaded = false;
    this->playing = false;

    this->widgetOscTime->reset();
}

void RundownVideoWidget::checkGpiConnection()
{
    this->labelGpiConnected->setVisible(this->command.getAllowGpi());

    if (GpiManager::getInstance().getGpiDevice()->isConnected())
        this->labelGpiConnected->setPixmap(QPixmap(":/Graphics/Images/GpiConnected.png"));
    else
        this->labelGpiConnected->setPixmap(QPixmap(":/Graphics/Images/GpiDisconnected.png"));
}

void RundownVideoWidget::checkDeviceConnection()
{
    const QSharedPointer<CasparDevice> device = DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName());
    if (device == NULL)
        this->labelDisconnected->setVisible(true);
    else
    {
        this->widgetOscTime->reset();
        this->labelDisconnected->setVisible(!device->isConnected());
    }
}

void RundownVideoWidget::configureOscSubscriptions()
{
    if (DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName()) == NULL)
        return;

    if (this->timeSubscription != NULL)
        this->timeSubscription->disconnect(); // Disconnect all events.

    if (this->frameSubscription != NULL)
        this->frameSubscription->disconnect(); // Disconnect all events.

    if (this->fpsSubscription != NULL)
        this->fpsSubscription->disconnect(); // Disconnect all events.

    if (this->pathSubscription != NULL)
        this->pathSubscription->disconnect(); // Disconnect all events.

    if (this->pausedSubscription != NULL)
        this->pausedSubscription->disconnect(); // Disconnect all events.

    if (this->loopSubscription != NULL)
        this->loopSubscription->disconnect(); // Disconnect all events.

    QString timeFilter = Osc::DEFAULT_TIME_FILTER;
    timeFilter.replace("#IPADDRESS#", QString("%1").arg(DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName())->getIpAddress()))
              .replace("#CHANNEL#", QString("%1").arg(this->command.getChannel()))
              .replace("#VIDEOLAYER#", QString("%1").arg(this->command.getVideolayer()));
    this->timeSubscription = new OscSubscription(timeFilter, this);
    QObject::connect(this->timeSubscription, SIGNAL(subscriptionReceived(const QString&, const QList<QVariant>&)),
                     this, SLOT(timeSubscriptionReceived(const QString&, const QList<QVariant>&)));

    QString frameFilter = Osc::DEFAULT_FRAME_FILTER;
    frameFilter.replace("#IPADDRESS#", QString("%1").arg(DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName())->getIpAddress()))
               .replace("#CHANNEL#", QString("%1").arg(this->command.getChannel()))
               .replace("#VIDEOLAYER#", QString("%1").arg(this->command.getVideolayer()));
    this->frameSubscription = new OscSubscription(frameFilter, this);
    QObject::connect(this->frameSubscription, SIGNAL(subscriptionReceived(const QString&, const QList<QVariant>&)),
                     this, SLOT(frameSubscriptionReceived(const QString&, const QList<QVariant>&)));

    QString fpsFilter = Osc::DEFAULT_FPS_FILTER;
    fpsFilter.replace("#IPADDRESS#", QString("%1").arg(DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName())->getIpAddress()))
             .replace("#CHANNEL#", QString("%1").arg(this->command.getChannel()))
             .replace("#VIDEOLAYER#", QString("%1").arg(this->command.getVideolayer()));
    this->fpsSubscription = new OscSubscription(fpsFilter, this);
    QObject::connect(this->fpsSubscription, SIGNAL(subscriptionReceived(const QString&, const QList<QVariant>&)),
                     this, SLOT(fpsSubscriptionReceived(const QString&, const QList<QVariant>&)));

    QString pathFilter = Osc::DEFAULT_PATH_FILTER;
    pathFilter.replace("#IPADDRESS#", QString("%1").arg(DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName())->getIpAddress()))
              .replace("#CHANNEL#", QString("%1").arg(this->command.getChannel()))
              .replace("#VIDEOLAYER#", QString("%1").arg(this->command.getVideolayer()));
    this->pathSubscription = new OscSubscription(pathFilter, this);
    QObject::connect(this->pathSubscription, SIGNAL(subscriptionReceived(const QString&, const QList<QVariant>&)),
                     this, SLOT(pathSubscriptionReceived(const QString&, const QList<QVariant>&)));

    QString pausedFilter = Osc::DEFAULT_PAUSED_FILTER;
    pausedFilter.replace("#IPADDRESS#", QString("%1").arg(DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName())->getIpAddress()))
                .replace("#CHANNEL#", QString("%1").arg(this->command.getChannel()))
                .replace("#VIDEOLAYER#", QString("%1").arg(this->command.getVideolayer()));
    this->pausedSubscription = new OscSubscription(pausedFilter, this);
    QObject::connect(this->pausedSubscription, SIGNAL(subscriptionReceived(const QString&, const QList<QVariant>&)),
                     this, SLOT(pausedSubscriptionReceived(const QString&, const QList<QVariant>&)));

    QString loopFilter = Osc::DEFAULT_LOOP_FILTER;
    loopFilter.replace("#IPADDRESS#", QString("%1").arg(DeviceManager::getInstance().getDeviceByName(this->model.getDeviceName())->getIpAddress()))
              .replace("#CHANNEL#", QString("%1").arg(this->command.getChannel()))
              .replace("#VIDEOLAYER#", QString("%1").arg(this->command.getVideolayer()));
    this->loopSubscription = new OscSubscription(loopFilter, this);
    QObject::connect(this->loopSubscription, SIGNAL(subscriptionReceived(const QString&, const QList<QVariant>&)),
                     this, SLOT(loopSubscriptionReceived(const QString&, const QList<QVariant>&)));
}

void RundownVideoWidget::channelChanged(int channel)
{
    this->labelChannel->setText(QString("Channel: %1").arg(channel));

    configureOscSubscriptions();
}

void RundownVideoWidget::videolayerChanged(int videolayer)
{
    this->labelVideolayer->setText(QString("Video layer: %1").arg(videolayer));

    configureOscSubscriptions();
}

void RundownVideoWidget::delayChanged(int delay)
{
    this->labelDelay->setText(QString("Delay: %1").arg(delay));
}

void RundownVideoWidget::allowGpiChanged(bool allowGpi)
{
    checkGpiConnection();
}

void RundownVideoWidget::loopChanged(bool loop)
{
    this->labelLoopOverlay->setVisible(loop);
}

void RundownVideoWidget::gpiConnectionStateChanged(bool connected, GpiDevice* device)
{
    checkGpiConnection();
}

void RundownVideoWidget::deviceConnectionStateChanged(CasparDevice& device)
{
    checkDeviceConnection();
}

void RundownVideoWidget::deviceAdded(CasparDevice& device)
{
    if (DeviceManager::getInstance().getDeviceModelByAddress(device.getAddress()).getName() == this->model.getDeviceName())
        QObject::connect(&device, SIGNAL(connectionStateChanged(CasparDevice&)), this, SLOT(deviceConnectionStateChanged(CasparDevice&)));

    checkDeviceConnection();
    configureOscSubscriptions();
}

void RundownVideoWidget::timeSubscriptionReceived(const QString& predicate, const QList<QVariant>& arguments)
{
    if (this->fileModel != NULL)
    {
        delete this->fileModel;
        this->fileModel = NULL;
    }

    this->fileModel = new OscFileModel();
    this->fileModel->setTime(arguments.at(0).toDouble());
    this->fileModel->setTotalTime(arguments.at(1).toDouble());
}

void RundownVideoWidget::frameSubscriptionReceived(const QString& predicate, const QList<QVariant>& arguments)
{
    if (this->fileModel == NULL)
        return;

    this->fileModel->setFrame(arguments.at(0).toInt());
    this->fileModel->setTotalFrames(arguments.at(1).toInt());
}

void RundownVideoWidget::fpsSubscriptionReceived(const QString& predicate, const QList<QVariant>& arguments)
{
    if (this->fileModel == NULL)
        return;

    this->fileModel->setFramesPerSecond(arguments.at(0).toInt());
}

void RundownVideoWidget::pathSubscriptionReceived(const QString& predicate, const QList<QVariant>& arguments)
{
    QString name = arguments.at(0).toString();
    name.remove(name.lastIndexOf('.'), name.length()); // Remove extension.

    if (this->model.getName().toLower() != name.toLower())
        return; // Wrong file.

    if (this->fileModel == NULL)
        return;

    this->fileModel->setPath(arguments.at(0).toString());

    this->widgetOscTime->setTime(this->fileModel->getFrame());

    if (this->reverseOscTime && this->fileModel->getFrame() > 0)
        this->widgetOscTime->setTime(this->fileModel->getTotalFrames() - this->fileModel->getFrame());
    else
        this->widgetOscTime->setTime(this->fileModel->getFrame());

    this->widgetOscTime->setProgress(this->fileModel->getFrame());

    if (this->command.getSeek() == 0 && this->command.getLength() == 0)
        this->widgetOscTime->setInOutTime(0, this->fileModel->getTotalFrames());
    else
        this->widgetOscTime->setInOutTime(this->command.getSeek(), this->command.getLength());

    this->widgetOscTime->setFramesPerSecond(this->fileModel->getFramesPerSecond());

    this->playing = true;
}

void RundownVideoWidget::pausedSubscriptionReceived(const QString& predicate, const QList<QVariant>& arguments)
{
    this->widgetOscTime->setPaused(arguments.at(0).toBool());
}

void RundownVideoWidget::loopSubscriptionReceived(const QString& predicate, const QList<QVariant>& arguments)
{
    this->widgetOscTime->setLoop(arguments.at(0).toBool());
}
