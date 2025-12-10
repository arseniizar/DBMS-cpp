#include "codeeditor.hpp"
#include <QAbstractItemView>
#include <QScrollBar>
#include <QKeyEvent>
#include <QDebug>

CodeEditor::CodeEditor(QWidget *parent) : QTextEdit(parent) {
    connect(this, &QTextEdit::cursorPositionChanged, this, &CodeEditor::onTextChanged);
}

void CodeEditor::setCompleter(QCompleter *completer)
{
    if (c) { c->disconnect(this); }
    c = completer;
    if (!c) { return; }

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    c->setFilterMode(Qt::MatchContains);

    connect(c, QOverload<const QString &>::of(&QCompleter::activated),
            this, &CodeEditor::insertCompletion);
}

void CodeEditor::insertCompletion(const QString &completion)
{
    if (c->widget() != this) return;
    QTextCursor tc = textCursor();

    tc.select(QTextCursor::WordUnderCursor);
    tc.removeSelectedText();

    tc.insertText(completion);
    tc.insertText(" ");
    setTextCursor(tc);
    c->popup()->hide();
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    return tc.selectedText();
}

void CodeEditor::onTextChanged()
{
    if (!c) return;

    QString prefix = textUnderCursor();

    if (prefix.length() < 2) {
        c->popup()->hide();
        return;
    }

    if (c->completionPrefix() != prefix) {
        c->setCompletionPrefix(prefix);
    }

    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr);
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    if (c && c->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Tab:
            if (c->currentIndex().isValid()) {
                c->activated(c->currentIndex().data(Qt::DisplayRole).toString());
                return;
            }
            break;

        case Qt::Key_Escape:
            c->popup()->hide();
            e->accept();
            return;

        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
            e->ignore();
            return;

        default:
            break;
        }
    }

    QTextEdit::keyPressEvent(e);
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (c) c->setWidget(this);
    QTextEdit::focusInEvent(e);
}
