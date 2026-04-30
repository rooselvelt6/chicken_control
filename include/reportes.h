#ifndef REPORTES_H
#define REPORTES_H

class Reportes {
public:
    static void reporteLote(int lote_id);
    static void reporteFinanciero();
    static void reporteInventario();
    static void reporteDeudas();
    static void reporteConsumo(int lote_id);
    static void reporteCompleto();
};

#endif // REPORTES_H