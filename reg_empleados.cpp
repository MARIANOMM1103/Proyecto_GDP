#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <limits>   
#include <cstdio>   
using namespace std;

class Empleado {
public:
    int id;
    string curp;
    string nombreCompleto;
    string puesto;
    string direccion;
    string empresaAsignada;
    string tipoContrato;
    string numeroCredencial;
    string estado;
    string fechaIngreso;

    // Registrar un nuevo empleado
    void registrar(MYSQL* conn);

    // Editar un empleado existente
    static void editar(MYSQL* conn);

    // Eliminar un empleado por ID
    static void eliminar(MYSQL* conn);

    // Buscar un empleado por ID
    static void buscar(MYSQL* conn);
};

void Empleado::registrar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Registrar Nuevo Empleado ---\n";
    cout << "ID (número): ";
    cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia el buffer

    cout << "CURP (18 caracteres): ";
    getline(cin, curp);
    if (curp.size() != 18) {
        cerr << "Error: CURP inválido. Debe tener 18 caracteres." << endl;
        return;
    }

    cout << "Nombre Completo: ";
    getline(cin, nombreCompleto);

    cout << "Puesto: ";
    getline(cin, puesto);

    cout << "Direccion: ";
    getline(cin, direccion);

    cout << "Empresa Asignada: ";
    getline(cin, empresaAsignada);

    cout << "Tipo de Contrato (ej. '40h', '48h'): ";
    getline(cin, tipoContrato);

    cout << "Numero de Credencial (opcional): ";
    getline(cin, numeroCredencial);

    cout << "Estado (Prueba/Completo): ";
    getline(cin, estado);

    cout << "Fecha de Ingreso (YYYY-MM-DD): ";
    getline(cin, fechaIngreso);

    // Construimos la consulta de inserción
    string query = 
        "INSERT INTO empleados (id, curp, nombreCompleto, puesto, direccion, "
        "empresaAsignada, tipoContrato, numeroCredencial, estado, fechaIngreso) "
        "VALUES (" + to_string(id) + ", '" + curp + "', '" + nombreCompleto + "', '" 
        + puesto + "', '" + direccion + "', '" + empresaAsignada + "', '" 
        + tipoContrato + "', '" + numeroCredencial + "', '" + estado + "', '" 
        + fechaIngreso + "')";

    // Se ejecuta la consulta
    if (mysql_query(conn, query.c_str()) == 0) {
        cout << "Empleado registrado correctamente." << endl;
    } else {
        cerr << "Error al registrar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::editar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Editar Empleado ---\n";
    cout << "Ingrese el ID del empleado a editar: ";
    int idEditar;
    cin >> idEditar;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Verificamos si existe el empleado
    // (Podrías usar una consulta SELECT para mostrarle los datos actuales antes de editarlos)
    {
        string checkQuery = "SELECT id FROM empleados WHERE id = " + to_string(idEditar);
        if (mysql_query(conn, checkQuery.c_str()) != 0) {
            cerr << "Error al buscar empleado: " << mysql_error(conn) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(conn);
        if (!res) {
            cerr << "Error al procesar resultado: " << mysql_error(conn) << endl;
            return;
        }
        // Si no hay filas, no existe
        if (mysql_num_rows(res) == 0) {
            cout << "No se encontró un empleado con ese ID." << endl;
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);
    }

    // Pedimos los datos actualizados al usuario
    string nuevoNombre, nuevoPuesto, nuevaDireccion, nuevaEmpresa, nuevoTipo, nuevaCredencial, nuevoEstado, nuevaFecha;
    cout << "Ingrese el nuevo Nombre Completo: ";
    getline(cin, nuevoNombre);
    cout << "Ingrese el nuevo Puesto: ";
    getline(cin, nuevoPuesto);
    cout << "Ingrese la nueva Direccion: ";
    getline(cin, nuevaDireccion);
    cout << "Ingrese la nueva Empresa Asignada: ";
    getline(cin, nuevaEmpresa);
    cout << "Ingrese el nuevo Tipo de Contrato: ";
    getline(cin, nuevoTipo);
    cout << "Ingrese la nueva Credencial: ";
    getline(cin, nuevaCredencial);
    cout << "Ingrese el nuevo Estado (Prueba/Completo): ";
    getline(cin, nuevoEstado);
    cout << "Ingrese la nueva Fecha de Ingreso (YYYY-MM-DD): ";
    getline(cin, nuevaFecha);

    // Creamos la consulta UPDATE
    string updateQuery = 
        "UPDATE empleados SET "
        "nombreCompleto = '" + nuevoNombre + "', "
        "puesto = '" + nuevoPuesto + "', "
        "direccion = '" + nuevaDireccion + "', "
        "empresaAsignada = '" + nuevaEmpresa + "', "
        "tipoContrato = '" + nuevoTipo + "', "
        "numeroCredencial = '" + nuevaCredencial + "', "
        "estado = '" + nuevoEstado + "', "
        "fechaIngreso = '" + nuevaFecha + "' "
        "WHERE id = " + to_string(idEditar);

    if (mysql_query(conn, updateQuery.c_str()) == 0) {
        cout << "Empleado actualizado correctamente." << endl;
    } else {
        cerr << "Error al actualizar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::eliminar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Eliminar Empleado ---\n";
    cout << "Ingrese el ID del empleado a eliminar: ";
    int idEliminar;
    cin >> idEliminar;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Verificamos si el empleado existe
    {
        string checkQuery = "SELECT id FROM empleados WHERE id = " + to_string(idEliminar);
        if (mysql_query(conn, checkQuery.c_str()) != 0) {
            cerr << "Error al buscar empleado: " << mysql_error(conn) << endl;
            return;
        }
        MYSQL_RES* res = mysql_store_result(conn);
        if (!res) {
            cerr << "Error al procesar resultado: " << mysql_error(conn) << endl;
            return;
        }
        if (mysql_num_rows(res) == 0) {
            cout << "No se encontró un empleado con ese ID." << endl;
            mysql_free_result(res);
            return;
        }
        mysql_free_result(res);
    }

    // Creamos y ejecutamos la consulta DELETE
    string deleteQuery = "DELETE FROM empleados WHERE id = " + to_string(idEliminar);
    if (mysql_query(conn, deleteQuery.c_str()) == 0) {
        cout << "Empleado eliminado correctamente." << endl;
    } else {
        cerr << "Error al eliminar empleado: " << mysql_error(conn) << endl;
    }
}

void Empleado::buscar(MYSQL* conn) {
    if (!conn) {
        cerr << "Error: No hay conexión a la base de datos." << endl;
        return;
    }

    cout << "\n--- Buscar Empleado ---\n";
    cout << "Ingrese el ID del empleado a buscar: ";
    int idBuscar;
    cin >> idBuscar;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Creamos la consulta SELECT
    string selectQuery = "SELECT id, curp, nombreCompleto, puesto, direccion, empresaAsignada, tipoContrato, "
                         "numeroCredencial, estado, fechaIngreso "
                         "FROM empleados WHERE id = " + to_string(idBuscar);
    if (mysql_query(conn, selectQuery.c_str()) != 0) {
        cerr << "Error al buscar empleado: " << mysql_error(conn) << endl;
        return;
    }

    // Procesamos el resultado
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        cerr << "Error al procesar resultado: " << mysql_error(conn) << endl;
        return;
    }

    // Si no hay filas, el empleado no existe
    if (mysql_num_rows(res) == 0) {
        cout << "No se encontró un empleado con ese ID." << endl;
        mysql_free_result(res);
        return;
    }

    // Tomamos la primera fila (asumiendo ID único)
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        cout << "\n--- Datos del Empleado ---\n";
        cout << "ID: " << (row[0] ? row[0] : "") << endl;
        cout << "CURP: " << (row[1] ? row[1] : "") << endl;
        cout << "Nombre Completo: " << (row[2] ? row[2] : "") << endl;
        cout << "Puesto: " << (row[3] ? row[3] : "") << endl;
        cout << "Direccion: " << (row[4] ? row[4] : "") << endl;
        cout << "Empresa Asignada: " << (row[5] ? row[5] : "") << endl;
        cout << "Tipo de Contrato: " << (row[6] ? row[6] : "") << endl;
        cout << "Numero de Credencial: " << (row[7] ? row[7] : "") << endl;
        cout << "Estado: " << (row[8] ? row[8] : "") << endl;
        cout << "Fecha de Ingreso: " << (row[9] ? row[9] : "") << endl;
    }

    mysql_free_result(res);
}
