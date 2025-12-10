//
// Created by Arsenii Zarudniuk on 10/12/2025.
//

#include "sqlhighlighter.hpp"

SqlHighlighter::SqlHighlighter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
    HighlightingRule rule;

    keywordFormat.setForeground(QColor(86, 156, 214));
    keywordFormat.setFontWeight(QFont::Bold);

    const QString keywordPatterns[] = {
        QStringLiteral("\\bSELECT\\b"), QStringLiteral("\\bFROM\\b"), QStringLiteral("\\bWHERE\\b"),
        QStringLiteral("\\bINSERT\\b"), QStringLiteral("\\bINTO\\b"), QStringLiteral("\\bVALUES\\b"),
        QStringLiteral("\\bUPDATE\\b"), QStringLiteral("\\bSET\\b"),
        QStringLiteral("\\bDELETE\\b"),
        QStringLiteral("\\bCREATE\\b"), QStringLiteral("\\bTABLE\\b"),
        QStringLiteral("\\bDROP\\b"),
        QStringLiteral("\\bGROUP\\b"), QStringLiteral("\\bBY\\b"), QStringLiteral("\\bHAVING\\b"),
        QStringLiteral("\\bPRIMARY\\b"), QStringLiteral("\\bFOREIGN\\b"), QStringLiteral("\\bKEY\\b"),
        QStringLiteral("\\bREFERENCES\\b")
    };

    for (const QString& pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    dataTypeFormat.setForeground(QColor(78, 201, 176));

    const QString dataTypePatterns[] = {
        QStringLiteral("\\bNVARCHAR2\\b"),
        QStringLiteral("\\bINTEGER\\b"),
        QStringLiteral("\\bDATE\\b"),
    };

    for (const QString& pattern : dataTypePatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = dataTypeFormat;
        highlightingRules.append(rule);
    }

    functionFormat.setForeground(QColor(220, 220, 170));
    rule.pattern = QRegularExpression(QStringLiteral("\\bCOUNT\\b"), QRegularExpression::CaseInsensitiveOption);
    rule.format = functionFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(QColor(206, 145, 120));
    rule.pattern = QRegularExpression(QStringLiteral("'[^']*'"));
    rule.format = quotationFormat;
    highlightingRules.append(rule);
}

void SqlHighlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
