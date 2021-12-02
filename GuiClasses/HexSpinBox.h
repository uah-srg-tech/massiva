#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <QAbstractSpinBox>

class QRegExpValidator;

class HexSpinBox : public QAbstractSpinBox
{
    Q_OBJECT
    
    Q_PROPERTY(qulonglong minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(qulonglong maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(qulonglong value READ value WRITE setValue NOTIFY valueChanged USER true)
	
public:
    HexSpinBox(QWidget *parent = 0, unsigned int bytes = 1);
    qulonglong value() const;
    qulonglong minimum() const;
    void setMinimum(qulonglong min);
    qulonglong maximum() const;
    void setMaximum(qulonglong max, bool limitLength = true);
    void setRange(qulonglong min, qulonglong max);
    void stepBy(int steps);
    static qulonglong ullpow(qulonglong base, uint exp);

public slots:
    void setValue(qulonglong value);
    void interpretText();

signals:
    void valueChanged(qulonglong v);

protected:
    QValidator::State validate(QString &text, int &pos) const;
    void fixup(QString &input) const;
    QAbstractSpinBox::StepEnabled stepEnabled() const;
    qulonglong valueFromText(const QString &text) const;
    QString textFromValue(qulonglong value) const;

private:
    QRegExpValidator *validator;
    
    qulonglong m_minimum;
    qulonglong m_maximum;
    qulonglong m_value;
    unsigned int m_maxTextLength;
    
    Q_DISABLE_COPY(HexSpinBox);
};

#endif /* HEXSPINBOX_H */