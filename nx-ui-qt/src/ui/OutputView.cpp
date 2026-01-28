#include "OutputView.h"
#include <QVBoxLayout>

OutputView::OutputView(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    
    layout->addWidget(m_textEdit);
}

void OutputView::setOutput(const QString& text)
{
    m_textEdit->setPlainText(text);
}