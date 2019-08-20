#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

class TestScanSSDP : public QObject
{
    Q_OBJECT

public:
    TestScanSSDP();
    ~TestScanSSDP();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

TestScanSSDP::TestScanSSDP()
{

}

TestScanSSDP::~TestScanSSDP()
{

}

void TestScanSSDP::initTestCase()
{

}

void TestScanSSDP::cleanupTestCase()
{

}

void TestScanSSDP::test_case1()
{

}

QTEST_MAIN(TestScanSSDP)

#include "tst_testscanssdp.moc"
