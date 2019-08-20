#include <QtTest>

// add necessary includes here

class TestRemote : public QObject
{
    Q_OBJECT

public:
    TestRemote();
    ~TestRemote();

private slots:
    void test_case1();

};

TestRemote::TestRemote()
{

}

TestRemote::~TestRemote()
{

}

void TestRemote::test_case1()
{

}

QTEST_APPLESS_MAIN(TestRemote)

#include "tst_testremote.moc"
