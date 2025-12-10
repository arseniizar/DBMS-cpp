#ifndef HELPWIDGET_HPP
#define HELPWIDGET_HPP

#include <QWidget>

class QTextEdit;

class HelpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HelpWidget(QWidget *parent = nullptr);
};

#endif // HELPWIDGET_HPP
