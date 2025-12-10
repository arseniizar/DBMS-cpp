#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include <QTextEdit>
#include <QCompleter>

class CodeEditor : public QTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    void setCompleter(QCompleter *c);

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void insertCompletion(const QString &completion);
    void onTextChanged();

private:
    QString textUnderCursor() const;
    QCompleter *c = nullptr;
};

#endif // CODEEDITOR_HPP
