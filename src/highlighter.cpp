/****************************************************************************
Copyright 2015 Chukong Technologies

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
****************************************************************************/

#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    _errorFormat.setFontWeight(QFont::Bold);
    _errorFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("\\b(error|failed)\\b");
    rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    rule.format = _errorFormat;
    _highlightingRules.append(rule);

    _warningFormat.setFontWeight(QFont::Bold);
    _warningFormat.setForeground(Qt::darkRed);
    rule.pattern = QRegExp("\\warning\\b");
    rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    rule.format = _warningFormat;
    _highlightingRules.append(rule);

    _successFormat.setFontWeight(QFont::Bold);
    _successFormat.setForeground(Qt::green);
    rule.pattern = QRegExp("\\success\\b");
    rule.pattern.setCaseSensitivity(Qt::CaseInsensitive);
    rule.format = _successFormat;
    _highlightingRules.append(rule);

    _runningFormat.setFontWeight(QFont::Thin);
    _runningFormat.setForeground(Qt::gray);
    _runningFormat.setFontItalic(true);
    rule.pattern = QRegExp("\\Running\\b");
    rule.format = _runningFormat;
    _highlightingRules.append(rule);
}

Highlighter::~Highlighter()
{
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, _highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        if (index >= 0) {
            setFormat(0, text.length(), rule.format);
            break;
        }
    }
}
