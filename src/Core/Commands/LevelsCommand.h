#pragma once

#include "../Shared.h"
#include "AbstractCommand.h"

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QXmlStreamWriter>

class CORE_EXPORT LevelsCommand : public AbstractCommand
{
    Q_OBJECT

    public:
        explicit LevelsCommand(QObject* parent = 0);

        virtual void readProperties(boost::property_tree::wptree& pt);
        virtual void writeProperties(QXmlStreamWriter* writer);

        float getMinIn() const;
        float getMaxIn() const;
        float getMinOut() const;
        float getMaxOut() const;
        float getGamma() const;
        int getDuration() const;
        const QString& getTween() const;
        bool getDefer() const;

        void setMinIn(float minIn);
        void setMaxIn(float maxIn);
        void setMinOut(float minOut);
        void setMaxOut(float maxOut);
        void setGamma(float gamma);
        void setDuration(int duration);
        void setTween(const QString& tween);
        void setDefer(bool defer);

    private:
        float minIn;
        float maxIn;
        float minOut;
        float maxOut;
        float gamma;
        int duration;
        QString tween;
        bool defer;

        Q_SIGNAL void minInChanged(float);
        Q_SIGNAL void maxInChanged(float);
        Q_SIGNAL void minOutChanged(float);
        Q_SIGNAL void maxOutChanged(float);
        Q_SIGNAL void gammaChanged(float);
        Q_SIGNAL void durationChanged(int);
        Q_SIGNAL void tweenChanged(const QString&);
        Q_SIGNAL void deferChanged(bool);
};
