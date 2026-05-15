#include "medicine.h"
#include <QStringList>

Medicine::Medicine() : m_id(0), m_stock(0), m_minStock(0), m_unitCost(0.0) {}

Medicine::Medicine(int id, const QString& name, const QString& genericName,
                   const QString& category, const QString& unit,
                   int stock, int minStock, const QString& supplier,
                   double unitCost, const QDate& expiryDate)
    : m_id(id), m_name(name), m_genericName(genericName),
      m_category(category), m_unit(unit), m_stock(stock),
      m_minStock(minStock), m_supplier(supplier),
      m_unitCost(unitCost), m_expiryDate(expiryDate) {}

int Medicine::getId() const { return m_id; }
QString Medicine::getName() const { return m_name; }
QString Medicine::getGenericName() const { return m_genericName; }
QString Medicine::getCategory() const { return m_category; }
QString Medicine::getUnit() const { return m_unit; }
int Medicine::getStock() const { return m_stock; }
int Medicine::getMinStock() const { return m_minStock; }
QString Medicine::getSupplier() const { return m_supplier; }
double Medicine::getUnitCost() const { return m_unitCost; }
QDate Medicine::getExpiryDate() const { return m_expiryDate; }

bool Medicine::updateStock(int delta) {
    if (m_stock + delta < 0) return false;
    m_stock += delta;
    return true;
}

void Medicine::setId(int id) { m_id = id; }
void Medicine::setName(const QString& n) { m_name = n; }
void Medicine::setGenericName(const QString& g) { m_genericName = g; }
void Medicine::setCategory(const QString& c) { m_category = c; }
void Medicine::setUnit(const QString& u) { m_unit = u; }
void Medicine::setStock(int s) { m_stock = s; }
void Medicine::setMinStock(int m) { m_minStock = m; }
void Medicine::setSupplier(const QString& s) { m_supplier = s; }
void Medicine::setUnitCost(double c) { m_unitCost = c; }
void Medicine::setExpiryDate(const QDate& d) { m_expiryDate = d; }

bool Medicine::isLowStock() const { return m_stock <= m_minStock; }
bool Medicine::isExpired() const { return m_expiryDate.isValid() && m_expiryDate < QDate::currentDate(); }
bool Medicine::isExpiringSoon(int days) const {
    if (!m_expiryDate.isValid()) return false;
    QDate t = QDate::currentDate();
    return m_expiryDate >= t && t.daysTo(m_expiryDate) <= days;
}

QString Medicine::toFileLine() const {
    return QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10")
        .arg(m_id).arg(m_name).arg(m_genericName).arg(m_category).arg(m_unit)
        .arg(m_stock).arg(m_minStock).arg(m_supplier)
        .arg(m_unitCost, 0, 'f', 2)
        .arg(m_expiryDate.toString("yyyy-MM-dd"));
}

Medicine Medicine::fromFileLine(const QString& line) {
    QStringList p = line.split('|');
    Medicine m;
    if (p.size() < 10) return m;
    m.m_id = p[0].toInt();
    m.m_name = p[1]; m.m_genericName = p[2]; m.m_category = p[3]; m.m_unit = p[4];
    m.m_stock = p[5].toInt(); m.m_minStock = p[6].toInt();
    m.m_supplier = p[7]; m.m_unitCost = p[8].toDouble();
    m.m_expiryDate = QDate::fromString(p[9], "yyyy-MM-dd");
    return m;
}
