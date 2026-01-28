#pragma once

#include <QWidget>
#include <QPlainTextEdit>

class OutputView : public QWidget
{
public:
    OutputView(QWidget *parent = nullptr);
    
    void setOutput(const QString& text);

private:
    QPlainTextEdit* m_textEdit;
};