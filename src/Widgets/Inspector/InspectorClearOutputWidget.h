#pragma once

#include "../Shared.h"
#include "ui_InspectorClearOutputWidget.h"

#include "Commands/ClearOutputCommand.h"
#include "Models/LibraryModel.h"

#include <QtCore/QEvent>
#include <QtCore/QObject>

#include <QtGui/QWidget>

class WIDGETS_EXPORT InspectorClearOutputWidget : public QWidget, Ui::InspectorClearOutputWidget
{
    Q_OBJECT

    public:
        explicit InspectorClearOutputWidget(QWidget* parent = 0);

    protected:
        virtual bool eventFilter(QObject* target, QEvent* event);

    private:
        LibraryModel* model;
        ClearOutputCommand* command;

        void blockAllSignals(bool block);

        Q_SLOT void clearChannelChanged(int);
        Q_SLOT void triggerOnNextChanged(int);
        Q_SLOT void resetClearChannel(QString);
        Q_SLOT void resetTriggerOnNext(QString);
};
