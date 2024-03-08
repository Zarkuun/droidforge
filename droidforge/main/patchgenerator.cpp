#include "patchgenerator.h"
#include "tuning.h"
#include "globals.h"

#include <QFile>
#include <QProcess>
#include <QJsonObject>

PatchGenerator::PatchGenerator(QString path, QString name)
    : _valid(false)
    , _name(name)
    , _path(path)
{
    QStringList lines;
    QFile inputFile(_path);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        _error = "Cannot open file: " + inputFile.errorString();
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
        _error = "Invalid patch generator: file empty or too short";
        return;
    }

    if (!lines[0].startsWith("#!/usr/bin/python")) {
        _error = "Invalid patch generator: line 1 does not start with #!/usr/bin/python";
        return;
    }

#ifdef Q_OS_WIN
    _interpreter = "python.exe";
#else
    _interpreter = lines[0].mid(2);
#endif

    QStringList params;
    params << "-s";
    bool ok;
    _jsonSource = run(params, ok);
    if (!ok)
        return;

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
