#include "patchgenerator.h"
#include "QtCore/qjsonarray.h"
#include "tuning.h"
#include "globals.h"

#include <QFile>
#include <QProcess>
#include <QJsonObject>
#include <QSettings>

PatchGenerator::PatchGenerator(QString path, QString name)
    : _valid(false)
    , _name(name)
    , _path(path)
{
    QStringList lines;
    QFile inputFile(_path);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        _error = TR("Cannot open file: ") + inputFile.errorString();
        return;
    }

    QTextStream in(&inputFile);
    while (!in.atEnd()) {
        lines.append(in.readLine());
        if (lines.length() >= 2)
            break;
    }
    inputFile.close();
    if (lines.length() != 2) {
        _error = TR("Invalid patch generator: file empty or too short");
        return;
    }

    if (!lines[0].startsWith("#!/usr/bin/python") &&
        !lines[0].startsWith("#!/usr/bin/env python"))
    {
        _error = TR("Invalid patch generator: line 1 must either start "
                    "with \"#!/usr/bin/python\" or with "
                    "\"#!/usr/bin/env python\" but starts with \"%1\".").arg(lines[0]);
        return;
    }

    QSettings settings;
    QString pythonPath = settings.value("system/python_path", "").toString();
    if (pythonPath != "")
        _interpreter = pythonPath;
    else {
        #ifdef Q_OS_WIN
        _interpreter = "python.exe";
        #else
        if (lines[0].startsWith("#!/usr/bin/env "))
            _interpreter = lines[0].mid(14).trimmed();
        else
            _interpreter = lines[0].mid(2).trimmed();
        #endif
    }

    QStringList params;
    params << "-s";
    bool ok;
    _jsonSource = run(params, ok).trimmed();

    if (_jsonSource == "") {
        _error = TR("The generator has produced an empty output. This could be "
                    "an issue with an installed security scanner. Try quitting "
                    "and restarting the Forge.");

        if (pythonPath != "") {
            _error += TR("\n\nIn the preferences you have specified the custom path '%1' "
                         "to the Python3 interpreter. Maybe this path "
                         "is wrong. Leave the setting empty to use the default.").arg(pythonPath);
        }

        return;
    }

    else if (!ok) {
        _error = TR("The patch generator exited with an error. Output:\n\n%1").arg(_jsonSource);
        return;
    }

    QJsonParseError jsonError;
    _parameterInfo = QJsonDocument::fromJson(_jsonSource.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        _error = TR("Invalid output from patch generator: JSON parse error:\n\n\"%1\"")
                     .arg(jsonError.errorString());
        return;
    }

    _title = _parameterInfo.object()["title"].toString();
    if (_title == "") {
        _error = TR("Invalid output from patch generator: missing global key \"title\"");
        return;
    }

    _description = _parameterInfo.object()["description"].toString();

    auto presets = _parameterInfo.object()["presets"].toArray();
    if (presets.empty()) {
        _error = TR("The patch generator has no presets. Each patch generator must "
                    "at least define one preset.");
        return;
    }

    _valid = true;
}

QString PatchGenerator::run(const QStringList &args, bool &ok)
{
    QStringList cmdline;
    cmdline << _path;
    cmdline << args;
    QProcess proc;
    proc.start(_interpreter, cmdline);
    proc.closeWriteChannel();
    proc.waitForFinished(GENERATOR_START_TIMEOUT_MS);
    QString output = proc.readAll();
    QString error = proc.readAllStandardError();
    ok = proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0;
    if (!ok) {
        output += error;
        _error = output;
    }

    return output;
}
