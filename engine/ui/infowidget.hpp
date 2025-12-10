#ifndef INFOWIDGET_HPP
#define INFOWIDGET_HPP

#include <QWidget>

class QTextEdit;

class InfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidget(QWidget* parent = nullptr);
};

#endif // INFOWIDGET_HPP
