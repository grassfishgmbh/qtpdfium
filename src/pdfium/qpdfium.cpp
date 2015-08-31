#include "qpdfium.h"
#include "qpdfiumpage.h"
#include "../3rdparty/pdfium/public/fpdfview.h"

QT_BEGIN_NAMESPACE

QPdfium::QPdfium(QObject *parent)
    : QObject(parent)
    , m_document(nullptr)
    , m_pageCount(0)
    , m_caching(true)
{
}

QPdfium::QPdfium(QString filename, QObject *parent)
    : QObject(parent)
    , m_document(nullptr)
    , m_pageCount(0)
{
    setFilename(filename);
}

QPdfium::~QPdfium() {
    if (m_document) {
        m_pages.clear();
        FPDF_CloseDocument(m_document);
    }
    m_document = nullptr;
}

bool QPdfium::isValid() const
{
    return m_document != NULL;
}

void QPdfium::setFilename(QString filename)
{
    if (m_filename != filename) {
        m_filename = filename;
        if (m_document) {
            m_pages.clear();
            FPDF_CloseDocument(m_document);
        }
        m_document = FPDF_LoadDocument(m_filename.toUtf8().constData(), NULL);
        if (m_document)
            m_pageCount = FPDF_GetPageCount(m_document);
    }
}

QString QPdfium::filename() const
{
    return m_filename;
}

int QPdfium::pageCount() const
{
    return m_pageCount;
}

QWeakPointer<QPdfiumPage> QPdfium::page(int i)
{
    Q_ASSERT( i < m_pageCount && i >=0 );
    if (m_pages[i].isNull())
        m_pages[i] = QSharedPointer<QPdfiumPage>(new QPdfiumPage(FPDF_LoadPage(m_document, i), i));

    if(!m_caching) {
        foreach (int key, m_pages.keys()) {
            if(i != key) {
                m_pages.remove(key);
            }
        }
    }

    return m_pages[i].toWeakRef();
}

void QPdfium::setCaching(bool caching)
{
    m_caching = caching;
}

bool QPdfium::caching()
{
    return m_caching;
}

QT_END_NAMESPACE
