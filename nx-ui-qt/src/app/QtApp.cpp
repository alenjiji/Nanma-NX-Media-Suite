#include "QtApp.h"
#include <QVBoxLayout>
#include <QWidget>

QtApp::QtApp(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Nanma NX-MediaSuite");
    resize(800, 600);
    
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    
    m_runButton = new QPushButton("Run CLI Test", this);
    m_outputText = new QPlainTextEdit(this);
    m_outputText->setReadOnly(true);
    
    layout->addWidget(m_runButton);
    layout->addWidget(m_outputText);
    
    connect(m_runButton, &QPushButton::clicked, this, &QtApp::onRunCliTest);
}

void QtApp::onRunCliTest()
{
    std::vector<std::string> args = {"nx", "--version"};
    auto result = m_adapter.run(args);
    
    QString output = QString::fromStdString(result.stdout_text) +
                    QString::fromStdString(result.stderr_text) +
                    QString::number(result.exit_code) + "\n";
    
    m_outputText->setPlainText(output);
}