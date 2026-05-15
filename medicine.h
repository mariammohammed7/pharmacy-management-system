#ifndef MEDICINE_H
#define MEDICINE_H

#include <QString>
#include <QDate>

class Medicine
{
public:
    Medicine();
    Medicine(int id, const QString& name, const QString& genericName,
             const QString& category, const QString& unit,
             int stock, int minStock, const QString& supplier,
             double unitCost, const QDate& expiryDate);

    int getId() const;
    QString getName() const;
    QString getGenericName() const;
    QString getCategory() const;
    QString getUnit() const;
    int getStock() const;
    int getMinStock() const;
    QString getSupplier() const;
    double getUnitCost() const;
    QDate getExpiryDate() const;

    void setId(int id);
    void setName(const QString& name);
    void setGenericName(const QString& g);
    void setCategory(const QString& c);
    void setUnit(const QString& u);
    void setStock(int s);
    void setMinStock(int m);
    void setSupplier(const QString& s);
    void setUnitCost(double c);
    void setExpiryDate(const QDate& d);

    bool updateStock(int delta);
    bool isLowStock() const;
    bool isExpired() const;
    bool isExpiringSoon(int days = 30) const;

    QString toFileLine() const;
    static Medicine fromFileLine(const QString& line);

private:
    int m_id;
    QString m_name;
    QString m_genericName;
    QString m_category;
    QString m_unit;
    int m_stock;
    int m_minStock;
    QString m_supplier;
    double m_unitCost;
    QDate m_expiryDate;
};

#endif
