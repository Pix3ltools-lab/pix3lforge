#ifndef FILTERBASE_H
#define FILTERBASE_H

#include <QImage>
#include <QObject>
#include <QString>

class FilterBase : public QObject
{
    Q_OBJECT

public:
    explicit FilterBase(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~FilterBase() = default;

    virtual QString name() const = 0;
    virtual QImage apply(const QImage &image) = 0;
    virtual QWidget* settingsWidget() = 0;
};

#endif // FILTERBASE_H