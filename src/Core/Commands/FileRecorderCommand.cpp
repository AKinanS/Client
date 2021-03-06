#include "FileRecorderCommand.h"

#include "Global.h"

FileRecorderCommand::FileRecorderCommand(QObject* parent)
    : AbstractCommand(parent),
      output(FileRecorder::DEFAULT_OUTPUT), codec(FileRecorder::DEFAULT_CODEC), preset(FileRecorder::DEFAULT_PRESET),
      tune(FileRecorder::DEFAULT_TUNE), withAlpha(FileRecorder::DEFAULT_WITH_ALPHA)
{
}

const QString& FileRecorderCommand::getOutput() const
{
    return this->output;
}

const QString& FileRecorderCommand::getCodec() const
{
    return this->codec;
}

const QString& FileRecorderCommand::getPreset() const
{
    return this->preset;
}

const QString& FileRecorderCommand::getTune() const
{
    return this->tune;
}

bool FileRecorderCommand::getWithAlpha() const
{
    return this->withAlpha;
}

void FileRecorderCommand::setOutput(const QString& output)
{
    this->output = output;
    emit outputChanged(this->output);
}

void FileRecorderCommand::setCodec(const QString& codec)
{
    this->codec = codec;
    emit codecChanged(this->codec);
}

void FileRecorderCommand::setPreset(const QString& preset)
{
    this->preset = preset;
    emit presetChanged(this->preset);
}

void FileRecorderCommand::setTune(const QString& tune)
{
    this->tune = tune;
    emit tuneChanged(this->tune);
}

void FileRecorderCommand::setWithAlpha(bool withAlpha)
{
    this->withAlpha = withAlpha;
    emit withAlphaChanged(this->withAlpha);
}

void FileRecorderCommand::readProperties(boost::property_tree::wptree& pt)
{
    AbstractCommand::readProperties(pt);

    if (pt.count(L"output") > 0) setOutput(QString::fromStdWString(pt.get<std::wstring>(L"output")));
    if (pt.count(L"codec") > 0) setCodec(QString::fromStdWString(pt.get<std::wstring>(L"codec")));
    if (pt.count(L"preset") > 0) setPreset(QString::fromStdWString(pt.get<std::wstring>(L"preset")));
    if (pt.count(L"tune") > 0) setTune(QString::fromStdWString(pt.get<std::wstring>(L"tune")));
    if (pt.count(L"withalpha") > 0) setWithAlpha(pt.get<bool>(L"withalpha"));
}

void FileRecorderCommand::writeProperties(QXmlStreamWriter* writer)
{
    AbstractCommand::writeProperties(writer);

    writer->writeTextElement("output", this->getOutput());
    writer->writeTextElement("codec", this->getCodec());
    writer->writeTextElement("preset", this->getPreset());
    writer->writeTextElement("tune", this->getTune());
    writer->writeTextElement("withalpha", (getWithAlpha() == true) ? "true" : "false");
}
