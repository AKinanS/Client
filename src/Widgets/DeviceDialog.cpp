#include "DeviceDialog.h"

#include "DatabaseManager.h"

#include <QtCore/QDebug>

#include <QtGui/QAbstractButton>
#include <QtGui/QCloseEvent>
#include <QtGui/QMessageBox>

DeviceDialog::DeviceDialog(QWidget* parent)
    : QDialog(parent),
      editMode(false)
{
    setupUi(this);

    this->lineEditDeviceName->setStyleSheet("border-color: red;");
    this->lineEditAddress->setStyleSheet("border-color: red;");
}

void DeviceDialog::setDeviceModel(const DeviceModel& model)
{
    this->editMode = true;

    setWindowTitle("Edit Server");

    this->lineEditDeviceName->setText(model.getName());
    this->lineEditAddress->setText(model.getAddress());
    this->lineEditPort->setText(QString("%1").arg(model.getPort()));
    this->lineEditUsername->setText(model.getUsername());
    this->lineEditPassword->setText(model.getPassword());
    this->lineEditDescription->setText(model.getDescription());
    this->checkBoxShadow->setChecked((model.getShadow() == "Yes") ? true : false);
}

const QString DeviceDialog::getName() const
{
    return this->lineEditDeviceName->text();
}

const QString DeviceDialog::getAddress() const
{
    return this->lineEditAddress->text();
}

const QString DeviceDialog::getPort() const
{
    if (!this->lineEditPort->text().isEmpty())
        return this->lineEditPort->text();
    else
        return this->lineEditPort->placeholderText();
}

const QString DeviceDialog::getUsername() const
{
    return this->lineEditUsername->text();
}

const QString DeviceDialog::getPassword() const
{
    return this->lineEditPassword->text();
}

const QString DeviceDialog::getDescription() const
{
    return this->lineEditDescription->text();
}

const QString DeviceDialog::getShadow() const
{
    return this->checkBoxShadow->checkState() == Qt::Checked ? "Yes" : "No";
}

void DeviceDialog::accept()
{
    if (this->lineEditDeviceName->text().isEmpty() || this->lineEditAddress->text().isEmpty())
        return;

    if (!this->editMode)
    {
        DeviceModel model = DatabaseManager::getInstance().getDeviceByName(this->lineEditDeviceName->text());
        if (!model.getName().isEmpty())
        {
            QMessageBox box(this);
            box.setWindowTitle("Add Device");
            box.setWindowIcon(QIcon(":/Graphics/Images/CasparCG.png"));
            box.setText("The name already exists in the database. Please choose a unique name.");
            box.setIconPixmap(QPixmap(":/Graphics/Images/Attention.png"));
            box.setStandardButtons(QMessageBox::Ok);
            box.buttons().at(0)->setFocusPolicy(Qt::NoFocus);
            box.exec();

            this->lineEditDeviceName->setFocus();
            this->lineEditDeviceName->selectAll();

            return;
        }

        model = DatabaseManager::getInstance().getDeviceByAddress(this->lineEditAddress->text());
        if (!model.getName().isEmpty())
        {
            QMessageBox box(this);
            box.setWindowTitle("Add Device");
            box.setWindowIcon(QIcon(":/Graphics/Images/CasparCG.png"));
            box.setText("The address already exists in the database. Please choose a unique address.");
            box.setIconPixmap(QPixmap(":/Graphics/Images/Attention.png"));
            box.setStandardButtons(QMessageBox::Ok);
            box.buttons().at(0)->setFocusPolicy(Qt::NoFocus);
            box.exec();

            this->lineEditAddress->setFocus();
            this->lineEditAddress->selectAll();

            return;
        }
    }

    QDialog::accept();
}

void DeviceDialog::testConnection()
{
    if (this->lineEditPort->text().isEmpty())
        this->device = QSharedPointer<CasparDevice>(new CasparDevice(this->lineEditAddress->text()));
    else
        this->device = QSharedPointer<CasparDevice>(new CasparDevice(this->lineEditAddress->text(), this->lineEditPort->text().toInt()));

    QObject::connect(this->device.data(), SIGNAL(connectionStateChanged(CasparDevice&)), this, SLOT(connectionStateChanged(CasparDevice&)));
    this->device->connectDevice();
}

void DeviceDialog::nameChanged(QString name)
{
    if (this->lineEditDeviceName->text().isEmpty())
        this->lineEditDeviceName->setStyleSheet("border-color: red;");
    else
        this->lineEditDeviceName->setStyleSheet("");
}

void DeviceDialog::addressChanged(QString name)
{
    if (this->lineEditAddress->text().isEmpty())
        this->lineEditAddress->setStyleSheet("border-color: red;");
    else
        this->lineEditAddress->setStyleSheet("");
}

void DeviceDialog::connectionStateChanged(CasparDevice& device)
{
    QObject::disconnect(this->device.data(), SIGNAL(connectionStateChanged(CasparDevice&)), this, SLOT(connectionStateChanged(CasparDevice&)));

    QMessageBox box(this);
    box.setWindowTitle("Test Connection");
    box.setText(QString("Successfully connected to device: %1:%2").arg(device.getAddress()).arg(device.getPort()));
    box.setIconPixmap(QPixmap(":/Graphics/Images/Information.png"));
    box.setStandardButtons(QMessageBox::Ok);
    box.buttons().at(0)->setFocusPolicy(Qt::NoFocus);
    box.exec();

    this->device->disconnectDevice();
}
