#pragma once

#include "Shared.h"

#include "AMCPDevice.h"
#include "Models/CasparData.h"
#include "Models/CasparMedia.h"
#include "Models/CasparTemplate.h"
#include "Models/CasparThumbnail.h"

class CASPAR_EXPORT CasparDevice : public AMCPDevice
{
    Q_OBJECT

    public:
        explicit CasparDevice(const QString& address, int port = 5250, QObject* parent = 0);

        const QString getIpAddress() const;

        void refreshData();
        void refreshMedia();
        void refreshTemplate();
        void refreshChannels();
        void refreshThumbnail();

        void refreshFlashVersion();
        void refreshServerVersion();
        void refreshTemplateHostVersion();

        void retrieveThumbnail(const QString& name);

        void sendCommand(const QString& command);

        void clearChannel(int channel);
        void clearMixerChannel(int channel);
        void clearVideolayer(int channel, int videolayer);
        void clearMixerVideolayer(int channel, int videolayer);

        void setMasterVolume(int channel, float masterVolume);

        void addTemplate(int channel, int videolayer, int flashlayer, const QString& name, bool playOnLoad);
        void addTemplate(int channel, int videolayer, int flashlayer, const QString& name, bool playOnLoad, const QString& data);
        void invokeTemplate(int channel, int videolayer, int flashlayer, const QString& label);
        void nextTemplate(int channel, int videolayer, int flashlayer);
        void playTemplate(int channel, int videolayer, int flashlayer);
        void playTemplate(int channel, int videolayer, int flashlayer, const QString& name);
        void playTemplate(int channel, int videolayer, int flashlayer, const QString& name, const QString& data);
        void removeTemplate(int channel, int videolayer, int flashlayer);
        void stopTemplate(int channel, int videolayer, int flashlayer);
        void updateTemplate(int channel, int videolayer, int flashlayer, const QString& data);

        void pauseVideo(int channel, int videolayer);
        void playVideo(int channel, int videolayer);
        void playVideo(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, int seek, int length, bool loop, bool useAuto);
        void loadVideo(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, int seek, int length, bool loop, bool freezeOnLoad, bool useAuto);
        void stopVideo(int channel, int videolayer);

        void pauseAudio(int channel, int videolayer);
        void playAudio(int channel, int videolayer);
        void playAudio(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, bool loop, bool useAuto);
        void loadAudio(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, bool loop, bool useAuto);
        void stopAudio(int channel, int videolayer);

        void pauseColor(int channel, int videolayer);
        void playColor(int channel, int videolayer);
        void playColor(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, bool useAuto);
        void loadColor(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, bool useAuto);
        void stopColor(int channel, int videolayer);

        void pauseImage(int channel, int videolayer);
        void playImage(int channel, int videolayer);
        void playImage(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, bool useAuto);
        void loadImage(int channel, int videolayer, const QString& name, const QString& transition, int duration,
                       const QString& easing, const QString& direction, bool useAuto);
        void stopImage(int channel, int videolayer);

        void startRecording(int channel, const QString& filename, const QString& codec, const QString& preset, const QString& tune, bool withAlpha);
        void stopRecording(int channel);

        void print(int channel, const QString& output);

        void pauseDeviceInput(int channel, int videolayer);
        void playDeviceInput(int channel, int videolayer);
        void playDeviceInput(int channel, int videolayer, int device, const QString& format);
        void loadDeviceInput(int channel, int videolayer, int device, const QString& format);
        void stopDeviceInput(int channel, int videolayer);

        void pauseImageScroll(int channel, int videolayer);
        void playImageScroll(int channel, int videolayer);
        void playImageScroll(int channel, int videolayer, const QString& name, int blur, int speed, bool premultiply, bool progressive);
        void loadImageScroll(int channel, int videolayer, const QString& name, int blur, int speed, bool premultiply, bool progressive);
        void stopImageScroll(int channel, int videolayer);

        void setCommit(int channel);
        void setChroma(int channel, int videolayer, const QString& key, float threshold, float softness, float spill, float blur, bool mask);
        void setBlendMode(int channel, int videolayer, const QString& blendMode);
        void setGrid(int channel, int grid, int duration, const QString& easing, bool defer = false);
        void setKeyer(int channel, int videolayer, int keyer, bool defer = false);
        void setVolume(int channel, int videolayer, float volume, bool defer = false);
        void setVolume(int channel, int videolayer, float volume, int duration, const QString& easing, bool defer = false);
        void setOpacity(int channel, int videolayer, float opacity, bool defer = false);
        void setOpacity(int channel, int videolayer, float opacity, int duration, const QString& easing, bool defer = false);
        void setBrightness(int channel, int videolayer, float brightness, bool defer = false);
        void setBrightness(int channel, int videolayer, float brightness, int duration, const QString& easing, bool defer = false);
        void setContrast(int channel, int videolayer, float contrast, bool defer = false);
        void setContrast(int channel, int videolayer, float contrast, int duration, const QString& easing, bool defer = false);
        void setSaturation(int channel, int videolayer, float saturation, bool defer = false);
        void setSaturation(int channel, int videolayer, float saturation, int duration, const QString& easing, bool defer = false);
        void setLevels(int channel, int videolayer, float minIn, float maxIn, float gamma, float minOut, float maxOut,
                       bool defer = false);
        void setLevels(int channel, int videolayer, float minIn, float maxIn, float gamma, float minOut, float maxOut,
                       int duration, const QString& easing, bool defer = false);
        void setGeometry(int channel, int videolayer, float positionX, float positionY, float scaleX, float scaleY,
                         bool defer = false);
        void setGeometry(int channel, int videolayer, float positionX, float positionY, float scaleX, float scaleY,
                         int duration, const QString& easing, bool defer = false);
        void setClipping(int channel, int videolayer, float positionX, float positionY, float scaleX, float scaleY,
                         bool defer = false);
        void setClipping(int channel, int videolayer, float positionX, float positionY, float scaleX, float scaleY,
                         int duration, const QString& easing, bool defer = false);

        Q_SIGNAL void connectionStateChanged(CasparDevice&);
        Q_SIGNAL void infoChanged(const QList<QString>&, CasparDevice&);
        Q_SIGNAL void infoSystemChanged(const QList<QString>&, CasparDevice&);
        Q_SIGNAL void mediaChanged(const QList<CasparMedia>&, CasparDevice&);
        Q_SIGNAL void mediaInfoChanged(const QList<QString>&, CasparDevice&);
        Q_SIGNAL void templateChanged(const QList<CasparTemplate>&, CasparDevice&);
        Q_SIGNAL void dataChanged(const QList<CasparData>&, CasparDevice&);
        Q_SIGNAL void versionChanged(const QString& version, CasparDevice&);
        Q_SIGNAL void responseChanged(const QString&, CasparDevice&);
        Q_SIGNAL void thumbnailChanged(const QList<CasparThumbnail>&, CasparDevice&);
        Q_SIGNAL void thumbnailRetrieveChanged(const QString& data, CasparDevice&);

    protected:
        void sendNotification();
};
