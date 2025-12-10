//
// Created by Arsenii Zarudniuk on 10/12/2025.
//

#ifndef DATABASEPROJECT_SQLHIGHLIGHTER_HPP
#define DATABASEPROJECT_SQLHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class SqlHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    explicit SqlHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat dataTypeFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};


#endif //DATABASEPROJECT_SQLHIGHLIGHTER_HPP
